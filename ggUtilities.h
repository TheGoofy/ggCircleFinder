#ifndef GGUTILITIES_H
#define GGUTILITIES_H

#include <string>

#include "ggVectorT.h"

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

  template <typename TValueType, unsigned int TDimensions>
  std::string ToString(const ggVectorT<TValueType, TDimensions>& aVector) {
    std::string vString = "(";
    for (ggSize vDimension = 0; vDimension < aVector.GetDimensions(); vDimension++) {
      vString += std::to_string(aVector[vDimension]);
      vString += (vDimension + 1 < aVector.GetDimensions()) ? "," : ")";
    }
    return vString;
  }

}

#endif // GGUTILITIES_H
