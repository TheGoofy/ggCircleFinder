#include "LibBase/ggUnitTest.h"

#include <vector>
#include <iostream>

#include "LibBase/ggNumberTypes.h"


typedef std::vector<std::function<void()>> tUnitTests;
static tUnitTests* ggUnitTests = nullptr;
static ggUInt64 ggCountFunctions = 0;
static ggUInt64 ggCountFunctionsFail = 0;


bool ggUnitTest::Register(std::function<void()> aUnitTest)
{
  static tUnitTests vUnitTests;
  vUnitTests.push_back(aUnitTest);
  ggUnitTests = &vUnitTests;
  return true;
}


void ggUnitTest::CountFunction()
{
  ggCountFunctions++;
}


void ggUnitTest::CountFunctionFail()
{
  ggCountFunctionsFail++;
}


void ggUnitTest::ExecuteAll()
{
  std::cout << "Executing " << ggUnitTests->size() << " unit tests." << std::endl;
  std::for_each(ggUnitTests->begin(), ggUnitTests->end(), [] (std::function<void()> aUnitTest) {
    aUnitTest();
  });
  std::cout << "Tested " << ggCountFunctions << " functions, " << ggCountFunctionsFail << " failed!" << std::endl;
}
