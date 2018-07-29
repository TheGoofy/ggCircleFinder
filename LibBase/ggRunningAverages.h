#ifndef GGRUNNINGAVERAGEST_H
#define GGRUNNINGAVERAGEST_H

#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggRound.h"

/**
 * Computes the averages: Sum, Min, Max, Mean, StdDev, and
 * Variation. The class requires a minimum amount of memory, and
 * does not store individual samples (for this reason it can not
 * compute the Median).
 */
template <class TValueType>
class ggRunningAveragesT
{

public:

  inline ggRunningAveragesT()
  : mSum(0),
    mSumOfSquares(0),
    mCount(0),
    mMin(),
    mMax() {
  }

  // reset the running averages
  inline void Reset() {
    mSum = 0;
    mSumOfSquares = 0;
    mCount = 0;
    mMin = TValueType();
    mMax = TValueType();
  }

  // adds a sample
  inline void AddSample(TValueType aValue, ggDouble aCount = 1.0) {
    ggDouble vValue = static_cast<ggDouble>(aValue);
    mSum += aCount * vValue;
    mSumOfSquares += aCount * vValue * vValue;
    if (mCount == 0) {
      mMin = aValue;
      mMax = aValue;
    }
    else {
      if (mMin > aValue) {
        mMin = aValue;
      }
      if (mMax < aValue) {
        mMax = aValue;
      }
    }
    mCount += aCount;
  }

  // removes a sample (min and max are not updated)
  inline void RemoveSample(TValueType aValue, ggDouble aCount = 1.0) {
    ggDouble vValue = static_cast<ggDouble>(aValue);
    mSum -= aCount * vValue;
    mSumOfSquares -= aCount * vValue * vValue;
    mCount -= aCount;
    if (mCount == 0) {
      mMin = aValue;
      mMax = aValue;
    }
  }

  // returns the number of samples
  inline ggDouble GetNumberOfSamples() const {
    return mCount;
  }

  // returns the sum of all samples
  inline TValueType GetSum() const {
    return static_cast<TValueType>(mSum);
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
  inline TValueType GetMean() const {
    if (mCount > 0) {
      return ggRound<TValueType>(mSum / mCount);
    }
    return TValueType();
  }

  // sum of squared errors, also known as "residual sum of scquares" (RSS, or SSR, or SSE)
  inline TValueType GetSumOfSquaredErrors() const {
    if (mCount > 0) {
      return ggRound<TValueType>(mSumOfSquares - mSum * mSum / mCount);
    }
    return TValueType();
  }

  // variance of sample (square of standard deviation)
  inline TValueType GetVariance() const {
    if (mCount > 1) {
      ggDouble vSumOfSquaredErrors = mSumOfSquares - mSum * mSum / mCount;
      return ggRound<TValueType>(vSumOfSquaredErrors / (mCount - 1));
    }
    return TValueType();
  }

  // variance of population (square the standard deviation)
  inline TValueType GetVarianceP() const {
    if (mCount > 0) {
      ggDouble vSumOfSquaredErrors = mSumOfSquares - mSum * mSum / mCount;
      return ggRound<TValueType>(vSumOfSquaredErrors / mCount);
    }
    return TValueType();
  }

  // returns the standard deviation of sample
  inline TValueType GetStdDev() const {
    if (mCount > 1) {
      ggDouble vSumOfSquaredErrors = mSumOfSquares - mSum * mSum / mCount;
      if (vSumOfSquaredErrors >= 0) return ggRound<TValueType>(sqrt(vSumOfSquaredErrors / (mCount - 1)));
    }
    return TValueType();
  }

  // returns the standard deviation of population
  inline TValueType GetStdDevP() const {
    if (mCount > 0) {
      ggDouble vSumOfSquaredErrors = mSumOfSquares - mSum * mSum / mCount;
      if (vSumOfSquaredErrors >= 0) return ggRound<TValueType>(sqrt(vSumOfSquaredErrors / mCount));
    }
    return TValueType();
  }

  // returns the coefficient of variation
  inline TValueType GetVariationCoefficient() const {
    if (mCount > 1) {
      ggDouble vMean = mSum / mCount;
      if (vMean != 0) {
        ggDouble vSumOfSquaredErrors = mSumOfSquares - mSum * mSum / mCount;
        return ggRound<TValueType>(sqrt(vSumOfSquaredErrors / (mCount - 1)) / vMean);
      }
    }
    return TValueType();
  }

private:

  ggDouble mSum;
  ggDouble mSumOfSquares;
  ggDouble mCount;
  TValueType mMin;
  TValueType mMax;

};

#endif // GGRUNNINGAVERAGEST_H__
