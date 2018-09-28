#ifndef GGFILTERCENTERT_H
#define GGFILTERCENTERT_H

#include <vector>
#include "LibBase/ggAssert.h"
#include "LibBase/ggFilterT.h"

/**
 * Default distance calculator
 */
template <class TValueType,
          class TScalar = TValueType>
struct ggFilterCenterDistanceT {
  TScalar operator () (const TValueType& aValueA, const TValueType& aValueB) const {
    return aValueA < aValueB ? aValueB - aValueA : aValueA - aValueB;
  }
};

/**
 * Finds the sample where the total distance to all other samples is minimal.
 * Note that the calculation effort is O(n^2)
 */
template <class TValueType,
          class TScalar = TValueType,
          class TDistance = ggFilterCenterDistanceT<TValueType>>
class ggFilterCenterT : public ggFilterT<TValueType> {

public:

  ggFilterCenterT(ggUSize aLength)
  : mLength(aLength),
    mIndex(static_cast<ggUSize>(-1)),
    mValues(),
    mDistance(mDistanceDefault) {
  }

  ggFilterCenterT(ggUSize aLength, const TDistance& aDistance)
  : mLength(aLength),
    mIndex(static_cast<ggUSize>(-1)),
    mValues(),
    mDistance(aDistance) {
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

    // find smallest of all sums of distances
    ggUSize vIndexMin = 0;
    TScalar vDistanceTotalMin = 0;
    for (ggUSize vIndexA = 0; vIndexA < mValues.size(); vIndexA++) {

      // calculate sum of all distances
      TScalar vDistanceTotal = 0;
      for (ggUSize vIndexB = 0; vIndexB < mValues.size(); vIndexB++) {
        vDistanceTotal += mDistance(mValues[vIndexA], mValues[vIndexB]);
      }

      // use the first iteration for initialisation, later iterations will test if "less"
      if ((vIndexA == 0) || (vDistanceTotal < vDistanceTotalMin)) {
        vDistanceTotalMin = vDistanceTotal;
        vIndexMin = vIndexA;
      }
    }

    // store center element as output
    mOutputValue = mValues[vIndexMin];

    // return the center
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
  const TDistance& mDistance;
  static TDistance mDistanceDefault;

  TValueType mOutputValue;

};

template <class TValueType, class TScalar, class TDistance>
TDistance ggFilterCenterT<TValueType, TScalar, TDistance>::mDistanceDefault;

#endif // GGFILTERCENTERT_H
