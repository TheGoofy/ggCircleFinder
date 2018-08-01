#ifndef GGUNITTEST_H
#define GGUNITTEST_H

#include <iostream>
#include <functional>

#define GG_STRINGIFY(x) #x
#define GG_TO_STRING(x) GG_STRINGIFY(x)
#define GG_FILE_LINE (__FILE__ ":" GG_TO_STRING(__LINE__))

#define GG_TEST(aCondition) { \
  bool vSucceededDummy = true; \
  GG_TEST2(aCondition, vSucceededDummy); }

#define GG_TEST2(aCondition, aSucceeded) { \
  ggUnitTest::CountFunction(); \
  if (!(aCondition)) { \
    aSucceeded = false; \
    ggUnitTest::CountFunctionFail(); \
    std::cout << GG_FILE_LINE << " " << #aCondition << " failed!" << std::endl; \
  } }

#define GG_TEST_EQUAL(aExpr1, aExpr2) { \
  bool vSucceededDummy = true; \
  GG_TEST_EQUAL2(aExpr1, aExpr2, vSucceededDummy); }

#define GG_TEST_EQUAL2(aExpr1, aExpr2, aSucceeded) { \
  ggUnitTest::CountFunction(); \
  if (!((aExpr1) == (aExpr2))) { \
    aSucceeded = false; \
    ggUnitTest::CountFunctionFail(); \
    std::cout << GG_FILE_LINE << " " << #aExpr1 << "=" << aExpr1 << " and " << #aExpr2 << "=" << aExpr2 << " are not equal!" << std::endl; \
  } }

namespace ggUnitTest
{
  bool Register(std::function<void()> aUnitTest);
  void CountFunction();
  void CountFunctionFail();
  void ExecuteAll();
}

#endif // GGUNITTEST_H
