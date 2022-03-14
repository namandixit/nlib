/*
 * Creator: Naman Dixit
 * Notice: © Copyright 2018 Naman Dixit
 * SPDX-License-Identifier: 0BSD
 * Version: 889
 */

// TODO(naman): Make all these data structures handle allocation failure gracefully.
// This is especially important since we are going to implement fixed buffer data
// structures by making the allocation function return NULL after the buffer has
// been filled. Any allocation failures have to be handled inside the Allocator.
// This means that the Allocator returning NULL doesn't imply a failure to allocate,
// but a refusal to do so (either due to underlying semantics, or failure).

// NOTE(naman): Some variables are initialized to 0 even when they will get initialized
// inside a conditional/switch anyway, since in MSVC, "potentially uninitialized local variable"
// warning number is 4701, and according to MSDN (docs.microsoft.com/en-us/cpp/preprocessor/warning):
//     Warning numbers in the range 4700-4999 are associated with code
//     generation. For these warnings, the state of the warning in
//     effect when the compiler reaches the function definition remains
//     in effect for the rest of the function. Use of the warning pragma
//     in the function to change the state of a warning number larger
//     than 4699 only takes effect after the end of the function.

#if !defined(NLIB_H_INCLUDE_GUARD)

/* COMMON ===================================================================== */

/* Platform Identification ---------------------------------------------------- */

# if defined(_MSC_VER)
#  if defined(__clang__)
#   define COMPILER_CLANG
#   define COMPILER_CLANG_WITH_MSVC
#  else
#   define COMPILER_MSVC
#  endif
# elif defined (__GNUC__)
#  if defined(__clang__)
#   define COMPILER_CLANG
#   define COMPILER_CLANG_WITH_GCC
#  else
#   define COMPILER_GCC
#  endif
# elif defined(__clang__)
#  define COMPILER_CLANG
# else
#  error Compiler not supported
# endif

# if defined(_WIN32)
#  if !defined(OS_WINDOWS) // Shlwapi.h defines it own OS_WINDOWS to 0. Thankfully, we only care
                           // whether it is defined or now.
#   define OS_WINDOWS
#  endif
# elif defined(__linux__)
#  define OS_LINUX
# else
#  error Operating system not supported
# endif

# if defined(COMPILER_MSVC) || defined(COMPILER_CLANG_WITH_MSVC)
#  if defined(_M_IX86)
#   define ARCH_X86
#  elif defined(_M_X64)
#   define ARCH_X64
#  endif
# elif defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#  if defined(__i386__)
#   define ARCH_X86
#  elif defined(__x86_64__)
#   define ARCH_X64
#  endif
# endif

# if !defined(ARCH_X64)  // && !defined(ARCH_X86)
#  error Architecture not supported
# endif

# if defined(ARCH_X86)
#  define BITWIDTH_32
# elif defined(ARCH_X64)
#  define BITWIDTH_64
# else
#  error "Bitwidth not supported"
# endif

// TODO(naman): Check the state of C11 support for MSVC, and update this properly
# if defined(COMPILER_MSVC)
#  if defined(__cplusplus)
#   if __cplusplus == 199711L
#    define LANG_CPP 1998
#   elif __cplusplus == 201402L
#    define LANG_CPP 2014
#   elif __cplusplus == 201703L
#    define LANG_CPP 2017
#   else
#    define LANG_CPP 1998 // A future C++ or bad compiler, just assume C++98 for safety
#   endif
#  elif defined(__STDC_VERSION__)
#   if (__STDC_VERSION__ == 201112) || (__STDC_VERSION__ == 201710)
#    define LANG_C 2011
#   else
#    define LANG_C 2011 // Earliest C version for which MSVC supports __STDC_VERSION__
#   endif
#  elif defined(__STDC__) // All microsoft extensions are off (/Za, similar to pedantic)
#   define LANG_C 1989
#  else // No /Za, so assuming C99 is good enough (C89 is too old to fallback on)
#   define LANG_C 1999
#  endif
# elif defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#  if defined(__cplusplus)
#   if __cplusplus == 199711L
#    define LANG_CPP 1998
#   elif __cplusplus == 201103L
#    define LANG_CPP 2011
#   elif __cplusplus == 201402L
#    define LANG_CPP 2014
#   elif __cplusplus == 201703L
#    define LANG_CPP 2017
#   else
#    define LANG_CPP 1998 // A future C++ or bad compiler, just assume C++98 for safety
#   endif
#  elif defined(__STDC_VERSION__) // Using C Language >= 1994 (1989)
#   if (__STDC_VERSION__ == 199409)
#    define LANG_C 1989
#   elif (__STDC_VERSION__ == 199901)
#    define LANG_C 1999
#   elif (__STDC_VERSION__ == 201112) || (__STDC_VERSION__ == 201710)
#    define LANG_C 2011
#   else
#    define LANG_C 1999 // Atleast C99 (__STDC_VERSION__ is defined, C94 is too old to fallback on)
#   endif
#  elif defined(__STDC__) && !defined(__STDC_VERSION__)
#    define LANG_C 1989 // Since C89 doesn't require definition of __STDC_VERSION__
#  endif
# endif

# if !defined(LANG_C) && !defined(LANG_CPP)
#  error Language not supported
# endif

# if defined(OS_WINDOWS)
#  if defined(ARCH_X86) || defined(ARCH_X64)
#   define ENDIAN_LITTLE
#  else
#   error Could not determine endianness on Windows
#  endif
# elif defined(OS_LINUX)
#  include <endian.h>
#  if __BYTE_ORDER == __LITTLE_ENDIAN
#   define ENDIAN_LITTLE
#  elif __BYTE_ORDER == __BIG_ENDIAN
#   define ENDIAN_BIG
#  else
#   error Could not determine endianness on Linux
#  endif
# else
#   error Can not determine endianness, unknown environment
# endif

/* Less noisy pragmas */

# if defined(COMPILER_CLANG)
#  define pragma_msvc(P)
#  define pragma_clang(P) _Pragma(P)
#  define pragma_gcc(P)
# elif defined(COMPILER_GCC)
#  define pragma_msvc(P)
#  define pragma_clang(P)
#  define pragma_gcc(P) _Pragma(P)
# elif defined(COMPILER_MSVC)
#  define pragma_msvc(P) _Pragma(P)
#  define pragma_clang(P)
#  define pragma_gcc(P)
# endif

/* Mandatory Defines ----------------------------------------------------- */

#if defined(OS_WINDOWS)
pragma_clang("clang diagnostic push")
pragma_clang("clang diagnostic ignored \"-Wreserved-macro-identifier\"")
#define UNICODE
#define _UNICODE
pragma_clang("clang diagnostic push")
#endif


/* Standard C Headers ----------------------------------------------------- */

// NOTE(naman): Define NLIB_NOLIBC_WINDOWS to prevent the inclusion of libc functions.
# if defined(NLIB_NOLIBC_WINDOWS) && !defined(OS_WINDOWS)
#  error NLIB_NOLIBC_WINDOWS only works on Windows
# endif

# include <stddef.h>
# include <stdint.h>
# include <stdbool.h>
# include <inttypes.h>
# include <limits.h>
# include <stdarg.h>
# include <uchar.h>
// FIXME(naman): Including stdnoreturn.h here causes issues on Windows Clang,
// just use _Noreturn for now.
//# include <stdnoreturn.h>
# include <float.h>

# if defined(NLIB_NOLIBC_WINDOWS)
#  define NLIB_NOLIBC
# endif

# if !defined(NLIB_NOLIBC)
#  include <stdio.h>
#  include <math.h>
#  include <stdlib.h>
#  include <errno.h>
#  include <string.h>
#  include <assert.h>

#  if (defined(LANG_C) && LANG_C >= 2011) || (defined(LANG_CPP) && LANG_CPP >= 2011)
#   include <stdalign.h>
#  endif

#  if !defined(__STDC_NO_ATOMICS__)
// #   include <stdatomic.h> UNCOMMENT WHEN NEEDED
#  endif

#  if !defined(__STDC_NO_THREADS__)
// #   include <threads.h> UNCOMMENT WHEN NEEDED
#  endif

#  if !defined(__STDC_NO_COMPLEX__)
// #   include <complex.h> UNCOMMENT WHEN NEEDED
#  endif
# endif // !defined(NLIB_NOLIBC)

/* Cross-platform Headers ------------------------------------------------------ */

# include <emmintrin.h> // SSE2 + SSE + MMX (Pentium 4, Athlon 64)

/* Platform dependent Headers -------------------------------------------------- */

# if defined(OS_WINDOWS)
#  define NOMINMAX
#  define COBJMACROS
#  include <Windows.h>
#  undef COBJMACROS
#  undef NOMINMAX
# elif defined(OS_LINUX)
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <sys/mman.h>
#  include <fcntl.h>
#  include <unistd.h>
#  include <pthread.h>
#  include <semaphore.h>
#  include <sys/ioctl.h>
#  include <linux/perf_event.h>
#  include <asm/unistd.h>
# endif

/* Compiler Extensions --------------------------------------------------------- */

# if defined(COMPILER_MSVC)

#  define fallthrough
#  define likely(x)   (x)
#  define unlikely(x) (x)

// NOTE(naman): MSVC doesn't seem to define max_align_t for C11 code, so this will suffice for now.
// The aignment is 8 for x86 and 16 for x64, see here:
// docs.microsoft.com/en-us/cpp/c-runtime-library/reference/malloc?view=msvc-160
#  if defined(ARCH_x86)
// Alignment is 8 bytes
typedef union {
    alignas(8) char alignment[8];
    double a;
} max_align_t;
static_assert(alignof(max_align_t) == 8, "Alignment of max_align_t is not 8");
#  elif defined(ARCH_X64)
// Alignment is 16 bytes
typedef union {
    __m128 sse; // 16-byte aligned (docs.microsoft.com/en-us/cpp/cpp/m128?view=msvc-160)
    alignas(16) char alignment[16];
    alignas(16) struct { double a, b; } f;
} max_align_t;
static_assert(alignof(max_align_t) == 16, "Alignment of max_align_t is not 16");
#  endif

# elif defined(COMPILER_CLANG) || defined(COMPILER_GCC)

#  define fallthrough __attribute__((fallthrough))
#  define likely(x)   __builtin_expect(!!(x), 1)
#  define unlikely(x) __builtin_expect(!!(x), 0)

# endif


/* ===============
 * Primitive Types
 */

typedef int8_t               S8;
typedef int16_t              S16;
typedef int32_t              S32;
typedef int64_t              S64;
typedef int                  Sint;

typedef uint8_t              U8;
typedef uint16_t             U16;
typedef uint32_t             U32;
typedef uint64_t             U64;
typedef unsigned             Uint;

typedef size_t               Size;

typedef uintptr_t            Uptr;
typedef intptr_t             Sptr;
typedef ptrdiff_t            Dptr;

typedef float                F32;
typedef double               F64;

typedef bool                 Bool;

typedef U8                   B8;
typedef U16                  B16;
typedef U32                  B32;
typedef U64                  B64;

typedef unsigned char        Byte;

typedef char                 Char;
typedef unsigned char        C8; // FIXME(naman): Change to char8_t with C23
typedef char16_t             C16;
typedef char32_t             C32;


/* ========================
 * Preprocessor Definitions
 */

# define elemin(array) (sizeof(array)/sizeof((array)[0]))
# define containerof(ptr, type, member)                         \
    ((type*)(void*)(((Byte*)(ptr)) - offsetof(type, member)))

# define isOdd(x)  ((x) % 2 ? true : false)
# define isEven(x) (!isOdd(x))

# define KiB(x) (   (x) * 1024ULL)
# define MiB(x) (KiB(x) * 1024ULL)
# define GiB(x) (MiB(x) * 1024ULL)
# define TiB(x) (GiB(x) * 1024ULL)

# define THOUSAND 1000L
# define MILLION  1000000L
# define BILLION  1000000000L

# define unused_variable(var) (void)var

# define global_variable   static
# define global_immutable  static const
# define persistent_value  static

# define internal_function static
# define header_function   static inline

# if defined(COMPILER_MSVC) || defined(COMPILER_CLANG_WITH_MSVC)
#  if defined(BUILD_DLL)
#   define exported_function __declspec(dllexport)
#  else
#   define exported_function __declspec(dllimport)
#  endif
# elif defined(COMPILER_GCC) || defined(COMPILER_CL)
#  define exported_function __attribute__((visibility("default")))
# endif


# if defined(LANG_C)
#  define NLIB_NULL NULL
#  define NLIB_ZERO_INIT_LIST {0}
# elif defined(LANG_CPP)
#  define NLIB_NULL nullptr
#  define NLIB_ZERO_INIT_LIST {}
# endif

// NOTE(naman): If in doubt, first attempt cast_val and then cast_mem
// NOTE(naman): For cast_mem, the `m` should be a variable (something which is in memory)
// and not just an expression.
# if defined(LANG_C)
#  define cast_mem(m, T) (*((T*)(&(m))))
#  define cast_val(v, T) ((T)(v))
# elif defined(LANG_CPP)
#  define cast_mem(m, T) (reinterpret_cast<T>(m))
#  define cast_val(v, T) (static_cast<T>(v))
#  define NLIB_ZERO_INIT_LIST {}
# endif

# define gensym_uniq(prefix) gensym2_(prefix, __COUNTER__)
# define gensym_line(prefix) gensym2_(prefix, __LINE__)
# define gensym_func(prefix) gensym2_(prefix, __func__)
# define gensym_file(prefix) gensym2_(prefix, __FILE__)

# define gensym2_(prefix, suffix) gensym_cat_(prefix, suffix)
# define gensym_cat_(prefix, suffix) prefix ## suffix

#define entail(...)                                                     \
    goto gensym_line(jump_to_else);                                     \
                                                                        \
    while (true)                                                        \
        if (true) {                                                     \
            __VA_ARGS__;                                                \
            break;                                                      \
        } else gensym_line(jump_to_else):

#define envelop(cloak_arg_pre_action, cloak_arg_post_action)            \
    cloak_arg_pre_action;                                               \
    goto gensym_line(jump_to_else);                                     \
                                                                        \
    while (true)                                                        \
        if (true) {                                                     \
            cloak_arg_post_action;                                      \
            break;                                                      \
        } else gensym_line(jump_to_else):


/* ==================
 * @CString
 */

# if !defined(NLIB_EXCLUDE_CSTRING)

#  if defined(NLIB_NOLIBC)
header_function
Sint strcmp (const Char *s1, const Char *s2)
{
    while(s1[0]) {
        if (s1[0] != s2[0]) break;
        s1++;
        s2++;
    }

    return s1[0] - s2[0];
}

header_function
Sint strncmp (const Char *s1, const Char *s2, Size count)
{
    Sint result = 0;

    for (Size i = 0; i < count; i++)
    {
        if (s1[i] != s2[i]) {
            result = (s1[i] < s2[i]) ? -1 : 1;
            break;
        }
    }

    return result;
}

header_function
Size strlen (const Char *s)
{
    Size len = 0;

    for (Size i = 0; s[i] != '\0'; i++) {
        len++;
    }

    len++;

    return len;
}
#  endif

header_function
Bool streq (const Char *str1, const Char *str2)
{
    Bool result = (strcmp(str1, str2) == 0);
    return result;
}

header_function
Bool strneq (const Char *str1, const Char *str2, Size count)
{
    Bool result = (strncmp(str1, str2, count) == 0);
    return result;
}

header_function
Size strprefix (Char *str, Char *pre)
{
    Size lenpre = strlen(pre);
    Size lenstr = strlen(str);

    if (lenstr < lenpre) {
        return 0;
    } else {
        if (strneq(pre, str, lenpre)) {
            return lenpre;
        } else {
            return 0;
        }
    }
}

header_function
Size strsuffix (Char *str, Char *suf)
{
    Size lensuf = strlen(suf);
    Size lenstr = strlen(str);

    if (lenstr < lensuf) {
        return 0;
    } else {
        if (strneq(suf, str + (lenstr - lensuf), lensuf)) {
            return lensuf;
        } else {
            return 0;
        }
    }
}

# endif // NLIB_EXCLUDE_CSTRING


/* ===============
 * @Memory
 */

# if !defined(NLIB_EXCLUDE_MEMORY)

#  define MEM_MAX_ALIGN_MINUS_ONE (alignof(max_align_t) - 1u)
#  define memAlignUp(p) (((p) + MEM_MAX_ALIGN_MINUS_ONE) & (~ MEM_MAX_ALIGN_MINUS_ONE))
#  define memAlignDown(p) (memAlignUp((p) - MEM_MAX_ALIGN_MINUS_ONE))

// TODO(naman): Should we add some sample allocators too? Or should they always be applications' concern?

typedef enum Memory_Allocator_Mode {
    Memory_ALLOCATE,
    Memory_REALLOCATE,
    Memory_DEALLOCATE,
    Memory_DEALLOCATE_ALL,
} Memory_Allocator_Mode;

#  define MEMORY_ALLOCATOR_FUNCTION(allocator)          \
    void* allocator (Memory_Allocator_Mode mode,        \
                     Size old_size,                     \
                     Size new_size,                     \
                     void* old_ptr,                     \
                     void *userdata)

typedef MEMORY_ALLOCATOR_FUNCTION(Memory_Allocator_Function);

typedef struct Memory_Allocator {
    Memory_Allocator_Function *function;
    void *userdata;
} Memory_Allocator;

#  define memAlloc(m, ns)          ((m).function(Memory_ALLOCATE,   0,  ns, NLIB_NULL, (m).userdata))
#  define memRealloc(m, p, ns, os) ((m).function(Memory_REALLOCATE, os, ns, p,         (m).userdata))
#  define memDealloc(m, p, os)     ((m).function(Memory_DEALLOCATE, os, 0,  p,         (m).userdata))


#  if defined(OS_WINDOWS)

global_variable HANDLE GLOBAL_win32_process_heap = NLIB_NULL;

header_function
MEMORY_ALLOCATOR_FUNCTION(memHeap)
{
    unused_variable(old_size);
    unused_variable(userdata);

    if (GLOBAL_win32_process_heap == NLIB_NULL) GLOBAL_win32_process_heap = GetProcessHeap();

    switch (mode) {
        case Memory_ALLOCATE: {
            void *memory = HeapAlloc(GLOBAL_win32_process_heap, 0, new_size);
            return memory;
        } break;

        case Memory_REALLOCATE: {
            void *memory = HeapReAlloc(GLOBAL_win32_process_heap, 0, old_ptr, new_size);
            return memory;
        } break;

        case Memory_DEALLOCATE: {
            HeapFree(GLOBAL_win32_process_heap, 0, old_ptr);
            return NLIB_NULL;
        } break;

        case Memory_DEALLOCATE_ALL: {
            return NLIB_NULL;
        } break;
    }

    return NLIB_NULL;
}

#  elif defined(OS_LINUX)

header_function
MEMORY_ALLOCATOR_FUNCTION(memHeap)
{
    unused_variable(old_size);
    unused_variable(userdata);

    switch (mode) {
        case Memory_ALLOCATE: {
            void *memory = malloc(new_size);
            return memory;
        } break;

        case Memory_REALLOCATE: {
            void *memory = realloc(old_ptr, new_size);
            return memory;
        } break;

        case Memory_DEALLOCATE: {
            free(old_ptr);
            return NLIB_NULL;
        } break;

        case Memory_DEALLOCATE_ALL: {
            return NLIB_NULL;
        } break;
    }
}

#  endif // memHeap (OS-defined)

header_function
Memory_Allocator memHeapGet (void)
{
    Memory_Allocator allocator;
    allocator.function = &memHeap;
    allocator.userdata = NLIB_NULL;

    return allocator;
}

#  define memHeapAlloc(size)        memHeap(Memory_ALLOCATE,   0, size, NLIB_NULL, NLIB_NULL)
#  define memHeapRealloc(ptr, size) memHeap(Memory_REALLOCATE, 0, size, ptr,  NLIB_NULL)
#  define memHeapDealloc(ptr)       memHeap(Memory_DEALLOCATE, 0, 0,    ptr,  NLIB_NULL)

# endif // @memory


/* ==============
 * @Console Output
 */

#  if !defined(NLIB_CONSOLE_OUTPUT_ALLOCATOR)
#   if defined(NLIB_ALLOCATOR)
#    define NLIB_CONSOLE_OUTPUT_ALLOCATOR NLIB_ALLOCATOR
#   elif defined(NLIB_NOLIBC)
#    error "nlib: Unicode: NLIB_CONSOLE_OUTPUT_ALLOCATOR required with nolibc"
#   else
#    define NLIB_CONSOLE_OUTPUT_ALLOCATOR memHeapGet()
#   endif
#  endif

typedef enum {
    Print_Console_Stream_STDOUT,
    Print_Console_Stream_STDERR,
} Print_Console_Stream;

#  if defined(OS_WINDOWS)

global_variable HANDLE GLOBAL_win32_stdout_handle = NLIB_NULL;
global_variable HANDLE GLOBAL_win32_stderr_handle = NLIB_NULL;

header_function
Size print_Console (Print_Console_Stream stream, Char const * const cstr)
{
    if (GLOBAL_win32_stdout_handle == NLIB_NULL) GLOBAL_win32_stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GLOBAL_win32_stderr_handle == NLIB_NULL) GLOBAL_win32_stderr_handle = GetStdHandle(STD_ERROR_HANDLE);

    HANDLE out_stream = NLIB_NULL;
    switch (stream) {
        case Print_Console_Stream_STDOUT: {
            out_stream = GLOBAL_win32_stdout_handle;
        } break;
        case Print_Console_Stream_STDERR: {
            out_stream = GLOBAL_win32_stderr_handle;
        } break;
    }

    DWORD written = 0;
    Size len = strlen(cstr);
    WriteFile(out_stream, cstr, cast_val(len, DWORD), &written, NLIB_NULL);

    return len;
}

header_function
Size printOut (Char const * const cstr)
{

    Size result = print_Console(Print_Console_Stream_STDOUT, cstr);
    return result;
}

header_function
Size printDebug (Char const *cstr)
{
    int wcstr_length = MultiByteToWideChar(CP_UTF8, 0, cstr, -1, NLIB_NULL, 0);
    DWORD wcstr_size = cast_val(wcstr_length, DWORD) * sizeof(wchar_t);
    LPWSTR wcstr = cast_val(memAlloc(NLIB_CONSOLE_OUTPUT_ALLOCATOR, wcstr_size), LPWSTR);
    MultiByteToWideChar(CP_UTF8, 0, cstr, -1, wcstr, wcstr_length);

    OutputDebugStringW(wcstr);

    memDealloc(NLIB_CONSOLE_OUTPUT_ALLOCATOR, wcstr, wcstr_size);

    return cast_val(wcstr_length, Size);
}

header_function
Size printErr (Char const * const cstr)
{
    Size result = 0;
    if (IsDebuggerPresent()) {
        result = printDebug(cstr);
    } else {
        result = print_Console(Print_Console_Stream_STDERR, cstr);
    }

    return result;
}

#  elif defined(OS_LINUX)

header_function
Size printOut (Char const * const cstr)
{
    Size result = fputs(cstr, stdout);
    return cast_val(result, Size);
}

header_function
Size printErr (Char const * const cstr)
{
    Size result = fputs(cstr, stderr);
    return cast_val(result, Size);
}

#  endif

header_function
Char* print_Number (U64 number, Char string[20])
{
    U64 num = number;
    Char *str = string + 19;
    Size len = 0;

    do {
        str--;
        *str = cast_val(num % 10, Char) + '0';
        num /= 10;
        len++;
    } while (num != 0);

    return str;
}

header_function
Size printOutNum (U64 num)
{
    Char string[20] = NLIB_ZERO_INIT_LIST;
    Char *str = print_Number(num, string);
    Size result = printOut(str);
    return result;
}

header_function
Size printErrNum (U64 num)
{
    Char string[20] = NLIB_ZERO_INIT_LIST;
    Char *str = print_Number(num, string);
    Size result = printErr(str);
    return result;
}

/* ====================
 * @Quit
 */

# if !defined(NLIB_EXCLUDE_QUIT)

/* API ----------------------------------------
 * Size breakpoint (Char *fmt, ...)
 *
 *     If BUILD_DEBUG is defined, this causes a breakpoint – if the program is open in
 *     a debugger, that breakpoint will be caught and handled; however, if the program is not
 *     in a debugger, the program will exit.
 */

#  if defined(OS_WINDOWS)
#   define quit() ExitProcess(0)
#  elif defined(OS_LINUX)
#   define quit() exit(0)
#  endif // !window && !linux

#  if defined(BUILD_DEBUG)
#   if defined(OS_WINDOWS)
#    define breakpoint() __debugbreak()
#   elif defined(OS_LINUX)
#    if defined(ARCH_X86) || defined(ARCH_X64)
#     if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#      define breakpoint() __asm__ volatile("int $0x03")
#     endif // !GCC && !Clang
#    endif // !x86 && !x64
#   endif // !window && !linux
#  else // !BUILD_DEBUG
header_function
void breakpoint(void) {
    printErr("Fired breakpoint in release code, quitting...\n");
    quit();
}
#  endif // !build_debug


# endif // @quit

/* ==============
 * @Claim (assert)
 */

# if !defined(NLIB_EXCLUDE_CLAIM)

#  if defined(BUILD_DEBUG)
#   define claim(cond)          claim_((cond), #cond, __FILE__, __LINE__)

header_function
void claim_ (Bool cond,
             Char const *cond_str,
             Char const *filename, U32 line_num)
{
    if (!cond) {
        printErr("Claim \"");
        printErr(cond_str);
        printErr("\" Failed in ");
        printErr(filename);
        printErr(":");
        printErrNum(line_num);
        printErr("\n\n");

        breakpoint();
    }
}
#  else // !BUILD_DEBUG
#   define claim(cond) ((void)(cond))
#  endif // BUILD_DEBUG

# endif // @claim

/* ===================
 * @Unit Test
 */

# if !defined(NLIB_EXCLUDE_UNIT_TEST)

global_variable Uint GLOBAL_ut_block_failed_tests = 0;
global_variable Uint GLOBAL_ut_suite_failed_tests = 0;
global_variable Uint GLOBAL_ut_suite_test_counter = 0;


#define utSuite(param_str, param_quit_on_fail)                          \
    { /* Initialization */                                              \
        printErr(param_str);                                            \
        printErr("\n");                                                 \
        GLOBAL_ut_suite_failed_tests = 0;                               \
        GLOBAL_ut_suite_test_counter = 0;                               \
    }                                                                   \
                                                                        \
    goto gensym_line(jump_to_else);                                     \
                                                                        \
    while (true)                                                        \
        if (true) { /* Finalization */                                  \
            /* This block executes after the following code block */    \
            printErr("REPORT: ");                                       \
            if (GLOBAL_ut_suite_failed_tests == 0) {                    \
                printErr("All tests succeeded\n");                      \
            } else {                                                    \
                printErrNum(GLOBAL_ut_suite_failed_tests);              \
                if (GLOBAL_ut_suite_failed_tests == 1) {                \
                    printErr(" TEST FAILED\n");                         \
                } else {                                                \
                    printErr(" TESTS FAILED\n");                        \
                }                                                       \
            }                                                           \
                                                                        \
            printf("------------------------------------------------------------\n"); \
            if (param_quit_on_fail) utQuitOnFail();                     \
            break;                                                      \
        } else gensym_line(jump_to_else):

#define utBlock(param_str)                                              \
    { /* Initialization */                                              \
        printErr("\t");                                                 \
        printErr(param_str);                                            \
        printErr("\n");                                                 \
        GLOBAL_ut_block_failed_tests = 0;                               \
    }                                                                   \
                                                                        \
    goto gensym_line(jump_to_else);                                     \
                                                                        \
    while (true)                                                        \
        if (true) { /* Finalization */                                  \
            /* This block executes after the following code block */    \
            if (GLOBAL_ut_block_failed_tests != 0) {                    \
                printErr("\t##### ");                                   \
                printErrNum(GLOBAL_ut_block_failed_tests);              \
                if (GLOBAL_ut_block_failed_tests == 1) {                \
                    printErr(" TEST FAILED #####\n");                   \
                } else {                                                \
                    printErr(" TESTS FAILED #####\n");                  \
                }                                                       \
            }                                                           \
                                                                        \
            break;                                                      \
        } else gensym_line(jump_to_else):

#define utTest(cond) ut_Test(cond, #cond, __FILE__, __LINE__)

header_function
void ut_Test (Bool cond, Char *cond_str, Char *filename, U32 line_num)
{
    GLOBAL_ut_suite_test_counter++;

    if (cond) {
//        printErr("\t\tTest #");
//        printErrNum(GLOBAL_ut_suite_test_counter);
//        printErr(": -passed-\n");
    } else {
        printErr("\t\tTest #");
        printErrNum(GLOBAL_ut_suite_test_counter);
        printf(": *FAILED*: \"");
        printErr(cond_str);
        printErr("\" in ");
        printErr(filename);
        printErr(":");
        printErrNum(line_num);
        printErr("\n");
        GLOBAL_ut_block_failed_tests++;
        GLOBAL_ut_suite_failed_tests++;
    }

    return;
}

header_function
Bool utSomeTestsFailed (void)
{
    return GLOBAL_ut_suite_failed_tests != 0;
}

header_function
void utQuitOnFail (void)
{
    if (utSomeTestsFailed()) {
        breakpoint();
    }
}

# endif // NLIB_EXCLUDE_UNIT_TEST


/* ==================
 * @Bit Operators
 */

# if !defined(NLIB_EXCLUDE_BIT)

// NOTE(naman): Bit vectors are supposed to be zero-indexed.
// NOTE(naman): Base type of bit vectors shouldn't have size > 8 bytes (to prevent shift overflow).

#  define bitToBytes(b) (((b)+(CHAR_BIT-1))/(CHAR_BIT))

#  define bit_ValInBuf(array, index) ((index)/(CHAR_BIT * sizeof(*(array))))
#  define bit_BitInVal(array, index) ((index)%(CHAR_BIT * sizeof(*(array))))
#  define bitSet(array, index)                                          \
    ((array)[bit_ValInBuf(array, index)] |= (1LLU << bit_BitInVal(array, index)))
#  define bitReset(array, index)                                        \
    ((array)[bit_ValInBuf(array, index)] &= ~(1LLU << bit_BitInVal(array, index)))
#  define bitToggle(array, index)                                       \
    ((array)[bit_ValInBuf(array, index)] ^= ~(1LLU << bit_BitInVal(array, index)))
#  define bitTest(array, index)                                         \
    ((array)[bit_ValInBuf(array, index)] & (1LLU << bit_BitInVal(array, index)))

#  if defined(COMPILER_MSVC)

/* _BitScanReverse(&r, x) scans for the first 1-bit from left in x. Once it finds it,
 * it returns the number of bits to the right of the found 1-bit.
 */
header_function
U32 bitFindMSBU32 (U32 x)
{
    unsigned long result = 0;
    Byte found = _BitScanReverse(&result, x);
    result = found ? result : 0;
    return result;
}

header_function
U64 bitFindMSBU64 (U64 x)
{
    unsigned long result = 0;
    Byte found = _BitScanReverse64(&result, x);
    result = found ? result : 0;
    return result;
}

/* _BitScanReverse(&r, x) scans for the first 1-bit from right in x. Once it finds it,
 * it returns the number of bits to the right of the found 1-bit.
 */
header_function
U32 bitFindLSBU32 (U32 x)
{
    unsigned long result = 0;
    Byte found = _BitScanForward(&result, x);
    result = found ? result : (U32)-1;
    return result;
}

header_function
U64 bitFindLSBU64 (U64 x)
{
    unsigned long result = 0;
    Byte found = _BitScanForward64(&result, x);
    result = found ? result : (U32)-1;
    return result;
}

#  elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)

/* __builtin_clz(x) returns the number of leading 0-bits in x, starting from
 * most significant position.
 */
header_function
U32 bitFindMSBU32 (U32 x)
{
    if (x == 0) return cast_val(-1, U32);
    U32 result = 32U - cast_val(__builtin_clz(x), U32) - 1U;
    return result;
}

header_function
U64 bitFindMSBU64 (U64 x)
{
    if (x == 0) return cast_val(-1LL, U64);
    U64 result = 64LLU - cast_val(__builtin_clzll(x), U64) - 1LLU;
    return result;
}

/* __builtin_ctz(x) returns the number of trailing 0-bits in x, starting from
 * least significant position.
 */
header_function
U32 bitFindLSBU32 (U32 x)
{
    if (x == 0) return cast_val(-1, U32);
    U32 result = cast_val(__builtin_ctz(x), U32);
    return result;
}

header_function
U64 bitFindLSBU64 (U64 x)
{
    if (x == 0) return cast_val(-1LL, U64);
    U64 result = cast_val(__builtin_ctzll(x), U64);
    return result;
}

#  endif

#  if defined(LANG_C) && LANG_C >= 2011
#   define bitFindMSB(x) _Generic((x),                  \
                                  U32: bitFindMSBU32,   \
                                  U64: bitFindMSBU64,   \
        )(x)

#   define bitFindLSB(x) _Generic((x),                  \
                                  U32: bitFindLSBU32,   \
                                  U64: bitFindLSBU64,   \
        )(x)
#  elif defined(LANG_CPP)
header_function U32 bitFindMSB (U32 x) { return bitFindMSBU32(x); }
header_function U64 bitFindMSB (U64 x) { return bitFindMSBU64(x); }
header_function U32 bitFindLSB (U32 x) { return bitFindLSBU32(x); }
header_function U64 bitFindLSB (U64 x) { return bitFindLSBU64(x); }
#  endif // !LANG_C && !LANG_CPP

# endif // NLIB_EXCLUDE_BIT


/* =================
 * @Maths
 */

# if !defined(NLIB_EXCLUDE_MATH)

#  define Mk_E(type)            (cast_val(2.718281828459045235360287471352662498, type)) /* e */
#  define Mk_LOG2_E(type)       (cast_val(1.442695040888963407359924681001892137, type)) /* log_2 e */
#  define Mk_LOG10_E(type)      (cast_val(0.434294481903251827651128918916605082, type)) /* log_10 e */
#  define Mk_LN_2(type)         (cast_val(0.693147180559945309417232121458176568, type)) /* log_e 2 */
#  define Mk_LN_10(type)        (cast_val(2.302585092994045684017991454684364208, type)) /* log_e 10 */
#  define Mk_PI(type)           (cast_val(3.141592653589793238462643383279502884, type)) /* pi */
#  define Mk_PI_BY_2(type)      (cast_val(1.570796326794896619231321691639751442, type)) /* pi/2 */
#  define Mk_PI_BY_4(type)      (cast_val(0.785398163397448309615660845819875721, type)) /* pi/4 */
#  define Mk_1_BY_PI(type)      (cast_val(0.318309886183790671537767526745028724, type)) /* 1/pi */
#  define Mk_2_BY_PI(type)      (cast_val(0.636619772367581343075535053490057448, type)) /* 2/pi */
#  define Mk_2_BY_SQRT_PI(type) (cast_val(1.128379167095512573896158903121545172, type)) /* 2/sqrt(pi) */
#  define Mk_SQRT_2(type)       (cast_val(1.414213562373095048801688724209698079, type)) /* sqrt(2) */
#  define Mk_1_BY_SQRT_2(type)  (cast_val(0.707106781186547524400844362104849039, type)) /* 1/sqrt(2) */

// FIXME(naman): Make these functions not depend on libm

#  define mMin(x, y) ((x) < (y) ? (x) : (y))
#  define mMax(x, y) ((x) > (y) ? (x) : (y))

#  if defined(LANG_C) && LANG_C >= 2011
#   define mSqrt(x) mGeneric((x),               \
                             F32 : mSqrtF32     \
                             F64 : mSqrtF64     \
        )(x)
#  endif

header_function F32 mSqrtF32 (F32 x) { F32 y = sqrtf(x); return y; }
header_function F64 mSqrtF64 (F64 x) { F64 y = sqrt(x);  return y; }

#  if defined(LANG_C) && LANG_C >= 2011
#   define mPow2(x) mGeneric((x),               \
                             F32 : mPow2F32,    \
                             F64 : mPow2F64,    \
                             U32 : mPow2U32     \
                             U64 : mPow2U64     \
        )(x)
#  endif

header_function F32 mPow2F32 (F32 x) { F32 y = exp2f(x); return y; }
header_function F64 mPow2F64 (F64 x) { F64 y = exp2(x);  return y; }
header_function U32 mPow2U32 (U32 x) { U32 y = 1U << x;   return y; }
header_function U64 mPow2U64 (U64 x) { U64 y = 1ULL << x;   return y; }

#  if defined(LANG_C) && LANG_C >= 2011
#   define mLog2(x) mGeneric((x),               \
                             F32 : mLog2F32,    \
                             F64 : mLog2F64,    \
                             U32:  mLog2U32,    \
                             U64:  mLog2U64     \
        )(x)
#  endif

header_function F32 mLog2F32 (F32 x) { F32 y = log2f(x); return y; }
header_function F64 mLog2F64 (F64 x) { F64 y = log2(x);  return y; }
header_function U32 mLog2U32 (U32 x) { U32 y = x ? bitFindMSBU32(x) : 0; return y; }
header_function U64 mLog2U64 (U64 x) { U64 y = x ? bitFindMSBU64(x) : 0; return y; }

#  if defined(LANG_C) && LANG_C >= 2011
#   define mIsPowerOf2(x) mGeneric((x),                 \
                                   U32 : mIsPowerOf2U32 \
                                   U64 : mIsPowerOf2U64 \
        )(x)
#  endif

header_function Bool mIsPowerOf2U32 (U32 x) { B32 b = (x & (x - 1)) == 0; return b; }
header_function Bool mIsPowerOf2U64 (U64 x) { B64 b = (x & (x - 1)) == 0; return b; }

#  if defined(LANG_C) && LANG_C >= 2011
#   define mNextPowerOf2(x) mGeneric((x),                       \
                                     U32 : mNextPowerOf2U32     \
                                     U64 : mNextPowerOf2U64     \
        )(x)
#  endif

header_function U32 mNextPowerOf2U32 (U32 x) { U32 y = mIsPowerOf2U32(x) ? x : (1U << (mLog2U32(x) + 1U)); return y; }
header_function U64 mNextPowerOf2U64 (U64 x) { U64 y = mIsPowerOf2U64(x) ? x : (1LLU << (mLog2U64(x) + 1LLU)); return y; }

#  if defined(LANG_C) && LANG_C >= 2011
#   define mPrevPowerOf2(x) mGeneric((x),                       \
                                     U32 : mPrevPowerOf2U32     \
                                     U64 : mPrevPowerOf2U64     \
        )(x)
#  endif

header_function U32 mPrevPowerOf2U32 (U32 x) { U32 y = mIsPowerOf2U32(x) ? (1U << (mLog2U32(x) - 1U)) : x; return y; }
header_function U64 mPrevPowerOf2U64 (U64 x) { U64 y = mIsPowerOf2U64(x) ? (1LLU << (mLog2U64(x) - 1LLU)) : x; return y; }

#  if defined(LANG_C) && LANG_C >= 2011
#   define mSin(x) mGeneric((x),                \
                            F32 : mSinF32       \
                            F64 : mSinF64       \
        )(x)
#  endif

header_function F32 mSinF32 (F32 x) { F32 y = sinf(x); return y; }
header_function F64 mSinF64 (F64 x) { F64 y = sin(x);  return y; }

#  if defined(LANG_C) && LANG_C >= 2011
#   define mCos(x) mGeneric((x),                \
                            F32 : mCosF32       \
                            F64 : mCosF64       \
        )(x)
#  endif

header_function F32 mCosF32 (F32 x) { F32 y = cosf(x); return y; }
header_function F64 mCosF64 (F64 x) { F64 y = cos(x);  return y; }

#  if defined(LANG_C) && LANG_C >= 2011
#   define mTan(x) mGeneric((x),                \
                            F32 : mTanF32       \
                            F64 : mTanF64       \
        )(x)
#  endif

header_function F32 mTanF32 (F32 x) { F32 y = tanf(x); return y; }
header_function F64 mTanF64 (F64 x) { F64 y = tan(x);  return y; }

#  if defined(LANG_C) && LANG_C >= 2011
#   define mRadians(x) mGeneric((x),                    \
                                F32 : mRadiansF64       \
                                F64 : mRadiansF64       \
        )(x)
#  endif

header_function F32 mRadiansF32 (F32 degrees) { F32 radians = (degrees * Mk_PI(F32)) / 180.0f; return radians; }
header_function F64 mRadiansF64 (F64 degrees) { F64 radians = (degrees * Mk_PI(F64)) / 180.0;  return radians; }

#  if defined(LANG_C) && LANG_C >= 2011
#   define mDegrees(x) mGeneric((x),                    \
                                F32 : mDegreesF32       \
                                F64 : mDegreesF64       \
        )(x)
#  endif

header_function F32 mDegreesF32 (F32 radians) { F32 degrees = (radians * 180.0f) / Mk_PI(F32); return degrees; }
header_function F64 mDegreesF64 (F64 radians) { F64 degrees = (radians * 180.0)  / Mk_PI(F64); return degrees; }

# endif // NLIB_EXCLUDE_MATH


/* ===============================
 * @Random Number Generator
 */

# if !defined(NLIB_EXCLUDE_RANDOM)

#  if defined(COMPILER_MSVC)

/* Linear Congruential Generator
 *
 * If x is the last random number,
 *    m is a number greater than zero that is a power of two,
 *    a is a number between 0 and m,
 * then the next random number is ((x * a) % m).
 *
 * Unfortunately, the lower bits don't have enought randomness in them. The LSB doesn't
 * change at all, the second LSB alternates, the one after that toggles every 2 turns and so
 * on. Therefore, we try to get rid of the LSBs by pulling in some MSBs.
 *
 * NOTE(naman): Seed should be an odd number or the randomness might drop drastically.
 * NOTE(naman): "a" should be equal to 5(mod 8) or 3(mod 8).
 */

header_function
U64 randomU64 (U64 seed)
{
    U64 previous = seed;
    if (previous == 0) {
        // This seed has been tested and should be preferred in normal circumstances.
        previous = 2531011ULL;
    }

    U64 a = 214013ULL;

    U64 upper = 0, lower = 0;
    lower = _umul128(previous, a, &upper);
    U64 log_upper = upper ? mLog2U64(upper) : 0;
    U64 shift_amount = 64 - (log_upper + 1);
    upper = (upper << shift_amount) | (lower >> log_upper);

    U64 result = upper * a;

    return result;
}

#  elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)

/* Linear Congruential Generator
 *
 * If x is the last random number,
 *    m is a number greater than zero that is a power of two,
 *    a is a number between 0 and m,
 * then the next random number is ((x * a) % m).
 *
 * Unfortunately, the lower bits don't have enought randomness in them. The LSB doesn't
 * change at all, the second LSB alternates, the one after that toggles every 2 turns and so
 * on. Therefore, we try to get rid of the LSBs by pulling in some MSBs.
 *
 * We do the multiplcation twice because Chi-Square Test indicated that this method
 * gives better randomness. Don't ask.
 *
 * NOTE(naman): Seed should be an odd number or the randomness might drop drastically.
 * NOTE(naman): "a" should be equal to 5(mod 8) or 3(mod 8).
 */

header_function
U64 randomU64 (U64 seed)
{
    U64 previous = seed;
    if (previous == 0) {
        // This seed has been tested and should be preferred in normal circumstances.
        previous = 2531011ULL;
    }

    U64 a = 214013ULL;

    __uint128_t product = cast_val(previous, __uint128_t) * cast_val(a, __uint128_t);
    U64 upper = product >> 64, lower = cast_val(product, U64);
    U64 log_upper = upper ? mLog2U64(upper) : 0;
    U64 shift_amount = 64 - (log_upper + 1);
    upper = (upper << shift_amount) | (lower >> log_upper);

    U64 result = upper * a;

    return result;
}

#  endif

# endif // NLIB_EXCLUDE_RANDOM

/* ==========================
 * @Hashing Infrastructure
 */

# if !defined(NLIB_EXCLUDE_HASHING)

// TODO(naman): Add hash collision detection

// FNV-1a
header_function
U64 hashString (Char *str)
{
    U64 hash = 0xCBF29CE484222325ULL; // FNV_offset_basis
    for (Size i = 0; str[i] != '\0'; i++) {
        hash = hash ^ cast_val(str[i], U64);
        hash = hash * 0x100000001B3ULL; // FNV_prime
    }

    claim(hash != 0);
    return hash;
}

// splitmix64 (xoshiro.di.unimi.it/splitmix64.c)
header_function
U64 hashInteger(U64 x)
{
    x ^= x >> 30;
    x *= 0xBF58476D1CE4E5B9ULL;
    x ^= x >> 27;
    x *= 0x94D049BB133111EBULL;
    x ^= x >> 31;

    claim(x != 0);
    return x;
}

/* Universal Hashing: en.wikipedia.org/wiki/Universal_hashing#Avoiding_modular_arithmetic
 *
 * NOTE(naman): Implementation notes
 * w is number of bits in machine word (64 in our case)
 * s is the number of buckets/bins (slots in the hash table) to which the
 *   universe of hashable objects is to be mapped
 * m is log2(s) (=> m = 2^s) and is equal to the number of bits in the final hash
 * a is a random odd positive integer < 2^w (fitting in w bits)
 * b is a random non-negative integer < 2^(w-m) (fitting in (w-m) bits)
 * en.wikipedia.org/wiki/Universal_hashing#Avoiding_modular_arithmetic
 *
 * r is the last random number generated and is just an implementation detail.
 */

typedef struct Hash_Universal {
    U64 a, b, m; /* Hashing constants */

    U64 r; /* Last random number for Universal Hashing */
} Hash_Universal;

header_function
void hashUniversalConstantsUpdate (Hash_Universal *h)
{
    do {
        h->r = randomU64(h->r);
        h->a = h->r;
    } while ((h->a == 0) || ((h->a > 0) && ((h->a & 0x01) != 0x01))); // Make sure that 'a' is odd

    h->r = randomU64(h->r);
    // b should be (64 - m) bits long
    h->b = h->r & (0xFFFFFFFFFFFFFFFFULL >> h->m);
}

header_function
U64 hashUniversal (Hash_Universal h, U64 key)
{
    // NOTE(naman): Remember that we don't want 64-bit hashes, we want hashes < 2^m (s)
    U64 result =  ((h.a * key) + h.b) >> (64 - h.m);
    return result;
}

# endif // NLIB_EXCLUDE_HASHING



/* =========================================================
 * Data Structures =========================================
 * =========================================================
 */

/* ==============
 * RA (array abstraction)
 */

/* API ----------------------------------------
 * ra(T)
 * T* raCreate           (ra(T) ptr)
 * T* raCreateSized      (ra(T) ptr, Size min_capacity)
 * T* raCreateAlloc      (ra(T) ptr, Memory_Allocator allocator)
 * T* raCreateAllocSized (ra(T) ptr, Size min_capacity, Memory_Allocator allocator)
 *
 * void  raDelete      (ra(T) ptr)
 *
 * Size  raAdd         (ra(T)ptr, T elem)
 * void  raClear       (ra(T)ptr) : Make everything zero
 * Size  raResize      (ra(T)ptr, Size new_size) : Increase size of the underlying buffer
 *
 * Size  raSizeof      (ra(T)ptr)
 * Size  raElemin      (ra(T)ptr)
 * Size  raMaxSizeof   (ra(T)ptr)
 * Size  raMaxElemin   (ra(T)ptr)
 *
 * T*    raLast        (ra(T)ptr) : Return the pointer to last filled element
 * T*    raOnePastLast (ra(T)ptr) : Return the pointer to the first empty element
 *
 * Size  raUnsortedRemove (T* ptr, Size index)
 */

// TODO(naman): Add support for stack-based arrays

# if defined(LANG_C) && !defined(NLIB_EXCLUDE_RA)

typedef struct Ra_Header {
    Size cap; // NOTE(naman): Maximum number of elements that can be stored
    Size len; // NOTE(naman): Count of elements actually stored
    void *userdata;
    Memory_Allocator allocator;
    Byte pad_[8];
    alignas(alignof(max_align_t)) Byte buf[];
} Ra_Header;

#  define ra_GetHeader(sb) ((Ra_Header*)(void*)((Byte*)(sb) - offsetof(Ra_Header, buf)))

#  define ra_Len(sb)          (ra_GetHeader(sb)->len)
#  define ra_Cap(sb)          (ra_GetHeader(sb)->cap)

#  define ra_IsFull(sb)       ((ra_Len(sb)+1) > ra_Cap(sb))
#  define ra_IsNULL(sb)       ((sb) == NLIB_NULL)

#  define ra_CheckAndGrow(sb) (ra_IsFull(sb) ? ra_Grow((sb), sizeof(*(sb))) : (sb))

#  define ra(T) T *

#  define raCreate(var)                           ra_Create(sizeof(*(var)),  0,       memHeapGet())
#  define raCreateSized(var, min_cap)             ra_Create(sizeof(*(var)),  min_cap, memHeapGet())
#  define raCreateAlloc(var, alloc)               ra_Create(sizeof(*(var)),  0,       alloc)
#  define raCreateAllocSized(var, min_cap, alloc) ra_Create(sizeof(*(var)),  min_cap, alloc)

#  define raDelete(sb)      (ra_Delete((sb), sizeof(*sb)), (sb) = NLIB_NULL)

#  define raAdd(sb, ...)    ((sb) = ra_CheckAndGrow(sb),        \
                             (sb)[ra_Len(sb)] = (__VA_ARGS__),  \
                             ((ra_GetHeader(sb))->len)++)

# define raRemoveUnsorted(sb, i) (((sb)[(i)] = (sb)[ra_Len(sb) - 1]),   \
                                  ((ra_GetHeader(sb)->len)--))

#  define raClear(sb)       (memset((sb), 0, raSizeof(sb)), ra_GetHeader(sb)->len = 0)

#  define raResize(sb, n)   ((n) > ra_Cap(sb) ?                         \
                             ((sb) = ra_GrowToSize(sb, n, sizeof(*(sb)))) : \
                             sb)

#  define raSizeof(sb)       (ra_Len(sb) * sizeof(*(sb)))
#  define raElemin(sb)       (ra_Len(sb))
#  define raMaxSizeof(sb)    (ra_Cap(sb) * sizeof(*(sb)))
#  define raMaxElemin(sb)    (ra_Cap(sb))

#  define raPtr(sb)          (sb)
#  define raOnePastLast(sb)  ((sb) + ra_Len(sb))
#  define raLast(sb)         ((sb) + (ra_Len(sb) - 1))



header_function
void* ra_Create (Size elem_size, Size min_cap, Memory_Allocator allocator)
{
    Size new_cap = (min_cap != 0) ? min_cap : 16;
    Size new_size = (new_cap * elem_size) + memAlignUp(sizeof(Ra_Header));

    Ra_Header *new_header = (Ra_Header *)memAlloc(allocator, new_size);
    *new_header = (Ra_Header)NLIB_ZERO_INIT_LIST;

    new_header->cap = new_cap;
    new_header->allocator = allocator;

    return new_header->buf;
}

header_function
void ra_Delete (void *buf, Size elem_size)
{
    Memory_Allocator allocator = ra_GetHeader(buf)->allocator;
    Size old_size = (raMaxElemin(buf) * elem_size) + memAlignUp(sizeof(Ra_Header));

    memDealloc(allocator, ra_GetHeader(buf), old_size);

    return;
}

header_function
void* ra_GrowToSize (void *buf, Size elem_count, Size elem_size)
{
    Memory_Allocator allocator = ra_GetHeader(buf)->allocator;
    Size old_size = (raMaxElemin(buf) * elem_size) + memAlignUp(sizeof(Ra_Header));

    Size new_cap = mMax(elem_count, 16U);
    Size new_size = (new_cap * elem_size) + memAlignUp(sizeof(Ra_Header));
    Ra_Header *new_header = NLIB_NULL;

    new_header = (Ra_Header *)memRealloc(allocator, ra_GetHeader(buf), new_size, old_size);

    new_header->cap = new_cap;
    new_header->allocator = allocator;

    return new_header->buf;
}

header_function
void* ra_Grow (void *buf, Size elem_size)
{
    Size new_cap = mMax(2 * ra_Cap(buf), 16U);
    void *result = ra_GrowToSize(buf, new_cap, elem_size);
    return result;
}

# elif defined(LANG_CPP) && !defined(NLIB_EXCLUDE_RA)

template <typename T>
struct Ra_Struct {
    Size cap; // NOTE(naman): Maximum number of elements that can be stored
    Size len; // NOTE(naman): Count of elements actually stored
    Memory_Allocator allocator;
    T *buf;

    void InsertElementAtIndexDirectlyWithoutUpdatingMetaData_ (Size index, T elem) {
        buf[index] = elem;
    }

    Bool IsTheVariablePointingToAllocatedBufferEqualToNULL_ (void) {
        return buf == NLIB_NULL;
    }

    T operator[] (Size index) const {
        T result = buf[index];
        return result;
    }

    Bool IsFull_ (void) {
        return len + 1 > cap;
    }

    void Add (T elem) {
        if (unlikely(IsFull_())) Grow();
        buf[len] = elem;
        len++;
    }

    void RemoveUnsorted (Size index) {
        buf[index] = buf[len - 1];
        len--;
    }

    void Clear (void) {
        memset(buf, 0, len * sizeof(T));
        len = 0;
    }

    void Resize (Size new_count) {
        if (new_count > cap) {
            GrowToSize(new_count);
        }
    }

    Size Sizeof    (void) { return len * sizeof(T); }
    Size Elemin    (void) { return len;             }
    Size MaxSizeof (void) { return cap * sizeof(T); }
    Size MaxElemin (void) { return len * sizeof(T); }

    T* OnePastLast (void) {
        return buf + len;
    }

    T* Last (void) {
        return buf + (len - 1);
    }

    void GrowToSize (Size elem_count) {
        Size old_size = MaxSizeof();

        Size new_cap = mMax(elem_count, 16U);
        Size new_size = new_cap * sizeof(T);

        buf = cast_val(memRealloc(allocator, buf, new_size, old_size), T *);

        cap = new_cap;
    }

    void Grow (void) {
        Size new_cap = mMax(2 * cap, 16U);
        GrowToSize(new_cap);
    }
};

#  define ra(T) Ra_Struct<T>

#  define ra_IsNULL(sb)       ((sb).IsTheVariablePointingToAllocatedBufferEqualToNULL_())

template <typename T>
header_function
ra(T) ra_Create (ra(T) *, Size min_cap, Memory_Allocator allocator)
{
    ra(T) result = NLIB_ZERO_INIT_LIST;

    Size new_cap = (min_cap != 0) ? min_cap : 16;
    Size new_size = (new_cap * sizeof(T));

    T *new_buffer = cast_val(memAlloc(allocator, new_size), T *);

    result.cap = new_cap;
    result.allocator = allocator;
    result.buf = new_buffer;

    return result;
}

#  define raCreate(var)                           ra_Create(&var, 0,       memHeapGet())
#  define raCreateSized(var, min_cap)             ra_Create(&var, min_cap, memHeapGet())
#  define raCreateAlloc(var, alloc)               ra_Create(&var, 0,       alloc)
#  define raCreateAllocSized(var, min_cap, alloc) ra_Create(&var, min_cap, alloc)

template <typename T>
header_function
void ra_Delete (ra(T) &sb)
{
    Size old_size = sb.MaxSizeof();
    memDealloc(sb.allocator, sb.buf, old_size);
    sb.buf = NLIB_NULL;
    sb.~ra(T)();
    return;
}

#  define raDelete(sb)      ra_Delete(sb)

#  define raAdd(sb, ...)    ((sb).Add(__VA_ARGS__))

# define raRemoveUnsorted(sb, i) ((sb).RemoveUnsorted(i))

#  define raClear(sb)       ((sb).Clear())

#  define raResize(sb, n)   ((sb).Resize(n))

#  define raSizeof(sb)       ((sb).Sizeof())
#  define raElemin(sb)       ((sb).Elemin())
#  define raMaxSizeof(sb)    ((sb).MaxSizeof())
#  define raMaxElemin(sb)    ((sb).MaxElemin())

#  define raPtr(sb)          ((sb).buf)
#  define raOnePastLast(sb)  ((sb).OnePastLast())
#  define raLast(sb)         ((sb).Last())


# endif // !defined(NLIB_EXCLUDE_RA)


/* ===============================
 * Intrusive Circular Doubly Linked List
 * Inspired from github.com/torvalds/linux/blob/master/include/linux/list.h
 */

# if defined(LANG_C) && !defined(NLIB_EXCLUDE_LIST)
typedef struct List_Node {
    struct List_Node *next, *prev;
} List_Node;

/* To get the node (container struct) holding the linked list node */
#  define listThisNode(node, type, member) containerof(&node,      type, member)
#  define listNextNode(node, type, member) containerof(node->next, type, member)
#  define listPrevNode(node, type, member) containerof(node->prev, type, member)

/* Create and initialize the node statically
 *
 * node n = {
 *     .data = 40,
 *     .list = listNodeInitCompound(n.list),
 * };
 */
#  define listNodeInitCompound(name) {&(name), &(name)}

/* Initialize the node only
 *
 * node *n = malloc(sizeof(*n));
 * n->data = 40;
 * listNodeInit(&n->list);
 */
#  define listNodeInit(ptr) do{(ptr)->next = (ptr); (ptr)->prev = (ptr);}while(0)

header_function
void list_Add (List_Node *new, List_Node *prev, List_Node *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

header_function
void listAddAfter (List_Node *new, List_Node *after_this)
{
    list_Add(new, after_this, after_this->next);
}

header_function
void listAddBefore (List_Node *new, List_Node *before_this)
{
    list_Add(new, before_this->prev, before_this);
}

header_function
void list_RemoveNodeBetween (List_Node * prev, List_Node * next)
{
    next->prev = prev;
    prev->next = next;
}

header_function
void listRemove (List_Node *entry)
{
    list_RemoveNodeBetween(entry->prev, entry->next);
    entry->next = NLIB_NULL;
    entry->prev = NLIB_NULL;
}

header_function
void listRemoveAndInit (List_Node *entry)
{
    list_RemoveNodeBetween(entry->prev, entry->next);
    listNodeInit(entry);
}

header_function
void listReplace(List_Node *old, List_Node *new)
{
    new->next = old->next;
    new->next->prev = new;
    new->prev = old->prev;
    new->prev->next = new;
}

header_function
void listReplaceAndInit(List_Node *old, List_Node *new)
{
    listReplace(old, new);
    listNodeInit(old);
}

header_function
void listSwap(List_Node *entry1, List_Node *entry2)
{
    List_Node *pos = entry2->prev;

    listRemove(entry2);
    listReplace(entry1, entry2);
    if (pos == entry1) pos = entry2;
    listAddAfter(entry1, pos);
}

header_function
void listMoveAfter (List_Node *list, List_Node *after_this)
{
    list_RemoveNodeBetween(list->prev, list->next);
    listAddAfter(list, after_this);
}

header_function
void listMoveBefore (List_Node *list, List_Node *before_this)
{
    list_RemoveNodeBetween(list->prev, list->next);
    listAddBefore(list, before_this);
}

header_function
Bool listIsEmpty (List_Node *node)
{
    Bool result = (node->next == node);
    return result;
}

// Splice in a List list, between the Nodes node and node->next
header_function
void list_Splice (List_Node *list, List_Node *node)
{
    List_Node *first = list->next;
    List_Node *last = list->prev;
    List_Node *at = node->next;

    first->prev = node;
    node->next = first;

    last->next = at;
    at->prev = last;
}

// Splice in a List list, between the Nodes node and node->next
header_function
void listSplice (List_Node *list, List_Node *node)
{
    if (!listIsEmpty(list)) list_Splice(list, node);
}

header_function
void listSpliceInit (List_Node *list, List_Node *node)
{
    if (!listIsEmpty(list)) {
        list_Splice(list, node);
        listNodeInit(list);
    }
}

# define LIST_LOOP(idx, node)                                           \
    for (List_Node *(idx) = (node)->next; (idx) != (node); (idx) = (idx)->next)
# define LIST_LOOP_REVERSE(idx, node)                                   \
    for (List_Node *(idx) = (node)->prev; (idx) != (node); pos = pos->prev)
# endif // defined(LANG_C)


/* ==============
 * Interning
 */

# if !defined(NLIB_EXCLUDE_INTERN)
#  define INTERN_EQUALITY(func_name) Bool func_name (void *a, void *b, Size b_index)
typedef INTERN_EQUALITY(Intern_Equality_Function);

typedef struct Intern {
    struct Intern_List {
        ra(Size) indices;
        ra(U8) secondary_hashes;
    } lists[256];
    Memory_Allocator allocator;
} Intern;

header_function
Intern internCreateAlloc (Memory_Allocator allocator)
{
    Intern it = NLIB_ZERO_INIT_LIST;

    for (Size i = 0; i < elemin(it.lists); i++) {
        it.lists[i].indices = raCreateAlloc(it.lists[i].indices, allocator);
        it.lists[i].secondary_hashes = raCreateAlloc(it.lists[i].secondary_hashes, allocator);
    }

    it.allocator = allocator;

    return it;
}

header_function
Intern internCreate (void)
{
    return internCreateAlloc(memHeapGet());
}


header_function
Bool internCheck (Intern *it, U8 hash1, U8 hash2,
                  void *datum, void *data, Intern_Equality_Function *eqf,
                  Size *result)
{
    if (ra_IsNULL(it->lists[hash1].secondary_hashes) == false) {
        // Our data has probably been inserted already.
        // (or atleast some data with same hash has been inserted :)
        for (Size i = 0;
             i < raElemin(it->lists[hash1].secondary_hashes);
             i++) {
            Size index = it->lists[hash1].indices[i];
            if ((it->lists[hash1].secondary_hashes[i] == hash2) && eqf(datum, data, index)) {
                // This is our data, return it
                if (result != NLIB_NULL) {
                    *result = index;
                }
                return true;
            }
        }

        return false;
    } else {
        return false;
    }
}

header_function
void internData (Intern *it, U8 hash1, U8 hash2, Size index)
{
    raAdd(it->lists[hash1].secondary_hashes, hash2);
    raAdd(it->lists[hash1].indices, index);
    claim(raElemin(it->lists[hash1].secondary_hashes) == raElemin(it->lists[hash1].indices));
}

header_function
U8 internStringPearsonHash (void *buffer, Size len, Bool which)
{
    // NOTE(naman): Pearson's hash for 8-bit hashing
    // en.wikipedia.org/wiki/Pearson_hashing
    persistent_value U8 hash_lookup_table1[256] =
        {
            // 0-255 shuffled in any (random) order suffices
            98,    6,   85, 150,  36,  23, 112, 164, 135, 207, 169,   5,  26,  64, 165, 219, // 01
            61,   20,   68,  89, 130,  63,  52, 102,  24, 229, 132, 245,  80, 216, 195, 115, // 02
            90,  168,  156, 203, 177, 120,   2, 190, 188,   7, 100, 185, 174, 243, 162,  10, // 03
            237,  18,  253, 225,   8, 208, 172, 244, 255, 126, 101,  79, 145, 235, 228, 121, // 04
            123, 251,   67, 250, 161,   0, 107,  97, 241, 111, 181,  82, 249,  33,  69,  55, // 05
            59,  153,   29,   9, 213, 167,  84,  93,  30,  46, 94,   75, 151, 114,  73, 222, // 06
            197,  96,  210,  45,  16, 227, 248, 202,  51, 152, 252, 125,  81, 206, 215, 186, // 07
            39,  158,  178, 187, 131, 136,   1,  49,  50,  17, 141,  91,  47, 129,  60,  99, // 08
            154,  35,   86, 171, 105,  34,  38, 200, 147,  58,  77, 118, 173, 246,  76, 254, // 09
            133, 232,  196, 144, 198, 124,  53,   4, 108,  74, 223, 234, 134, 230, 157, 139, // 10
            189, 205,  199, 128, 176,  19, 211, 236, 127, 192, 231,  70, 233,  88, 146,  44, // 11
            183, 201,   22,  83,  13, 214, 116, 109, 159,  32,  95, 226, 140, 220,  57,  12, // 12
            221,  31,  209, 182, 143,  92, 149, 184, 148,  62, 113,  65,  37,  27, 106, 166, // 13
            3,    14,  204,  72,  21,  41,  56,  66,  28, 193,  40, 217,  25,  54, 179, 117, // 14
            238,  87,  240, 155, 180, 170, 242, 212, 191, 163,  78, 218, 137, 194, 175, 110, // 15
            43,  119,  224,  71, 122, 142,  42, 160, 104,  48, 247, 103,  15,  11, 138, 239, // 16
        };

    persistent_value U8 hash_lookup_table2[256] =
        {
            251, 175, 119, 215,  81,  14,  79, 191, 103,  49, 181, 143, 186, 157,   0, 232, // 01
            31,   32,  55,  60, 152,  58,  17, 237, 174,  70, 160, 144, 220,  90,  57, 223, // 02
            59,    3,  18, 140, 111, 166, 203, 196, 134, 243, 124,  95, 222, 179, 197,  65, // 03
            180,  48,  36,  15, 107,  46, 233, 130, 165,  30, 123, 161, 209,  23,  97,  16, // 04
            40,   91, 219,  61, 100,  10, 210, 109, 250, 127,  22, 138,  29, 108, 244,  67, // 05
            207,   9, 178, 204,  74,  98, 126, 249, 167, 116,  34,  77, 193, 200, 121,   5, // 06
            20,  113,  71,  35, 128,  13, 182,  94,  25, 226, 227, 199,  75,  27,  41, 245, // 07
            230, 224,  43, 225, 177,  26, 155, 150, 212, 142, 218, 115, 241,  73,  88, 105, // 08
            39,  114,  62, 255, 192, 201, 145, 214, 168, 158, 221, 148, 154, 122,  12,  84, // 09
            82,  163,  44, 139, 228, 236, 205, 242, 217,  11, 187, 146, 159,  64,  86, 239, // 10
            195,  42, 106, 198, 118, 112, 184, 172,  87,   2, 173, 117, 176, 229, 247, 253, // 11
            137, 185,  99, 164, 102, 147,  45,  66, 231,  52, 141, 211, 194, 206, 246, 238, // 12
            56,  110,  78, 248,  63, 240, 189,  93,  92,  51,  53, 183,  19, 171,  72,  50, // 13
            33,  104, 101,  69,   8, 252,  83, 120,  76, 135,  85,  54, 202, 125, 188, 213, // 14
            96,  235, 136, 208, 162, 129, 190, 132, 156,  38,  47,   1,   7, 254,  24,   4, // 15
            216, 131,  89,  21,  28, 133,  37, 153, 149,  80, 170,  68,   6, 169, 234, 151, // 16
        };

    Char *string = cast_val(buffer, Char*);

    U8 hash = cast_val(len, U8);
    for (Size i = 0; i < len; i++) {
        if (which == true) {
            hash = hash_lookup_table1[hash ^ string[i]];
        } else {
            hash = hash_lookup_table2[hash ^ string[i]];
        }
    }

    return hash;
}

typedef struct Intern_String {
    Intern intern;
    ra(ra(Char)) strings;
} Intern_String;

header_function
INTERN_EQUALITY(internStringEquality) {
    Char *sa = cast_val(a, Char*);
    ra(Char) *ss = cast_val(b, ra(Char) *);
    ra(Char) sb = ss[b_index];
    Bool result = streq(sa, raPtr(sb));
    return result;
}

header_function
Intern_String internStringCreateAlloc (Memory_Allocator allocator)
{
    Intern_String its = NLIB_ZERO_INIT_LIST;
    its.intern = internCreateAlloc(allocator);
    its.strings = raCreateAlloc(its.strings, allocator);

    return its;
}

header_function
Intern_String internStringCreate (void)
{
    return internStringCreateAlloc(memHeapGet());
}

header_function
Char *internString (Intern_String *is, Char *str)
{
    U8 hash1 = internStringPearsonHash(str, strlen(str), true);
    U8 hash2 = internStringPearsonHash(str, strlen(str), false);

    Size index = 0;

    if (internCheck(&is->intern, hash1, hash2,
                    str, raPtr(is->strings), internStringEquality, &index)) {
        Char *result = raPtr(is->strings[index]);
        return result;
    } else {
        Size index_new = raElemin(is->strings);

        ra(Char) str_new = raCreateAlloc(str_new, is->intern.allocator);
        for (Char *s = str; s[0] != '\0'; s++) {
            raAdd(str_new, s[0]);
        }
        raAdd(str_new, '\0');

        raAdd(is->strings, str_new);

        internData(&is->intern, hash1, hash2, index_new);
        Char *result = raPtr(is->strings[index_new]);
        return result;
    }
}

header_function
Char *internStringCheck (Intern_String *is, Char *str)
{
    U8 hash1 = internStringPearsonHash(str, strlen(str), true);
    U8 hash2 = internStringPearsonHash(str, strlen(str), false);

    Size index = 0;

    if (internCheck(&is->intern, hash1, hash2,
                    str, raPtr(is->strings), &internStringEquality, &index)) {
        Char *result = raPtr(is->strings[index]);
        return result;
    } else {
        return NLIB_NULL;
    }
}

#  if defined(BUILD_DEBUG)

header_function
void internStringDebugPrint (Intern_String *is)
{
    for (Size i = 0; i < elemin(is->intern.lists); i++) {
        for (Size j = 0; j < raElemin(is->intern.lists[i].indices); j++) {
            printErr(raPtr(is->strings[is->intern.lists[i].indices[j]]));
            printErr("\n");
        }
    }
}

#  endif

typedef struct Intern_Integer {
    Intern intern;
    ra(U64) integers;
} Intern_Integer;

header_function
Intern_Integer internIntegerCreateAlloc (Memory_Allocator allocator)
{
    Intern_Integer iti = NLIB_ZERO_INIT_LIST;
    iti.intern = internCreateAlloc(allocator);
    iti.integers = raCreateAlloc(iti.integers, allocator);

    return iti;
}


header_function
Intern_Integer internIntegerCreate (void)
{
    return internIntegerCreateAlloc(memHeapGet());
}

header_function
INTERN_EQUALITY(internIntegerEquality) {
    U64 ia = (cast_val(a, U64*))[0];
    U64 ib = (cast_val(b, U64*))[b_index];

    Bool result = (ia == ib);
    return result;
}

// NOTE(naman): stackoverflow.com/a/8546542
header_function
U8 internIntegerHash8Bit (U64 key, Bool which)
{
    U8 result = 0;
    U64 q = 0;

    // NOTE(naman): Both q are prime.
    if (which == true) {
        q = 33149;
    } else {
        q = 146519;
    }

    U64 *key_addr = &key;
    Byte *b = cast_mem(key_addr, Byte*);
    for (Size i = 0; i < sizeof(key); i++) {
        result += cast_val((cast_val(b[i], U64) * q), U8);
    }

    return result;
}

header_function
U64 internInteger (Intern_Integer *ii, U64 num)
{
    U8 hash1 = internIntegerHash8Bit(num, true);
    U8 hash2 = internIntegerHash8Bit(num, false);

    U64 num_copy = num; // Just in case
    Size index = 0;

    if (internCheck(&ii->intern, hash1, hash2,
                    &num_copy, raPtr(ii->integers), &internIntegerEquality, &index)) {
        return num;
    } else {
        raAdd(ii->integers, num);
        internData(&ii->intern, hash1, hash2, raElemin(ii->integers));
        return num;
    }
}

header_function
U64 internIntegerCheck (Intern_Integer *ii, U64 num)
{
    U8 hash1 = internIntegerHash8Bit(num, true);
    U8 hash2 = internIntegerHash8Bit(num, false);

    U64 num_copy = num; // Just in case
    Size index = 0;

    if (internCheck(&ii->intern, hash1, hash2,
                    &num_copy, raPtr(ii->integers), &internIntegerEquality, &index)) {
        return num;
    } else {
        return false;
    }
}
# endif // NLIB_EXCLUDE_INTERN

/* ==============
 * Hash Table
 */

/* API ---------------------------------------------------
 * NOTE(naman): Zero key and value are considered invalid.
 *
 * Hash_Table htCreate (Size min_slots);
 * void       htDelete (Hash_Table ht);
 * U64        htInsert (Hash_Table *ht, U64 key, U64 value);
 * U64        htLookup (Hash_Table *ht, U64 key);
 * U64        htRemove (Hash_Table *ht, U64 key);
 */

# if !defined(NLIB_EXCLUDE_HASH_TABLE)

typedef struct Hash_Table {
    Hash_Universal univ;
    Memory_Allocator allocator;
    U64 *keys;
    U64 *values;
    Size slot_count;
    Size slot_filled;
    U64 collision_count;
} Hash_Table;

#  define htCreate()                           ht_Create(0,         memHeapGet())
#  define htCreateSlots(min_slots)             ht_Create(min_slots, memHeapGet())
#  define htCreateAlloc(alloc)                 ht_Create(0,         alloc)
#  define htCreateSlotsAlloc(min_slots, alloc) ht_Create(min_slots, alloc)

header_function
Hash_Table ht_Create (Size slots_atleast, Memory_Allocator allocator)
{
    Hash_Table ht = NLIB_ZERO_INIT_LIST;

    // NOTE(naman): We try to make the initial hash table a bit larger than expected.
    // The reason for this is that if we have only a small amount of elements, we would
    // just use a associative array instead of a hash table to do the lookup.
    Size slots = mMax(slots_atleast, 64U);
    ht.slot_count = mNextPowerOf2U64(slots);

    ht.univ.m = mLog2U64(ht.slot_count);
    hashUniversalConstantsUpdate(&ht.univ);

    ht.allocator = allocator;

    ht.keys     = cast_val(memAlloc(ht.allocator, ht.slot_count * sizeof(*(ht.keys))),   U64*);
    ht.values   = cast_val(memAlloc(ht.allocator, ht.slot_count * sizeof(*(ht.values))), U64*);

    memset(ht.keys,   0, ht.slot_count * sizeof(*(ht.keys)));
    memset(ht.values, 0, ht.slot_count * sizeof(*(ht.values)));

    return ht;
}

header_function
void htDelete (Hash_Table ht)
{
    Memory_Allocator allocator = ht.allocator;

    memDealloc(allocator, ht.keys, ht.slot_count * sizeof(*(ht.keys)));
    memDealloc(allocator, ht.values, ht.slot_count * sizeof(*(ht.values)));

    return;
}

header_function
Size ht_GetFilledSlot (Hash_Table *ht, Size *last_index_ptr,
                       U64 *key_out, U64 *value_out)
{
    Bool found = 0;
    Size index = 0;

    Size last_index = 0;
    if (last_index_ptr) last_index = (*last_index_ptr) + 1;

    for (Size i = last_index; i < ht->slot_count; ++i) {
        U64 key = ht->keys[i];
        if (key != 0) {
            found = true;
            index = i;

            U64 value = ht->values[i];
            *key_out = key;
            *value_out = value;

            break;
        }
    }

    if (found) {
        return index;
    } else {
        *key_out = 0;
        *value_out = 0;

        return 0;
    }
}

#define ht_ForEachWithStmt(arg_ht, arg_key, arg_val, arg_code_stmt)     \
    goto gensym_line(jump_to_else);                                     \
                                                                        \
    while (true)                                                        \
        if (true) {                                                     \
            /* This block executes after the following code block */    \
            break;                                                      \
        } else gensym_line(jump_to_else):                               \
            for (Size gensym_line(ht_i) = ht_GetFilledSlot((arg_ht),    \
                                                           NLIB_NULL,   \
                                                           &(arg_key),  \
                                                           &(arg_val)); \
                 (arg_code_stmt), (arg_key) != 0;                       \
                 gensym_line(ht_i) = ht_GetFilledSlot((arg_ht),         \
                                                      &(gensym_line(ht_i)), \
                                                      &(arg_key),       \
                                                      &(arg_val)))

#define htForEach(arg_ht, arg_key, arg_val) ht_ForEachWithStmt(arg_ht, arg_key, arg_val, (void)arg_ht)

header_function
Bool ht_LinearProbeSearch (Hash_Table *ht, U64 key, U64 hash, Size *value_index)
{
    Size index = 0;
    Bool found = false;

    for (Size i = 0; !found && (i < ht->slot_count); ++i) {
        index = (hash + i) % (ht->slot_count);
        if (ht->keys[index] == key) {
            found = true;
            break;
        }
    }

    *value_index = index;

    return found;
}

header_function
U64 ht_LinearProbeInsertion (Hash_Table *ht,
                             U64 hash, U64 key, U64 value)
{
    U64 result_value = value;

    for (Size i = 0; i < ht->slot_count; ++i) {
        Size index = (hash + i) % (ht->slot_count);
        if ((ht->keys[index] == key) || (ht->values[index] == 0))  {
            result_value = ht->values[index];
            ht->keys[index] = key;
            ht->values[index] = value;
            break;
        } else {
            ht->collision_count++;
        }
    }

    return result_value;
}

header_function U64 htRemove (Hash_Table *ht, U64 key); // Forward Declaration

header_function
U64 htInsert (Hash_Table *ht, U64 key, U64 value)
{
    // FIXME(naman): Figure out the correct condition on which to resize on.
    const Uint max_collisions_allowed = cast_val(ht->slot_count, Uint);

    if (key == 0) {
        return 0;
    } else if (value == 0) {
        U64 result = htRemove(ht, key);
        return result;
    }

    if ((ht->collision_count > max_collisions_allowed) ||
        ((2 * ht->slot_filled) >= ht->slot_count)) {
        Size slot_count_prev = ht->slot_count;
        U64 *keys   = ht->keys;
        U64 *values = ht->values;

        Size k_old_size = ht->slot_count * sizeof(*(ht->keys));
        Size v_old_size = ht->slot_count * sizeof(*(ht->values));

        if ((2 * ht->slot_filled) >= ht->slot_count) { // Only increase size if need be
            ht->univ.m = ht->univ.m + 1;
            ht->slot_count = 1ULL << (ht->univ.m);
            hashUniversalConstantsUpdate(&(ht->univ));
        }

        Size k_size = ht->slot_count * sizeof(*(ht->keys));
        Size v_size = ht->slot_count * sizeof(*(ht->values));

        ht->keys   = cast_val(memAlloc(ht->allocator, k_size), U64*);
        ht->values = cast_val(memAlloc(ht->allocator, v_size), U64*);

        memset(ht->keys,   0, k_size);
        memset(ht->values, 0, v_size);

        for (Size i = 0; i < slot_count_prev; ++i) {
            U64 key_i   = keys[i];
            U64 value_i = values[i];
            if (value_i != 0) {
                U64 hash_new = hashUniversal(ht->univ, key_i);
                ht_LinearProbeInsertion(ht, hash_new, key_i, value_i);
            }
        }

        memDealloc(ht->allocator, keys,   k_old_size);
        memDealloc(ht->allocator, values, v_old_size);

        ht->collision_count = 0;
    }

    U64 hash = hashUniversal(ht->univ, key);
    U64 result_value = ht_LinearProbeInsertion(ht, hash, key, value);
    if (result_value == 0) {
        ht->slot_filled++;
    }

    return result_value;
}

header_function
U64 htLookup (Hash_Table *ht, U64 key)
{
    if (key == 0) return 0;

    Size location = 0;
    U64 result_value = 0;

    U64 hash = hashUniversal(ht->univ, key);
    if (ht_LinearProbeSearch(ht, key, hash, &location)) {
        result_value = ht->values[location];
    }

    return result_value;
}

header_function
U64 htRemove (Hash_Table *ht, U64 key)
{
    if (key == 0) return 0;

    Size location = 0;
    U64 result_value = 0;

    U64 hash = hashUniversal(ht->univ, key);
    if (ht_LinearProbeSearch(ht, key, hash, &location)) {
        result_value = ht->values[location];
    }

    if (result_value != 0) {
        ht->values[location] = 0;
        ht->keys[location] = 0;
        ht->slot_filled -= 1;
    }

    return result_value;
}

# endif // NLIB_EXCLUDE_HASH_TABLE

/* ==============
 * Map
 * (also works as a sparse-set)
 */

/* API ----------------------------------------
 * T* mapCreate           (map(T) ptr)
 * T* mapCreateSized      (map(T) ptr, Size min_capacity)
 * T* mapCreateAlloc      (map(T) ptr, Memory_Allocator allocator)
 * T* mapCreateAllocSized (map(T) ptr, Size min_capacity, Memory_Allocator allocator)
 *
 * void  mapDelete     (T *ptr)
 *
 * void  mapInsert     (T *ptr, U64 key, T value)
 * void  mapRemove     (T *ptr, U64 key)
 * Bool  mapExists     (T *ptr, U64 key)
 * T     mapLookup     (T *ptr, U64 key)
 * T*    mapGetRef     (T *ptr, U64 key)
 */

# if defined(LANG_C) && !defined(NLIB_EXCLUDE_MAP)
typedef struct Map_Userdata {
    Hash_Table table;
    Memory_Allocator allocator;
    ra(Size) free_list;
} Map_Userdata;

#  define map(T) ra(T)

#  define map_DataPtrType(T) Map_Userdata *
#  define map_GetDataPtr(v)  (ra_GetHeader(v)->userdata)

#  define map_DirtySlots(m) raElemin(m)
#  define map_TotalSlots(m) raMaxElemin(m)

#  define mapCreate(var)                           map_Create(sizeof(*(var)),  0,       memHeapGet())
#  define mapCreateSized(var, min_cap)             map_Create(sizeof(*(var)),  min_cap, memHeapGet())
#  define mapCreateAlloc(var, alloc)               map_Create(sizeof(*(var)),  0,       alloc)
#  define mapCreateAllocSized(var, min_cap, alloc) map_Create(sizeof(*(var)),  min_cap, alloc)

header_function
void* map_Create (Size elem_size, Size min_cap, Memory_Allocator allocator)
{
    void *value_ra = ra_Create(elem_size, min_cap, allocator);
    Ra_Header *rhdr = ra_GetHeader(value_ra);
    (rhdr->len)++;
    rhdr->userdata = memAlloc(allocator, sizeof(Map_Userdata));

    Map_Userdata *mud = rhdr->userdata;
    *mud = (Map_Userdata)NLIB_ZERO_INIT_LIST;
    mud->table = htCreateAlloc(allocator);
    mud->free_list = ra_Create(sizeof(*mud->free_list), min_cap, allocator);
    mud->allocator = allocator;

    return value_ra;
}

// FIXME(naman): Once C23 comes out, use typeof() or auto to assign arg_value to a variable,
// then move everything into a function (since value can then be passed as pointer)
#  define mapInsert(arg_map, arg_key, arg_value) do {           \
        Ra_Header *rhdr = ra_GetHeader((arg_map));              \
        Map_Userdata *mud = rhdr->userdata;                     \
                                                                \
        Size insertion_index = 0;                               \
                                                                \
        claim(arg_key != 0);                                    \
                                                                \
        if (raElemin(mud->free_list) > 0) {                     \
            (arg_map)[mud->free_list[0]] = (arg_value);         \
            insertion_index = mud->free_list[0];                \
            raRemoveUnsorted(mud->free_list, 0);                \
        } else {                                                \
            raAdd((arg_map), (arg_value));                      \
            insertion_index = rhdr->len - 1;                    \
        }                                                       \
                                                                \
        rhdr = ra_GetHeader((arg_map));                         \
        mud = rhdr->userdata;                                   \
        htInsert(&(mud->table), (arg_key), insertion_index);    \
    } while (0)

header_function
Bool mapExists (void *map, U64 key)
{
    Ra_Header *rhdr = ra_GetHeader(map);
    Map_Userdata *mud = rhdr->userdata;

    if (key == 0) return false;

    Size index = htLookup(&(mud->table), key);
    if (index != 0) {
        return true;
    }
    return false;
}

header_function
void mapRemove (void *map, U64 key)
{
    Ra_Header *rhdr = ra_GetHeader(map);
    Map_Userdata *mud = rhdr->userdata;

    if (key == 0) return;

    Size index = htLookup(&(mud->table), key);
    raAdd(mud->free_list, index);
    htRemove(&(mud->table), key);

    return;
}

#  define mapLookup(arg_map, arg_key) ((arg_map)[(htLookup(&((Map_Userdata*)(ra_GetHeader(arg_map))->userdata)->table, \
                                                           (arg_key)))])
#  define mapGetRef(arg_map, arg_key) (&mapLookup((arg_map), (arg_key)))

header_function
void map_Delete (void *map, Size elem_size)
{
    Ra_Header *rhdr = ra_GetHeader(map);
    Map_Userdata *mud = rhdr->userdata;

    htDelete(mud->table);
    raDelete(mud->free_list);

    Memory_Allocator allocator = mud->allocator;
    memDealloc(allocator, mud, sizeof(Map_Userdata));
    ra_Delete(map, elem_size);
}

#  define mapDelete(arg_map) (map_Delete((arg_map), sizeof(*(arg_map))))

#define mapForEach(arg_map, arg_key, arg_val)                           \
    U64 gensym_line(map_iter_ht_value) = 0;                             \
    ht_ForEachWithStmt(&((Map_Userdata*)(ra_GetHeader(arg_map))->userdata)->table, \
                       arg_key,                                         \
                       gensym_line(map_iter_ht_value),                  \
                       ((arg_val) = (arg_map)[gensym_line(map_iter_ht_value)]))

# elif defined(LANG_CPP) && !defined(NLIB_EXCLUDE_MAP)

template <typename T>
struct Map_Struct {
    Hash_Table table;
    Memory_Allocator allocator;
    ra(Size) free_list;
    ra(T) data;

    void Insert (U64 key, T value) {
        Size insertion_index = 0;

        claim(key != 0);

        if (raElemin(free_list) > 0) {
            Size index = free_list[0];
            data.InsertElementAtIndexDirectlyWithoutUpdatingMetaData_(index, value);
            insertion_index = free_list[0];
            raRemoveUnsorted(free_list, 0);
        } else {
            raAdd(data, value);
            insertion_index = raElemin(data) - 1;
        }

        htInsert(&table, key, insertion_index);
    }

    Bool Exists (U64 key) {
        Size index = htLookup(&table, key);
        if (index != 0) return true;
        return false;
    }

    T Lookup (U64 key) {
        U64 index = htLookup(&table, key);
        T result = data[index];
        return result;
    }

    T* GetRef (U64 key) {
        U64 index = htLookup(&table, key);
        T *result = &(data[index]);
        return result;
    }

    void Remove (U64 key) {
        Size index = htLookup(&table, key);
        raAdd(free_list, index);
        htRemove(&table, key);
    }

};

#define mapForEach(arg_map, arg_key, _val)                              \
    U64 gensym_line(map_iter_htarg_value) = 0;                          \
    ht_ForEachWithStmt(&((arg_map).table),                              \
                       (arg_key),                                       \
                       gensym_line(map_iter_htarg_value),               \
                       ((_val) = ((arg_map).data)[gensym_line(map_iter_htarg_value)]))


#  define map(T) Map_Struct<T>

#  define map_DirtySlots(m) raElemin(m.data)
#  define map_TotalSlots(m) raMaxElemin(m.data)

#  define map_DataPtrType(T) Map_Struct<T> *
#  define map_GetDataPtr(v)  &v

#  define mapCreate(var)                           map_Create(&var, 0,       memHeapGet())
#  define mapCreateSized(var, min_cap)             map_Create(&var, min_cap, memHeapGet())
#  define mapCreateAlloc(var, alloc)               map_Create(&var, 0,       alloc)
#  define mapCreateAllocSized(var, min_cap, alloc) map_Create(&var, min_cap, alloc)

template <typename T>
header_function
map(T) map_Create (map(T) *, Size min_cap, Memory_Allocator allocator)
{
    map(T) m = NLIB_ZERO_INIT_LIST;

    m.data = raCreateAllocSized(m.data, min_cap, allocator);
    m.data.len++;

    m.table = htCreateAlloc(allocator);
    m.free_list = raCreateAllocSized(m.free_list, min_cap, allocator);
    m.allocator = allocator;

    return m;
}

#  define mapDelete(m) map_Delete(m)

template <typename T>
header_function
void map_Delete (map(T) &m)
{
    htDelete(m.table);
    raDelete(m.free_list);
    raDelete(m.data);

    m.~map(T)();
    return;
}

#  define mapInsert(m, k, v) ((m).Insert(k, v))
#  define mapRemove(m, k)    ((m).Remove(k))

#  define mapExists(m, k)   ((m).Exists(k))
#  define mapLookup(m, k)   ((m).Lookup(k))
#  define mapGetRef(m, k)   ((m).GetRef(k))

# endif // !defined(NLIB_EXCLUDE_MAP)

#define NLIB_H_INCLUDE_GUARD
#endif // NLIB_H_INCLUDE_GUARD
