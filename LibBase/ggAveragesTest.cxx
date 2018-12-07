#include "LibBase/ggUnitTest.h"
#include "LibBase/ggAveragesT.h"


template <typename TValueType>
static void ggAveragesPrint(const ggAveragesT<TValueType>& aAverages,
                            const char* aHeaderText = nullptr)
{
  if (aHeaderText != nullptr) std::cout << aHeaderText << std::endl;
  std::cout << "NumberOfSamples = " << aAverages.GetNumberOfSamples() << std::endl;
  std::cout << "Min = " << aAverages.GetMin() << std::endl;
  std::cout << "Max = " << aAverages.GetMax() << std::endl;
  std::cout << "Sum = " << aAverages.GetSum() << std::endl;
  std::cout << "Mean = " << aAverages.GetMean() << std::endl;
  std::cout << "StdDev = " << aAverages.GetStdDev() << std::endl;
  std::cout << "StdDevP = " << aAverages.GetStdDevP() << std::endl;
  std::cout << "Variance = " << aAverages.GetVariance() << std::endl;
  std::cout << "VarianceP = " << aAverages.GetVarianceP() << std::endl;
  std::cout << "VariationCoefficient = " << aAverages.GetVariationCoefficient() << std::endl;
  std::cout << "SumOfSquaredErrors = " << aAverages.GetSumOfSquaredErrors() << std::endl;
}


static void ggAveragesTest()
{
  ggAveragesT<ggInt32> vAveragesInt32;
  // ggAveragesPrint(vAveragesInt32, "*******************************");
  GG_TEST_EQUAL(vAveragesInt32.GetNumberOfSamples(), 0.0);
  GG_TEST_EQUAL(vAveragesInt32.GetSum(), 0);

  vAveragesInt32.AddSample(10);
  // ggAveragesPrint(vAveragesInt32, "*******************************");
  GG_TEST_EQUAL(vAveragesInt32.GetNumberOfSamples(), 1.0);
  GG_TEST_EQUAL(vAveragesInt32.GetMin(), 10);
  GG_TEST_EQUAL(vAveragesInt32.GetMax(), 10);
  GG_TEST_EQUAL(vAveragesInt32.GetSum(), 10);
  GG_TEST_EQUAL(vAveragesInt32.GetMean(), 10);
  GG_TEST_EQUAL(vAveragesInt32.GetStdDev(), 0);
  GG_TEST_EQUAL(vAveragesInt32.GetStdDevP(), 0);
  GG_TEST_EQUAL(vAveragesInt32.GetVariance(), 0);
  GG_TEST_EQUAL(vAveragesInt32.GetVarianceP(), 0);
  GG_TEST_EQUAL_FLOAT(vAveragesInt32.GetVariationCoefficient(), 0, 1000);
  GG_TEST_EQUAL(vAveragesInt32.GetSumOfSquaredErrors(), 0);

  vAveragesInt32.AddSample(50);
  // ggAveragesPrint(vAveragesInt32, "*******************************");
  GG_TEST_EQUAL(vAveragesInt32.GetNumberOfSamples(), 2.0);
  GG_TEST_EQUAL(vAveragesInt32.GetMin(), 10);
  GG_TEST_EQUAL(vAveragesInt32.GetMax(), 50);
  GG_TEST_EQUAL(vAveragesInt32.GetSum(), 60);
  GG_TEST_EQUAL(vAveragesInt32.GetMean(), 30);
  GG_TEST_EQUAL(vAveragesInt32.GetStdDev(), 28);
  GG_TEST_EQUAL(vAveragesInt32.GetStdDevP(), 20);
  GG_TEST_EQUAL(vAveragesInt32.GetVariance(), 800);
  GG_TEST_EQUAL(vAveragesInt32.GetVarianceP(), 400);
  GG_TEST_EQUAL_FLOAT(vAveragesInt32.GetVariationCoefficient(), 0.943, 1000);
  GG_TEST_EQUAL(vAveragesInt32.GetSumOfSquaredErrors(), 800);

  vAveragesInt32.AddSample(90);
  // ggAveragesPrint(vAveragesInt32, "*******************************");
  GG_TEST_EQUAL(vAveragesInt32.GetNumberOfSamples(), 3.0);
  GG_TEST_EQUAL(vAveragesInt32.GetMin(), 10);
  GG_TEST_EQUAL(vAveragesInt32.GetMax(), 90);
  GG_TEST_EQUAL(vAveragesInt32.GetSum(), 150);
  GG_TEST_EQUAL(vAveragesInt32.GetMean(), 50);
  GG_TEST_EQUAL(vAveragesInt32.GetStdDev(), 40);
  GG_TEST_EQUAL(vAveragesInt32.GetStdDevP(), 33);
  GG_TEST_EQUAL(vAveragesInt32.GetVariance(), 1600);
  GG_TEST_EQUAL(vAveragesInt32.GetVarianceP(), 1067);
  GG_TEST_EQUAL_FLOAT(vAveragesInt32.GetVariationCoefficient(), 0.8, 1000);
  GG_TEST_EQUAL(vAveragesInt32.GetSumOfSquaredErrors(), 3200);

  vAveragesInt32.AddSample(30, 2.0);
  // ggAveragesPrint(vAveragesInt32, "*******************************");
  GG_TEST_EQUAL(vAveragesInt32.GetNumberOfSamples(), 5.0);
  GG_TEST_EQUAL(vAveragesInt32.GetMin(), 10);
  GG_TEST_EQUAL(vAveragesInt32.GetMax(), 90);
  GG_TEST_EQUAL(vAveragesInt32.GetSum(), 210);
  GG_TEST_EQUAL(vAveragesInt32.GetMean(), 42);
  GG_TEST_EQUAL(vAveragesInt32.GetStdDev(), 30);
  GG_TEST_EQUAL(vAveragesInt32.GetStdDevP(), 27);
  GG_TEST_EQUAL(vAveragesInt32.GetVariance(), 920);
  GG_TEST_EQUAL(vAveragesInt32.GetVarianceP(), 736);
  GG_TEST_EQUAL_FLOAT(vAveragesInt32.GetVariationCoefficient(), 0.722, 1000);
  GG_TEST_EQUAL(vAveragesInt32.GetSumOfSquaredErrors(), 3680);
}


GG_TEST_REGISTER(ggAveragesTest);
