#ifndef GGUTILITIES_H
#define GGUTILITIES_H

// 1) include system
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

// 2) include own project-related (sort by component dependency)
#include "Base/ggTypes.h"
#include "Base/ggVectorTypes.h"
#include "Base/ggWalkerT.h"

// 3) forward declarations

/**
 * commonly useful macros
 */
#define GG_UTILITY_UNUSED(aExpression) (void)(aExpression);

/**
 * collection of general, simple utilities
 */
namespace ggUtility {

  enum class cNumberType {
    eUnknown,
    eChar,  eInt8,  eInt16,  eInt32,  eInt64,  eSize,
    eUChar, eUInt8, eUInt16, eUInt32, eUInt64, eUSize,
    eFloat, eDouble
  };

  template <typename T> inline cNumberType NumberType() { return cNumberType::eUnknown; }

  template <> inline cNumberType NumberType<ggChar>() { return cNumberType::eChar; }
  template <> inline cNumberType NumberType<ggInt8>() { return cNumberType::eInt8; }
  template <> inline cNumberType NumberType<ggInt16>() { return cNumberType::eInt16; }
  template <> inline cNumberType NumberType<ggInt32>() { return cNumberType::eInt32; }
  template <> inline cNumberType NumberType<ggInt64>() { return cNumberType::eInt64; }
  template <> inline cNumberType NumberType<ggSize>() { return cNumberType::eSize; }
  template <> inline cNumberType NumberType<ggUChar>() { return cNumberType::eUChar; }
  //template <> inline cNumberType NumberType<ggUInt8>() { return cNumberType::eUInt8; }
  template <> inline cNumberType NumberType<ggUInt16>() { return cNumberType::eUInt16; }
  template <> inline cNumberType NumberType<ggUInt32>() { return cNumberType::eUInt32; }
  template <> inline cNumberType NumberType<ggUInt64>() { return cNumberType::eUInt64; }
  template <> inline cNumberType NumberType<ggUSize>() { return cNumberType::eUSize; }
  template <> inline cNumberType NumberType<ggFloat>() { return cNumberType::eFloat; }
  template <> inline cNumberType NumberType<ggDouble>() { return cNumberType::eDouble; }

  // different rounding resolutions
  enum class cRoundType {
    eFactor20,
    eFactor15,
    eFactor12
  };

  // rounds to ... 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1, 2, 5, 10, 20, 50 ...
  ggFloat RoundTo125(const ggFloat aValue, cRoundType aRoundType = cRoundType::eFactor20);

  // round to Order of MaGnitude: 5321.215 => 5320.0 ... 0.08716813 => 0.0872 ... 2.575612 => 2.58
  ggFloat RoundToOMG(ggFloat aValue, ggUInt16 aOrderOfMagnitude = 3);

  template <typename T>
  inline const T& Clamp(const T& aValue, const T& aValueMin, const T& aValueMax) {
    return (aValue < aValueMin) ? aValueMin : ((aValue > aValueMax) ? aValueMax : aValue);
  }

  template <typename T>
  inline void Swap(T& aValueA, T& aValueB) {
    T vValueT = aValueA;
    aValueA = aValueB;
    aValueB = vValueT;
  }

  template <typename T>
  inline const T& Min(const T& aA, const T& aB) {
    return (aA < aB) ? aA : aB;
  }

  template <typename T>
  inline const T& Max(const T& aA, const T& aB) {
    return (aA < aB) ? aB : aA;
  }

  template <typename T>
  inline const T& Min(const T& aA, const T& aB, const T& aC) {
    if (aA < aB) return Min(aA, aC);
    return Min(aA, aC);
  }

  template <typename T>
  inline const T& Max(const T& aA, const T& aB, const T& aC) {
    if (aA < aB) return Max(aB, aC);
    return Max(aA, aC);
  }

  template <typename T>
  inline const T& Min(const T& aA, const T& aB, const T& aC, const T& aD) {
    if (aA < aB) return Min(aA, aC, aD);
    return Min(aB, aC, aD);
  }

  template <typename T>
  inline const T& Max(const T& aA, const T& aB, const T& aC, const T& aD) {
    if (aA < aB) return Max(aB, aC, aD);
    return Max(aA, aC, aD);
  }  

  template <typename T1, typename T2>
  inline bool Find(const std::set<T1>& aItems, const T2& aItem) {
    return aItems.find(aItem) != aItems.end();
  }

  template <typename TValueType>
  inline std::string ToString(const TValueType& aValue) {
    std::ostringstream vStrStream;
    vStrStream << aValue;
    return vStrStream.str();
  }

  template <typename TValueType>
  inline std::string ToString(const TValueType& aValue, int aPrecision) {
    std::ostringstream vStrStream;
    vStrStream << std::fixed << std::setprecision(aPrecision);
    vStrStream << aValue;
    return vStrStream.str();
  }

  template <>
  inline std::string ToString(const bool& aValue) {
    return aValue ? "true" : "false";
  }

  template <>
  inline std::string ToString(const ggInt8& aValue) {
    std::ostringstream vStrStream;
    vStrStream << (ggInt16)aValue;
    return vStrStream.str();
  }

  template <>
  inline std::string ToString(const ggUInt8& aValue) {
    std::ostringstream vStrStream;
    vStrStream << (ggUInt16)aValue;
    return vStrStream.str();
  }

  template <typename TValueType, unsigned int TDimensions>
  inline std::string VectorToString(const ggVectorT<TValueType, TDimensions>& aVector) {
    std::string vString("(");
    for (ggSize vDimension = 0; vDimension < aVector.GetDimensions(); vDimension++) {
      vString += ToString(aVector[vDimension]) + std::string((vDimension + 1 < aVector.GetDimensions()) ? "," : ")");
    }
    return vString;
  }

  template <typename TValueType, unsigned int TDimensions>
  inline std::string VectorToString(const ggVectorT<TValueType, TDimensions>& aVector, int aPrecision) {
    std::string vString("(");
    for (ggSize vDimension = 0; vDimension < aVector.GetDimensions(); vDimension++) {
      vString += ToString(aVector[vDimension], aPrecision) + std::string((vDimension + 1 < aVector.GetDimensions()) ? "," : ")");
    }
    return vString;
  }

  template <> inline std::string ToString(const ggVector2T<bool>& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector2Int8& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector2Int16& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector2Int32& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector2Int64& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector2UInt8& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector2UInt16& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector2UInt32& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector2UInt64& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector2Size& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector2Float& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector2Double& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector2Float& aValue, int aPrecision) { return VectorToString(aValue, aPrecision); }
  template <> inline std::string ToString(const ggVector2Double& aValue, int aPrecision) { return VectorToString(aValue, aPrecision); }

  template <> inline std::string ToString(const ggVector3T<bool>& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector3Int8& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector3Int16& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector3Int32& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector3Int64& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector3UInt8& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector3UInt16& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector3UInt32& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector3UInt64& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector3Size& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector3Float& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector3Double& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector3Float& aValue, int aPrecision) { return VectorToString(aValue, aPrecision); }
  template <> inline std::string ToString(const ggVector3Double& aValue, int aPrecision) { return VectorToString(aValue, aPrecision); }

  template <typename T1, typename T2>
  void MoveUp(std::vector<T1>& aItemsVector, const std::set<T2>& aItemsSet) {
    if (aItemsSet.empty()) return;
    if (aItemsVector.size() < 2) return;
    for (ggUSize vIndex = aItemsVector.size()-1; vIndex > 0; vIndex--) {
      if (Find(aItemsSet, aItemsVector[vIndex-1])) {
        if (!Find(aItemsSet, aItemsVector[vIndex])) {
          Swap(aItemsVector[vIndex-1], aItemsVector[vIndex]);
        }
      }
    }
  }

  template <typename T1, typename T2>
  void MoveDown(std::vector<T1>& aItemsVector, const std::set<T2>& aItemsSet) {
    if (aItemsSet.empty()) return;
    if (aItemsVector.size() < 2) return;
    for (ggUSize vIndex = 0; vIndex+1 < aItemsVector.size(); vIndex++) {
      if (Find(aItemsSet, aItemsVector[vIndex+1])) {
        if (!Find(aItemsSet, aItemsVector[vIndex])) {
          Swap(aItemsVector[vIndex+1], aItemsVector[vIndex]);
        }
      }
    }
  }

  template <typename T1, typename T2>
  void MoveTop(std::vector<T1>& aItemsVector, const std::set<T2>& aItemsSet) {
    if (aItemsSet.empty()) return;
    std::vector<T1> vItemsVectorA;
    std::vector<T1> vItemsVectorB;
    ggWalkerT<typename std::vector<T1>::iterator> vItemsVectorWalker(aItemsVector);
    while (vItemsVectorWalker) {
      T1 vItem = *vItemsVectorWalker;
      if (Find(aItemsSet, vItem)) vItemsVectorA.push_back(vItem);
      else vItemsVectorB.push_back(vItem);
    }
    aItemsVector.clear();
    aItemsVector.insert(aItemsVector.end(), vItemsVectorB.begin(), vItemsVectorB.end());
    aItemsVector.insert(aItemsVector.end(), vItemsVectorA.begin(), vItemsVectorA.end());
  }

  template <typename T1, typename T2>
  void MoveBottom(std::vector<T1>& aItemsVector, const std::set<T2>& aItemsSet) {
    if (aItemsSet.empty()) return;
    std::vector<T1> vItemsVectorA;
    std::vector<T1> vItemsVectorB;
    ggWalkerT<typename std::vector<T1>::iterator> vItemsVectorWalker(aItemsVector);
    while (vItemsVectorWalker) {
      T1 vItem = *vItemsVectorWalker;
      if (Find(aItemsSet, vItem)) vItemsVectorA.push_back(vItem);
      else vItemsVectorB.push_back(vItem);
    }
    aItemsVector.clear();
    aItemsVector.insert(aItemsVector.end(), vItemsVectorA.begin(), vItemsVectorA.end());
    aItemsVector.insert(aItemsVector.end(), vItemsVectorB.begin(), vItemsVectorB.end());
  }
}

#endif // GGUTILITIES_H
