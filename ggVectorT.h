#ifndef GGVECTORT_H
#define GGVECTORT_H

#include "ggAssert.h"
#include <math.h>


template <class TValueType,
          unsigned int TDimensions>
class ggVectorT
{
public:

  typedef TValueType tValueType;
  typedef TValueType value_type;

  inline static unsigned int GetDimensions() {
    return TDimensions;
  }

  inline ggVectorT() {
    for (unsigned int vDimension = 0; vDimension < GetDimensions(); vDimension++) {
      mValues[vDimension] = TValueType();
    }
  }

  inline ggVectorT(const ggVectorT& aOther) {
    for (unsigned int vDimension = 0; vDimension < GetDimensions(); vDimension++) {
      mValues[vDimension] = aOther.mValues[vDimension];
    }
  }

  inline ggVectorT& operator = (const  ggVectorT& aOther) {
    for (unsigned int vDimension = 0; vDimension < GetDimensions(); vDimension++) {
      mValues[vDimension] = aOther.mValues[vDimension];
    }
    return *this;
  }

  template <typename TConvertedValueType>
  inline ggVectorT<TConvertedValueType, TDimensions> GetConverted() const {
    ggVectorT<TConvertedValueType, TDimensions> vConverted;
    for (unsigned int vDimension = 0; vDimension < GetDimensions(); vDimension++) {
      vConverted[vDimension] = mValues[vDimension];
    }
    return vConverted;
  }

  template <typename TConvertedValueType>
  inline void SetConverted(const ggVectorT<TConvertedValueType, TDimensions>& aOther) {
    for (unsigned int vDimension = 0; vDimension < GetDimensions(); vDimension++) {
      mValues[vDimension] = aOther[vDimension];
    }
  }

  template <unsigned int TAdaptedDimensions>
  inline ggVectorT<TValueType, TAdaptedDimensions> GetAdaped() const {
    ggVectorT<TValueType, TAdaptedDimensions> vAdapted;
    for (unsigned int vDimension = 0; vDimension < vAdapted.GetDimensions(); vDimension++) {
      vAdapted[vDimension] = vDimension < GetDimensions() ? mValues[vDimension] : TValueType();
    }
    return vAdapted;
  }

  template <unsigned int TAdaptedDimensions>
  inline void SetAdaped(const ggVectorT<TValueType, TAdaptedDimensions>& aOther) {
    for (unsigned int vDimension = 0; vDimension < GetDimensions(); vDimension++) {
      mValues[vDimension] = vDimension < aOther.GetDimensions() ? aOther[vDimension] : TValueType();
    }
  }

  inline TValueType& operator [] (unsigned int aDimension) {
    GG_ASSERT(aDimension < GetDimensions());
    return mValues[aDimension];
  }

  inline const TValueType& operator [] (unsigned int aDimension) const {
    GG_ASSERT(aDimension < GetDimensions());
    return mValues[aDimension];
  }

  inline TValueType& Get(unsigned int aDimension) {
    return operator [] (aDimension);
  }

  inline const TValueType& Get(unsigned int aDimension) const {
    return operator [] (aDimension);
  }

  inline void Set(unsigned int aDimension, const TValueType& aValue) {
    operator [] (aDimension) = aValue;
  }

  inline bool operator == (const ggVectorT& aOther) const {
    for (unsigned int vDimension = 0; vDimension < GetDimensions(); vDimension++) {
      if (!(mValues[vDimension] == aOther.mValues[vDimension])) return false;
    }
    return true;
  }

  inline bool operator != (const ggVectorT& aOther) const {
    for (unsigned int vDimension = 0; vDimension < GetDimensions(); vDimension++) {
      if (mValues[vDimension] != aOther.mValues[vDimension]) return true;
    }
    return false;
  }

  template <typename TOtherVector>
  inline ggVectorT& operator += (const TOtherVector& aOther) {
    for (unsigned int vDimension = 0; vDimension < GetDimensions(); vDimension++) {
      (*this)[vDimension] += aOther[vDimension];
    }
    return *this;
  }

  template <typename TOtherVector>
  inline ggVectorT& operator -= (const TOtherVector& aOther) {
    for (unsigned int vDimension = 0; vDimension < GetDimensions(); vDimension++) {
      (*this)[vDimension] -= aOther[vDimension];
    }
    return *this;
  }

  template <typename TOtherValueType>
  inline ggVectorT& operator *= (const TOtherValueType& aOtherValue) {
    for (unsigned int vDimension = 0; vDimension < GetDimensions(); vDimension++) {
      (*this)[vDimension] *= aOtherValue;
    }
    return *this;
  }

  template <typename TOtherValueType>
  inline ggVectorT& operator /= (const TOtherValueType& aOtherValue) {
    for (unsigned int vDimension = 0; vDimension < GetDimensions(); vDimension++) {
      (*this)[vDimension] /= aOtherValue;
    }
    return *this;
  }

  inline ggVectorT operator + (const ggVectorT& aOther) const {
    ggVectorT vResult(*this);
    vResult += aOther;
    return vResult;
  }

  inline ggVectorT operator - (const ggVectorT& aOther) const {
    ggVectorT vResult(*this);
    vResult -= aOther;
    return vResult;
  }

  template <typename TOtherValueType>
  inline ggVectorT operator * (const TOtherValueType& aOtherValue) const {
    ggVectorT vResult(*this);
    vResult *= aOtherValue;
    return vResult;
  }

  template <typename TOtherValueType>
  inline ggVectorT operator / (const TOtherValueType& aOtherValue) const {
    ggVectorT vResult(*this);
    vResult /= aOtherValue;
    return vResult;
  }

  inline TValueType Dot(const ggVectorT& aOther) const {
    TValueType vResult = 0;
    for (unsigned int vDimension = 0; vDimension < GetDimensions(); vDimension++) {
      vResult += mValues[vDimension] * aOther.mValues[vDimension];
    }
    return vResult;
  }

  inline static TValueType Dot(const ggVectorT& aVectorA, const ggVectorT& aVectorB) {
    return aVectorA.Dot(aVectorB);
  }

  inline TValueType Length() const {
    return Sqrt(Dot(*this));
  }

  inline void Normalize() {
    TValueType vLength = Length();
    if (vLength != 0) operator /= (vLength);
  }

  inline ggVectorT Normalized() const {
    ggVectorT vResult(*this);
    vResult.Normalize();
    return vResult;
  }

private:

  template <typename T>
  inline static TValueType Sqrt(T aValue) {
    return sqrtf(aValue) + 0.5; // rounding from float to integer-types
  }

  inline static TValueType Sqrt(float aValue) {
    return sqrtf(aValue);
  }

  inline static TValueType Sqrt(double aValue) {
    return sqrt(aValue);
  }

  inline static TValueType Sqrt(long double aValue) {
    return sqrtl(aValue);
  }

  TValueType mValues[TDimensions];

};


template <typename TScalarType, class TValueType, unsigned int TDimensions>
ggVectorT<TValueType, TDimensions> operator * (const TScalarType& aScalar,
                                               const ggVectorT<TValueType, TDimensions>& aVector)
{
  return aVector * aScalar;
}


#endif // GGVECTORT_H
