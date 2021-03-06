#include "LibBase/ggUnitTest.h"
#include "LibBase/ggAverages2T.h"
#include "LibBase/ggAveragesTest.h"


template <typename TValueType>
static void ggAverages2Print(const ggAverages2T<TValueType>& aAverages,
                            const char* aHeaderText = nullptr)
{
  ggDouble vA, vB;
  if (aHeaderText != nullptr) std::cout << aHeaderText << std::endl;
  std::cout << "NumberOfSamples = " << aAverages.GetNumberOfSamples() << std::endl;
  std::cout << "Covariance = " << aAverages.GetCovarianceP() << std::endl;
  if (aAverages.GetRegressionX(vA, vB)) std::cout << "RegressionX: a = " << vA << ", b = " << vB << std::endl;
  if (aAverages.GetRegressionY(vA, vB)) std::cout << "RegressionY: a = " << vA << ", b = " << vB << std::endl;
}


static void ggAverages2Test()
{
  ggDouble vA, vB;
  ggAverages2T<ggInt32> vAveragesInt32;
  // ggAverages2Print(vAveragesInt32, "*******************************");
  GG_TEST_EQUAL(vAveragesInt32.GetNumberOfSamples(), 0.0);
  GG_TEST_EQUAL(vAveragesInt32.GetCovarianceP(), 0.0);

  vAveragesInt32.AddSample(10, -10);
  // ggAverages2Print(vAveragesInt32, "*******************************");
  GG_TEST_EQUAL_FLOAT(vAveragesInt32.GetNumberOfSamples(), 1.0);
  GG_TEST_EQUAL(vAveragesInt32.X().GetMin(), 10);
  GG_TEST_EQUAL(vAveragesInt32.X().GetMax(), 10);
  GG_TEST_EQUAL(vAveragesInt32.Y().GetMin(), -10);
  GG_TEST_EQUAL(vAveragesInt32.Y().GetMax(), -10);
  GG_TEST_EQUAL_FLOAT(vAveragesInt32.GetCovarianceP(), 0.0);

  vAveragesInt32.AddSample(50, -30);
  // ggAverages2Print(vAveragesInt32, "*******************************");
  GG_TEST_EQUAL_FLOAT(vAveragesInt32.GetNumberOfSamples(), 2.0);
  GG_TEST_EQUAL_FLOAT(vAveragesInt32.GetCovarianceP(), -200.0);
  GG_TEST(vAveragesInt32.GetRegressionX(vA, vB));
  GG_TEST_EQUAL_FLOAT(vA, -0.5);
  GG_TEST_EQUAL_FLOAT(vB, -5.0);
  GG_TEST(vAveragesInt32.GetRegressionY(vA, vB));
  GG_TEST_EQUAL_FLOAT(vA, -2.0);
  GG_TEST_EQUAL_FLOAT(vB, -10.0);

  vAveragesInt32.AddSample(90, -40);
  // ggAverages2Print(vAveragesInt32, "*******************************");
  GG_TEST_EQUAL_FLOAT(vAveragesInt32.GetNumberOfSamples(), 3.0);
  GG_TEST_EQUAL_FLOAT(vAveragesInt32.GetCovarianceP(), -400.0);

  vAveragesInt32.AddSample(30, 20, 2.0);
  // ggAverages2Print(vAveragesInt32, "*******************************");
  GG_TEST_EQUAL_FLOAT(vAveragesInt32.GetNumberOfSamples(), 5.0);
  GG_TEST_EQUAL_FLOAT(vAveragesInt32.GetCovarianceP(), -464.0);

  ggAverages2T<ggDouble> vAveragesDouble;
  vAveragesDouble.AddSample(-1.5, 0.00);
  vAveragesDouble.AddSample( 0.0, 1.01);
  vAveragesDouble.AddSample( 1.5, 2.01);
  vAveragesDouble.AddSample( 3.0, 3.00);
  // ggAverages2Print(vAveragesDouble, "**********");
  // ggAveragesTest::Print(vAveragesDouble.X(), "vAveragesDouble.X() **********");
  // ggAveragesTest::Print(vAveragesDouble.Y(), "vAveragesDouble.Y() **********");
  GG_TEST(vAveragesDouble.GetRegressionX(vA, vB));
  GG_TEST_EQUAL_FLOAT(vA, 0.667);
  GG_TEST_EQUAL_FLOAT(vB, 1.005);
  GG_TEST(vAveragesDouble.GetRegressionY(vA, vB));
  GG_TEST_EQUAL_FLOAT(vA, 1.500);
  GG_TEST_EQUAL_FLOAT(vB, -1.507);

}


GG_TEST_REGISTER(ggAverages2Test);
