#ifndef GGFILTERT_H
#define GGFILTERT_H

#include "LibBase/ggTypes.h"

template <class TValueType>
class ggFilterInterfaceT {

public:

  virtual void Reset() = 0;
  virtual const TValueType& PushIn(const TValueType& aInputValue) = 0;
  virtual const TValueType& GetOut() const = 0;

};

typedef ggFilterInterfaceT<ggInt32> ggFilterInterfaceInt32;
typedef ggFilterInterfaceT<ggFloat> ggFilterInterfaceFloat;
typedef ggFilterInterfaceT<ggDouble> ggFilterInterfaceDouble;

#include <vector>

template <class TValueType>
class ggFilterMedianT : public ggFilterInterfaceT<TValueType> {

public:

  ggFilterMedianT(ggUSize aLength)
  : mLength(aLength),
    mIndex(0),
    mValues() {
  }

  virtual void Reset() override {
    mValues.clear();
    mIndex = 0;
  }

  virtual const TValueType& PushIn(const TValueType& aInputValue) override {

    // store input values in ring-buffer
    mValues.resize(std::max(mValues.size(), mIndex + 1));
    mValues[mIndex] = aInputValue;
    mIndex = (mIndex + 1) % mLength;

    // copy buffer, partially sort and get median
    std::vector<TValueType> vValuesSorted(mValues);
    std::nth_element(vValuesSorted.begin(), vValuesSorted.begin() + vValuesSorted.size() / 2, vValuesSorted.end());
    mOutputValue = vValuesSorted[vValuesSorted.size() / 2];

    // return the median
    return mOutputValue;
  }

  virtual const TValueType& GetOut() const override {
    return mOutputValue;
  }

private:

  ggUSize mLength;
  ggUSize mIndex;
  std::vector<TValueType> mValues;

  TValueType mOutputValue;

};

typedef ggFilterMedianT<ggInt32> ggFilterMedianInt32;
typedef ggFilterMedianT<ggFloat> ggFilterMedianFloat;
typedef ggFilterMedianT<ggDouble> ggFilterMedianDouble;

#endif // GGFILTERT_H
