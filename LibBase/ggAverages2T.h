#ifndef GGAVERAGES2T_H
#define GGAVERAGES2T_H

#include <math.h>

#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggRound.h"
#include "LibBase/ggVector2T.h"
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
    mSumOfProductsXY(0) {
  }

  // reset the running averages
  inline void Reset() {
    mAveragesX.Reset();
    mAveragesY.Reset();
    mSumOfProductsXY = 0;
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

  // variance of sample (square of standard deviation)
  inline TValueType GetCovariance() const {
    const ggDouble vCount = mAveragesX.mCount;
    if (vCount > 1) {
      const ggDouble vProductOfSumsXY = mAveragesX.mSum * mAveragesY.mSum / vCount;
      return (mSumOfProductsXY - vProductOfSumsXY) / vCount;
    }
    return TValueType();
  }

private:

  tAverages mAveragesX;
  tAverages mAveragesY;
  ggDouble mSumOfProductsXY;

};


#endif // GGAVERAGES2T_H
