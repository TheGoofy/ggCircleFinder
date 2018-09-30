#include "LibBase/ggUnitTest.h"

#include <vector>
#include <iostream>
#include <assert.h>
#include <algorithm>

#include "LibBase/ggNumberTypes.h"


static ggUInt64 ggCountFunctions = 0;
static ggUInt64 ggCountFunctionsFail = 0;


struct ggUnitTestFunc {

  bool Execute() const {
    try {
      ggUInt64 vCountFunctionsFail = ggCountFunctionsFail;
      mTest();
      return vCountFunctionsFail == ggCountFunctionsFail;
    }
    catch (const std::exception& vException) {
      Print(vException.what());
    }
    catch (const char* vException) {
      Print(vException);
    }
    catch (int vException) {
      Print(vException);
    }
    catch (...) {
      Print("unknown");
    }
    return false;
  }

  template <typename TWhat>
  void Print(const TWhat& aWhat) const {
    if (mFileLine != nullptr) std::cout << mFileLine << " - ";
    std::cout << "Caught exception \"" << aWhat << "\" while executing unit test";
    if (mTestName != nullptr) std::cout << " \"" << mTestName << "\"";
    std::cout << "!" << std::endl;
  }

  std::function<void()> mTest;
  const char* mTestName;
  const char* mFileLine;
};


typedef std::vector<ggUnitTestFunc> tUnitTests;
static tUnitTests* ggUnitTests = nullptr;


bool ggUnitTest::Register(std::function<void()> aTest,
                          const char* aTestName,
                          const char* aFileLine)
{
  static tUnitTests vUnitTests;
  ggUnitTestFunc vUnitTestFunc;
  vUnitTestFunc.mTest = aTest;
  vUnitTestFunc.mTestName = aTestName;
  vUnitTestFunc.mFileLine = aFileLine;
  vUnitTests.push_back(vUnitTestFunc);
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
  ggUSize vCountUnitsSucceeded = 0;
  ggUSize vCountUnitsFail = 0;

  ggUnitTestFunc vUnitTestFunc;

  vUnitTestFunc.mTest = [&vCountUnitsSucceeded, &vCountUnitsFail] () {
    assert(ggUnitTests != nullptr);
    std::for_each(ggUnitTests->begin(), ggUnitTests->end(), [&vCountUnitsSucceeded, &vCountUnitsFail] (const ggUnitTestFunc& aUnitTestFunc) {
      aUnitTestFunc.Execute() ? ++vCountUnitsSucceeded : ++vCountUnitsFail;
    });
  };

  vUnitTestFunc.mTestName = __PRETTY_FUNCTION__;
  vUnitTestFunc.mFileLine = GG_FILE_LINE;

  std::cout << "Executing " << ggUnitTests->size() << " unit tests." << std::endl;
  vUnitTestFunc.Execute();
  std::cout << "Tested " << vCountUnitsSucceeded + vCountUnitsFail << " units, " << vCountUnitsFail << " failed!" << std::endl;
  std::cout << "Tested " << ggCountFunctions << " functions, " << ggCountFunctionsFail << " failed!" << std::endl;
}
