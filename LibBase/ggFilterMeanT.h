#ifndef GGFILTERMEANT_H
#define GGFILTERMEANT_H

#include <vector>
#include "LibBase/ggAssert.h"
#include "LibBase/ggFilterT.h"

template <class TValueType>
class ggFilterMeanT : public ggFilterT<TValueType> {

public:

  ggFilterMeanT(ggUSize aLength)
  : mLength(aLength),
    mIndex(static_cast<ggUSize>(-1)),
    mValues() {
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

    // calculate mean value
    TValueType vSum = mValues.front();
    std::for_each(mValues.begin() + 1, mValues.end(), [&vSum] (const TValueType& aValue) {
      vSum += aValue;
    });
    mOutputValue = vSum / static_cast<ggSize>(mValues.size());

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

  TValueType mOutputValue;

};

typedef ggFilterMeanT<ggInt32> ggFilterMeanInt32;
typedef ggFilterMeanT<ggFloat> ggFilterMeanFloat;
typedef ggFilterMeanT<ggDouble> ggFilterMeanDouble;

#endif // GGFILTERMEANT_H
