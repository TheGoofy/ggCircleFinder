#ifndef GGVECTOR2T_H
#define GGVECTOR2T_H

#include "ggVectorT.h"

template <class TValueType,
          class TBaseVector = ggVectorT<TValueType, 2> >
class ggVector2T : public TBaseVector
{
public:

  inline ggVector2T()
  : TBaseVector() {
  }

  inline ggVector2T(const ggVector2T& aOther)
  : TBaseVector(aOther) {
  }

  inline ggVector2T(const TBaseVector& aOther)
  : TBaseVector(aOther) {
  }

  inline ggVector2T(const TValueType& aX,
                    const TValueType& aY) {
    (*this)[0] = aX;
    (*this)[1] = aY;
  }

  inline ggVector2T& operator = (const ggVector2T& aOther) {
    TBaseVector::operator = (aOther);
    return *this;
  }

  inline ggVector2T& operator = (const TBaseVector& aOther) {
    TBaseVector::operator = (aOther);
    return *this;
  }

  inline TValueType& X() {
    return (*this)[0];
  }

  inline TValueType& Y() {
    return (*this)[1];
  }

  inline const TValueType& X() const {
    return (*this)[0];
  }

  inline const TValueType& Y() const {
    return (*this)[1];
  }

  inline void SetX(const TValueType& aX) {
    (*this)[0] = aX;
  }

  inline void SetY(const TValueType& aY) {
    (*this)[1] = aY;
  }

  inline void Set(const TValueType& aX,
                  const TValueType& aY) {
    (*this)[0] = aX;
    (*this)[1] = aY;
  }

  inline ggVector2T Cross() const {
    return ggVector2T((*this)[1], -(*this)[0]);
  }

  inline static ggVector2T Cross(const ggVector2T& aVector) {
    return aVector.Cross();
  }

};

#endif // GGVECTOR2T_H
