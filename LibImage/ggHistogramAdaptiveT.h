#ifndef GGHISTOGRAMADAPTIVET_H
#define GGHISTOGRAMADAPTIVET_H

#include <vector>

#include "LibBase/ggTypes.h"


template <class TValueType, ggUInt64 TNumberOfBins = 512>
class ggHistogramAdaptiveT
{

public:

  ggHistogramAdaptiveT()
  : mCountTotal(0),
    mCountBins(TNumberOfBins, 0),
    mBinValueDelta(0) {
  }

  virtual ~ggHistogramAdaptiveT() {
  }

  inline ggUInt64 GetNumberOfBins() const {
    return TNumberOfBins;
  }

  inline ggUInt64 GetCountTotal() const {
    return mCountTotal;
  }

  inline ggInt64 GetBinIndex(const TValueType& aValue) const {
    if (mBinValueDelta == 0) return 0;
    return (ggInt64)floor((aValue - mBinValueMin) / mBinValueDelta);
  }

  inline ggInt64 GetBinIndexMin() const {
    return GetBinIndex(GetValueMin());
  }

  inline ggInt64 GetBinIndexMax() const {
    return GetBinIndex(GetValueMax());
  }

  inline ggUInt64 GetCountBin(ggInt64 aBinIndex) const {
    if (aBinIndex < 0) return 0;
    if (aBinIndex >= (ggInt64)TNumberOfBins) return 0;
    return mCountBins[aBinIndex];
  }

  inline ggUInt64 GetCountBin(const TValueType& aValue) const {
    return GetCountBin(GetBinIndex(aValue));
  }

  inline ggDouble GetBinValue(ggInt64 aBinIndex) const {
    return mBinValueMin + aBinIndex * mBinValueDelta + mBinValueDelta / 2;
  }

  inline ggDouble GetBinValueDelta() const {
    return mBinValueDelta;
  }

  inline const TValueType& GetValueMin() const {
    return mValueMin;
  }

  inline const TValueType& GetValueMax() const {
    return mValueMax;
  }

  inline void Insert(const TValueType& aValue) {
    if (mCountTotal == 0) {
      // the very first sample is inserted
      mValueMin = aValue;
      mValueMax = aValue;
      mBinValueMin = aValue;
      mBinValueMax = aValue;
      mBinValueDelta = 0;
    }
    else if (mValueMin == mValueMax) {
      // a second (or later) sample is inserted.
      // if its value is different than the previous samples, the vector with the bins can be initialized
      if (aValue < mValueMin) {
        mValueMin = aValue;
        mBinValueDelta = (ggDouble)(mValueMax - mValueMin) / (ggDouble)(TNumberOfBins - 1);
        mBinValueMin = mValueMin - mBinValueDelta / 2;
        mBinValueMax = mValueMax + mBinValueDelta / 2;
        mCountBins.front() = 1;
        mCountBins.back() = mCountTotal;
      }
      if (aValue > mValueMax) {
        mValueMax = aValue;
        mBinValueDelta = (ggDouble)(mValueMax - mValueMin) / (ggDouble)(TNumberOfBins - 1);
        mBinValueMin = mValueMin - mBinValueDelta / 2;
        mBinValueMax = mValueMax + mBinValueDelta / 2;
        mCountBins.front() = mCountTotal;
        mCountBins.back() = 1;
      }
    }
    else {
      // find the proper bin for the new sample
      ggInt64 vBinIndex = GetBinIndex(aValue);
      // if the bin index is too low, the bins are packed and shifted right (up)
      if (vBinIndex < 0) {
        do {
          PackBinsRight();
          vBinIndex = GetBinIndex(aValue);
        } while (vBinIndex < 0);
      }
      // if the bin index is too high, the bins are packed and shifted left (down)
      else if (vBinIndex >= (ggInt64)TNumberOfBins) {
        do {
          PackBinsLeft();
          vBinIndex = GetBinIndex(aValue);
        } while (vBinIndex >= (ggInt64)TNumberOfBins);
      }
      // now count the sample
      if (aValue < mValueMin) mValueMin = aValue;
      if (aValue > mValueMax) mValueMax = aValue;
      ++mCountBins[vBinIndex];
    }
    // the total count can be increased in any case
    ++mCountTotal;
  }

private:

  void PackBinsRight() {
    ggUInt64 vBinIndexDst = TNumberOfBins;
    ggUInt64 vBinIndexSrc = TNumberOfBins;
    while (vBinIndexSrc > 0) {
      mCountBins[--vBinIndexDst] = mCountBins[--vBinIndexSrc];
      if (vBinIndexSrc > 0) mCountBins[vBinIndexDst] += mCountBins[--vBinIndexSrc];
    }
    while (vBinIndexDst > 0) mCountBins[--vBinIndexDst] = 0;
    mBinValueDelta *= 2;
    mBinValueMin = mBinValueMax - TNumberOfBins * mBinValueDelta;
  }

  void PackBinsLeft() {
    ggUInt64 vBinIndexDst = 0;
    ggUInt64 vBinIndexSrc = 0;
    while (vBinIndexSrc < TNumberOfBins) {
      mCountBins[vBinIndexDst] = mCountBins[vBinIndexSrc++];
      if (vBinIndexSrc < TNumberOfBins) mCountBins[vBinIndexDst] += mCountBins[vBinIndexSrc++];
      ++vBinIndexDst;
    }
    while (vBinIndexDst < TNumberOfBins) mCountBins[vBinIndexDst++] = 0;
    mBinValueDelta *= 2;
    mBinValueMax = mBinValueMin + TNumberOfBins * mBinValueDelta;
  }

  TValueType mValueMin;
  TValueType mValueMax;

  ggUInt64 mCountTotal;
  std::vector<ggUInt64> mCountBins;

  ggDouble mBinValueMin;
  ggDouble mBinValueMax;
  ggDouble mBinValueDelta;

};

typedef ggHistogramAdaptiveT<ggInt32> ggHistogramInt32;
typedef ggHistogramAdaptiveT<ggInt64> ggHistogramInt64;
typedef ggHistogramAdaptiveT<ggUInt32> ggHistogramUInt32;
typedef ggHistogramAdaptiveT<ggUInt64> ggHistogramUInt64;
typedef ggHistogramAdaptiveT<ggFloat> ggHistogramFloat;
typedef ggHistogramAdaptiveT<ggDouble> ggHistogramDouble;

#endif // GGHISTOGRAMADAPTIVET_H
