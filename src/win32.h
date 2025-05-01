#ifndef WIN32_H
#define WIN32_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <timeapi.h>
#include <xinput.h>

struct window_win32_t {
    HMODULE window_instance;
    ATOM window_atom;
    HWND window_handle;
    BITMAPINFO bitmap_info;
    WINDOWPLACEMENT window_placement;
};

#define WIN32_GET_PROC_ADDR(v, m, s) (*(PROC*)(&(v))) = GetProcAddress((m), (s))

#define XINPUT_GET_STATE(x) DWORD WINAPI x(DWORD, XINPUT_STATE *)
#define XINPUT_SET_STATE(x) DWORD WINAPI x(DWORD, XINPUT_VIBRATION *)

typedef XINPUT_GET_STATE(xinput_get_state_t);
typedef XINPUT_SET_STATE(xinput_set_state_t);

internal window_size_t win32_get_window_size(HWND window);
internal void win32_toggle_window_fullscreen(HWND window, WINDOWPLACEMENT *placement);
internal LRESULT CALLBACK win32_window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

#endif // WIN32_H
