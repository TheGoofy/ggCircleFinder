#ifndef GGVECTOR3T_H
#define GGVECTOR3T_H

#include "ggVectorT.h"

template <class TValueType,
          class TBaseVector = ggVectorT<TValueType, 3> >
class ggVector3T : public TBaseVector
{
public:

  inline ggVector3T()
  : TBaseVector() {
  }

  inline ggVector3T(const ggVector3T& aOther)
  : TBaseVector(aOther) {
  }

  inline ggVector3T(const TBaseVector& aOther)
    : TBaseVector(aOther) {
  }

  inline ggVector3T(const TValueType& aX,
                    const TValueType& aY,
                    const TValueType& aZ) {
    (*this)[0] = aX;
    (*this)[1] = aY;
    (*this)[2] = aZ;
  }

  inline ggVector3T& operator = (const ggVector3T& aOther) {
    TBaseVector::operator = (aOther);
    return *this;
  }

  inline ggVector3T& operator = (const TBaseVector& aOther) {
    TBaseVector::operator = (aOther);
    return *this;
  }

  inline TValueType& X() {
    return (*this)[0];
  }

  inline TValueType& Y() {
    return (*this)[1];
  }

  inline TValueType& Z() {
    return (*this)[2];
  }

  inline const TValueType& X() const {
    return (*this)[0];
  }

  inline const TValueType& Y() const {
    return (*this)[1];
  }

  inline const TValueType& Z() const {
    return (*this)[2];
  }

  inline void SetX(const TValueType& aX) {
    (*this)[0] = aX;
  }

  inline void SetY(const TValueType& aY) {
    (*this)[1] = aY;
  }

  inline void SetZ(const TValueType& aZ) {
    (*this)[3] = aZ;
  }

  inline void Set(const TValueType& aX,
                  const TValueType& aY,
                  const TValueType& aZ) {
    (*this)[0] = aX;
    (*this)[1] = aY;
    (*this)[2] = aZ;
  }

  inline ggVector3T Cross(const ggVector3T& aOther) const {
    return ggVector3T((*this)[1]*aOther[2] - (*this)[2]*aOther[1],
                      (*this)[2]*aOther[0] - (*this)[0]*aOther[2],
                      (*this)[0]*aOther[1] - (*this)[1]*aOther[0]);
  }

  inline static ggVector3T Cross(const ggVector3T& aVectorA, const ggVector3T& aVectorB) {
    return aVectorA.Cross(aVectorB);
  }

};

#endif // GGVECTOR3T_H
