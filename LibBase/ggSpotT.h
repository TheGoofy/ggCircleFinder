#ifndef GGPOINTT_H
#define GGPOINTT_H

#include "ggVectorT.h"

template <class TSpotValueType,
          class TBaseVectorType>
class ggSpotT : public TBaseVectorType
{
public:

  typedef TBaseVectorType tVectorType;

  inline ggSpotT()
  : TBaseVectorType(),
    mValue() {
  }

  inline ggSpotT(const ggSpotT& aOther)
  : TBaseVectorType(aOther),
    mValue(aOther.mValue) {
  }

  inline ggSpotT(const TBaseVectorType& aOther)
  : TBaseVectorType(aOther),
    mValue() {
  }

  inline ggSpotT(const TBaseVectorType& aOther, const TSpotValueType& aValue)
  : TBaseVectorType(aOther),
    mValue(aValue) {
  }

  inline TSpotValueType& GetValue() {
    return mValue;
  }

  inline const TSpotValueType& GetValue() const {
    return mValue;
  }

  inline TSpotValueType& operator * () {
    return mValue;
  }

  inline const TSpotValueType& operator * () const {
    return mValue;
  }

  inline void SetValue(const TSpotValueType& aValue) {
    mValue = aValue;
  }

  inline bool operator < (const ggSpotT& aOther) const {
    return mValue < aOther.mValue;
  }

  inline bool operator > (const ggSpotT& aOther) const {
    return mValue > aOther.mValue;
  }

private:

  TSpotValueType mValue;

};

#endif // GGPOINTT_H
