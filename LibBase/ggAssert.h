#ifndef GGASSERT_H
#define GGASSERT_H

#ifdef QT_NO_DEBUG
  #define GG_ASSERT(aCondition)
#else
  #include "assert.h"
  #define GG_ASSERT(aCondition) assert(aCondition);
#endif

#endif // GGASSERT_H
