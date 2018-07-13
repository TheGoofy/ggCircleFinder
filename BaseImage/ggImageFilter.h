#ifndef GGIMAGEFILTER_H
#define GGIMAGEFILTER_H

#include <math.h>
#include <random>
#include <vector>

#include "Base/ggWalkerT.h"
#include "Base/ggVectorTypes.h"
#include "Base/ggSpotTypes.h"
#include "Base/ggUtility.h"
#include "BaseImage/ggImageT.h"
#include "BaseImage/ggHistogramAdaptiveT.h"


namespace ggImageFilter {


  template <typename TValueType>
  inline void Deriche1D(const TValueType* aValuesSrcBegin,
                        const TValueType* aValuesSrcEnd,
                        TValueType* aValuesDstBegin,
                        const ggFloat aSigma)
  {
    ggFloat e, e2, norm;
    ggFloat p_filter[5];
    ggFloat n_filter[5];

    const ggFloat alpha = aSigma>0 ? 1.695f/aSigma : 0;

    const TValueType* data_in = aValuesSrcBegin;
    TValueType* data_out = aValuesDstBegin;

    e = exp(-alpha);
    e2 = exp(-2 * alpha);
    norm = (1 - e) * (1 - e) / (1 + 2 * alpha * e - e2);

    p_filter[0] = -e2;
    p_filter[1] = 2 * e;
    p_filter[2] = norm;
    p_filter[3] = norm * (alpha - 1) * e;
    p_filter[4] = 0;
    n_filter[0] = -e2;
    n_filter[1] = 2 * e;
    n_filter[2] = 0;
    n_filter[3] = norm * (alpha + 1) * e;
    n_filter[4] = -norm * e2;

    // forward loop
    {
      ggFloat d2, d1, n0, n1, n2;
      ggFloat in0, in1, in2, out0, out1, out2;
      ggSize count = aValuesSrcEnd - aValuesSrcBegin;

      d2 = p_filter[0];
      d1 = p_filter[1];
      n0 = p_filter[2];
      n1 = p_filter[3];
      n2 = p_filter[4];
      in1 = *data_in;
      in2 = *data_in;
      out1 = (n2 + n1 + n0)*in1/(1.0f-d1-d2);
      out2 = (n2 + n1 + n0)*in1/(1.0f-d1-d2);
      for ( ; count > 0; count--, data_out++, data_in++ ) {
        in0 = *data_in;
        out0 = n2*in2 + n1*in1 + n0*in0 + d1*out1 + d2*out2;
        in2 = in1;
        in1 = in0;
        out2 = out1;
        out1 = out0;
        *data_out = out0;
      }
    }

    data_in = aValuesSrcBegin + (aValuesSrcEnd-aValuesSrcBegin-1);
    data_out = aValuesDstBegin + (aValuesSrcEnd-aValuesSrcBegin-1);

    // backward loop
    {
      ggFloat d2, d1, n0, n1, n2;
      ggFloat in0, in1, in2, out0, out1, out2;
      ggSize count = aValuesSrcEnd - aValuesSrcBegin;

      d2 = n_filter[0];
      d1 = n_filter[1];
      n0 = n_filter[2];
      n1 = n_filter[3];
      n2 = n_filter[4];
      in1 = *data_in;
      in2 = *data_in;
      out1 = (n2 + n1 + n0)*in1/(1.0f-d1-d2);
      out2 = (n2 + n1 + n0)*in1/(1.0f-d1-d2);
      for ( ; count > 0; count--, data_out--, data_in-- ) {
        in0 = *data_in;
        out0 = n2*in2 + n1*in1 + n0*in0 + d1*out1 + d2*out2;
        in2 = in1;
        in1 = in0;
        out2 = out1;
        out1 = out0;
        *data_out += out0;
      }
    }
  }


  template <typename TValueTypeDst, typename TValueTypeSrc>
  void Copy(ggImageT<TValueTypeDst>& aImageDst,
            const ggImageT<TValueTypeSrc>& aImageSrc,
            const ggInt32 aPositionSrcX0,
            const ggInt32 aPositionSrcY0)
  {
    const ggSize vSrcX0 = ggUtility::Clamp<ggInt32>(aPositionSrcX0, 0, aImageSrc.GetSizeX());
    const ggSize vSrcY0 = ggUtility::Clamp<ggInt32>(aPositionSrcY0, 0, aImageSrc.GetSizeY());
    const ggSize vSrcX1 = ggUtility::Clamp<ggInt32>(aPositionSrcX0 + aImageDst.GetSizeX(), 0, aImageSrc.GetSizeX());
    const ggSize vSrcY1 = ggUtility::Clamp<ggInt32>(aPositionSrcY0 + aImageDst.GetSizeY(), 0, aImageSrc.GetSizeY());
    for (ggSize vSrcY = vSrcY0; vSrcY < vSrcY1; vSrcY++) {
      ggSize vDstY = vSrcY - aPositionSrcY0;
      for (ggSize vSrcX = vSrcX0; vSrcX < vSrcX1; vSrcX++) {
        ggSize vDstX = vSrcX - aPositionSrcX0;
        aImageDst(vDstX, vDstY) = aImageSrc(vSrcX, vSrcY);
      }
    }
  }


  template <typename TValueType>
  void GetMinMax(const ggImageT<TValueType>& aImage, TValueType& aMin, TValueType& aMax)
  {
    const TValueType* vData = aImage.GetValues();
    const TValueType* vDataEnd = aImage.GetValues() + aImage.GetSizeX() * aImage.GetSizeY();
    if (vData != vDataEnd) aMin = aMax = *vData++;
    while (vData != vDataEnd) {
      if (*vData < aMin) aMin = *vData;
      if (*vData > aMax) aMax = *vData;
      ++vData;
    }
  }


  template <typename TValueType>
  std::vector<ggSize> GetHistogram(const ggImageT<TValueType>& aImage)
  {
    std::vector<ggSize> vHistogram;
    for (ggSize vIndexY = 0; vIndexY < aImage.GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < aImage.GetSizeX(); vIndexX++) {
        ggUInt16 vValue = aImage(vIndexX, vIndexY);
        if (vValue >= vHistogram.size()) vHistogram.resize(vValue+1, 0);
        ++vHistogram[vValue];
      }
    }
    return vHistogram;
  }


  template <typename TValueType, ggUInt64 TNumberOfBins = 512>
  ggHistogramAdaptiveT<TValueType, TNumberOfBins> GetHistogram2(const ggImageT<TValueType>& aImage)
  {
    ggHistogramAdaptiveT<TValueType, TNumberOfBins> vHistogram;
    for (ggSize vIndexY = 0; vIndexY < aImage.GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < aImage.GetSizeX(); vIndexX++) {
        vHistogram.Insert(aImage(vIndexX, vIndexY));
      }
    }
    return vHistogram;
  }


  template <typename TValueType, typename TValueTypeInternal = TValueType>
  void Gauss(ggImageT<TValueType>& aImage, const ggFloat aSigma)
  {
    { // filter x-direction
      TValueTypeInternal* vSrcBegin = new TValueTypeInternal[aImage.GetSizeX()];
      TValueTypeInternal* vDstBegin = new TValueTypeInternal[aImage.GetSizeX()];
      for (ggSize vIndexY = 0; vIndexY < aImage.GetSizeY(); vIndexY++) {
        for (ggSize vIndexX = 0; vIndexX < aImage.GetSizeX(); vIndexX++) vSrcBegin[vIndexX] = aImage(vIndexX, vIndexY);
        Deriche1D(vSrcBegin, vSrcBegin + aImage.GetSizeX(), vDstBegin, aSigma);
        for (ggSize vIndexX = 0; vIndexX < aImage.GetSizeX(); vIndexX++) aImage(vIndexX, vIndexY) = vDstBegin[vIndexX];
      }
      delete[] vSrcBegin;
      delete[] vDstBegin;
    }
    { // filter y-direction
      TValueTypeInternal* vSrcBegin = new TValueTypeInternal[aImage.GetSizeY()];
      TValueTypeInternal* vDstBegin = new TValueTypeInternal[aImage.GetSizeY()];
      for (ggSize vIndexX = 0; vIndexX < aImage.GetSizeX(); vIndexX++) {
        for (ggSize vIndexY = 0; vIndexY < aImage.GetSizeY(); vIndexY++) vSrcBegin[vIndexY] = aImage(vIndexX, vIndexY);
        Deriche1D(vSrcBegin, vSrcBegin + aImage.GetSizeY(), vDstBegin, aSigma);
        for (ggSize vIndexY = 0; vIndexY < aImage.GetSizeY(); vIndexY++) aImage(vIndexX, vIndexY) = vDstBegin[vIndexY];
      }
      delete[] vSrcBegin;
      delete[] vDstBegin;
    }
  }


  template <typename TValueType>
  void AddNoise(ggImageT<TValueType>& aImage, const ggFloat aSigma)
  {
    // set up random generator
    std::random_device vRandomDevice;
    std::mt19937 vEngine(vRandomDevice());
    std::normal_distribution<> vRandom(0.0f, aSigma);
    // get iterators
    const TValueType* vSrc = aImage.GetValues();
    const TValueType* vSrcEnd = aImage.GetValues() + aImage.GetSizeX()*aImage.GetSizeY();
    TValueType* vDst = aImage.GetValues();
    while (vSrc != vSrcEnd) {
      *vDst++ = *vSrc++ + vRandom(vEngine);
    }
  }


  template <typename TValueType>
  void Invert(ggImageT<TValueType>& aImage,
              const TValueType& aReference) {
    const TValueType* vSrc = aImage.GetValues();
    const TValueType* vSrcEnd = aImage.GetValues() + aImage.GetSizeX()*aImage.GetSizeY();
    TValueType* vDst = aImage.GetValues();
    while (vSrc != vSrcEnd) {
      *vDst++ = aReference - *vSrc++;
    }
  }


  template <typename TValueType>
  void MirrorX(ggImageT<TValueType>& aImage) {
    for (ggSize vIndexY = 0; vIndexY < aImage.GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < aImage.GetSizeX() / 2; vIndexX++) {
        ggUtility::Swap(aImage(vIndexX, vIndexY), aImage(aImage.GetSizeX()-vIndexX-1), vIndexY);
      }
    }
  }


  template <typename TValueType>
  void MirrorY(ggImageT<TValueType>& aImage) {
    for (ggSize vIndexY = 0; vIndexY < aImage.GetSizeY() / 2; vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < aImage.GetSizeX(); vIndexX++) {
        ggUtility::Swap(aImage(vIndexX, vIndexY), aImage(vIndexX, aImage.GetSizeY()-vIndexY-1));
      }
    }
  }


  template <typename TValueType>
  ggVector2Float Gradient(const ggImageT<TValueType>& aImage,
                          const ggSize aIndexX,
                          const ggSize aIndexY)
  {
    ggVector2Float vGradient(0.0f, 0.0f);

    if (aImage.IsInside(aIndexX-1, aIndexY-1) &&
        aImage.IsInside(aIndexX+1, aIndexY+1)) {

      // scharr operator
      vGradient.SetX( 3 * aImage(aIndexX+1, aIndexY-1) -  3 * aImage(aIndexX-1, aIndexY-1) +
                     10 * aImage(aIndexX+1, aIndexY  ) - 10 * aImage(aIndexX-1, aIndexY  ) +
                      3 * aImage(aIndexX+1, aIndexY+1) -  3 * aImage(aIndexX-1, aIndexY+1));
      vGradient.SetY( 3 * aImage(aIndexX-1, aIndexY+1) -  3 * aImage(aIndexX-1, aIndexY-1) +
                     10 * aImage(aIndexX  , aIndexY+1) - 10 * aImage(aIndexX  , aIndexY-1) +
                      3 * aImage(aIndexX+1, aIndexY+1) -  3 * aImage(aIndexX+1, aIndexY-1));

      // normalize gradient by total operator weight
      // the gradient then represents "value change per pixel"
      vGradient /= 32; // 3 + 10 + 3 + 3 + 10 + 3;
    }

    return vGradient;
  }


  template <typename TValueType>
  void Gradient(ggImageT<TValueType>& aImageDst,
                const ggImageT<TValueType>& aImageSrc)
  {
    GG_ASSERT(aImageSrc.GetSizeX() == aImageDst.GetSizeX());
    GG_ASSERT(aImageSrc.GetSizeY() == aImageDst.GetSizeY());
    for (ggSize vIndexY = 0; vIndexY < aImageSrc.GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < aImageSrc.GetSizeX(); vIndexX++) {
        aImageDst(vIndexX, vIndexY) = Gradient(aImageSrc, vIndexX, vIndexY).Length();
      }
    }
  }


  template <typename TValueType>
  void Gradient(ggImageT<ggVector2Float>& aImageDst,
                const ggImageT<TValueType>& aImageSrc) {
    GG_ASSERT(aImageSrc.GetSizeX() == aImageDst.GetSizeX());
    GG_ASSERT(aImageSrc.GetSizeY() == aImageDst.GetSizeY());
    for (ggSize vIndexY = 0; vIndexY < aImageSrc.GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < aImageSrc.GetSizeX(); vIndexX++) {
        aImageDst(vIndexX, vIndexY) = Gradient(aImageSrc, vIndexX, vIndexY);
      }
    }
  }

  // y[i] - vx = a * (x[i] - vx)^2
  inline bool GetParabolaVertex(const ggDouble& aY0, // assuming aX0 = -1
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
      if (aVertexX < -1.0 || aVertexX > 1.0) qDebug() << aVertexX;
      return true;
    }
    return false;
  }


  template <typename TValueType>
  bool GetParabolaVertex(const ggImageT<TValueType>& aImage,
                         const ggSize aIndexX,
                         const ggSize aIndexY,
                         ggDouble& aVertexX,
                         ggDouble& aVertexY,
                         TValueType& aVertexValue) {
    if (aImage.IsInside(aIndexX - 1, aIndexY - 1) &&
        aImage.IsInside(aIndexX + 1, aIndexX + 1)) {

      ggDouble vVertexOffsetX = 0.0;
      ggDouble vVertexOffsetY = 0.0;
      ggDouble vVertexWeightSumX = 0.0;
      ggDouble vVertexWeightSumY = 0.0;
      ggDouble vVertexValueMax = aImage(aIndexX, aIndexY);
      const ggInt32 vDelta = 1;

      for (ggInt32 vOffY = -vDelta; vOffY <= vDelta; vOffY++) {
        ggDouble vVertexOffset = 0.0;
        ggDouble vVertexValue = 0.0;
        if (GetParabolaVertex(aImage(aIndexX-1, aIndexY+vOffY),
                              aImage(aIndexX  , aIndexY+vOffY),
                              aImage(aIndexX+1, aIndexY+vOffY),
                              vVertexOffset, vVertexValue, true)) {
          ggDouble vWeight = 1 + vDelta * vDelta - vOffY * vOffY;
          vVertexWeightSumX += vWeight;
          vVertexOffsetX += vVertexOffset * vWeight;
          vVertexValueMax = std::max(vVertexValueMax, vVertexValue);
        }
      }

      for (ggInt32 vOffX = -vDelta; vOffX <= vDelta; vOffX++) {
        ggDouble vVertexOffset = 0.0;
        ggDouble vVertexValue = 0.0;
        if (GetParabolaVertex(aImage(aIndexX+vOffX, aIndexY-1),
                              aImage(aIndexX+vOffX, aIndexY  ),
                              aImage(aIndexX+vOffX, aIndexY+1),
                              vVertexOffset, vVertexValue, true)) {
          ggDouble vWeight = 1 + vDelta * vDelta - vOffX * vOffX;
          vVertexWeightSumY += vWeight;
          vVertexOffsetY += vVertexOffset * vWeight;
          vVertexValueMax = std::max(vVertexValueMax, vVertexValue);
        }
      }

      if ((vVertexWeightSumX != 0.0) &&
          (vVertexWeightSumY != 0.0)) {
        aVertexX = aIndexX + vVertexOffsetX / vVertexWeightSumX;
        aVertexY = aIndexY + vVertexOffsetY / vVertexWeightSumY;
        aVertexValue = vVertexValueMax;
        return true;
      }

    }
    return false;
  }


  template <typename TValueType,
            typename TSpotType = ggSpotT<TValueType, ggVector2Double> >
  std::vector<TSpotType> FindLocalMaxima(const ggImageT<TValueType>& aImage,
                                         bool aInterpolatePosition = true) {

    std::vector<TSpotType> vLocalMaxima;

    for (ggSize vIndexY = 1; vIndexY + 1 < aImage.GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 1; vIndexX + 1 < aImage.GetSizeX(); vIndexX++) {

        bool vIsLocalMax = true;
        const TValueType& aValue = aImage(vIndexX, vIndexY);
        const ggInt32 vDelta = 1;

        for (ggInt32 vOffY = -vDelta; (vOffY <= vDelta) && vIsLocalMax; vOffY++) {
          for (ggInt32 vOffX = -vDelta; (vOffX <= vDelta) && vIsLocalMax; vOffX++) {
            if ((vOffX != 0) || (vOffY != 0)) {
              vIsLocalMax = vIsLocalMax && (aValue > aImage(vIndexX + vOffX, vIndexY + vOffY));
            }
          }
        }

        if (vIsLocalMax) {
          typedef typename TSpotType::tVectorType tVector;
          TSpotType vSpot(tVector(vIndexX, vIndexY), aValue);
          if (aInterpolatePosition) GetParabolaVertex(aImage, vIndexX, vIndexY, vSpot[0], vSpot[1], *vSpot);
          vLocalMaxima.push_back(vSpot);
        }
      }
    }

    return vLocalMaxima;
  }

}

#endif // GGIMAGEFILTER_H
