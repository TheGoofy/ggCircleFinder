#ifndef GGAVERAGEST_H
#define GGAVERAGEST_H

#include <math.h>

#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggRound.h"

template <class TValueType>
class ggAverages2T;

/**
 * Computes the averages: Sum, Min, Max, Mean, StdDev, and
 * Variation. The class requires a minimum amount of memory, and
 * does not store individual samples (for this reason it can not
 * compute the Median).
 *
 * see: https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
 */
template <class TValueType>
class ggAveragesT
{

public:

  inline ggAveragesT()
  : mShiftK(0),
    mSum(0),
    mSumOfSquares(0),
    mCount(0),
    mMin(),
    mMax() {
  }

  // reset the running averages
  inline void Reset() {
    mShiftK = 0;
    mSum = 0;
    mSumOfSquares = 0;
    mCount = 0;
    mMin = TValueType();
    mMax = TValueType();
  }

  // adds a sample
  inline void AddSample(TValueType aValue,
                        ggDouble aCount = 1.0) {
    const ggDouble vValue = ggRound<ggDouble>(aValue);
    if (mCount == 0) {
      mShiftK = vValue;
      mMin = aValue;
      mMax = aValue;
    }
    else {
      if (aValue < mMin) mMin = aValue;
      if (aValue > mMax) mMax = aValue;
    }
    const ggDouble vValueShifted = vValue - mShiftK;
    mSum += aCount * vValueShifted;
    mSumOfSquares += aCount * vValueShifted * vValueShifted;
    mCount += aCount;
  }

  // removes a sample (min and max are not updated)
  inline void RemoveSample(TValueType aValue,
                           ggDouble aCount = 1.0) {
    const ggDouble vValue = ggRound<ggDouble>(aValue);
    const ggDouble vValueShifted = vValue - mShiftK;
    mSum -= aCount * vValueShifted;
    mSumOfSquares -= aCount * vValueShifted * vValueShifted;
    mCount -= aCount;
    if (mCount == 0) {
      Reset();
    }
  }

  // returns the number of samples
  inline ggDouble GetNumberOfSamples() const {
    return mCount;
  }

  // returns the sum of all samples
  inline ggDouble GetSum() const {
    return GetSumDouble();
  }

  // returns the minimum
  inline TValueType GetMin() const {
    return mMin;
  }

  // returns the maximum
  inline TValueType GetMax() const {
    return mMax;
  }

  // returns the mean value
  inline ggDouble GetMean() const {
    if (mCount > 0) {
      return GetMeanDouble();
    }
    return TValueType();
  }

  // sum of squared errors, also known as "residual sum of scquares" (RSS, or SSR, or SSE)
  inline ggDouble GetSumOfSquaredErrors() const {
    if (mCount > 0) {
      return GetSumOfSquaredErrorsDouble();
    }
    return TValueType();
  }

  // variance of sample (square of standard deviation)
  inline ggDouble GetVariance() const {
    if (mCount > 1) {
      ggDouble vSumOfSquaredErrors = GetSumOfSquaredErrorsDouble();
      return vSumOfSquaredErrors / (mCount - 1);
    }
    return TValueType();
  }

  // variance of population (square the standard deviation)
  inline ggDouble GetVarianceP() const {
    if (mCount > 0) {
      ggDouble vSumOfSquaredErrors = GetSumOfSquaredErrorsDouble();
      return vSumOfSquaredErrors / mCount;
    }
    return TValueType();
  }

  // returns the standard deviation of sample
  inline ggDouble GetStdDev() const {
    if (mCount > 1) {
      ggDouble vSumOfSquaredErrors = GetSumOfSquaredErrorsDouble();
      if (vSumOfSquaredErrors >= 0) return sqrt(vSumOfSquaredErrors / (mCount - 1));
    }
    return TValueType();
  }

  // returns the standard deviation of population
  inline ggDouble GetStdDevP() const {
    if (mCount > 0) {
      ggDouble vSumOfSquaredErrors = GetSumOfSquaredErrorsDouble();
      if (vSumOfSquaredErrors >= 0) return sqrt(vSumOfSquaredErrors / mCount);
    }
    return TValueType();
  }

  // returns the coefficient of variation
  inline ggDouble GetVariationCoefficient() const {
    if (mCount > 1) {
      ggDouble vMean = GetMeanDouble();
      if (vMean != 0) {
        ggDouble vSumOfSquaredErrors = GetSumOfSquaredErrorsDouble();
        return sqrt(vSumOfSquaredErrors / (mCount - 1)) / vMean;
      }
    }
    return TValueType();
  }

private:

  // friends will be friends ...
  friend ggAverages2T<TValueType>;

  // sum
  inline ggDouble GetSumDouble() const {
    return mSum + mCount * mShiftK;
  }

  // mean, internal (note: mCount must not be zero)
  inline ggDouble GetMeanDouble() const {
    return mSum / mCount + mShiftK;
  }

  // sum of squared errors, internal (note: mCount must not be zero)
  inline ggDouble GetSumOfSquaredErrorsDouble() const {
    return mSumOfSquares - mSum * mSum / mCount;
  }

  ggDouble mShiftK;
  ggDouble mSum;
  ggDouble mSumOfSquares;
  ggDouble mCount;
  TValueType mMin;
  TValueType mMax;

};

#endif // GGAVERAGEST_H
