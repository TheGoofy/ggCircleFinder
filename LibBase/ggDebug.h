#ifndef GGDEBUG_H
#define GGDEBUG_H

#define GG_STRINGIFY(x) #x
#define GG_TO_STRING(x) GG_STRINGIFY(x)
#define GG_FILE_LINE (__FILE__ ":" GG_TO_STRING(__LINE__))

#endif // GGDEBUG_H
