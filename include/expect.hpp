#pragma once

#ifdef __GNUC__
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) x
#define unlikely(x) x
#endif

#define if_likely(x) if (likely(x))
#define if_unlikely(x) if (unlikely(x))