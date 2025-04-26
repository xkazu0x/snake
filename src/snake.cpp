#include "base.h"
#include "math.h"
#include "os.h"

#include "base.cpp"
#include "math.cpp"
#include "os.cpp"

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

#define GRID_SIZE WINDOW_HEIGHT
#define GRID_X (WINDOW_WIDTH - GRID_SIZE)/2
#define GRID_Y 0

#define GRID_CELL_COUNT 15
#define GRID_CELL_SIZE GRID_SIZE / GRID_CELL_COUNT

internal void
draw_at_cell(renderer_t *renderer, vec2 cell, vec3 color) {
    vec2 min = make_vec2(GRID_X + cell.x*GRID_CELL_SIZE, GRID_Y + cell.y*GRID_CELL_SIZE);
    vec2 max = make_vec2(min.x + GRID_CELL_SIZE, min.y + GRID_CELL_SIZE);
    renderer_draw_rect(renderer, min, max, color);
}

int main(void) {
    window_t window = create_window("snake", WINDOW_WIDTH, WINDOW_HEIGHT);
    input_t input = create_input(&window);

    renderer_t renderer = create_renderer(&window, WINDOW_WIDTH, WINDOW_HEIGHT);

    vec2 snake_pos = make_vec2(GRID_CELL_COUNT/2);
    vec2 snake_delta = make_vec2(0.0f);
    vec3 snake_color = make_rgb(230, 234, 238);

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
        f32 seconds_per_frame = (((f32)(end_time - start_time))/((f32)performance_frequency));
        f32 ms_per_frame = 1000.0f*seconds_per_frame;
        
        start_time = end_time;

        if (pause) continue;
        
        if (!lose) {
            if (input.keyboard[KEY_UP].down    && (snake_delta.y !=  1)) snake_delta = make_vec2( 0, -1);
            if (input.keyboard[KEY_DOWN].down  && (snake_delta.y != -1)) snake_delta = make_vec2( 0,  1);
            if (input.keyboard[KEY_LEFT].down  && (snake_delta.x !=  1)) snake_delta = make_vec2(-1,  0);
            if (input.keyboard[KEY_RIGHT].down && (snake_delta.x != -1)) snake_delta = make_vec2( 1,  0);

            ms_counter += ms_per_frame;
            if (ms_counter - last_update > ms_update) {
                snake_pos += snake_delta;
                last_update = ms_counter;
            }

            if ((snake_pos.x >= GRID_CELL_COUNT) ||
                (snake_pos.y >= GRID_CELL_COUNT) ||
                (snake_pos.x < 0) ||
                (snake_pos.y < 0)) {
                lose = true;
            }
        } else {
            if (input.keyboard[KEY_ENTER].pressed) {
                snake_pos = make_vec2(GRID_CELL_COUNT/2);
                snake_delta = make_vec2(0.0f);
                lose = false;
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

        draw_at_cell(&renderer, snake_pos, snake_color);
        renderer_present(&renderer, &window);
    }

    destroy_window(&window);
    return(0);
}
