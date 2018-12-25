#ifndef GGIMAGEPAINTERT_H
#define GGIMAGEPAINTERT_H

#define _USE_MATH_DEFINES
#include <math.h>
#include <random>

#include "LibBase/ggAssert.h"
#include "LibBase/ggVectorTypes.h"
#include "LibBase/ggLineTypes.h"
#include "LibImage/ggImageT.h"


enum class ggPainterBlendType {
  eSet,
  eAdd
};


template <class TImageValueType>
class ggImagePainterT
{
public:

  typedef ggImageT<TImageValueType> tImage;

  ggImagePainterT(tImage& aImage)
  : mImage(aImage) {
  }

  tImage& Image() {
    return mImage;
  }

  const tImage& Image() const {
    return mImage;
  }

  inline void BlendPixel(ggSize aIndexX, ggSize aIndexY,
                         const TImageValueType& aValue,
                         ggFloat aOpacity = 1.0f,
                         ggPainterBlendType aBlendType = ggPainterBlendType::eSet) {
    if (Image().IsInside(aIndexX, aIndexY)) {
      TImageValueType& vPixel = Image().Get(aIndexX, aIndexY);
      switch (aBlendType) {
        case ggPainterBlendType::eSet: vPixel = (1.0f - aOpacity) * vPixel + aOpacity * aValue; break;
        case ggPainterBlendType::eAdd: vPixel += aOpacity * aValue; break;
      }
    }
  }

  inline void DrawPixelInterpolated(ggDouble aIndexX, ggDouble aIndexY,
                                    const TImageValueType& aValue,
                                    ggFloat aOpacity = 1.0f,
                                    ggPainterBlendType aBlendType = ggPainterBlendType::eSet) {
    DrawPixelInterpolated(ggVector2Double(aIndexX, aIndexY), aValue, aOpacity, aBlendType);
  }

  inline void DrawPixelInterpolated(const ggVector2Double& aIndex,
                                    const TImageValueType& aValue,
                                    ggFloat aOpacity = 1.0f,
                                    ggPainterBlendType aBlendType = ggPainterBlendType::eSet) {

    // raster offset (compatibility with QT painter)
    const ggDouble vOffset = 0.0; //-0.5;

    // index of involved 4 pixels
    ggInt32 vIndexX0 = static_cast<ggInt32>(floor(aIndex.X() + vOffset));
    ggInt32 vIndexY0 = static_cast<ggInt32>(floor(aIndex.Y() + vOffset));
    ggInt32 vIndexX1 = vIndexX0 + 1;
    ggInt32 vIndexY1 = vIndexY0 + 1;

    // only need to continue, if one of the 4 pixels is inside
    if (Image().IsInside(vIndexX0, vIndexY0) ||
        Image().IsInside(vIndexX1, vIndexY1)) {

      // calculate weights depending on index location
      ggDouble vWeightX1 = aIndex.X() + vOffset - vIndexX0;
      ggDouble vWeightY1 = aIndex.Y() + vOffset - vIndexY0;
      ggDouble vWeightX0 = 1.0 - vWeightX1;
      ggDouble vWeightY0 = 1.0 - vWeightY1;

      // blend the 4 pixels
      BlendPixel(vIndexX0, vIndexY0, aValue, aOpacity * vWeightX0 * vWeightY0, aBlendType);
      BlendPixel(vIndexX0, vIndexY1, aValue, aOpacity * vWeightX0 * vWeightY1, aBlendType);
      BlendPixel(vIndexX1, vIndexY1, aValue, aOpacity * vWeightX1 * vWeightY1, aBlendType);
      BlendPixel(vIndexX1, vIndexY0, aValue, aOpacity * vWeightX1 * vWeightY0, aBlendType);
    }
  }

  void DrawLine(ggDouble aPosX0, ggDouble aPosY0,
                ggDouble aPosX1, ggDouble aPosY1,
                const TImageValueType& aValue,
                ggFloat aOpacity = 1.0f,
                ggPainterBlendType aBlendType = ggPainterBlendType::eSet,
                const ggFloat aFragmentaion = 0.0f)
  {
    DrawLine(ggVector2Double(aPosX0, aPosY0),
             ggVector2Double(aPosX1, aPosY1),
             aValue, aOpacity, aBlendType, aFragmentaion);
  }

  void DrawLine(const ggLine2Double& aLine,
                const TImageValueType& aValue,
                ggFloat aOpacity = 1.0f,
                ggPainterBlendType aBlendType = ggPainterBlendType::eSet,
                const ggFloat aFragmentaion = 0.0f)
  {
    DrawLine(aLine.Pos0(), aLine.Pos1(),
             aValue, aOpacity, aBlendType, aFragmentaion);
  }

  void DrawLine(const ggVector2Double& aPos0,
                const ggVector2Double& aPos1,
                const TImageValueType& aValue,
                ggFloat aOpacity = 1.0f,
                ggPainterBlendType aBlendType = ggPainterBlendType::eSet,
                const ggFloat aFragmentaion = 0.0f)
  {
    ggVector2Double vDirection(aPos1 - aPos0);
    ggDouble vLength = vDirection.Length();
    ggInt32 vNumSteps = static_cast<ggInt32>(vLength + 0.5);
    if (vNumSteps <= 0) vNumSteps = 1;
    ggDouble vStepLength = vLength / vNumSteps;
    if (vLength != 0.0) vDirection /= vLength;
    for (ggInt32 vStep = 0; vStep <= vNumSteps; vStep++) {
      ggVector2Double vDelta(vStep * vStepLength * vDirection);
      ggFloat vOpacity = GetOpacity(aOpacity, aFragmentaion);
      DrawPixelInterpolated(aPos0 + vDelta, aValue, vOpacity, aBlendType);
    }
  }


  void DrawCircle(ggDouble aCenterX, ggDouble aCenterY,
                  ggDouble aRadius,
                  const TImageValueType& aValue,
                  ggFloat aOpacity = 1.0f,
                  ggPainterBlendType aBlendType = ggPainterBlendType::eSet,
                  const ggFloat aFragmentaion = 0.0f)
  {
    DrawCircle(ggVector2Double(aCenterX, aCenterY), aRadius,
               aValue, aOpacity, aBlendType, aFragmentaion);
  }

  void DrawCircle(const ggVector2Double& aCenter,
                  ggDouble aRadius,
                  const TImageValueType& aValue,
                  ggFloat aOpacity = 1.0f,
                  ggPainterBlendType aBlendType = ggPainterBlendType::eSet,
                  const ggFloat aFragmentaion = 0.0f)
  {
    ggDouble vStepLength = atan2(1.0, aRadius);
    ggInt32 vNumSteps = static_cast<ggInt32>(M_PI_2 / vStepLength + 0.5);
    if (vNumSteps <= 0) vNumSteps = 1;
    vStepLength = M_PI_2 / vNumSteps;
    for (ggInt32 vStep = 0; vStep < vNumSteps; vStep++) {
      ggDouble vAlpha = vStep * vStepLength;
      ggVector2Double vRadius0(aRadius * cos(vAlpha), aRadius * sin(vAlpha));
      ggVector2Double vRadius1(vRadius0.Cross()); // radius 90° rotated
      DrawPixelInterpolated(aCenter + vRadius0, aValue, GetOpacity(aOpacity, aFragmentaion), aBlendType);
      DrawPixelInterpolated(aCenter - vRadius0, aValue, GetOpacity(aOpacity, aFragmentaion), aBlendType);
      DrawPixelInterpolated(aCenter + vRadius1, aValue, GetOpacity(aOpacity, aFragmentaion), aBlendType);
      DrawPixelInterpolated(aCenter - vRadius1, aValue, GetOpacity(aOpacity, aFragmentaion), aBlendType);
    }
  }

private:

  inline ggFloat GetOpacity(ggFloat aOpavity,
                            ggFloat aFragmentaion) {
    return (rand() >= aFragmentaion*RAND_MAX) ? aOpavity : 0.0f;
  }

  tImage& mImage;

};

#endif // GGIMAGEPAINTERT_H
