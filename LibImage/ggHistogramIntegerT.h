#ifndef GGHISTOGRAMINTEGERT_H
#define GGHISTOGRAMINTEGERT_H

#include <vector>

#include "LibBase/ggRound.h"
#include "LibImage/ggHistogram.h"

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

  void Reset() {
    mValueMin = TValueType();
    mValueMax = TValueType();
    mBinWidth = 1;
    mCountBins = std::vector<ggInt64>(TCountBinCapacity, 0);
    mCountTotal = 0;
  }

  void Add(const TValueType& aValue, ggInt64 aCount = 1) {
    if (mCountTotal == 0) {
      mValueMin = aValue;
      mValueMax = aValue;
    }
    else {
      if (aValue < mValueMin) mValueMin = aValue;
      if (aValue > mValueMax) mValueMax = aValue;
    }
    while (GetBinIndexInternal(aValue) >= static_cast<ggInt64>(mCountBins.size())) {
      for (ggUInt64 vBinIndex = 0; vBinIndex < mCountBins.size(); vBinIndex++) {
        if (2 * vBinIndex + 1 >= mCountBins.size()) mCountBins[vBinIndex] = 0;
        else mCountBins[vBinIndex] = mCountBins[2 * vBinIndex] + mCountBins[2 * vBinIndex + 1];
      }
      mBinWidth *= 2;
    }
    mCountBins[GetBinIndexInternal(aValue)] += aCount;
    mCountTotal += aCount;
  }

  virtual ggInt64 GetNumberOfBins() const override {
    return GetBinIndexInternalMax() - GetBinIndexInternalMin() + 1;
  }

  virtual ggInt64 GetCountTotal() const override {
    return mCountTotal;
  }

  virtual ggInt64 GetCount(ggInt64 aBinIndex) const override {
    return mCountTotal > 0 ? mCountBins[GetBinIndexInternalMin() + aBinIndex] : 0;
  }

  virtual ggInt64 GetCountF(const ggDouble& aValueF) const override {
    return mCountTotal > 0 ? mCountBins[GetBinIndexInternal(ggRound<TValueType>(aValueF))] : 0;
  }

  virtual ggInt64 GetBinIndexF(const ggDouble& aValueF) const override {
    return GetBinIndexInternal(ggRound<TValueType>(aValueF)) - GetBinIndexInternalMin();
  }

  virtual ggDouble GetBinValueF(ggInt64 aBinIndex) const override {
    return static_cast<ggDouble>((aBinIndex + GetBinIndexInternalMin()) * mBinWidth + (mBinWidth - 1.0) / 2.0 + GetBinIndexOffset());
  }

  virtual ggDouble GetBinWidthF() const override {
    return static_cast<ggDouble>(mBinWidth);
  }

  virtual ggDouble GetValueMinF() const override {
    return static_cast<ggDouble>(mValueMin);
  }

  virtual ggDouble GetValueMaxF() const override {
    return static_cast<ggDouble>(mValueMax);
  }

private:

  static ggInt64 GetBinIndexOffset() {
    return std::numeric_limits<TValueType>::min();
  }

  TValueType GetBinIndexScale() const {
    return mBinWidth;
  }

  inline ggInt64 GetBinIndexInternal(const TValueType& aValue) const {
    return (ggRound<ggInt64>(aValue) - GetBinIndexOffset()) / mBinWidth;
  }

  inline ggInt64 GetBinIndexInternalMin() const {
    return GetBinIndexInternal(mValueMin);
  }

  inline ggInt64 GetBinIndexInternalMax() const {
    return GetBinIndexInternal(mValueMax);
  }

  TValueType mValueMin;
  TValueType mValueMax;

  TValueType mBinWidth;
  std::vector<ggInt64> mCountBins;
  ggInt64 mCountTotal;

};

#endif // GGHISTOGRAMINTEGERT_H
