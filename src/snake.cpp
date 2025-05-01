#include "base.h"
#include "math.h"
#include "os.h"

#include "base.cpp"
#include "math.cpp"
#include "os.cpp"

#include <time.h>

/* TODO(xkazu0x):
   ARCHITECTURE
   - draw bitmaps
   - hot reloading
   - resize renderer to a high resolution?
   
   GAME
   - snake smooth movement
   - snake animation
*/

#define SCALE 4

#define RENDER_WIDTH  320
#define RENDER_HEIGHT 180

#define WINDOW_WIDTH  (SCALE*RENDER_WIDTH)
#define WINDOW_HEIGHT (SCALE*RENDER_HEIGHT)

#define GRID_CELL_COUNT_X 10
#define GRID_CELL_COUNT_Y 10
#define GRID_CELL_SIZE 16
#define GRID_WIDTH  (GRID_CELL_COUNT_X*GRID_CELL_SIZE)
#define GRID_HEIGHT (GRID_CELL_COUNT_Y*GRID_CELL_SIZE)

#define SNAKE_BODY_COUNT (GRID_CELL_COUNT_X*GRID_CELL_COUNT_Y)

enum snake_state_t {
    SNAKE_STATE_UNDEFINED,
    SNAKE_STATE_UP,
    SNAKE_STATE_DOWN,
    SNAKE_STATE_LEFT,
    SNAKE_STATE_RIGHT,
};

internal void
draw_at_cell(renderer_t *renderer, vec2 cell, vec3 color) {
    u32 offset_x = (RENDER_WIDTH - GRID_WIDTH)/2;
    u32 offset_y = (RENDER_HEIGHT - GRID_HEIGHT)/2;

    vec2 min = make_vec2(offset_x + cell.x*GRID_CELL_SIZE, offset_y + cell.y*GRID_CELL_SIZE);
    vec2 max = make_vec2(min.x + GRID_CELL_SIZE, min.y + GRID_CELL_SIZE);
    renderer_draw_rect(renderer, min, max, color);
}

int main(void) {
    srand(time(0));

    monitor_info_t monitor = get_monitor_info();
    
    window_t window = create_window("snake", WINDOW_WIDTH, WINDOW_HEIGHT);
    renderer_t renderer = create_renderer(&window, RENDER_WIDTH, RENDER_HEIGHT);
    
    input_t input = create_input(&window);
    gamepad_t *gamepad = &input.gamepads[0];
    
    u32 snake_body_count = 0;
    vec2 snake_body[SNAKE_BODY_COUNT];
    memset(&snake_body, 0, SNAKE_BODY_COUNT*sizeof(vec2));

    vec2 snake_head = make_vec2(GRID_CELL_COUNT_X/2, GRID_CELL_COUNT_Y/2);
    vec2 snake_delta = make_vec2(0.0f);
    vec2 berry = make_vec2(rand() % GRID_CELL_COUNT_X, rand() % GRID_CELL_COUNT_Y);
    
    vec3 snake_head_color = make_rgb(115, 129, 123);
    vec3 snake_body_color = make_rgb(98, 105, 106);
    vec3 berry_color = make_rgb(213, 190, 205);

    snake_state_t old_snake_state = SNAKE_STATE_UNDEFINED;
    snake_state_t new_snake_state = old_snake_state;
    
    b32 lose = false;
    b32 pause = false;
    
    f32 ms_counter = 0.0f;
    f32 last_update = 0.0f;
    f32 ms_update = 250.0f;

    f32 target_seconds_per_frame = 1.0f/(f32)monitor.refresh_rate;
    f32 target_ms_per_frame = 1000.0f*target_seconds_per_frame;

    b32 sleep_is_granular = is_sleep_granular(1);
    s64 performance_frequency = get_performance_frequency();
    s64 start_time = get_performance_counter();

    while (!window.should_quit) {
        update_window_events(&window, &input);
        if (input.keyboard[KEY_ESCAPE].pressed) {
            window.should_quit = true;
            break;
        }
        
        if (input.keyboard[KEY_F11].pressed) {
            toggle_window_fullscreen(&window);
        }
        
        if (input.keyboard[KEY_SPACE].pressed ||
            gamepad->start.pressed) {
            pause = !pause;
        }

        if (!pause) {
            if (!lose) {
                if (input.keyboard[KEY_UP].down)    new_snake_state = SNAKE_STATE_UP;
                if (input.keyboard[KEY_DOWN].down)  new_snake_state = SNAKE_STATE_DOWN;
                if (input.keyboard[KEY_LEFT].down)  new_snake_state = SNAKE_STATE_LEFT;
                if (input.keyboard[KEY_RIGHT].down) new_snake_state = SNAKE_STATE_RIGHT;

                if (gamepad->up.down)    new_snake_state = SNAKE_STATE_UP;
                if (gamepad->down.down)  new_snake_state = SNAKE_STATE_DOWN;
                if (gamepad->left.down)  new_snake_state = SNAKE_STATE_LEFT;
                if (gamepad->right.down) new_snake_state = SNAKE_STATE_RIGHT;

                if (gamepad->left_stick.y > 0.0f) new_snake_state = SNAKE_STATE_UP;
                if (gamepad->left_stick.y < 0.0f) new_snake_state = SNAKE_STATE_DOWN;
                if (gamepad->left_stick.x < 0.0f) new_snake_state = SNAKE_STATE_LEFT;
                if (gamepad->left_stick.x > 0.0f) new_snake_state = SNAKE_STATE_RIGHT;
                
                ms_counter += target_ms_per_frame;
                if (ms_counter - last_update > ms_update) {
                    switch (new_snake_state) {
                        case SNAKE_STATE_UP: {
                            if (old_snake_state != SNAKE_STATE_DOWN) {
                                snake_delta = make_vec2(0.0f, -1.0f);
                                old_snake_state = new_snake_state;
                            }
                        } break;
                        case SNAKE_STATE_DOWN: {
                            if (old_snake_state != SNAKE_STATE_UP) {
                                snake_delta = make_vec2(0.0f, 1.0f);
                                old_snake_state = new_snake_state;
                            }
                        } break;
                        case SNAKE_STATE_LEFT: {
                            if (old_snake_state != SNAKE_STATE_RIGHT) {
                                snake_delta = make_vec2(-1.0f, 0.0f);
                                old_snake_state = new_snake_state;
                            }
                        } break;
                        case SNAKE_STATE_RIGHT: {
                            if (old_snake_state != SNAKE_STATE_LEFT) {
                                snake_delta = make_vec2(1.0f, 0.0f);
                                old_snake_state = new_snake_state;
                            }
                        } break;
                        default: {
                            old_snake_state = new_snake_state;
                        } break;
                    }

                    for (u32 body_index = snake_body_count;
                         body_index > 0;
                         --body_index) {
                        snake_body[body_index] = snake_body[body_index - 1];
                    }
                    snake_body[0] = snake_head;
                    snake_head += snake_delta;
                    
                    last_update = ms_counter;
                }
                
                if (snake_head == berry) {
                    snake_body_count++;
                    // TODO(xkazu0x):
                    snake_body[snake_body_count] = snake_body[snake_body_count - 1];
                    
                    vec2 random_pos;
                    b32 same_pos;
                    do {
                        random_pos = make_vec2(rand() % GRID_CELL_COUNT_X, rand() % GRID_CELL_COUNT_Y);
                        same_pos = false;
                        for (u32 body_index = 0;
                             body_index < snake_body_count;
                             ++body_index) {
                            if (random_pos == snake_body[body_index]) {
                                same_pos = true;
                                break;
                            }
                        }
                    } while (same_pos);
                    
                    berry = random_pos;
                }

                for (u32 body_index = 0;
                     body_index < snake_body_count;
                     ++body_index) {
                    if (snake_head == snake_body[body_index]) {
                        new_snake_state = SNAKE_STATE_UNDEFINED;
                        lose = true;
                    }
                }
                    
                if ((snake_head.x >= GRID_CELL_COUNT_X) ||
                    (snake_head.y >= GRID_CELL_COUNT_Y) ||
                    (snake_head.x < 0) ||
                    (snake_head.y < 0)) {
                    new_snake_state = SNAKE_STATE_UNDEFINED;
                    lose = true;
                }
            } else {
                ms_counter += target_ms_per_frame;
                if (ms_counter - last_update > (6.0f*ms_update)/snake_body_count) {
                    if (snake_body_count > 0) {
                        snake_body_count--;
                    }
                    last_update = ms_counter;
                }
                
                if (input.keyboard[KEY_ENTER].pressed ||
                    gamepad->a.pressed) {
                    snake_body_count = 0;
                   
                    snake_head = make_vec2(GRID_CELL_COUNT_X/2, GRID_CELL_COUNT_Y/2);
                    snake_delta = make_vec2(0.0f);
                    
                    berry = make_vec2(rand() % GRID_CELL_COUNT_X, rand() % GRID_CELL_COUNT_Y);
                    new_snake_state = SNAKE_STATE_UNDEFINED;
                    lose = false;
                }
            }
        }
        
        renderer_clear(&renderer, make_rgb(38, 28, 32));
        for (u32 y = 0; y < GRID_CELL_COUNT_Y; ++y) {
            for (u32 x = 0; x < GRID_CELL_COUNT_X; ++x) {
                vec3 color = make_rgb(57, 44, 49);
                if ((x+y) % 2 == 0) {
                    color = make_rgb(74, 60, 74);
                }

                draw_at_cell(&renderer, make_vec2(x, y), color);
            }
        }
        
        for (u32 body_index = 0;
             body_index < snake_body_count;
             ++body_index) {
            draw_at_cell(&renderer, snake_body[body_index], snake_body_color);
        }
        draw_at_cell(&renderer, snake_head, snake_head_color);
        draw_at_cell(&renderer, berry, berry_color);

        // NOTE(xkazu0x): time
        s64 end_time = get_performance_counter();

        f32 frames_per_second = (((f32)performance_frequency)/((f32)(end_time - start_time)));
        f32 seconds_per_frame = (((f32)(end_time - start_time))/((f32)performance_frequency));
        f32 ms_per_frame = 1000.0f*seconds_per_frame;

        f32 seconds_elapsed_for_frame = seconds_per_frame;
        if (seconds_elapsed_for_frame < target_seconds_per_frame) {
            if (sleep_is_granular) {
                u32 sleep_ms = (u32)(1000.0f*(target_seconds_per_frame - seconds_elapsed_for_frame));
                if (sleep_ms > 0)  {
                    sleep(sleep_ms);
                }
            }
            while (seconds_elapsed_for_frame < target_seconds_per_frame) {
                seconds_elapsed_for_frame = (((f32)(get_performance_counter() - start_time))/((f32)performance_frequency));
            }
        }
        
        start_time = end_time;

        // NOTE(xkazu0x): present
        renderer_present(&renderer, &window);

        // TODO(xkazu0x): debug only
#if _DEBUG
        local f32 title_ms_counter = 0.0f;
        local f32 last_title_update = 0.0f;
        title_ms_counter += ms_per_frame;
        if (title_ms_counter - last_title_update > 250.0f) {
            char new_window_title[512];
            sprintf(new_window_title, "snake [%.02ffps - %.02fms]", frames_per_second, ms_per_frame);
            change_window_title(&window, new_window_title);
            last_title_update = title_ms_counter;
        }
#endif
    }
    
    destroy_window(&window);
    return(0);
}
