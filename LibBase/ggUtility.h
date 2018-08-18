#ifndef GGUTILITIES_H
#define GGUTILITIES_H

// 1) include system
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

// 2) include own project-related (sort by component dependency)
#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggVectorTypes.h"
#include "LibBase/ggWalkerT.h"
#include "LibBase/ggColorTypes.h"

// 3) forward declarations

/**
 * commonly useful macros
 */
#define GG_UTILITY_UNUSED(aExpression) (void)(aExpression);

/**
 * collection of general, simple utilities
 */
namespace ggUtility {

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
    vStrStream << static_cast<ggInt16>(aValue);
    return vStrStream.str();
  }

  template <>
  inline std::string ToString(const ggUInt8& aValue) {
    std::ostringstream vStrStream;
    vStrStream << static_cast<ggUInt16>(aValue);
    return vStrStream.str();
  }

  template <>
  inline std::string ToString(const ggNumberType& aValue) {
    switch (aValue) {
      case ggNumberType::eUnknown: return "eUnknown";
      case ggNumberType::eChar: return "eChar";
      case ggNumberType::eInt8: return "eInt8";
      case ggNumberType::eInt16: return "eInt16";
      case ggNumberType::eInt32: return "eInt32";
      case ggNumberType::eInt64: return "eInt64";
      case ggNumberType::eSize: return "eSize";
      case ggNumberType::eUChar: return "eUChar";
      case ggNumberType::eUInt8: return "eUInt8";
      case ggNumberType::eUInt16: return "eUInt16";
      case ggNumberType::eUInt32: return "eUInt32";
      case ggNumberType::eUInt64: return "eUInt64";
      case ggNumberType::eUSize: return "eUSize";
      case ggNumberType::eFloat: return "eFloat";
      case ggNumberType::eDouble: return "eDouble";
    }
  }

  template <typename TValueType, ggUSize TDimensions>
  inline std::string VectorToString(const ggVectorT<TValueType, TDimensions>& aVector) {
    std::string vString("(");
    for (ggUSize vDimension = 0; vDimension < aVector.GetDimensions(); vDimension++) {
      vString += ToString(aVector[vDimension]) + std::string((vDimension + 1 < aVector.GetDimensions()) ? "/" : ")");
    }
    return vString;
  }

  template <typename TValueType, ggUSize TDimensions>
  inline std::string VectorToString(const ggVectorT<TValueType, TDimensions>& aVector, int aPrecision) {
    std::string vString("(");
    for (ggUSize vDimension = 0; vDimension < aVector.GetDimensions(); vDimension++) {
      vString += ToString(aVector[vDimension], aPrecision) + std::string((vDimension + 1 < aVector.GetDimensions()) ? "/" : ")");
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
  // template <> inline std::string ToString(const ggVector2Size& aValue) { return VectorToString(aValue); }
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
  // template <> inline std::string ToString(const ggVector3Size& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector3Float& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector3Double& aValue) { return VectorToString(aValue); }
  template <> inline std::string ToString(const ggVector3Float& aValue, int aPrecision) { return VectorToString(aValue, aPrecision); }
  template <> inline std::string ToString(const ggVector3Double& aValue, int aPrecision) { return VectorToString(aValue, aPrecision); }

  template <typename T1, typename T2>
  inline bool Find(const std::set<T1>& aItems, const T2& aItem) {
    return aItems.find(aItem) != aItems.end();
  }

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

  std::vector<ggColorUInt8> ColorTableHot();
  std::vector<ggColorUInt8> ColorTableRainbow(bool aIndexZeroTransparent = false);
  std::vector<ggColorUInt8> ColorTableRandom(bool aIndexZeroTransparent = false);
  std::vector<ggColorUInt8> ColorTableRandomRainbow(bool aIndexZeroTransparent = false);
  std::vector<ggColorUInt8> ColorTableRandomHot(bool aIndexZeroTransparent = false);
  std::vector<ggColorUInt8> ColorTableRandomCold(bool aIndexZeroTransparent = false);

}

#endif // GGUTILITIES_H
