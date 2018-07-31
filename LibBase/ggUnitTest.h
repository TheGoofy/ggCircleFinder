#ifndef GGUNITTEST_H
#define GGUNITTEST_H

#define GG_TEST(aCondition) \
  if (!(aCondition)) { \
    std::cout << __FILE__ << ":" << __LINE__ << " " << #aCondition << " failed!" << std::endl; \
  }

#define GG_TEST_EQUAL(aExpr1, aExpr2) \
  if (!((aExpr1) == (aExpr2))) { \
    std::cout << __FILE__ << ":" << __LINE__ << " " << #aExpr1 << "=" << aExpr1 << " and " << #aExpr2 << "=" << aExpr2 << " are not equal!" << std::endl; \
  }

#endif // GGUNITTEST_H
