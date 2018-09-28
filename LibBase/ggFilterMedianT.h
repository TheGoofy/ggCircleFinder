#ifndef GGFILTERMEDIANT_H
#define GGFILTERMEDIANT_H

#include <vector>
#include "LibBase/ggAssert.h"
#include "LibBase/ggFilterT.h"

template <class TValueType, class TLess = std::less<TValueType>>
class ggFilterMedianT : public ggFilterT<TValueType> {

public:

  ggFilterMedianT(ggUSize aLength)
  : mLength(aLength),
    mIndex(static_cast<ggUSize>(-1)),
    mValues(),
    mLess(mLessDefault) {
  }

  ggFilterMedianT(ggUSize aLength, const TLess& aLess)
  : mLength(aLength),
    mIndex(static_cast<ggUSize>(-1)),
    mValues(),
    mLess(aLess) {
  }

  virtual void Reset() override {
    mValues.clear();
    mIndex = static_cast<ggUSize>(-1);
  }

  virtual const TValueType& Filter(const TValueType& aInputValue) override {

    // store input values in ring-buffer
    mIndex = (mIndex + 1) % mLength;
    mValues.resize(std::max(mValues.size(), mIndex + 1));
    mValues[mIndex] = aInputValue;

    // copy buffer, partially sort and get median
    std::vector<TValueType> vValuesSorted(mValues);
    std::nth_element(vValuesSorted.begin(),
                     vValuesSorted.begin() + vValuesSorted.size() / 2,
                     vValuesSorted.end(),
                     mLess);
    mOutputValue = vValuesSorted[vValuesSorted.size() / 2];

    // return the median
    return mOutputValue;
  }

  virtual const TValueType& GetIn() const override {
    GG_ASSERT(mValues.size() > 0);
    return mValues[mIndex];
  }

  virtual const TValueType& GetOut() const override {
    GG_ASSERT(mValues.size() > 0);
    return mOutputValue;
  }

private:

  ggUSize mLength;
  ggUSize mIndex;
  std::vector<TValueType> mValues;
  const TLess& mLess;
  static TLess mLessDefault;

  TValueType mOutputValue;

};

template <class TValueType, class TLess>
TLess ggFilterMedianT<TValueType, TLess>::mLessDefault;

typedef ggFilterMedianT<ggInt32> ggFilterMedianInt32;
typedef ggFilterMedianT<ggFloat> ggFilterMedianFloat;
typedef ggFilterMedianT<ggDouble> ggFilterMedianDouble;

#endif // GGFILTERMEDIANT_H
