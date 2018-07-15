#ifndef GGGEOMETRY_H
#define GGGEOMETRY_H

#include "ggTypes.h"
#include "ggVectorT.h"
#include "ggVector2T.h"
#include "ggVector3T.h"
#include "ggLineTypes.h"

namespace ggGeometry {


  template <typename TValueType>
  ggDouble GetAngle(const ggVector2T<TValueType>& aVectorA,
                    const ggVector2T<TValueType>& aVectorB) {
    ggVector2T<TValueType> vNormalAN(aVectorA.Cross());
    ggDouble vCosA = aVectorB.Dot(aVectorA);
    ggDouble vSinA = -aVectorB.Dot(vNormalAN);
    return atan2(vSinA, vCosA);
  }


  template <typename TValueType>
  ggDouble GetAngle(const ggVector3T<TValueType>& aVectorA,
                    const ggVector3T<TValueType>& aVectorB) {
    ggVector3T<TValueType> vNormalAB(aVectorA.Cross(aVectorB).Normalized());
    ggVector3T<TValueType> vNormalAN(aVectorA.Cross(vNormalAB));
    ggDouble vCosA = aVectorB.Dot(aVectorA);
    ggDouble vSinA = -aVectorB.Dot(vNormalAN);
    return atan2(vSinA, vCosA);
  }


  template <typename TValueType>
  ggVector2T<TValueType> Rotate(const ggVector2T<TValueType>& aVector,
                                ggDouble aAngle) {
    ggDouble vCos = cos(aAngle);
    ggDouble vSin = sin(aAngle);
    return ggVector2T<TValueType>(aVector[0] * vCos - aVector[1] * vSin,
                                  aVector[0] * vSin + aVector[1] * vCos);
  }


  template <typename TValueType>
  bool Intersect(const ggLine2T<TValueType>& aLineA,
                 const ggLine2T<TValueType>& aLineB,
                 ggVector2T<TValueType>& aIntersection)
  {
    const typename ggLine2T<TValueType>::tVector& vDirA(aLineA.Dir());
    const typename ggLine2T<TValueType>::tVector& vDirB(aLineB.Dir());
    const TValueType vDenomA = vDirB.X() * vDirA.Y() - vDirA.X() * vDirB.Y();
    const TValueType vDenomB = vDirA.X() * vDirB.Y() - vDirB.X() * vDirA.Y();
    if ((vDenomA != 0) && (fabs(vDenomA) > fabs(vDenomB))) {
      const typename ggLine2T<TValueType>::tVector vDelta(aLineB.Pos0() - aLineA.Pos0());
      const TValueType vNomA = vDirB.X() * vDelta.Y() - vDelta.X() * vDirB.Y();
      aIntersection = aLineA.Pos0() + (vNomA / vDenomA) * vDirA;
      return true;
    }
    else if (vDenomB != 0) {
      const typename ggLine2T<TValueType>::tVector vDelta(aLineA.Pos0() - aLineB.Pos0());
      const TValueType vNomB = vDirA.X() * vDelta.Y() - vDelta.X() * vDirA.Y();
      aIntersection = aLineB.Pos0() + (vNomB / vDenomB) * vDirB;
      return true;
    }
    else {
      return false;
    }
  }


}

#endif // GGGEOMETRY_H
