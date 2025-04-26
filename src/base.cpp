#include <math.h>
internal inline f32
abs_f32(f32 x) {
    union { f32 f; u32 u; } result;
    result.f = x;
    result.u &= 0x7fffffff;
    return(result.f);
}

internal inline f32
cos_f32(f32 x) {
    f32 result = cosf(x);
    return(result);
}

internal inline f32
sin_f32(f32 x) {
    f32 result = sinf(x);
    return(result);
}

internal inline f32
tan_f32(f32 x) {
    f32 result = tanf(x);
    return(result);
}

internal inline f32
square_root(f32 x) {
    f32 result = sqrtf(x);
    return(result);
}

internal inline f32
square(f32 x) {
    f32 result = x*x;
    return(result);
}

internal inline s32
ceil_f32_to_s32(f32 x) {
    s32 result = (s32)ceilf(x);
    return(result);
}

internal inline s32
floor_f32_to_s32(f32 x) {
    s32 result = (s32)floorf(x);
    return(result);
}

internal inline s32
round_f32_to_s32(f32 x) {
    s32 result = (s32)roundf(x);
    return(result);
}

internal inline u32
round_f32_to_u32(f32 x) {
    u32 result = (u32)roundf(x);
    return(result);
}

internal inline s32
truncate_f32_to_s32(f32 x) {
    s32 result = (s32)x;
    return(result);
}

internal inline u32
truncate_f32_to_u32(f32 x) {
    u32 result = (u32)x;
    return(result);
}

internal inline s32
sign_of(s32 x) {
    s32 result = (x >= 0) ? 1 : -1;
    return(result);
}

internal inline u32
rotate_left(u32 value, s32 amount) {
#if COMPILER_CL
    u32 result = _rotl(value, amount);
#else
    // TODO(xkazu0x): port this to other compiler platform
    amount &= 31;
    u32 result = ((value << amount) | (value >> (32 - amount)));
#endif
    return(result);
}

internal inline u32
rotate_right(u32 value, s32 amount) {
#if COMPILER_CL
    u32 result = _rotr(value, amount);
#else
    // TODO(xkazu0x): port this to other compiler platform
    amount &= 31;
    u32 result = ((value >> amount) | (value << (32 - amount)));
#endif
    return(result);
}

#if COMPILER_CL
#include <intrin.h>
#endif

internal inline bit_scan_result_t
find_least_significant_set_bit(u32 value) {
    bit_scan_result_t result = {};
#if COMPILER_CL
    result.found = _BitScanForward((unsigned long *)&result.index, value);
#else
    for (u32 test = 0; test < 32; test++) {
        if (value & (1 << test)) {
            result.index = test;
            result.found = true;
            break;
        }
    }
#endif
    return(result);
}

internal operating_system_t
operating_system_from_context(void) {
    operating_system_t result = OPERATING_SYSTEM_UNDEFINED;
#if OS_WINDOWS
    result = OPERATING_SYSTEM_WINDOWS;
#elif OS_LINUX
    result = OPERATING_SYSTEM_LINUX;
#elif OS_MAC
    result = OPERATING_SYSTEM_MAC;
#endif
    return(result);
}

internal architecture_t
architecture_from_context(void) {
    architecture_t result = ARCHITECTURE_UNDEFINED;
#if ARCH_X64
    result = ARCHITECTURE_X64;
#elif ARCH_X86
    result = ARCHITECTURE_X86;
#elif ARCH_ARM
    result = ARCHITECTURE_ARM;
#elif ARCH_ARM64
    result = ARCHITECTURE_ARM64;
#endif
    return(result);
}

internal char *
string_from_operating_system(operating_system_t os) {
    char *result;
    switch(os) {
        case OPERATING_SYSTEM_WINDOWS: {
            result = "windows";
        } break;
        case OPERATING_SYSTEM_LINUX: {
            result = "linux";
        } break;
        case OPERATING_SYSTEM_MAC: {
            result = "mac";
        } break;
        default: {
            result = "undefined";
        }
    }
    return(result);
}

internal char *
string_from_architecture(architecture_t arch) {
    char *result;
    switch(arch) {
        case ARCHITECTURE_X64: {
            result = "x64";
        } break;
        case ARCHITECTURE_X86: {
            result = "x86";
        } break;
        case ARCHITECTURE_ARM: {
            result = "arm";
        } break;
        case ARCHITECTURE_ARM64: {
            result = "arm64";
        } break;
        default: {
            result = "undefined";
        }
    }
    return(result);
}

#include <stdio.h>
#include <stdarg.h>

internal void
_print(char *message, ...) {
    local char message_buffer[2048];
    
    va_list arg_list;
    va_start(arg_list, message);
    vsprintf_s(message_buffer, sizeof(message_buffer), message, arg_list);
    va_end(arg_list);

    printf(message_buffer);
}
