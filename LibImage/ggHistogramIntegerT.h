#ifndef GGHISTOGRAMINTEGERT_H
#define GGHISTOGRAMINTEGERT_H

#include <vector>

#include "LibBase/ggRound.h"
#include "LibImage/ggHistogram.h"

/**
 * Histogram optimized for integer values. The bin width is automatically adjusted
 * depending on the added values. For 8-bit values aand a histogram with 256 bins capacity
 * there is (almost) no internal overhead - the vector with the bins already fits well.
 * The sequence of adding values does not affect the final histogram.
 * The internal vector of bins is left-aligned with the lowest limit depending on the type:
 * e.g. Given a histogram with 8 bins: Adding the values 2, 3, 6, 7 will result into bins
 * which are 1 unit wide. Adding the values 10, 11, 12, 13 requires to pack values into bins
 * which are 2 units wide. Similarly for signed value-types: -128, -127, -125 fit well into
 * a 8-bit-8-bin histogram, while adding 0 results into bin-width 32.
 */
template <class TValueType, ggInt64 TCountBinCapacity = 256>
class ggHistogramIntegerT : public ggHistogram
{

public:

  ggHistogramIntegerT()
  : mValueMin(),
    mValueMax(),
    mBinWidth(1),
    mCountBins(TCountBinCapacity, 0),
    mCountTotal(0) {
  }

  virtual ~ggHistogramIntegerT() override {
  }

  inline void Reset() {
    mValueMin = TValueType();
    mValueMax = TValueType();
    mBinWidth = 1;
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
    return GetCountV(ggRound<TValueType>(aValueF));
  }

  virtual ggInt64 GetBinIndexF(const ggDouble& aValueF) const override {
    return GetBinIndexInternalFromValue(ggRound<TValueType>(aValueF)) - GetBinIndexInternalMin();
  }

  virtual ggDouble GetBinValueF(ggInt64 aBinIndex) const override {
    return static_cast<ggDouble>((aBinIndex + GetBinIndexInternalMin()) * mBinWidth + (mBinWidth - 1.0) / 2.0 + GetBinIndexOffset());
  }

  virtual ggDouble GetBinWidthF(ggInt64) const override {
    return static_cast<ggDouble>(mBinWidth);
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
      mValueMin = aValue;
      mValueMax = aValue;
    }
    else {
      if (aValue < mValueMin) mValueMin = aValue;
      if (aValue > mValueMax) mValueMax = aValue;
    }
    while (GetBinIndexInternalFromValue(aValue) >= static_cast<ggInt64>(mCountBins.size())) {
      for (ggUInt64 vBinIndex = 0; vBinIndex < mCountBins.size(); vBinIndex++) {
        if (2 * vBinIndex + 1 >= mCountBins.size()) mCountBins[vBinIndex] = 0;
        else mCountBins[vBinIndex] = mCountBins[2 * vBinIndex] + mCountBins[2 * vBinIndex + 1];
      }
      mBinWidth *= 2;
    }
    mCountBins[GetBinIndexInternalFromValue(aValue)] += aCount;
    mCountTotal += aCount;
  }

  static ggInt64 GetBinIndexOffset() {
    return std::numeric_limits<TValueType>::min();
  }

  inline ggInt64 GetBinIndexInternalFromIndex(ggInt64 aBinIndex) const {
    return GetBinIndexInternalMin() + aBinIndex;
  }

  inline ggInt64 GetBinIndexInternalFromValue(const TValueType& aValue) const {
    return (ggRound<ggInt64>(aValue) - GetBinIndexOffset()) / mBinWidth;
  }

  inline ggInt64 GetBinIndexInternalMin() const {
    return GetBinIndexInternalFromValue(mValueMin);
  }

  inline ggInt64 GetBinIndexInternalMax() const {
    return GetBinIndexInternalFromValue(mValueMax);
  }

  inline ggInt64 GetCountFromBinIndexInternal(ggInt64 aBinIndexInternal) const {
    if (aBinIndexInternal < 0) return 0;
    if (aBinIndexInternal >= static_cast<ggInt64>(TCountBinCapacity)) return 0;
    return mCountTotal > 0 ? mCountBins[aBinIndexInternal] : 0;
  }

  TValueType mValueMin;
  TValueType mValueMax;

  ggInt64 mBinWidth;
  std::vector<ggInt64> mCountBins;
  ggInt64 mCountTotal;

};

#endif // GGHISTOGRAMINTEGERT_H
