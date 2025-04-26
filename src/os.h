#ifndef OS_H
#define OS_H

enum key_code_t {
    KEY_ENTER = 0x0D,
        
    KEY_ESCAPE = 0x1B,
    KEY_SPACE  = 0x20,
    
    KEY_LEFT  = 0x25,
    KEY_UP    = 0x26,
    KEY_RIGHT = 0x27,
    KEY_DOWN  = 0x28,
    
    KEY_0 = 0x30,
    KEY_1 = 0x31,
    KEY_2 = 0x32,
    KEY_3 = 0x33,
    KEY_4 = 0x34,
    KEY_5 = 0x35,
    KEY_6 = 0x36,
    KEY_7 = 0x37,
    KEY_8 = 0x38,
    KEY_9 = 0x39,
    
    KEY_A = 0x41,
    KEY_B = 0x42,
    KEY_C = 0x43,
    KEY_D = 0x44,
    KEY_E = 0x45,
    KEY_F = 0x46,
    KEY_G = 0x47,
    KEY_H = 0x48,
    KEY_I = 0x49,
    KEY_J = 0x4A,
    KEY_K = 0x4B,
    KEY_L = 0x4C,
    KEY_M = 0x4D,
    KEY_N = 0x4E,
    KEY_O = 0x4F,
    KEY_P = 0x50,
    KEY_Q = 0x51,
    KEY_R = 0x52,
    KEY_S = 0x53,
    KEY_T = 0x54,
    KEY_U = 0x55,
    KEY_V = 0x56,
    KEY_W = 0x57,
    KEY_X = 0x58,
    KEY_Y = 0x59,
    KEY_Z = 0x5A,
    
    KEY_F1 = 0x70,
    KEY_F2 = 0x71,
    KEY_F3 = 0x72,
    KEY_F4 = 0x73,
    KEY_F5 = 0x74,
    KEY_F6 = 0x75,
    KEY_F7 = 0x76,
    KEY_F8 = 0x77,
    KEY_F9 = 0x78,
    KEY_F10 = 0x79,
    KEY_F11 = 0x7A,
    KEY_F12 = 0x7B,
    KEY_F13 = 0x7C,
    KEY_F14 = 0x7D,
    KEY_F15 = 0x7E,
    KEY_F16 = 0x7F,
    KEY_F17 = 0x80,
    KEY_F18 = 0x81,
    KEY_F19 = 0x82,
    KEY_F20 = 0x83,
    KEY_F21 = 0x84,
    KEY_F22 = 0x85,
    KEY_F23 = 0x86,
    KEY_F24 = 0x87,
    
    KEY_MAX = 256,
};

struct digital_button_t {
    b32 down;
    b32 pressed;
    b32 released;
};

struct mouse_t {
    digital_button_t left;
    digital_button_t right;
    digital_button_t middle;
    digital_button_t x1;
    digital_button_t x2;
    s32 wheel;
    s32 dwheel;
    s32 x;
    s32 y;
    s32 dx;
    s32 dy;
};

struct input_t {
    digital_button_t keyboard[KEY_MAX];
    mouse_t mouse;
};

struct window_size_t {
    s32 width;
    s32 height;
};

struct window_t {
    b32 should_quit;
    void *platform;
};

struct renderer_t {
    s32 width;
    s32 height;
    s32 bytes_per_pixel;
    s32 pitch;
    void *memory;
};

internal window_t create_window(char *title, s32 width, s32 height);
internal void destroy_window(window_t *window);
internal void update_window_events(window_t *window);
internal void toggle_window_fullscreen(window_t *window);
internal window_size_t get_window_size(window_t *window);

internal input_t create_input(window_t *window);
internal void input_process_digital_button(digital_button_t *button, b32 down);
internal void update_window_events(window_t *window, input_t *input);

internal renderer_t create_renderer(window_t *window, s32 width, s32 height);
internal void renderer_clear(renderer_t *renderer, vec3 color);
internal void renderer_present(renderer_t *renderer, window_t *window);
internal void renderer_draw_pixel(renderer_t *renderer, f32 x, f32 y, vec3 color);
internal void renderer_draw_pixel(renderer_t *renderer, vec2 position, vec3 color);
internal void renderer_draw_rect(renderer_t *renderer, vec2 min, vec2 max, vec3 color);

internal s64 get_performance_frequency();
internal s64 get_performance_counter();
internal void sleep(u32 ms);

#endif // OS_H
