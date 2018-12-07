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
    std::cout << GG_FILE_LINE << " test equal failed: " << #aExpr1 << " == " << #aExpr2 << std::endl; \
  } }

#define GG_TEST_EQUAL_FLOAT(aExpr1, aExpr2, aFactor) { \
  ggUnitTest::CountFunction(); \
  if (!(ggRound<ggInt64>(aFactor*(aExpr1)) == ggRound<ggInt64>(aFactor*(aExpr2)))) { \
    ggUnitTest::CountFunctionFail(); \
    std::cout << GG_FILE_LINE << " test equal failed: " << #aExpr1 << " == " << #aExpr2 << std::endl; \
  } }

#define GG_TEST_REGISTER(aUnitTest) \
  static bool ggRegistered_##aUnitTest = ggUnitTest::Register(aUnitTest, #aUnitTest, GG_FILE_LINE);

namespace ggUnitTest
{
  bool Register(std::function<void()> aTest,
                const char* aTestName,
                const char* aFileLine);

  void CountFunction();
  void CountFunctionFail();
  void ExecuteAll();
}

#endif // GGUNITTEST_H
