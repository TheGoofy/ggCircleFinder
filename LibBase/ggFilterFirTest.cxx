#include <vector>

#include "LibBase/ggUnitTest.h"
#include "LibBase/ggFilterFirT.h"
#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggVectorTypes.h"


/**
 * Since "ggFilterFirT" is abstract, we need to derive a class for testing it
 */
template <class TValueType>
class ggFilterFirTestT : public ggFilterFirT<TValueType> {

public:

  // base filter type (shortcut)
  typedef ggFilterFirT<TValueType> tFilterFir;

  // construct filter with specific order
  ggFilterFirTestT(ggUSize aOrder)
  : tFilterFir(aOrder) {
  }

  // virtual destructor (in order to satisfy compiler warning)
  virtual ~ggFilterFirTestT() {
  }

  // allow test function to access all members
  friend void ggFilterFirTestInt32();

protected:

  // implement abstract member, simply output the current input
  virtual void Calculate(TValueType& aOutputValue) override {
    aOutputValue = tFilterFir::GetIn();
  }

};


static void ggFilterFirTestInt32()
{
  ggFilterFirTestT<ggInt32> vFilter(4);

  GG_TEST_EQUAL(vFilter.GetOrder(), 4);
  GG_TEST_EQUAL(vFilter.mInputIndex, static_cast<ggUSize>(-1));
  GG_TEST_EQUAL(vFilter.GetInputIndex(0), static_cast<ggUSize>(-1));
  GG_TEST_EQUAL(vFilter.mInputValues.size(), 0);

  vFilter.PushIn(3); // filter contains {3}
  GG_TEST_EQUAL(vFilter.GetIn(), 3);
  GG_TEST_EQUAL(vFilter.GetOut(), 3);
  GG_TEST_EQUAL(vFilter.mInputValues.size(), 1);
  GG_TEST_EQUAL(vFilter.mInputIndex, static_cast<ggUSize>(0));
  GG_TEST_EQUAL(vFilter.GetInputIndex(0), static_cast<ggUSize>(0));
  GG_TEST_EQUAL(vFilter.GetInputIndex(1), static_cast<ggUSize>(0));
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(0)], 3);
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(1)], 3);

  vFilter.PushIn(4); // filter contains {4,3}
  GG_TEST_EQUAL(vFilter.GetIn(), 4);
  GG_TEST_EQUAL(vFilter.GetOut(), 4);
  GG_TEST_EQUAL(vFilter.mInputValues.size(), 2);
  GG_TEST_EQUAL(vFilter.mInputIndex, static_cast<ggUSize>(1));
  GG_TEST_EQUAL(vFilter.GetInputIndex(0), static_cast<ggUSize>(1));
  GG_TEST_EQUAL(vFilter.GetInputIndex(1), static_cast<ggUSize>(0));
  GG_TEST_EQUAL(vFilter.GetInputIndex(2), static_cast<ggUSize>(0));
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(0)], 4);
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(1)], 3);
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(2)], 3);

  vFilter.PushIn(5); // filter contains {5,4,3}
  GG_TEST_EQUAL(vFilter.GetIn(), 5);
  GG_TEST_EQUAL(vFilter.GetOut(), 5);
  GG_TEST_EQUAL(vFilter.mInputValues.size(), 3);
  GG_TEST_EQUAL(vFilter.mInputIndex, static_cast<ggUSize>(2));
  GG_TEST_EQUAL(vFilter.GetInputIndex(0), static_cast<ggUSize>(2));
  GG_TEST_EQUAL(vFilter.GetInputIndex(1), static_cast<ggUSize>(1));
  GG_TEST_EQUAL(vFilter.GetInputIndex(2), static_cast<ggUSize>(0));
  GG_TEST_EQUAL(vFilter.GetInputIndex(3), static_cast<ggUSize>(0));
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(0)], 5);
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(1)], 4);
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(2)], 3);
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(3)], 3);

  vFilter.PushIn(6); // filter contains {6,5,4,3}
  GG_TEST_EQUAL(vFilter.GetIn(), 6);
  GG_TEST_EQUAL(vFilter.GetOut(), 6);
  GG_TEST_EQUAL(vFilter.mInputValues.size(), 4);
  GG_TEST_EQUAL(vFilter.mInputIndex, static_cast<ggUSize>(3));
  GG_TEST_EQUAL(vFilter.GetInputIndex(0), static_cast<ggUSize>(3));
  GG_TEST_EQUAL(vFilter.GetInputIndex(1), static_cast<ggUSize>(2));
  GG_TEST_EQUAL(vFilter.GetInputIndex(2), static_cast<ggUSize>(1));
  GG_TEST_EQUAL(vFilter.GetInputIndex(3), static_cast<ggUSize>(0));
  GG_TEST_EQUAL(vFilter.GetInputIndex(4), static_cast<ggUSize>(0));
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(0)], 6);
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(1)], 5);
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(2)], 4);
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(3)], 3);
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(4)], 3);

  vFilter.PushIn(7); // filter contains {7,6,5,4} (3 has gone)
  GG_TEST_EQUAL(vFilter.GetIn(), 7);
  GG_TEST_EQUAL(vFilter.GetOut(), 7);
  GG_TEST_EQUAL(vFilter.mInputValues.size(), 4);
  GG_TEST_EQUAL(vFilter.mInputIndex, static_cast<ggUSize>(0));
  GG_TEST_EQUAL(vFilter.GetInputIndex(0), static_cast<ggUSize>(0));
  GG_TEST_EQUAL(vFilter.GetInputIndex(1), static_cast<ggUSize>(3));
  GG_TEST_EQUAL(vFilter.GetInputIndex(2), static_cast<ggUSize>(2));
  GG_TEST_EQUAL(vFilter.GetInputIndex(3), static_cast<ggUSize>(1));
  GG_TEST_EQUAL(vFilter.GetInputIndex(4), static_cast<ggUSize>(1));
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(0)], 7);
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(1)], 6);
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(2)], 5);
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(3)], 4);
  GG_TEST_EQUAL(vFilter.mInputValues[vFilter.GetInputIndex(4)], 4);
}


static void ggFilterFirTest()
{
  ggFilterFirTestInt32();
}


GG_TEST_REGISTER(ggFilterFirTest);
