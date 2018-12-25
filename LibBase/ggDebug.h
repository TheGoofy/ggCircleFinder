#ifndef GGDEBUG_H
#define GGDEBUG_H

#define GG_STRINGIFY(x) #x
#define GG_TO_STRING(x) GG_STRINGIFY(x)
#define GG_FILE_LINE (__FILE__ ":" GG_TO_STRING(__LINE__))

// define GG_FUNCTION
#if defined(__GNUC__)
  #define GG_FUNCTION __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
  #define GG_FUNCTION __FUNCSIG__
#elif defined(__STDC_VERSION__)
  #define GG_FUNCTION __func__
#else
  #define GG_FUNCTION __FUNCTION__
#endif

#endif // GGDEBUG_H
