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
    mBinDelta(1),
    mCountBins(TCountBinCapacity, 0),
    mCountTotal(0) {
  }

  virtual ~ggHistogramIntegerT() override {
  }

  void Reset() {
    mValueMin = TValueType();
    mValueMax = TValueType();
    mBinDelta = 1;
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
      mBinDelta *= 2;
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
    return mCountBins[GetBinIndexInternalMin() + aBinIndex];
  }

  virtual ggInt64 GetCountF(const ggDouble& aValueF) const override {
    return mCountBins[GetBinIndexInternal(ggRound<TValueType>(aValueF))];
  }

  virtual ggInt64 GetBinIndexF(const ggDouble& aValueF) const override {
    return GetBinIndexInternal(ggRound<TValueType>(aValueF)) - GetBinIndexInternalMin();
  }

  virtual ggDouble GetBinValueF(ggInt64 aBinIndex) const override {
    return static_cast<ggDouble>((aBinIndex + GetBinIndexInternalMin()) * mBinDelta + GetBinIndexOffset());
  }

  virtual ggDouble GetBinDeltaF() const override {
    return static_cast<ggDouble>(mBinDelta);
  }

  virtual ggDouble GetValueMinF() const override {
    return static_cast<ggDouble>(mValueMin);
  }

  virtual ggDouble GetValueMaxF() const override {
    return static_cast<ggDouble>(mValueMax);
  }

private:

  static TValueType GetBinIndexOffset() {
    return -std::numeric_limits<TValueType>::min();
  }

  TValueType GetBinIndexScale() const {
    return mBinDelta;
  }

  inline ggInt64 GetBinIndexInternal(const TValueType& aValue) const {
    return (aValue - GetBinIndexOffset()) / mBinDelta;
  }

  inline ggInt64 GetBinIndexInternalMin() const {
    return GetBinIndexInternal(mValueMin);
  }

  inline ggInt64 GetBinIndexInternalMax() const {
    return GetBinIndexInternal(mValueMax);
  }

  TValueType mValueMin;
  TValueType mValueMax;

  TValueType mBinDelta;
  std::vector<ggInt64> mCountBins;
  ggInt64 mCountTotal;

};

#endif // GGHISTOGRAMINTEGERT_H
