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
template <class TValueType>
class ggHistogramAdaptiveFloatT : public ggHistogram
{

public:

  ggHistogramAdaptiveFloatT(ggInt64 aCountBinsCapacity = 512,
                            const TValueType& aValueRangeMin = TValueType(),
                            const TValueType& aValueRangeMax = TValueType())
  : mValueMin(),
    mValueMax(),
    mBinValueMin(0),
    mBinValueMax(0),
    mBinWidth(0),
    mCountBinsCapacity(aCountBinsCapacity),
    mCountBins(static_cast<ggUSize>(aCountBinsCapacity), 0),
    mCountTotal(0) {
    AdjustBinRange(aValueRangeMin, aValueRangeMax);
  }

  virtual ~ggHistogramAdaptiveFloatT() override {
  }

  inline void Reset(const TValueType& aValueRangeMin = TValueType(),
                    const TValueType& aValueRangeMax = TValueType()) {
    mValueMin = TValueType();
    mValueMax = TValueType();
    AdjustBinRange(aValueRangeMin, aValueRangeMax);
    std::fill(mCountBins.begin(), mCountBins.end(), 0);
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
    return ggRound<ggDouble>(mValueMin);
  }

  virtual ggDouble GetValueMaxF() const override {
    return ggRound<ggDouble>(mValueMax);
  }

  inline const TValueType& GetValueMin() const {
    return mValueMin;
  }

  inline const TValueType& GetValueMax() const {
    return mValueMax;
  }

private:

  inline void AddPrivate(const TValueType& aValue, ggInt64 aCount)
  {
    // initialize value min/max when the very first value is added
    if (mCountTotal == 0) {
      mValueMin = aValue;
      mValueMax = aValue;
    }
    // check if the range for the bins is not yet initialized
    if (mBinValueMin == mBinValueMax) {
      // bin range is not initialized: adjust it to the first two different values
      if (aValue < mValueMin) {
        mValueMin = aValue;
        AdjustBinRange(mValueMin, mValueMax);
        mCountBins.front() = aCount;
        mCountBins.back() = mCountTotal;
      }
      if (aValue > mValueMax) {
        mValueMax = aValue;
        AdjustBinRange(mValueMin, mValueMax);
        mCountBins.front() = mCountTotal;
        mCountBins.back() = aCount;
      }
    }
    else {
      // bin range is initialized: find the proper bin for the new sample
      ggInt64 vBinIndex = GetBinIndexInternalFromValue(aValue);
      // if the bin index is too low, the bins are packed and shifted right (up)
      while (vBinIndex < 0) {
        PackBinsRight();
        vBinIndex = GetBinIndexInternalFromValue(aValue);
      }
      // if the bin index is too high, the bins are packed and shifted left (down)
      while (vBinIndex >= mCountBinsCapacity) {
        PackBinsLeft();
        vBinIndex = GetBinIndexInternalFromValue(aValue);
      }
      // now count the sample
      if (aValue < mValueMin) mValueMin = aValue;
      if (aValue > mValueMax) mValueMax = aValue;
      mCountBins[vBinIndex] += aCount;
    }
    // the total count must be increased in any case
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
    return ggRound<ggInt64>((ggRound<ggDouble>(aValue) - mBinValueMin) / mBinWidth);
  }

  inline ggInt64 GetBinIndexInternalMin() const {
    return GetBinIndexInternalFromValue(GetValueMin());
  }

  inline ggInt64 GetBinIndexInternalMax() const {
    return GetBinIndexInternalFromValue(GetValueMax());
  }

  inline ggInt64 GetCountFromBinIndexInternal(ggInt64 aBinIndexInternal) const {
    if (aBinIndexInternal < 0) return 0;
    if (aBinIndexInternal >= mCountBinsCapacity) return 0;
    return mValueMin == mValueMax ? mCountTotal : mCountBins[aBinIndexInternal];
  }

  void PackBinsRight() {
    ggInt64 vBinIndexDst = mCountBinsCapacity;
    ggInt64 vBinIndexSrc = mCountBinsCapacity;
    while (vBinIndexSrc > 0) {
      mCountBins[--vBinIndexDst] = mCountBins[--vBinIndexSrc];
      if (vBinIndexSrc > 0) mCountBins[vBinIndexDst] += mCountBins[--vBinIndexSrc];
    }
    while (vBinIndexDst > 0) mCountBins[--vBinIndexDst] = 0;
    mBinWidth *= 2;
    mBinValueMax -= mBinWidth / 4;
    mBinValueMin = mBinValueMax - (mCountBinsCapacity - 1) * mBinWidth;
  }

  void PackBinsLeft() {
    ggInt64 vBinIndexDst = 0;
    ggInt64 vBinIndexSrc = 0;
    while (vBinIndexSrc < mCountBinsCapacity) {
      mCountBins[vBinIndexDst] = mCountBins[vBinIndexSrc++];
      if (vBinIndexSrc < mCountBinsCapacity) mCountBins[vBinIndexDst] += mCountBins[vBinIndexSrc++];
      ++vBinIndexDst;
    }
    while (vBinIndexDst < mCountBinsCapacity) mCountBins[vBinIndexDst++] = 0;
    mBinWidth *= 2;
    mBinValueMin += mBinWidth / 4;
    mBinValueMax = mBinValueMin + (mCountBinsCapacity - 1) * mBinWidth;
  }

  inline void AdjustBinRange(const TValueType& aValueRangeMin,
                             const TValueType& aValueRangeMax) {
    mBinWidth = ggRound<ggDouble>(aValueRangeMax - aValueRangeMin) / mCountBinsCapacity;
    mBinValueMin = ggRound<ggDouble>(aValueRangeMin) + mBinWidth / 2;
    mBinValueMax = ggRound<ggDouble>(aValueRangeMax) - mBinWidth / 2;
    mBinValueMin -= fabs(mBinValueMin * std::numeric_limits<ggDouble>::epsilon());
  }

  TValueType mValueMin;
  TValueType mValueMax;

  ggDouble mBinValueMin;
  ggDouble mBinValueMax;
  ggDouble mBinWidth;
  const ggInt64 mCountBinsCapacity;
  std::vector<ggInt64> mCountBins;
  ggInt64 mCountTotal;

};

#endif // GGHISTOGRAMADAPTIVET_H
