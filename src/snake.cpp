#include "base.h"
#include "math.h"
#include "os.h"

#include "base.cpp"
#include "math.cpp"
#include "os.cpp"

#include <time.h>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

#define GRID_SIZE WINDOW_HEIGHT
#define GRID_X (WINDOW_WIDTH - GRID_SIZE)/2
#define GRID_Y 0

#define GRID_CELL_COUNT 15
#define GRID_CELL_SIZE GRID_SIZE / GRID_CELL_COUNT

#define SNAKE_BODY_COUNT (GRID_CELL_COUNT*GRID_CELL_COUNT)

internal void
draw_at_cell(renderer_t *renderer, vec2 cell, vec3 color) {
    vec2 min = make_vec2(GRID_X + cell.x*GRID_CELL_SIZE, GRID_Y + cell.y*GRID_CELL_SIZE);
    vec2 max = make_vec2(min.x + GRID_CELL_SIZE, min.y + GRID_CELL_SIZE);
    renderer_draw_rect(renderer, min, max, color);
}

int main(void) {
    srand(time(0));

    window_t window = create_window("snake", WINDOW_WIDTH, WINDOW_HEIGHT);
    renderer_t renderer = create_renderer(&window, WINDOW_WIDTH, WINDOW_HEIGHT);
    input_t input = create_input(&window);

    u32 snake_body_count = 0;
    vec2 snake_body[SNAKE_BODY_COUNT];
    memset(&snake_body, 0, SNAKE_BODY_COUNT*sizeof(vec2));

    vec2 snake_head = make_vec2(GRID_CELL_COUNT/2);
    vec2 snake_delta = make_vec2(0.0f);

    //vec3 snake_color = make_rgb(230, 234, 238);
    vec3 snake_head_color = make_rgb(164, 157, 164);
    vec3 snake_body_color = make_rgb(131, 137, 139);
    
    vec2 berry_pos = make_vec2(rand() % GRID_CELL_COUNT, rand() % GRID_CELL_COUNT);
    vec3 berry_color = make_rgb(213, 190, 205);
    
    s64 performance_frequency = get_performance_frequency();
    s64 start_time = get_performance_counter();

    f32 ms_counter = 0.0f;
    f32 last_update = 0.0f;
    f32 ms_update = 150.0f;

    b32 win = false;
    b32 lose = false;
    b32 pause = false;
    while (!window.should_quit) {
        update_window_events(&window, &input);
        if (input.keyboard[KEY_ESCAPE].pressed) {
            window.should_quit = true;
            break;
        }
        if (input.keyboard[KEY_F11].pressed) {
            toggle_window_fullscreen(&window);
        }
        if (input.keyboard[KEY_SPACE].pressed) {
            pause = !pause;
        }
        
        s64 end_time = get_performance_counter();

        f32 frames_per_second = (((f32)performance_frequency)/((f32)(end_time - start_time)));
        f32 seconds_per_frame = (((f32)(end_time - start_time))/((f32)performance_frequency));
        f32 ms_per_frame = 1000.0f*seconds_per_frame;
        
        start_time = end_time;

#if _DEBUG
        local f32 title_ms_counter = 0.0f;
        local f32 last_title_update = 0.0f;
        title_ms_counter += ms_per_frame;
        if (title_ms_counter - last_title_update > 250.0f) {
            char new_window_title[512];
            sprintf(new_window_title, "snake - %.02ff/s, %.02fms/f", frames_per_second, ms_per_frame);
            change_window_title(&window, new_window_title);
            last_title_update = title_ms_counter;
        }
#endif

        if (!pause) {
            if (!lose) {
                if (input.keyboard[KEY_UP].down && (snake_delta.y !=  1)) {
                    snake_delta = make_vec2( 0, -1);
                }
                if (input.keyboard[KEY_DOWN].down && (snake_delta.y != -1)) {
                    snake_delta = make_vec2( 0,  1);
                }
                if (input.keyboard[KEY_LEFT].down && (snake_delta.x !=  1)) {
                    snake_delta = make_vec2(-1,  0);
                }
                if (input.keyboard[KEY_RIGHT].down && (snake_delta.x != -1)) {
                    snake_delta = make_vec2( 1,  0);
                }

                ms_counter += ms_per_frame;
                if (ms_counter - last_update > ms_update) {
                    for (u32 body_index = snake_body_count;
                         body_index > 0;
                         --body_index) {
                        snake_body[body_index] = snake_body[body_index - 1];
                    }

                    snake_body[0] = snake_head;
                    snake_head += snake_delta;
                                        
                    if (snake_head == berry_pos) {
                        snake_body_count++;
                        
                        b32 same_place = true;
                        vec2 random_pos = make_vec2(rand() % GRID_CELL_COUNT, rand() % GRID_CELL_COUNT);
                
                        for (u32 body_index = 0;
                             body_index < snake_body_count;
                             ++body_index) {
                            while (random_pos == snake_body[body_index]) {
                                random_pos = make_vec2(rand() % GRID_CELL_COUNT, rand() % GRID_CELL_COUNT);
                                if (random_pos != snake_body[body_index]) {
                                    same_place = false;
                                    break;
                                }
                            }
                            if (!same_place) {
                                break;
                            }
                        }
                    
                        berry_pos = random_pos;
                    }

                    for (u32 body_index = 0;
                         body_index < snake_body_count;
                         ++body_index) {
                        if (snake_head == snake_body[body_index]) {
                            lose = true;
                        }
                    }
                    
                    if ((snake_head.x >= GRID_CELL_COUNT) ||
                        (snake_head.y >= GRID_CELL_COUNT) ||
                        (snake_head.x < 0) ||
                        (snake_head.y < 0)) {
                        lose = true;
                    }
                    
                    last_update = ms_counter;
                }
            } else {
                if (input.keyboard[KEY_ENTER].pressed) {
                    snake_body_count = 0;
                    
                    snake_head = make_vec2(GRID_CELL_COUNT/2);
                    snake_delta = make_vec2(0.0f);
                    
                    berry_pos = make_vec2(rand() % GRID_CELL_COUNT, rand() % GRID_CELL_COUNT);
                    lose = false;
                }
            }
        }
        
        renderer_clear(&renderer, make_rgb(38, 28, 32));
        for (u32 y = 0; y < GRID_CELL_COUNT; ++y) {
            for (u32 x = 0; x < GRID_CELL_COUNT; ++x) {
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
        draw_at_cell(&renderer, berry_pos, berry_color);
        
        renderer_present(&renderer, &window);
    }
    
    destroy_window(&window);
    return(0);
}
