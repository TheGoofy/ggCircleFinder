#ifndef GGRUNNINGAVERAGEST_H
#define GGRUNNINGAVERAGEST_H

#include "ggTypes.h"

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
      mSumSquare(0),
      mCount(0),
      mMin(),
      mMax() {
  }

  // reset the running averages
  inline void Reset() {
    mSum = 0;
    mSumSquare = 0;
    mCount = 0;
    mMin = TValueType();
    mMax = TValueType();
  }

  // adds a sample for computing an average
  inline void AddSample(TValueType aValue, ggDouble aCount = 1.0) {
    ggDouble vValue = static_cast<ggDouble>(aValue);
    mSum += aCount * vValue;
    mSumSquare += aCount * vValue * vValue;
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
    if (mCount != 0) {
      return static_cast<TValueType>(mSum / mCount);
    }
    return TValueType();
  }

  // returns the standart deviation
  inline TValueType GetStdDev() const {
    if (mCount != 0) {
      ggDouble vMean = mSum / mCount;
      return static_cast<TValueType>(sqrt((mSumSquare / mCount) - (vMean * vMean)));
    }
    return TValueType();
  }

  // returns the coefficient of variation
  inline TValueType GetVariation() const {
    if (mCount != 0) {
      ggDouble vMean = mSum / mCount;
      if (vMean != 0) {
        return static_cast<TValueType>(sqrt((mSumSquare / mCount) - (vMean * vMean)) / vMean);
      }
    }
    return TValueType();
  }

private:

  ggDouble mSum;
  ggDouble mSumSquare;
  ggDouble mCount;
  TValueType mMin;
  TValueType mMax;

};

#endif // GGRUNNINGAVERAGEST_H__
