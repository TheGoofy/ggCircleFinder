#include <vector>

#include "LibBase/ggUnitTest.h"
#include "LibBase/ggFilterFirInputBufferT.h"
#include "LibBase/ggNumberTypes.h"
#include "LibBase/ggVectorTypes.h"


/**
 * Since "ggFilterFirT" is abstract, we need to derive a class for testing it
 */
template <class TValueType>
class ggFilterFirInputBufferTestT : public ggFilterFirInputBufferT<TValueType> {

public:

  // base filter type (shortcut)
  typedef ggFilterFirInputBufferT<TValueType> tFilterFirInputBuffer;

  // construct filter with specific order
  ggFilterFirInputBufferTestT(ggUSize aOrder)
  : tFilterFirInputBuffer(aOrder) {
  }

  // virtual destructor (in order to satisfy compiler warning)
  virtual ~ggFilterFirInputBufferTestT() {
  }

  // allow test function to access all members
  friend void ggFilterFirInputBufferTestInt32();

protected:

  // implement abstract member, simply output the current input
  virtual void Calculate(TValueType& aOutputValue) override {
    aOutputValue = tFilterFirInputBuffer::GetIn();
  }

};


static void ggFilterFirInputBufferTestInt32()
{
  ggFilterFirInputBufferTestT<ggInt32> vFilter(4);

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


static void ggFilterFirInputBufferTest()
{
  ggFilterFirInputBufferTestInt32();
}


GG_TEST_REGISTER(ggFilterFirInputBufferTest);
