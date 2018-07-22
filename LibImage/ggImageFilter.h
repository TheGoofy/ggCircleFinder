#ifndef GGIMAGEFILTER_H
#define GGIMAGEFILTER_H

#include <math.h>
#include <random>
#include <vector>

#include "LibBase/ggWalkerT.h"
#include "LibBase/ggVectorTypes.h"
#include "LibBase/ggSpotTypes.h"
#include "LibBase/ggUtility.h"
#include "LibImage/ggImageT.h"
#include "LibImage/ggHistogramAdaptiveT.h"


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
  std::vector<ggSize> GetHistogram(const ggImageT<TValueType>& aImage)
  {
    std::vector<ggSize> vHistogram;
    aImage.ProcessValues([&vHistogram] (const TValueType& aValue) {
      if (aValue >= vHistogram.size()) vHistogram.resize(aValue + 1, 0);
      ++vHistogram[aValue];
    });
    return vHistogram;
  }


  template <typename TValueType, ggUInt64 TNumberOfBins = 512>
  ggHistogramAdaptiveT<TValueType, TNumberOfBins> GetHistogram2(const ggImageT<TValueType>& aImage)
  {
    ggHistogramAdaptiveT<TValueType, TNumberOfBins> vHistogram;
    aImage.ProcessValues([&vHistogram] (const TValueType& aValue) {
      vHistogram.Insert(aValue);
    });
    return vHistogram;
  }


  template <typename TValueType>
  void AddNoise(ggImageT<TValueType>& aImage, const ggFloat aSigma)
  {
    // set up random generator
    std::random_device vRandomDevice;
    std::mt19937 vEngine(vRandomDevice());
    std::normal_distribution<TValueType> vRandom(0, aSigma);
    // add random values
    aImage.ProcessValues([&vRandom, &vEngine] (TValueType& aValue) {
      aValue += vRandom(vEngine);
    });
  }


  template <typename TValueType>
  ggVector2Float Gradient(const ggImageT<TValueType>& aImage,
                          const ggSize aIndexX,
                          const ggSize aIndexY)
  {
    ggVector2Float vGradient(0.0f, 0.0f);

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

    return vGradient;
  }


  template <typename TValueType>
  ggVector2Float GradientClamp(const ggImageT<TValueType>& aImage,
                               const ggSize aIndexX,
                               const ggSize aIndexY)
  {
    ggVector2Float vGradient(0.0f, 0.0f);

    // scharr operator
    vGradient.SetX( 3 * aImage.GetClamp(aIndexX+1, aIndexY-1) -  3 * aImage.GetClamp(aIndexX-1, aIndexY-1) +
                   10 * aImage.GetClamp(aIndexX+1, aIndexY  ) - 10 * aImage.GetClamp(aIndexX-1, aIndexY  ) +
                    3 * aImage.GetClamp(aIndexX+1, aIndexY+1) -  3 * aImage.GetClamp(aIndexX-1, aIndexY+1));
    vGradient.SetY( 3 * aImage.GetClamp(aIndexX-1, aIndexY+1) -  3 * aImage.GetClamp(aIndexX-1, aIndexY-1) +
                   10 * aImage.GetClamp(aIndexX  , aIndexY+1) - 10 * aImage.GetClamp(aIndexX  , aIndexY-1) +
                    3 * aImage.GetClamp(aIndexX+1, aIndexY+1) -  3 * aImage.GetClamp(aIndexX+1, aIndexY-1));

    // normalize gradient by total operator weight
    // the gradient then represents "value change per pixel"
    vGradient /= 32; // 3 + 10 + 3 + 3 + 10 + 3;

    return vGradient;
  }


  template <typename TValueType>
  void Gradient(ggImageT<TValueType>& aImageDst,
                const ggImageT<TValueType>& aImageSrc)
  {
    GG_ASSERT(aImageSrc.GetSizeX() == aImageDst.GetSizeX());
    GG_ASSERT(aImageSrc.GetSizeY() == aImageDst.GetSizeY());
    auto vFunctionGradient = [&aImageSrc, &aImageDst] (ggSize aIndexX, ggSize aIndexY) {
      aImageDst(aIndexX, aIndexY) = Gradient(aImageSrc, aIndexX, aIndexY).Length();
    };
    auto vFunctionGradientClamp = [&aImageSrc, &aImageDst] (ggSize aIndexX, ggSize aIndexY) {
      aImageDst(aIndexX, aIndexY) = GradientClamp(aImageSrc, aIndexX, aIndexY).Length();
    };
    aImageSrc.ProcessIndexBorder(1, vFunctionGradient, vFunctionGradientClamp);
  }


  template <typename TValueType>
  void Gradient(ggImageT<ggVector2Float>& aImageDst,
                const ggImageT<TValueType>& aImageSrc) {
    GG_ASSERT(aImageSrc.GetSizeX() == aImageDst.GetSizeX());
    GG_ASSERT(aImageSrc.GetSizeY() == aImageDst.GetSizeY());
    auto vFunctionGradient = [&aImageSrc, &aImageDst] (ggSize aIndexX, ggSize aIndexY) {
      aImageDst(aIndexX, aIndexY) = Gradient(aImageSrc, aIndexX, aIndexY);
    };
    auto vFunctionGradientClamp = [&aImageSrc, &aImageDst] (ggSize aIndexX, ggSize aIndexY) {
      aImageDst(aIndexX, aIndexY) = GradientClamp(aImageSrc, aIndexX, aIndexY);
    };
    aImageSrc.ProcessIndexBorder(1, vFunctionGradient, vFunctionGradientClamp);
  }


}

#endif // GGIMAGEFILTER_H
