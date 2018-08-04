#ifndef GGIMAGEALGORITHM_H
#define GGIMAGEALGORITHM_H

#include <algorithm>

#include "LibBase/ggSpotT.h"
#include "LibBase/ggGeometry.h"
#include "LibBase/ggAveragesT.h"
#include "LibImage/ggImageFilter.h"
#include "LibImage/ggImagePainterT.h"


namespace ggImageAlgorithm {


  template <typename TValueType>
  void Invert(ggImageT<TValueType>& aImage,
              const TValueType& aReference) {
    aImage.ProcessValues([aReference] (TValueType& aValue) {
      aValue = aReference - aValue;
    });
  }


  template <typename TValueType>
  void Invert(ggImageT<TValueType>& aImage) {
    Invert(aImage, aImage.GetMinLimit() + aImage.GetMaxLimit());
  }


  template <typename TValueType>
  ggImageT<TValueType> Inverted(const ggImageT<TValueType>& aImage,
                                const TValueType& aReference) {
    ggImageT<TValueType> vImageInverted(aImage);
    vImageInverted.Invert(aReference);
    return vImageInverted;
  }


  template <typename TValueType>
  ggImageT<TValueType> Inverted(const ggImageT<TValueType>& aImage) {
    ggImageT<TValueType> vImageInverted(aImage);
    vImageInverted.Invert();
    return vImageInverted;
  }


  template <typename TValueType>
  void MirrorX(ggImageT<TValueType>& aImage) {
    for (ggSize vIndexY = 0; vIndexY < aImage.GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < aImage.GetSizeX() / 2; vIndexX++) {
        std::swap(aImage(vIndexX, vIndexY), aImage(aImage.GetSizeX()-vIndexX-1), vIndexY);
      }
    }
  }


  template <typename TValueType>
  void MirrorY(ggImageT<TValueType>& aImage) {
    for (ggSize vIndexY = 0; vIndexY < aImage.GetSizeY() / 2; vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < aImage.GetSizeX(); vIndexX++) {
        std::swap(aImage(vIndexX, vIndexY), aImage(vIndexX, aImage.GetSizeY()-vIndexY-1));
      }
    }
  }


  template <typename TValueType>
  void Clamp(ggImageT<TValueType>& aImage,
             const TValueType& aValueMin,
             const TValueType& aValueMax) {
    aImage.ProcessValues([aValueMin, aValueMax] (TValueType& aValue) {
      aValue = aValue < aValueMin ? aValueMin : (aValueMax < aValue ? aValueMax : aValue);
    });
  }


  template <typename TValueType>
  ggImageT<ggFloat> CalculateHoughImage(const ggImageT<TValueType>& aImage,
                                        const ggDouble aCircleModelDiameter,
                                        const ggDouble aCircleModelLineThickness)
  {
    // calculate gradient vector field
    ggImageT<ggVector2Float> vGradientVectorField(aImage.GetSize());
    ggImageFilter::Gradient(vGradientVectorField, aImage);

    // do gradient based hough transformation
    ggImageT<ggFloat> vImageHough(aImage.GetSize(), 0.0f);
    ggImagePainterT<ggFloat> vPainterHough(vImageHough);
    for (ggSize vIndexY = 1; vIndexY+1 < vGradientVectorField.GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 1; vIndexX+1 < vGradientVectorField.GetSizeX(); vIndexX++) {
        const ggVector2Double vGradient(vGradientVectorField(vIndexX, vIndexY).GetConverted<ggDouble>());
        if (vGradient.Length() > 0.0) {

          // try to figure out some sort of circle curvature depending on the neighbor gradients.
          // - triangulate a virtual center-point (accuracy might be a problem due to the close
          //   proximity of the two triangulation points). nevertheless the virtual center can be used
          //   as indicator, on which side to expect the circle center
          // - the two triangulation points are selected perpendicular to the actual gradient

          // calculate the two triangulation points A and B
          bool vDiagonalQ1 = vGradient.X() * vGradient.Y() > 0.0;
          bool vHorizontal = fabs(vGradient.X()) > fabs(vGradient.Y());
          ggDouble vSlope = vHorizontal ? (vGradient.Y() / vGradient.X()) : (vGradient.X() / vGradient.Y());
          // position of two (intersecting) lines
          ggVector2Size vIndexA(vIndexX, vIndexY);
          ggVector2Size vIndexB(vIndexX, vIndexY);
          if (fabs(vSlope) < 0.4142) { // tan(22.5) = sqrt(2) - 1
            vIndexA += ggVector2Int32(vHorizontal ? 0 : 1, vHorizontal ? 1 : 0);
            vIndexB += ggVector2Int32(vHorizontal ? 0 : -1, vHorizontal ? -1 : 0);
          }
          else {
            vIndexA += ggVector2Int32(vDiagonalQ1 ? -1 : 1, vDiagonalQ1 ? 1 : 1);
            vIndexB += ggVector2Int32(vDiagonalQ1 ? 1 : -1, vDiagonalQ1 ? -1 : -1);
          }

          // direction of two (intersecting) lines
          const ggVector2Double vGradientA(vGradientVectorField(vIndexA).GetConverted<ggDouble>());
          const ggVector2Double vGradientB(vGradientVectorField(vIndexB).GetConverted<ggDouble>());

          // two lines potentially intersect at the virtual center-point
          ggLine2Double vLineA = ggLine2Double::FromDir(vIndexA.GetConverted<ggDouble>(), vGradientA);
          ggLine2Double vLineB = ggLine2Double::FromDir(vIndexB.GetConverted<ggDouble>(), vGradientB);
          ggVector2Double vIntersection;
          // if the two lines are not parallel, we'll get an intersection
          if (ggGeometry::Intersect(vLineA, vLineB, vIntersection)) {

            // point on circle
            ggVector2Double vCirclePoint(vIndexX, vIndexY);
            // direction to the virtual center-point (if the circle radius was unknown, we could use this as an estimate)
            ggVector2Double vCenterDirection(vIntersection - vCirclePoint);
            // (known) radius needs to be negative, when the gradient points outward the circle
            ggDouble vRadius = vCenterDirection.Dot(vGradient) > 0.0 ? aCircleModelDiameter / 2.0 : -aCircleModelDiameter / 2.0;
            // use the actual gradient (and not the gradient from neighbors) for the center voting
            vCenterDirection = vRadius * vGradient.Normalized();
            // the position of the center point may varies by the thickenss of the circle line
            ggVector2Double vRadiusRange(0.5 * aCircleModelLineThickness * vGradient.Normalized());
            // if the length of the gradient is high, it's more likely we've detected some reasonable structures (not noise)
            ggFloat vIntensity = static_cast<ggFloat>(vGradient.Length());

            // voting for center point candidates on one side of the (potential) circle point ...
            vPainterHough.DrawLine(vCirclePoint + vCenterDirection - vRadiusRange,
                                   vCirclePoint + vCenterDirection + vRadiusRange,
                                   vIntensity, 1.0f,  ggPainterBlendType::eAdd);
          }
        }
      }
    }

    return vImageHough;
  }


  // y[i] - vx = a * (x[i] - vx)^2
  inline bool CalculateParabolaVertex(const ggDouble& aY0, // assuming aX0 = -1
                                      const ggDouble& aY1, // assuming aX1 =  0
                                      const ggDouble& aY2, // assuming aX2 =  1
                                      ggDouble& aVertexX,
                                      ggDouble& aVertexY,
                                      bool aCheckVertexInRange) {
    if (aCheckVertexInRange && ((aY0 > aY1) || (aY2 > aY1))) return false;
    ggDouble vASquare = (aY0 + aY2) / 2.0 - aY1;
    if (vASquare != 0.0) {
      aVertexX = (aY0 - aY2) / (4.0 * vASquare);
      aVertexY = aY1 - vASquare * aVertexX * aVertexX;
      GG_ASSERT(-1.0 <= aVertexX && aVertexX <= 1.0);
      return true;
    }
    return false;
  }


  template <typename TValueType>
  bool CalculateParabolaVertex(const ggImageT<TValueType>& aImage,
                               const ggSize aIndexX,
                               const ggSize aIndexY,
                               ggDouble& aVertexX,
                               ggDouble& aVertexY,
                               TValueType& aVertexValue) {

    if (aImage.IsInside(aIndexX - 1, aIndexY - 1) &&
        aImage.IsInside(aIndexX + 1, aIndexY + 1)) {

      ggVector2T<ggAveragesT<ggDouble>> vVertex;
      ggDouble vVertexValueMax = aImage(aIndexX, aIndexY);
      const ggInt32 vDelta = 1;

      for (ggInt32 vOffY = -vDelta; vOffY <= vDelta; vOffY++) {
        ggDouble vVertexOffset = 0.0;
        ggDouble vVertexValue = 0.0;
        if (CalculateParabolaVertex(aImage(aIndexX-1, aIndexY+vOffY),
                                    aImage(aIndexX  , aIndexY+vOffY),
                                    aImage(aIndexX+1, aIndexY+vOffY),
                                    vVertexOffset, vVertexValue, true)) {
          ggDouble vWeight = 1 + vDelta * vDelta - vOffY * vOffY;
          vVertex.X().AddSample(aIndexX + vVertexOffset, vWeight);
          vVertexValueMax = std::max(vVertexValueMax, vVertexValue);
        }
      }

      for (ggInt32 vOffX = -vDelta; vOffX <= vDelta; vOffX++) {
        ggDouble vVertexOffset = 0.0;
        ggDouble vVertexValue = 0.0;
        if (CalculateParabolaVertex(aImage(aIndexX+vOffX, aIndexY-1),
                                    aImage(aIndexX+vOffX, aIndexY  ),
                                    aImage(aIndexX+vOffX, aIndexY+1),
                                    vVertexOffset, vVertexValue, true)) {
          ggDouble vWeight = 1 + vDelta * vDelta - vOffX * vOffX;
          vVertex.Y().AddSample(aIndexY + vVertexOffset, vWeight);
          vVertexValueMax = std::max(vVertexValueMax, vVertexValue);
        }
      }

      if (vVertex.X().GetNumberOfSamples() > 0) {
        aVertexX = vVertex.X().GetMean();
        aVertexY = vVertex.Y().GetMean();
        aVertexValue = vVertexValueMax;
        return true;
      }

    }
    return false;
  }


  template <typename TValueType>
  void CalculateCenterOfGravity(const ggImageT<TValueType>& aImage,
                                const ggSize aCircleCenterIndexX,
                                const ggSize aCircleCenterIndexY,
                                const ggSize aCircleRadius,
                                ggDouble& aCenterOfGravityX,
                                ggDouble& aCenterOfGravityY) {

    if (aImage.IsInside(aCircleCenterIndexX - aCircleRadius, aCircleCenterIndexY - aCircleRadius) &&
        aImage.IsInside(aCircleCenterIndexX + aCircleRadius, aCircleCenterIndexY + aCircleRadius)) {

      const ggDouble vCircleRadiusSquare = aCircleRadius * aCircleRadius;
      ggVector2T<ggAveragesT<ggDouble>> vCenterOfGravity;

      for (ggSize vOffY = -aCircleRadius; vOffY <= aCircleRadius; vOffY++) {
        for (ggSize vOffX = -aCircleRadius; vOffX <= aCircleRadius; vOffX++) {
          const ggDouble vDistanceSquare = vOffX * vOffX + vOffY * vOffY;
          if (vDistanceSquare <= vCircleRadiusSquare) {
            const ggSize vIndexX = aCircleCenterIndexX + vOffX;
            const ggSize vIndexY = aCircleCenterIndexY + vOffY;
            const TValueType& vImageValue(aImage(vIndexX, vIndexY));
            vCenterOfGravity.X().AddSample(vIndexX, vImageValue);
            vCenterOfGravity.Y().AddSample(vIndexY, vImageValue);
          }
        }
      }

      if (vCenterOfGravity.X().GetNumberOfSamples() > 0) {
        aCenterOfGravityX = vCenterOfGravity.X().GetMean();
        aCenterOfGravityY = vCenterOfGravity.Y().GetMean();
      }
    }
  }


  template <typename TValueType,
            typename TSpotType = ggSpotT<TValueType, ggVector2Double> >
  std::vector<TSpotType> FindLocalMaxima(const ggImageT<TValueType>& aImage,
                                         bool aInterpolatePosition = true) {

    std::vector<TSpotType> vLocalMaxima;

    // region size around the center ("radius")
    const ggSize vDelta = 1;

    // lambda function which checks, if a pixel at a certain location (index-xy) is a local maximum
    auto vLocalMaximumCheck = [&aImage, &vLocalMaxima, aInterpolatePosition] (ggSize aIndexX, ggSize aIndexY) {

      const TValueType& vValue = aImage(aIndexX, aIndexY);
      bool vIsLocalMaximum = true;

      // loop over local region (no special handling for border in order to improve performance)
      for (ggSize vOffY = -vDelta; (vOffY <= vDelta) && vIsLocalMaximum; vOffY++) {
        for (ggSize vOffX = -vDelta; (vOffX <= vDelta) && vIsLocalMaximum; vOffX++) {
          // center pixel needs to be excluded
          if ((vOffX != 0) || (vOffY != 0)) {
            const TValueType& vNeighborValue = aImage(aIndexX + vOffX, aIndexY + vOffY);
            vIsLocalMaximum &= vValue > vNeighborValue;
          }
        }
      }

      if (vIsLocalMaximum) {
        typedef typename TSpotType::tVectorType tVector;
        TSpotType vSpot(tVector(aIndexX, aIndexY), vValue);
        if (aInterpolatePosition) CalculateParabolaVertex(aImage, aIndexX, aIndexY, vSpot[0], vSpot[1], *vSpot);
        vLocalMaxima.push_back(vSpot);
      }
    };

    // actual processing (exclude outer border since our lambda is not implemented for border pixels)
    aImage.ProcessIndexBorderInside(vDelta, vLocalMaximumCheck);

    // sort the spots from highest to lowest
    std::sort(vLocalMaxima.begin(), vLocalMaxima.end(), [] (const TSpotType& aSpotA, const TSpotType& aSpotB) {
      return aSpotA.GetValue() > aSpotB.GetValue();
    });

    return vLocalMaxima;
  }


}

#endif // GGIMAGEALGORITHM_H
