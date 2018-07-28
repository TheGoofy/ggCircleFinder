#ifndef GGLINETYPES_H
#define GGLINETYPES_H

#include "ggNumberTypes.h"
#include "ggVector2T.h"
#include "ggVector3T.h"
#include "ggLineT.h"

template <class TValueType>
using ggLine2T = ggLineT<ggVector2T<TValueType>>;

typedef ggLine2T<ggInt32> ggLine2Int32;
typedef ggLine2T<ggInt64> ggLine2Int64;
typedef ggLine2T<ggFloat> ggLine2Float;
typedef ggLine2T<ggDouble> ggLine2Double;

template <class TValueType>
using ggLine3T = ggLineT<ggVector3T<TValueType>>;

typedef ggLine3T<ggInt32> ggLine3Int32;
typedef ggLine3T<ggInt64> ggLine3Int64;
typedef ggLine3T<ggFloat> ggLine3Float;
typedef ggLine3T<ggDouble> ggLine3Double;

#endif // GGLINETYPES_H
