#ifndef GGFILTERT_H
#define GGFILTERT_H

#include "LibBase/ggNumberTypes.h"

template <class TValueType>
class ggFilterInterfaceT {

public:

  virtual void Reset() = 0;
  virtual const TValueType& Filter(const TValueType& aInputValue) = 0;
  virtual const TValueType& GetIn() const = 0;
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

    // copy buffer, partially sort and get median
    std::vector<TValueType> vValuesSorted(mValues);
    std::nth_element(vValuesSorted.begin(), vValuesSorted.begin() + vValuesSorted.size() / 2, vValuesSorted.end());
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

  TValueType mOutputValue;

};

typedef ggFilterMedianT<ggInt32> ggFilterMedianInt32;
typedef ggFilterMedianT<ggFloat> ggFilterMedianFloat;
typedef ggFilterMedianT<ggDouble> ggFilterMedianDouble;


template <class TValueType>
class ggFilterMeanT : public ggFilterInterfaceT<TValueType> {

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
    mValues.resize(std::max(mValues.size(), mIndex + 1));
    mValues[mIndex] = aInputValue;
    mIndex = (mIndex + 1) % mLength;

    // calculate mean value
    ggDouble vSum = 0;
    std::for_each(mValues.begin(), mValues.end(), [&vSum] (const TValueType& aValue) {
      vSum += aValue;
    });
    mOutputValue = vSum / mValues.size();

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

#endif // GGFILTERT_H
