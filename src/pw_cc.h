#ifndef _PICOWEATHER_CC_H
#define _PICOWEATHER_CC_H

#define pw_alignof(t) __alignof__(t)
#define pw_typeof(e) __typeof__(e)
#define pw_expect(expr, expect) __builtin_expect(expr, expect)

#define pw_compiler_barrier() __asm__ __volatile__("" ::: "memory");
#define pw_unreachable() __builtin_unreachable()
#define pw_assume_aligned(p, a) __builtin_assume_aligned(p, a)
#define pw_prefetch(p) __builtin_prefetch(p)

#define PW_ATTR_ALIGNED(a) __attribute__((aligned(a)))
#define PW_ATTR_ALLOC_ALIGNED(arg_p) __attribute__((alloc_align(arg_p)))
#define PW_ATTR_NORETURN __attribute__((noreturn))
#define PW_ATTR_PACKED __attribute__((packed))
#define PW_ATTR_ALWAYS_INLINE __attribute__((always_inline))
#define PW_ATTR_COLD __attribute__((cold))
#define PW_ATTR_HOT __attribute__((hot))
#define PW_ATTR_CONST __attribute__((const))
#define PW_ATTR_WEAK __attribute__((weak))
#define PW_ATTR_PUBLIC __attribute__((visibility("default")))
#define PW_ATTR_PRIVATE __attribute__((visibility("hidden")))
#define PW_ATTR_FORMAT(fmt_n, va_n) __attribute__((format(printf, fmt_n, va_n)))

#endif /* _PICOWEATHER_CC_H */
