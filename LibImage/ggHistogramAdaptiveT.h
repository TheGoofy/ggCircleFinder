#ifndef GGHISTOGRAMADAPTIVET_H
#define GGHISTOGRAMADAPTIVET_H

#include <vector>
#include <math.h>

#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggRound.h"
#include "LibImage/ggHistogram.h"


/**
 * Histogram for any number type (in particular floating-point numbers).
 * The range of the bins is automatically adjusted to the range of the
 * values, when they are added to the histogram. The effective number of bins
 * is roughly half of the specified capacity (when a new value is added, which
 * is either much lower or higher than previously added values, the existing
 * neighboring bins are packed together until the new value fits into the
 * histogram). The final histogram potentially depends on the sequence of
 * the added values - i.e. bin grouping or bin width could be different.
 * However total count, or distribution are independent on the sequence.
 */
template <class TValueType, ggInt64 TCountBinCapacity = 512>
class ggHistogramAdaptiveT : public ggHistogram
{

public:

  ggHistogramAdaptiveT()
  : mValueMin(),
    mValueMax(),
    mBinValueMin(0),
    mBinValueMax(0),
    mBinWidth(0),
    mCountBins(TCountBinCapacity, 0),
    mCountTotal(0) {
  }

  virtual ~ggHistogramAdaptiveT() override {
  }

  inline void Reset() {
    mValueMin = TValueType();
    mValueMax = TValueType();
    mBinValueMin = 0;
    mBinValueMax = 0;
    mBinWidth = 0;
    mCountBins = std::vector<ggInt64>(TCountBinCapacity, 0);
    mCountTotal = 0;
  }

  inline void Add(const TValueType& aValue, ggInt64 aCount = 1) {
    AddPrivate(aValue, aCount);
  }

  virtual ggInt64 GetNumberOfBins() const override {
    return GetBinIndexInternalMax() - GetBinIndexInternalMin() + 1;
  }

  virtual ggInt64 GetCountTotal() const override {
    return mCountTotal;
  }

  virtual ggInt64 GetCount(ggInt64 aBinIndex) const override {
    return GetCountFromBinIndexInternal(GetBinIndexInternalFromIndex(aBinIndex));
  }

  inline ggInt64 GetCountV(const TValueType& aValue) const {
    return GetCountFromBinIndexInternal(GetBinIndexInternalFromValue(aValue));
  }

  virtual ggInt64 GetCountF(const ggDouble& aValueF) const override {
    return GetCountFromBinIndexInternal(GetBinIndexInternalFromValueF(aValueF));
  }

  virtual ggInt64 GetBinIndexF(const ggDouble& aValueF) const override {
    return GetBinIndexInternalFromValueF(aValueF) - GetBinIndexInternalMin();
  }

  virtual ggDouble GetBinValueF(ggInt64 aBinIndex) const override {
    return mBinValueMin + GetBinIndexInternalFromIndex(aBinIndex) * mBinWidth;
  }

  virtual ggDouble GetBinWidthF(ggInt64) const override {
    return mBinWidth;
  }

  virtual ggDouble GetValueMinF() const override {
    return static_cast<ggDouble>(mValueMin);
  }

  virtual ggDouble GetValueMaxF() const override {
    return static_cast<ggDouble>(mValueMax);
  }

  inline const TValueType& GetValueMin() const {
    return mValueMin;
  }

  inline const TValueType& GetValueMax() const {
    return mValueMax;
  }

private:

  inline void AddPrivate(const TValueType& aValue, ggInt64 aCount) {
    if (mCountTotal == 0) {
      // the very first sample is inserted, vector can not yet be initialized
      mValueMin = aValue;
      mValueMax = aValue;
      mBinValueMin = aValue;
      mBinValueMax = aValue;
      mBinWidth = 0;
    }
    else if (mValueMin == mValueMax) {
      // a second (or later) sample is inserted.
      // if its value is different than the previous samples, the vector with the bins can be initialized
      if (aValue < mValueMin) {
        mValueMin = aValue;
        mBinWidth = static_cast<ggDouble>(mValueMax - mValueMin) / static_cast<ggDouble>(TCountBinCapacity - 1);
        mBinValueMin = mValueMin;
        mBinValueMax = mValueMax;
        mCountBins.front() = aCount;
        mCountBins.back() = mCountTotal;
      }
      if (aValue > mValueMax) {
        mValueMax = aValue;
        mBinWidth = static_cast<ggDouble>(mValueMax - mValueMin) / static_cast<ggDouble>(TCountBinCapacity - 1);
        mBinValueMin = mValueMin;
        mBinValueMax = mValueMax;
        mCountBins.front() = mCountTotal;
        mCountBins.back() = aCount;
      }
    }
    else {
      // find the proper bin for the new sample
      ggInt64 vBinIndex = GetBinIndexInternalFromValue(aValue);
      // if the bin index is too low, the bins are packed and shifted right (up)
      while (vBinIndex < 0) {
        PackBinsRight();
        vBinIndex = GetBinIndexInternalFromValue(aValue);
      }
      // if the bin index is too high, the bins are packed and shifted left (down)
      while (vBinIndex >= static_cast<ggInt64>(TCountBinCapacity)) {
        PackBinsLeft();
        vBinIndex = GetBinIndexInternalFromValue(aValue);
      }
      // now count the sample
      if (aValue < mValueMin) mValueMin = aValue;
      if (aValue > mValueMax) mValueMax = aValue;
      mCountBins[vBinIndex] += aCount;
    }
    // the total count can be increased in any case
    mCountTotal += aCount;
  }

  inline ggInt64 GetBinIndexInternalFromIndex(ggInt64 aBinIndex) const {
    return GetBinIndexInternalMin() + aBinIndex;
  }

  inline ggInt64 GetBinIndexInternalFromValueF(ggDouble aValueF) const {
    if (mBinWidth == 0) return 0;
    return ggRound<ggInt64>((aValueF - mBinValueMin) / mBinWidth);
  }

  inline ggInt64 GetBinIndexInternalFromValue(const TValueType& aValue) const {
    if (mBinWidth == 0) return 0;
    return ggRound<ggInt64>((aValue - mBinValueMin) / mBinWidth);
  }

  inline ggInt64 GetBinIndexInternalMin() const {
    return GetBinIndexInternalFromValue(GetValueMin());
  }

  inline ggInt64 GetBinIndexInternalMax() const {
    return GetBinIndexInternalFromValue(GetValueMax());
  }

  inline ggInt64 GetCountFromBinIndexInternal(ggInt64 aBinIndexInternal) const {
    if (aBinIndexInternal < 0) return 0;
    if (aBinIndexInternal >= static_cast<ggInt64>(TCountBinCapacity)) return 0;
    return mValueMin == mValueMax ? mCountTotal : mCountBins[aBinIndexInternal];
  }

  void PackBinsRight() {
    ggInt64 vBinIndexDst = TCountBinCapacity;
    ggInt64 vBinIndexSrc = TCountBinCapacity;
    while (vBinIndexSrc > 0) {
      mCountBins[--vBinIndexDst] = mCountBins[--vBinIndexSrc];
      if (vBinIndexSrc > 0) mCountBins[vBinIndexDst] += mCountBins[--vBinIndexSrc];
    }
    while (vBinIndexDst > 0) mCountBins[--vBinIndexDst] = 0;
    mBinWidth *= 2;
    mBinValueMax -= mBinWidth / 4;
    mBinValueMin = mBinValueMax - (TCountBinCapacity - 1) * mBinWidth;
  }

  void PackBinsLeft() {
    ggInt64 vBinIndexDst = 0;
    ggInt64 vBinIndexSrc = 0;
    while (vBinIndexSrc < TCountBinCapacity) {
      mCountBins[vBinIndexDst] = mCountBins[vBinIndexSrc++];
      if (vBinIndexSrc < TCountBinCapacity) mCountBins[vBinIndexDst] += mCountBins[vBinIndexSrc++];
      ++vBinIndexDst;
    }
    while (vBinIndexDst < TCountBinCapacity) mCountBins[vBinIndexDst++] = 0;
    mBinWidth *= 2;
    mBinValueMin += mBinWidth / 4;
    mBinValueMax = mBinValueMin + (TCountBinCapacity - 1) * mBinWidth;
  }

  TValueType mValueMin;
  TValueType mValueMax;

  ggDouble mBinValueMin;
  ggDouble mBinValueMax;
  ggDouble mBinWidth;
  std::vector<ggInt64> mCountBins;
  ggInt64 mCountTotal;

};

typedef ggHistogramAdaptiveT<ggInt32> ggHistogramInt32;
typedef ggHistogramAdaptiveT<ggInt64> ggHistogramInt64;
typedef ggHistogramAdaptiveT<ggUInt32> ggHistogramUInt32;
typedef ggHistogramAdaptiveT<ggUInt64> ggHistogramUInt64;
typedef ggHistogramAdaptiveT<ggFloat> ggHistogramFloat;
typedef ggHistogramAdaptiveT<ggDouble> ggHistogramDouble;

#endif // GGHISTOGRAMADAPTIVET_H
