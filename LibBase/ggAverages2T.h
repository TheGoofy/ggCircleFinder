#ifndef GGAVERAGES2T_H
#define GGAVERAGES2T_H

#include <math.h>

#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggRound.h"
#include "LibBase/ggAveragesT.h"

/**
 * Computes the averages for 2 dimensions incl. Covariance.
 */
template <class TValueType>
class ggAverages2T
{

public:

  typedef ggAveragesT<TValueType> tAverages;

  inline ggAverages2T()
  : mAveragesX(),
    mAveragesY(),
    mSumOfProductsXY(0.0) {
  }

  // reset the running averages
  inline void Reset() {
    mAveragesX.Reset();
    mAveragesY.Reset();
    mSumOfProductsXY = 0.0;
  }

  // adds a sample
  inline void AddSample(TValueType aValueX,
                        TValueType aValueY,
                        ggDouble aCount = 1.0) {
    mAveragesX.AddSample(aValueX, aCount);
    mAveragesY.AddSample(aValueY, aCount);
    const ggDouble vValueShiftedX = ggRound<ggDouble>(aValueX) - mAveragesX.mShiftK;
    const ggDouble vValueShiftedY = ggRound<ggDouble>(aValueY) - mAveragesY.mShiftK;
    mSumOfProductsXY += aCount * vValueShiftedX * vValueShiftedY;
  }

  // removes a sample (min and max are not updated)
  inline void RemoveSample(TValueType aValueX,
                           TValueType aValueY,
                           ggDouble aCount = 1.0) {
    const ggDouble vValueShiftedX = ggRound<ggDouble>(aValueX) - mAveragesX.mShiftK;
    const ggDouble vValueShiftedY = ggRound<ggDouble>(aValueY) - mAveragesY.mShiftK;
    mSumOfProductsXY -= aCount * vValueShiftedX * vValueShiftedY;
    mAveragesX.RemoveSample(aValueX, aCount);
    mAveragesY.RemoveSample(aValueY, aCount);
  }

  // returns the number of samples
  inline ggDouble GetNumberOfSamples() const {
    return mAveragesX.mCount;
  }

  // averages of x-dimension
  inline const tAverages& X() const {
    return mAveragesX;
  }

  // averages of y-dimension
  inline const tAverages& Y() const {
    return mAveragesY;
  }

  // covariance of sample
  inline ggDouble GetCovariance() const {
    const ggDouble vCount = mAveragesX.mCount;
    if (vCount > 1.0) {
      const ggDouble vProductOfSumsXY = mAveragesX.mSum * mAveragesY.mSum / vCount;
      return (mSumOfProductsXY - vProductOfSumsXY) / (vCount - 1);
    }
    return TValueType();
  }

  // covariance of population
  inline ggDouble GetCovarianceP() const {
    const ggDouble vCount = mAveragesX.mCount;
    if (vCount > 0.0) {
      const ggDouble vProductOfSumsXY = mAveragesX.mSum * mAveragesY.mSum / vCount;
      return (mSumOfProductsXY - vProductOfSumsXY) / vCount;
    }
    return TValueType();
  }

  // linear regression y = a * x + b
  inline bool GetRegressionX(ggDouble& aA, ggDouble& aB) const {
    const ggDouble vVarianceX = mAveragesX.GetVarianceP();
    if (vVarianceX != 0.0) {
      aA = GetCovarianceP() / vVarianceX;
      aB = mAveragesY.GetMean() - aA * mAveragesX.GetMean();
      return true;
    }
    return false;
  }

  // linear regression x = a * y + b
  inline bool GetRegressionY(ggDouble& aA, ggDouble& aB) const {
    const ggDouble vVarianceY = mAveragesY.GetVarianceP();
    if (vVarianceY != 0.0) {
      aA = GetCovarianceP() / vVarianceY;
      aB = mAveragesX.GetMean() - aA * mAveragesY.GetMean();
      return true;
    }
    return false;
  }

private:

  tAverages mAveragesX;
  tAverages mAveragesY;
  ggDouble mSumOfProductsXY;

};


#endif // GGAVERAGES2T_H
