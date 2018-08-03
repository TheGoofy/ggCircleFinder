#ifndef GGUNITTEST_H
#define GGUNITTEST_H

#include <iostream>
#include <functional>

#include "LibBase/ggDebug.h"

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
    std::cout << GG_FILE_LINE << " expressions not equal: " << #aExpr1 << " ==> " << aExpr1 << " is different from " << #aExpr2 << " ==> " << aExpr2 << std::endl; \
  } }

namespace ggUnitTest
{
  bool Register(std::function<void()> aUnitTest);
  void CountFunction();
  void CountFunctionFail();
  void ExecuteAll();
}

#endif // GGUNITTEST_H
