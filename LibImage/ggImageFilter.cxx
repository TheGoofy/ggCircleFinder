#include "LibImage/ggImageFilter.h"

#include "LibBase/ggRound.h"


template <typename TValueType>
static void Deriche1D(const TValueType* aValuesSrcBegin,
                      const TValueType* aValuesSrcEnd,
                      TValueType* aValuesDstBegin,
                      const TValueType& aSigma)
{
  TValueType e, e2, norm;
  TValueType p_filter[5];
  TValueType n_filter[5];

  const TValueType alpha = aSigma > 0 ? static_cast<TValueType>(1.695) / aSigma : 0;

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
    TValueType d2, d1, n0, n1, n2;
    TValueType in0, in1, in2, out0, out1, out2;
    ggSize count = aValuesSrcEnd - aValuesSrcBegin;

    d2 = p_filter[0];
    d1 = p_filter[1];
    n0 = p_filter[2];
    n1 = p_filter[3];
    n2 = p_filter[4];
    in1 = *data_in;
    in2 = *data_in;
    out1 = (n2 + n1 + n0) * in1 / (1 - d1 - d2);
    out2 = (n2 + n1 + n0) * in1 / (1 - d1 - d2);
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

  data_in = aValuesSrcBegin + (aValuesSrcEnd - aValuesSrcBegin - 1);
  data_out = aValuesDstBegin + (aValuesSrcEnd - aValuesSrcBegin - 1);

  // backward loop
  {
    TValueType d2, d1, n0, n1, n2;
    TValueType in0, in1, in2, out0, out1, out2;
    ggSize count = aValuesSrcEnd - aValuesSrcBegin;

    d2 = n_filter[0];
    d1 = n_filter[1];
    n0 = n_filter[2];
    n1 = n_filter[3];
    n2 = n_filter[4];
    in1 = *data_in;
    in2 = *data_in;
    out1 = (n2 + n1 + n0) * in1 / (1 - d1 - d2);
    out2 = (n2 + n1 + n0) * in1 / (1 - d1 - d2);
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


template <typename TValueType, typename TValueTypeInternal>
void ggImageFilter::Gauss(ggImageT<TValueType>& aImage, const TValueTypeInternal& aSigma)
{
  { // filter x-direction
    TValueTypeInternal* vSrcBegin = new TValueTypeInternal[aImage.GetSizeX()];
    TValueTypeInternal* vDstBegin = new TValueTypeInternal[aImage.GetSizeX()];
    for (ggSize vIndexY = 0; vIndexY < aImage.GetSizeY(); vIndexY++) {
      for (ggSize vIndexX = 0; vIndexX < aImage.GetSizeX(); vIndexX++) vSrcBegin[vIndexX] = ggRound<TValueTypeInternal>(aImage(vIndexX, vIndexY));
      Deriche1D<TValueTypeInternal>(vSrcBegin, vSrcBegin + aImage.GetSizeX(), vDstBegin, aSigma);
      for (ggSize vIndexX = 0; vIndexX < aImage.GetSizeX(); vIndexX++) aImage(vIndexX, vIndexY) = ggRound<TValueType>(vDstBegin[vIndexX]);
    }
    delete[] vSrcBegin;
    delete[] vDstBegin;
  }
  { // filter y-direction
    TValueTypeInternal* vSrcBegin = new TValueTypeInternal[aImage.GetSizeY()];
    TValueTypeInternal* vDstBegin = new TValueTypeInternal[aImage.GetSizeY()];
    for (ggSize vIndexX = 0; vIndexX < aImage.GetSizeX(); vIndexX++) {
      for (ggSize vIndexY = 0; vIndexY < aImage.GetSizeY(); vIndexY++) vSrcBegin[vIndexY] = ggRound<TValueTypeInternal>(aImage(vIndexX, vIndexY));
      Deriche1D<TValueTypeInternal>(vSrcBegin, vSrcBegin + aImage.GetSizeY(), vDstBegin, aSigma);
      for (ggSize vIndexY = 0; vIndexY < aImage.GetSizeY(); vIndexY++) aImage(vIndexX, vIndexY) = ggRound<TValueType>(vDstBegin[vIndexY]);
    }
    delete[] vSrcBegin;
    delete[] vDstBegin;
  }
}


template void ggImageFilter::Gauss<ggInt8, ggFloat>(ggImageT<ggInt8>& aImage, const ggFloat& aSigma);
template void ggImageFilter::Gauss<ggInt16, ggFloat>(ggImageT<ggInt16>& aImage, const ggFloat& aSigma);
template void ggImageFilter::Gauss<ggInt32, ggFloat>(ggImageT<ggInt32>& aImage, const ggFloat& aSigma);
template void ggImageFilter::Gauss<ggInt64, ggFloat>(ggImageT<ggInt64>& aImage, const ggFloat& aSigma);
// template void ggImageFilter::Gauss<ggSize, ggFloat>(ggImageT<ggSize>& aImage, const ggFloat& aSigma);
template void ggImageFilter::Gauss<ggUInt8, ggFloat>(ggImageT<ggUInt8>& aImage, const ggFloat& aSigma);
template void ggImageFilter::Gauss<ggUInt16, ggFloat>(ggImageT<ggUInt16>& aImage, const ggFloat& aSigma);
template void ggImageFilter::Gauss<ggUInt32, ggFloat>(ggImageT<ggUInt32>& aImage, const ggFloat& aSigma);
template void ggImageFilter::Gauss<ggUInt64, ggFloat>(ggImageT<ggUInt64>& aImage, const ggFloat& aSigma);
// template void ggImageFilter::Gauss<ggUSize, ggFloat>(ggImageT<ggUSize>& aImage, const ggFloat& aSigma);
template void ggImageFilter::Gauss<ggFloat, ggFloat>(ggImageT<ggFloat>& aImage, const ggFloat& aSigma);
template void ggImageFilter::Gauss<ggFloat, ggDouble>(ggImageT<ggFloat>& aImage, const ggDouble& aSigma);
template void ggImageFilter::Gauss<ggDouble, ggDouble>(ggImageT<ggDouble>& aImage, const ggDouble& aSigma);
