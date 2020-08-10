#pragma once

#ifdef NDEBUG
#define DBG_ONLY(X)
#else
#define DBG_ONLY(X) X
#endif