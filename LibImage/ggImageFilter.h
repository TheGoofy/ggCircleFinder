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
#include "LibImage/ggHistogramFixedT.h"


namespace ggImageFilter {


  template <typename TValueType, typename TValueTypeInternal = ggFloat>
  void Gauss(ggImageT<TValueType>& aImage, const TValueTypeInternal& aSigma);


  template <typename TValueType>
  void Median(ggImageT<TValueType>& aImageDst,
              const ggImageT<TValueType>& aImageSrc,
              const ggSize aWidth)
  {
    // src- and dst-image must have same size
    GG_ASSERT(aImageSrc.GetSize() == aImageDst.GetSize());

    // check if filter width is 2 or larger
    if (aWidth > 1) {

      // calculate kernel offsets
      const ggSize vOffBegin = - (aWidth - 1) / 2;
      const ggSize vOffEnd = vOffBegin + aWidth;

      // the median filter kernel (index processor)
      auto vMedianKernel = [&aImageDst, &aImageSrc, &vOffBegin, &vOffEnd] (ggSize aIndexX, ggSize aIndexY) {

        // calculate proper index range (consider image borders)
        const ggSize vIndexBeginX = std::max<ggSize>(aIndexX + vOffBegin, 0);
        const ggSize vIndexBeginY = std::max<ggSize>(aIndexY + vOffBegin, 0);
        const ggSize vIndexEndX = std::min<ggSize>(aIndexX + vOffEnd, aImageSrc.GetSizeY());
        const ggSize vIndexEndY = std::min<ggSize>(aIndexY + vOffEnd, aImageSrc.GetSizeY());

        // container for image values inside kernel region
        std::vector<TValueType> vValues;
        vValues.reserve((vIndexEndX - vIndexBeginX) * (vIndexEndY - vIndexBeginY));
        for (ggSize vIndexY = vIndexBeginY; vIndexY < vIndexEndY; vIndexY++) {
          for (ggSize vIndexX = vIndexBeginX; vIndexX < vIndexEndX; vIndexX++) {
            vValues.push_back(aImageSrc(vIndexX, vIndexY));
          }
        }

        // sort the values (partially)
        std::nth_element(vValues.begin(), vValues.begin() + vValues.size() / 2, vValues.end());

        // assign the meddian value (from the middle of the partially sorted vector)
        aImageDst(aIndexX, aIndexY) = vValues[vValues.size() / 2];
      };

      // apply the median filter kernel
      aImageSrc.ProcessIndex(vMedianKernel);
    }
    else {

      // for filter width of 0 or 1 there is no median to calculate => just copy the image values
      aImageDst = aImageSrc;
    }
  }


  template <typename TValueType>
  void Median(ggImageT<TValueType>& aImage,
              const ggSize aSize)
  {
    // can't do median in-place, make a temporary copy
    ggImageT<TValueType> vImageMedian(aImage.GetSize());
    Median(vImageMedian, aImage, aSize);
    aImage = vImageMedian;
  }


  template <typename TValueType>
  ggHistogramFixedT<TValueType> GetHistogram(const ggImageT<TValueType>& aImage,
                                             ggInt64 aNumberOfBins = 256)
  {
    TValueType vMin, vMax;
    aImage.GetMinMax(vMin, vMax);
    ggHistogramFixedT<TValueType> vHistogram(aNumberOfBins, vMin, vMax);
    aImage.ProcessValues([&vHistogram] (const TValueType& aValue) {
      vHistogram.Add(aValue);
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
    GG_ASSERT(aImageSrc.GetSize() == aImageDst.GetSize());
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
    GG_ASSERT(aImageSrc.GetSize() == aImageDst.GetSize());
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
