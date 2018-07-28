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
    mIndex(0),
    mValues() {
  }

  virtual void Reset() override {
    mValues.clear();
    mIndex = 0;
  }

  virtual const TValueType& Filter(const TValueType& aInputValue) override {

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
    mIndex(0),
    mValues() {
  }

  virtual void Reset() override {
    mValues.clear();
    mIndex = 0;
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


template <class TValueType>
class ggFilterMedianBesiT : public ggFilterInterfaceT<TValueType> {

public:

  ggFilterMedianBesiT(const TValueType& aLimit)
  : mLimit(aLimit) {
    Reset();
  }

  virtual void Reset() override {
    mRepeat = false;
    mX0 = TValueType();
    mX1 = TValueType();
    mX2 = TValueType();
    mXAct = TValueType();
    mXCorr = TValueType();
  }

  virtual const TValueType& Filter(const TValueType& aXAct) override {

    mXAct = aXAct;

    if (mRepeat) {
      mX2 = mX1;
      mX1 = mX0;
      mX0 = mXAct;
      mXCorr = GetMedian(mX0, mX1, mX2);
      mRepeat = false;
    }
    else if (GetAbs(mXAct - mX0) >= mLimit) {
      mX1 = mX0;
      mX0 = mXAct;
      mXCorr = GetMedian(mX0, mX1);
      mRepeat = true;
    }
    else {
      mX0 = mXAct;
      mXCorr = mX0;
    }

    return mXCorr;
  }

  virtual const TValueType& GetIn() const override {
    return mXAct;
  }

  virtual const TValueType& GetOut() const override {
    return mXCorr;
  }

private:

  static TValueType GetAbs(const TValueType& aX) {
    return aX < TValueType() ? -aX : aX;
  }

  static TValueType GetMedian(const TValueType& aX0,
                              const TValueType& aX1) {
    return (aX0 + aX1) / 2;
  }

  static const TValueType& GetMedian(const TValueType& aX0,
                                     const TValueType& aX1,
                                     const TValueType& aX2) {
    if (aX0 < aX1) {
      if (aX1 < aX2) return aX1;
      else if (aX0 < aX2) return aX2;
      else return aX0;
    }
    else {
      if (aX2 < aX1) return aX1;
      else if (aX0 < aX2) return aX0;
      else return aX2;
    }
  }

  const TValueType mLimit;

  bool mRepeat;

  TValueType mX0;
  TValueType mX1;
  TValueType mX2;

  TValueType mXAct;
  TValueType mXCorr;

};

typedef ggFilterMedianBesiT<ggInt32> ggFilterMedianBesiInt32;
typedef ggFilterMedianBesiT<ggFloat> ggFilterMedianBesiFloat;
typedef ggFilterMedianBesiT<ggDouble> ggFilterMedianBesiDouble;

#endif // GGFILTERT_H
