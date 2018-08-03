#ifndef GGHISTOGRAMFIXEDT_H
#define GGHISTOGRAMFIXEDT_H

#include <vector>
#include <math.h>

#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggRound.h"
#include "LibImage/ggHistogram.h"


/**
 * Histogram with fixed range ans fixed number of bins. Values outside of the range are
 * counted in the border-bins.
 */
template <class TValueType>
class ggHistogramFixedT : public ggHistogram
{

public:

  ggHistogramFixedT(ggInt64 aNumberOfBins = 256)
  : mValueMin(),
    mValueMax(),
    mBinValueMin(0),
    mBinWidth(0),
    mCountBins(static_cast<ggUSize>(aNumberOfBins), 0),
    mCountTotal(0) {
    AdjustBinRange();
  }

  ggHistogramFixedT(ggInt64 aNumberOfBins,
                    const TValueType& aValueRangeMin,
                    const TValueType& aValueRangeMax)
  : mValueMin(),
    mValueMax(),
    mBinValueMin(0),
    mBinWidth(0),
    mCountBins(static_cast<ggUSize>(aNumberOfBins), 0),
    mCountTotal(0) {
    AdjustBinRange(aValueRangeMin, aValueRangeMax);
  }

  virtual ~ggHistogramFixedT() override {
  }

  inline void Reset() {
    mValueMin = TValueType();
    mValueMax = TValueType();
    std::fill(mCountBins.begin(), mCountBins.end(), 0);
    mCountTotal = 0;
  }

  inline void Reset(const TValueType& aValueRangeMin,
                    const TValueType& aValueRangeMax) {
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
    return mCountBins.size();
  }

  virtual ggInt64 GetCountTotal() const override {
    return mCountTotal;
  }

  virtual ggInt64 GetCount(ggInt64 aBinIndex) const override {
    if ((aBinIndex < 0) || (aBinIndex > GetBinIndexMax())) return 0;
    else return mCountBins[aBinIndex];
  }

  inline ggInt64 GetCountV(const TValueType& aValue) const {
    return GetCount(GetBinIndexFromValueV(aValue));
  }

  virtual ggInt64 GetCountF(const ggDouble& aValueF) const override {
    return GetCount(GetBinIndexFromValueF(aValueF));
  }

  virtual ggInt64 GetBinIndexF(const ggDouble& aValueF) const override {
    return ggClamp<ggInt64>(GetBinIndexFromValueF(aValueF), 0, GetBinIndexMax());
  }

  virtual ggDouble GetBinValueF(ggInt64 aBinIndex) const override {
    return mBinValueMin + aBinIndex * mBinWidth;
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
    // update value min/max
    if (mCountTotal == 0) {
      mValueMin = aValue;
      mValueMax = aValue;
    }
    else {
      if (aValue < mValueMin) mValueMin = aValue;
      if (aValue > mValueMax) mValueMax = aValue;
    }

    // increment bin count
    ggInt64 vBinIndex = GetBinIndexFromValueV(aValue);
    vBinIndex = ggClamp<ggInt64>(vBinIndex, 0, GetBinIndexMax());
    mCountBins[static_cast<ggUSize>(vBinIndex)] += aCount;

    // increment total
    mCountTotal += aCount;
  }

  inline ggInt64 GetBinIndexMax() const {
    return static_cast<ggInt64>(mCountBins.size()) - 1;
  }

  inline ggInt64 GetBinIndexFromValueF(ggDouble aValueF) const {
    return ggRound<ggInt64>((aValueF - mBinValueMin) / mBinWidth);
  }

  inline ggInt64 GetBinIndexFromValueV(const TValueType& aValue) const {
    return GetBinIndexFromValueF(ggRound<ggDouble>(aValue));
  }

  inline static TValueType GetMinLimit() {
    return std::is_floating_point<TValueType>() ? 0 : std::numeric_limits<TValueType>::lowest();
  }

  inline static TValueType GetMaxLimit() {
    return std::is_floating_point<TValueType>() ? 1 : std::numeric_limits<TValueType>::max();
  }

  inline static ggDouble GetExtraRange() {
    return std::is_floating_point<TValueType>() ? 0.0 : 1.0;
  }

  inline void AdjustBinRange() {
    AdjustBinRange(GetMinLimit(), GetMaxLimit());
  }

  inline void AdjustBinRange(const TValueType& aValueRangeMin,
                             const TValueType& aValueRangeMax) {
    ggDouble vValueRangeMin = ggRound<ggDouble>(aValueRangeMin);
    ggDouble vValueRangeMax = ggRound<ggDouble>(aValueRangeMax);
    mBinWidth = (vValueRangeMax - vValueRangeMin + GetExtraRange()) / GetNumberOfBins();
    mBinValueMin = vValueRangeMin + mBinWidth / 2.0;
  }

  TValueType mValueMin;
  TValueType mValueMax;

  ggDouble mBinValueMin;
  ggDouble mBinWidth;
  std::vector<ggInt64> mCountBins;
  ggInt64 mCountTotal;

};

#endif // GGHISTOGRAMFIXEDT_H
