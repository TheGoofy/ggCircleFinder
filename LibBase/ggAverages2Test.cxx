#include "LibBase/ggUnitTest.h"
#include "LibBase/ggAverages2T.h"


template <typename TValueType>
static void ggAverages2Print(const ggAverages2T<TValueType>& aAverages,
                            const char* aHeaderText = nullptr)
{
  if (aHeaderText != nullptr) std::cout << aHeaderText << std::endl;
  std::cout << "NumberOfSamples = " << aAverages.GetNumberOfSamples() << std::endl;
  std::cout << "Covariance = " << aAverages.GetCovariance() << std::endl;
}


static void ggAverages2Test()
{
  ggAverages2T<ggInt32> vAveragesInt32;
  // ggAverages2Print(vAveragesInt32, "*******************************");
  GG_TEST_EQUAL(vAveragesInt32.GetNumberOfSamples(), 0.0);
  GG_TEST_EQUAL(vAveragesInt32.GetCovariance(), 0.0);

  vAveragesInt32.AddSample(10, -10);
  // ggAverages2Print(vAveragesInt32, "*******************************");
  GG_TEST_EQUAL(vAveragesInt32.GetNumberOfSamples(), 1.0);
  GG_TEST_EQUAL(vAveragesInt32.X().GetMin(), 10);
  GG_TEST_EQUAL(vAveragesInt32.X().GetMax(), 10);
  GG_TEST_EQUAL(vAveragesInt32.Y().GetMin(), -10);
  GG_TEST_EQUAL(vAveragesInt32.Y().GetMax(), -10);
  GG_TEST_EQUAL(vAveragesInt32.GetCovariance(), 0.0);

  vAveragesInt32.AddSample(50, -30);
  // ggAverages2Print(vAveragesInt32, "*******************************");
  GG_TEST_EQUAL(vAveragesInt32.GetNumberOfSamples(), 2.0);
  GG_TEST_EQUAL(vAveragesInt32.GetCovariance(), -200);

  vAveragesInt32.AddSample(90, -40);
  // ggAverages2Print(vAveragesInt32, "*******************************");
  GG_TEST_EQUAL(vAveragesInt32.GetNumberOfSamples(), 3.0);
  GG_TEST_EQUAL(vAveragesInt32.GetCovariance(), -400);

  vAveragesInt32.AddSample(30, 20, 2.0);
  // ggAverages2Print(vAveragesInt32, "*******************************");
  GG_TEST_EQUAL(vAveragesInt32.GetNumberOfSamples(), 5.0);
  GG_TEST_EQUAL(vAveragesInt32.GetCovariance(), -464);

}


GG_TEST_REGISTER(ggAverages2Test);
