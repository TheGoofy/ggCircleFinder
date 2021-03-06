// 0) include own header
#include "ggDecoration.h"

// 1) include system or QT

// 2) include own project-related (sort by component dependency)


ggDecoration::ggDecoration() :
  mType(cType::eNothing),
  mLength(0.0),
  mFill(cFill::eEmpty)
{
}


ggDecoration::ggDecoration(cType aType,
                           double aLength,
                           cFill aFill) :
  mType(aType),
  mLength(aLength),
  mFill(aFill)
{
}


void ggDecoration::SetType(cType aType)
{
  mType = aType;
}


ggDecoration::cType ggDecoration::GetType() const
{
  return mType;
}


void ggDecoration::SetLength(double aLength)
{
  mLength = aLength;
}


double ggDecoration::GetLength() const
{
  return mLength;
}


void ggDecoration::SetFill(cFill aFill)
{
  mFill = aFill;
}


ggDecoration::cFill ggDecoration::GetFill() const
{
  return mFill;
}


bool ggDecoration::GetFillEmpty() const
{
  return mFill == cFill::eEmpty;
}


bool ggDecoration::GetFillSolid() const
{
  return mFill == cFill::eSolid;
}


void ggDecoration::Set(cType aType,
                       double aLength,
                       cFill aFill)
{
  mType = aType;
  mLength = aLength;
  mFill = aFill;
}


const ggDecoration::tTypes& ggDecoration::Types()
{
  static tTypes vTypes = {cType::eNothing,
                          cType::eLine,
                          cType::eArrow,
                          cType::eArrowBack,
                          cType::eTriangle,
                          cType::eTriangleBack,
                          cType::eDiamond,
                          cType::eCross,
                          cType::eCircle};
  return vTypes;
}

