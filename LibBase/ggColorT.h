#ifndef GGCOLORT_H
#define GGCOLORT_H

#include "ggVectorT.h"

template <class TValueType,
          class TBaseVector = ggVectorT<TValueType, 4> >
class ggColorT : public TBaseVector
{
public:

  inline ggColorT()
  : TBaseVector() {
    (*this)[0] = MinLimit();
    (*this)[1] = MinLimit();
    (*this)[2] = MinLimit();
    (*this)[3] = MaxLimit();
  }

  inline ggColorT(const ggColorT& aOther)
  : TBaseVector(aOther) {
  }

  inline ggColorT(const TBaseVector& aOther)
    : TBaseVector(aOther) {
  }

  inline ggColorT(const TValueType& aR,
                  const TValueType& aG,
                  const TValueType& aB) {
    (*this)[0] = aR;
    (*this)[1] = aG;
    (*this)[2] = aB;
    (*this)[3] = MaxLimit();
  }

  inline ggColorT(const TValueType& aR,
                  const TValueType& aG,
                  const TValueType& aB,
                  const TValueType& aA) {
    (*this)[0] = aR;
    (*this)[1] = aG;
    (*this)[2] = aB;
    (*this)[3] = aA;
  }

  inline ggColorT& operator = (const ggColorT& aOther) {
    TBaseVector::operator = (aOther);
    return *this;
  }

  inline ggColorT& operator = (const TBaseVector& aOther) {
    TBaseVector::operator = (aOther);
    return *this;
  }

  inline TValueType& R() {
    return (*this)[0];
  }

  inline TValueType& G() {
    return (*this)[1];
  }

  inline TValueType& B() {
    return (*this)[2];
  }

  inline TValueType& A() {
    return (*this)[3];
  }

  inline const TValueType& R() const {
    return (*this)[0];
  }

  inline const TValueType& G() const {
    return (*this)[1];
  }

  inline const TValueType& B() const {
    return (*this)[2];
  }

  inline const TValueType& A() const {
    return (*this)[3];
  }

  inline void SetR(const TValueType& aR) {
    (*this)[0] = aR;
  }

  inline void SetG(const TValueType& aG) {
    (*this)[1] = aG;
  }

  inline void SetB(const TValueType& aB) {
    (*this)[2] = aB;
  }

  inline void SetA(const TValueType& aA) {
    (*this)[3] = aA;
  }

  inline void Set(const TValueType& aR,
                  const TValueType& aG,
                  const TValueType& aB) {
    (*this)[0] = aR;
    (*this)[1] = aG;
    (*this)[2] = aB;
  }

  inline void Set(const TValueType& aR,
                  const TValueType& aG,
                  const TValueType& aB,
                  const TValueType& aA) {
    (*this)[0] = aR;
    (*this)[1] = aG;
    (*this)[2] = aB;
    (*this)[3] = aA;
  }

  inline static TValueType MinLimit() {
    return std::numeric_limits<TValueType>::lowest();
  }

  inline static TValueType MaxLimit() {
    return std::numeric_limits<TValueType>::max();
  }

  inline static TValueType MidLimit() {
    return (MinLimit() + MaxLimit()) / 2;
  }

  template <typename TConvertedValueType>
  inline ggColorT<TConvertedValueType> GetConverted() const {
    ggColorT<TConvertedValueType> vColorConverted;
    double vScale = static_cast<double>(vColorConverted.MaxLimit()) / static_cast<double>(MaxLimit());
    for (unsigned int vDimension = 0; vDimension < TBaseVector::GetDimensions(); vDimension++) {
      vColorConverted[vDimension] = vScale * (*this)[vDimension];
    }
    return vColorConverted;
  }

  template <typename TConvertedValueType>
  inline void SetConverted(const ggColorT<TConvertedValueType>& aOther) {
    double vScale = static_cast<double>(MaxLimit()) / static_cast<double>(aOther.MaxLimit());
    for (unsigned int vDimension = 0; vDimension < TBaseVector::GetDimensions(); vDimension++) {
      (*this)[vDimension] = vScale * aOther[vDimension];
    }
  }

  inline void ClampRGB() {
    Clamp((*this)[0]);
    Clamp((*this)[1]);
    Clamp((*this)[2]);
  }

  inline void ClampRGBA() {
    Clamp((*this)[0]);
    Clamp((*this)[1]);
    Clamp((*this)[2]);
    Clamp((*this)[3]);
  }

  inline ggColorT ClampedRGB() const {
    ggColorT vColor(*this);
    vColor.ClampRGB();
    return vColor;
  }

  inline ggColorT ClampedRGBA() const {
    ggColorT vColor(*this);
    vColor.ClampRGBA();
    return vColor;
  }

  inline void Saturize() {
    double vMax = static_cast<double>(TBaseVector::Max());
    if (vMax != 0.0) {
      (*this) *= static_cast<double>(MaxLimit()) / vMax;
      ClampRGBA();
    }
  }

  inline ggColorT Saturized() const {
    ggColorT vColor(*this);
    vColor.Saturize();
    return vColor;
  }

  inline TValueType Luma() const {
    return 0.299 * R() + 0.587 * G() + 0.114 * B();
  }

  inline ggColorT Contrast() const {
    return Luma() < MidLimit() ? White() : Black();
  }

  inline static ggColorT Black()   { return ggColorT(MinLimit(), MinLimit(), MinLimit()); }
  inline static ggColorT Blue()    { return ggColorT(MinLimit(), MinLimit(), MaxLimit()); }
  inline static ggColorT Green()   { return ggColorT(MinLimit(), MaxLimit(), MinLimit()); }
  inline static ggColorT Cyan()    { return ggColorT(MinLimit(), MaxLimit(), MaxLimit()); }
  inline static ggColorT Red()     { return ggColorT(MaxLimit(), MinLimit(), MinLimit()); }
  inline static ggColorT Magenta() { return ggColorT(MaxLimit(), MinLimit(), MaxLimit()); }
  inline static ggColorT Yellow()  { return ggColorT(MaxLimit(), MaxLimit(), MinLimit()); }
  inline static ggColorT White()   { return ggColorT(MaxLimit(), MaxLimit(), MaxLimit()); }

  inline static ggColorT Gray()    { return ggColorT(MidLimit(), MidLimit(), MidLimit()); }
  inline static ggColorT SkyBlue() { return ggColorT(MinLimit(), MidLimit(), MaxLimit()); }
  inline static ggColorT Violet()  { return ggColorT(MidLimit(), MinLimit(), MaxLimit()); }
  inline static ggColorT Mint()    { return ggColorT(MinLimit(), MaxLimit(), MidLimit()); }
  inline static ggColorT Lime()    { return ggColorT(MidLimit(), MaxLimit(), MinLimit()); }
  inline static ggColorT Purple()  { return ggColorT(MaxLimit(), MinLimit(), MidLimit()); }
  inline static ggColorT Orange()  { return ggColorT(MaxLimit(), MidLimit(), MinLimit()); }

private:

  inline static void Clamp(TValueType& aValue) {
    Clamp(aValue, MinLimit(), MaxLimit());
  }

  inline static void Clamp(TValueType &aValue,
                           const TValueType& aValueMin,
                           const TValueType& aValueMax) {
    aValue = (aValue < aValueMin ? aValueMin : (aValue > aValueMax ? aValueMax : aValue));
  }

};

template <>
inline float ggColorT<float>::MinLimit() {
  return 0;
}

template <>
inline double ggColorT<double>::MinLimit() {
  return 0;
}

template <>
inline long double ggColorT<long double>::MinLimit() {
  return 0;
}

template <>
inline float ggColorT<float>::MaxLimit() {
  return 1;
}

template <>
inline double ggColorT<double>::MaxLimit() {
  return 1;
}

template <>
inline long double ggColorT<long double>::MaxLimit() {
  return 1;
}

#endif // GGCOLORT_H
