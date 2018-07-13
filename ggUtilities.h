#ifndef GGUTILITIES_H
#define GGUTILITIES_H

#include <string>
#include <sstream>
#include <iomanip>

#include "ggTypes.h"
#include "ggVectorT.h"
#include "ggVectorTypes.h"

namespace ggUtilities {

  template <typename T>
  inline T Clamp(T aValue, T aValueMin, T aValueMax) {
    return (aValue < aValueMin) ? (aValueMin) : (aValue > aValueMax ? aValueMax : aValue);
  }

  template <typename T>
  inline void Swap(T& aValue0, T& aValue1) {
    T vValueTmp(aValue0);
    aValue0 = aValue1;
    aValue1 = vValueTmp;
  }

  template <typename TValueType>
  std::string ToString(const TValueType& aValue) {
    std::ostringstream vStrStream;
    vStrStream << aValue;
    return vStrStream.str();
  }

  template <typename TValueType>
  std::string ToString(const TValueType& aValue, int aPrecision) {
    std::ostringstream vStrStream;
    vStrStream << std::fixed << std::setprecision(aPrecision);
    vStrStream << aValue;
    return vStrStream.str();
  }

  template <>
  std::string ToString(const bool& aValue) {
    return aValue ? "true" : "false";
  }

  template <>
  std::string ToString(const ggInt8& aValue) {
    std::ostringstream vStrStream;
    vStrStream << (ggInt16)aValue;
    return vStrStream.str();
  }

  template <>
  std::string ToString(const ggUInt8& aValue) {
    std::ostringstream vStrStream;
    vStrStream << (ggUInt16)aValue;
    return vStrStream.str();
  }

  template <typename TValueType, unsigned int TDimensions>
  std::string VectorToString(const ggVectorT<TValueType, TDimensions>& aVector) {
    std::string vString("(");
    for (ggSize vDimension = 0; vDimension < aVector.GetDimensions(); vDimension++) {
      vString += ToString(aVector[vDimension]) + std::string((vDimension + 1 < aVector.GetDimensions()) ? "," : ")");
    }
    return vString;
  }

  template <typename TValueType, unsigned int TDimensions>
  std::string VectorToString(const ggVectorT<TValueType, TDimensions>& aVector, int aPrecision) {
    std::string vString("(");
    for (ggSize vDimension = 0; vDimension < aVector.GetDimensions(); vDimension++) {
      vString += ToString(aVector[vDimension], aPrecision) + std::string((vDimension + 1 < aVector.GetDimensions()) ? "," : ")");
    }
    return vString;
  }

  template <> std::string ToString(const ggVector2T<bool>& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector2Int8& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector2Int16& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector2Int32& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector2Int64& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector2UInt8& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector2UInt16& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector2UInt32& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector2UInt64& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector2Size& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector2Float& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector2Double& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector2Float& aValue, int aPrecision) { return VectorToString(aValue, aPrecision); }
  template <> std::string ToString(const ggVector2Double& aValue, int aPrecision) { return VectorToString(aValue, aPrecision); }

  template <> std::string ToString(const ggVector3T<bool>& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector3Int8& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector3Int16& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector3Int32& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector3Int64& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector3UInt8& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector3UInt16& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector3UInt32& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector3UInt64& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector3Size& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector3Float& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector3Double& aValue) { return VectorToString(aValue); }
  template <> std::string ToString(const ggVector3Float& aValue, int aPrecision) { return VectorToString(aValue, aPrecision); }
  template <> std::string ToString(const ggVector3Double& aValue, int aPrecision) { return VectorToString(aValue, aPrecision); }

}

#endif // GGUTILITIES_H
