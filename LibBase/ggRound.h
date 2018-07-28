#ifndef GGROUND_H
#define GGROUND_H

#include <algorithm>

#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggNumerics.h"

template <typename TOut, typename TIn>
TOut ggRound(const TIn aValue) {
  return ggClampNumericLimits<TOut>(aValue);
}

template <typename TOut>
TOut ggRound(const ggFloat aValue) {
  if (std::is_floating_point<TOut>()) return static_cast<TOut>(aValue);
  else return ggClampNumericLimits<TOut>(static_cast<ggInt64>(aValue >= 0 ? aValue + 0.5f : aValue - 0.5f));
}

template <typename TOut>
TOut ggRound(const ggDouble aValue) {
  if (std::is_floating_point<TOut>()) return static_cast<TOut>(aValue);
  else return ggClampNumericLimits<TOut>(static_cast<ggInt64>(aValue >= 0 ? aValue + 0.5 : aValue - 0.5));
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
