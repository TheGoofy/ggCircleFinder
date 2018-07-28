#ifndef GGROUND_H
#define GGROUND_H

#include <algorithm>

#include "LibBase/ggTypes.h"

template <typename T1, typename T2>
inline bool ggLess(const T1& aValue1, const T2& aValue2) {
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
inline bool ggGreater(const T1& aValue1, const T2& aValue2) {
  return ggLess(aValue2, aValue1);
}

template <typename TOut, typename TIn>
TOut ggClampToNumericLimits(const TIn aValue) {
  if (ggLess(aValue, std::numeric_limits<TOut>::min())) return std::numeric_limits<TOut>::min();
  if (ggGreater(aValue, std::numeric_limits<TOut>::max())) return std::numeric_limits<TOut>::max();
  return static_cast<TOut>(aValue);
}

template <typename TOut, typename TIn>
TOut ggRound(const TIn aValue) {
  return ggClampToNumericLimits<TOut>(aValue);
}

template <typename TOut>
TOut ggRound(const ggFloat aValue) {
  if (std::is_floating_point<TOut>()) return static_cast<TOut>(aValue);
  else return ggClampToNumericLimits<TOut>(static_cast<ggInt64>(aValue >= 0 ? aValue + 0.5f : aValue - 0.5f));
}

template <typename TOut>
TOut ggRound(const ggDouble aValue) {
  if (std::is_floating_point<TOut>()) return static_cast<TOut>(aValue);
  else return ggClampToNumericLimits<TOut>(static_cast<ggInt64>(aValue >= 0 ? aValue + 0.5 : aValue - 0.5));
}

// different rounding resolutions
enum class ggRoundType {
  eFactor20, // 1.0             2.0             5.0             10.0
  eFactor15, // 1.0     1.5     2.0     3.0     5.0     7.0     10.0
  eFactor12  // 1.0 1.2 1.5 1.7 2.0 2.5 3.0 4.0 5.0 6.0 7.0 8.0 10.0
};

// rounds to ... 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1, 2, 5, 10, 20, 50 ...
ggFloat ggRoundTo125(const ggFloat aValue, ggRoundType aRoundType = ggRoundType::eFactor20);
ggDouble ggRoundTo125(const ggDouble aValue, ggRoundType aRoundType = ggRoundType::eFactor20);

// round to Significant Digits: 5321.215 => 5320.0 ... 0.08716813 => 0.0872 ... 2.575612 => 2.58
ggFloat ggRoundToSD(ggFloat aValue, ggUInt16 aSignificantDigits = 3);
ggDouble ggRoundToSD(ggDouble aValue, ggUInt16 aSignificantDigits = 3);

#endif // GGROUND_H
