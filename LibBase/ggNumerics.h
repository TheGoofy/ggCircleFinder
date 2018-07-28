#ifndef GGNUMERICS_H
#define GGNUMERICS_H

#include <algorithm>

template <typename T1, typename T2>
inline bool ggIsLess(const T1& aValue1, const T2& aValue2) {
  if (std::is_signed<T1>()) {
    if (std::is_signed<T2>()) return aValue1 < aValue2;
    else if (aValue1 < 0) return true;
    else return static_cast<T2>(aValue1) < aValue2;
  }
  else {
    if (std::is_unsigned<T2>()) return aValue1 < aValue2;
    else if (aValue2 < 0) return false;
    else return aValue1 < static_cast<T1>(aValue2);
  }
}

template <typename T1, typename T2>
inline bool ggIsGreater(const T1& aValue1, const T2& aValue2) {
  return ggIsLess(aValue2, aValue1);
}

template <typename T1, typename T2>
inline T1 ggClamp(const T2& aValue, const T1& aValueMin, const T1& aValueMax) {
  if (ggIsLess(aValue, aValueMin)) return aValueMin;
  if (ggIsGreater(aValue, aValueMax)) return aValueMax;
  return static_cast<T1>(aValue);
}

template <typename T1, typename T2>
inline T1 ggClampNumericLimits(const T2 aValue) {
  return ggClamp(aValue, std::numeric_limits<T1>::min(), std::numeric_limits<T1>::max());
}

template <typename T>
inline const T& ggClamp(const T& aValue, const T& aValueMin, const T& aValueMax) {
  return (aValue < aValueMin) ? aValueMin : ((aValue > aValueMax) ? aValueMax : aValue);
}

template <typename T>
inline const T& ggMin(const T& aA, const T& aB) {
  return (aA < aB) ? aA : aB;
}

template <typename T>
inline const T& ggMax(const T& aA, const T& aB) {
  return (aA < aB) ? aB : aA;
}

template <typename T>
inline const T& ggMin(const T& aA, const T& aB, const T& aC) {
  if (aA < aB) return ggMin(aA, aC);
  else return ggMin(aA, aC);
}

template <typename T>
inline const T& ggMax(const T& aA, const T& aB, const T& aC) {
  if (aA < aB) return ggMax(aB, aC);
  else return ggMax(aA, aC);
}

template <typename T>
inline const T& ggMin(const T& aA, const T& aB, const T& aC, const T& aD) {
  if (aA < aB) return ggMin(aA, aC, aD);
  else return ggMin(aB, aC, aD);
}

template <typename T>
inline const T& ggMax(const T& aA, const T& aB, const T& aC, const T& aD) {
  if (aA < aB) return ggMax(aB, aC, aD);
  else return ggMax(aA, aC, aD);
}

#endif // GGNUMERICS_H
