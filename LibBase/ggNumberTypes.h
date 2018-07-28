#ifndef GGNUMBERTYPES_H
#define GGNUMBERTYPES_H

// 1) include system
// 2) include own project-related (sort by component dependency)
// 3) forward declarations

typedef char ggChar;
typedef __INT8_TYPE__ ggInt8;
typedef __INT16_TYPE__ ggInt16;
typedef __INT32_TYPE__ ggInt32;
typedef __INT64_TYPE__ ggInt64;
typedef __PTRDIFF_TYPE__ ggSize;

typedef unsigned char ggUChar;
typedef __UINT8_TYPE__ ggUInt8;
typedef __UINT16_TYPE__ ggUInt16;
typedef __UINT32_TYPE__ ggUInt32;
typedef __UINT64_TYPE__ ggUInt64;
typedef __SIZE_TYPE__ ggUSize;

typedef float ggFloat;
typedef double ggDouble;

enum class ggNumberType {
  eUnknown,
  eChar,  eInt8,  eInt16,  eInt32,  eInt64,  eSize,
  eUChar, eUInt8, eUInt16, eUInt32, eUInt64, eUSize,
  eFloat, eDouble
};

template <typename T> inline ggNumberType ggGetNumberType() { return ggNumberType::eUnknown; }

template <> inline ggNumberType ggGetNumberType<ggChar>() { return ggNumberType::eChar; }
template <> inline ggNumberType ggGetNumberType<ggInt8>() { return ggNumberType::eInt8; }
template <> inline ggNumberType ggGetNumberType<ggInt16>() { return ggNumberType::eInt16; }
template <> inline ggNumberType ggGetNumberType<ggInt32>() { return ggNumberType::eInt32; }
template <> inline ggNumberType ggGetNumberType<ggInt64>() { return ggNumberType::eInt64; }
template <> inline ggNumberType ggGetNumberType<ggSize>() { return ggNumberType::eSize; }
template <> inline ggNumberType ggGetNumberType<ggUChar>() { return ggNumberType::eUChar; }
// duplicate of ggChar ... template <> inline cNumberType NumberType<ggUInt8>() { return cNumberType::eUInt8; }
template <> inline ggNumberType ggGetNumberType<ggUInt16>() { return ggNumberType::eUInt16; }
template <> inline ggNumberType ggGetNumberType<ggUInt32>() { return ggNumberType::eUInt32; }
template <> inline ggNumberType ggGetNumberType<ggUInt64>() { return ggNumberType::eUInt64; }
template <> inline ggNumberType ggGetNumberType<ggUSize>() { return ggNumberType::eUSize; }
template <> inline ggNumberType ggGetNumberType<ggFloat>() { return ggNumberType::eFloat; }
template <> inline ggNumberType ggGetNumberType<ggDouble>() { return ggNumberType::eDouble; }

#endif // GGNUMBERTYPES_H
