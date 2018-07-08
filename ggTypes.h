#ifndef GGTYPES_H
#define GGTYPES_H

#include <iostream>

#if defined(_WIN32)

typedef char ggChar;
typedef signed char ggInt8;
typedef short int ggInt16;
typedef int ggInt32;
typedef long long ggInt64;

typedef unsigned char ggUChar;
typedef unsigned char ggUInt8;
typedef unsigned short int ggUInt16;
typedef unsigned int ggUInt32;
typedef unsigned long long ggUInt64;

typedef size_t ggSize;

#endif //end Win32 ****

#if !defined(_WIN32)

typedef char ggChar;
typedef int8_t ggInt8;
typedef int16_t ggInt16;
typedef int32_t ggInt32;
typedef int64_t ggInt64;

typedef unsigned char ggUChar;
typedef uint8_t ggUInt8;
typedef uint16_t ggUInt16;
typedef uint32_t ggUInt32;
typedef uint64_t ggUInt64;

typedef size_t ggSize;

#endif

typedef float ggFloat;
typedef double ggDouble;
typedef long double ggLongDouble;

#endif // GGTYPES_H
