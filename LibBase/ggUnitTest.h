#ifndef GGUNITTEST_H
#define GGUNITTEST_H

#include <functional>

#define GG_TEST(aCondition) \
  ggUnitTest::CountFunction(); \
  if (!(aCondition)) { \
    ggUnitTest::CountFunctionFail(); \
    std::cout << __FILE__ << ":" << __LINE__ << " " << #aCondition << " failed!" << std::endl; \
  }

#define GG_TEST_EQUAL(aExpr1, aExpr2) \
  ggUnitTest::CountFunction(); \
  if (!((aExpr1) == (aExpr2))) { \
    ggUnitTest::CountFunctionFail(); \
    std::cout << __FILE__ << ":" << __LINE__ << " " << #aExpr1 << "=" << aExpr1 << " and " << #aExpr2 << "=" << aExpr2 << " are not equal!" << std::endl; \
  }

namespace ggUnitTest
{
  bool Register(std::function<void()> aUnitTest);
  void CountFunction();
  void CountFunctionFail();
  void ExecuteAll();
}

#endif // GGUNITTEST_H
