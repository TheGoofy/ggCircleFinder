#ifndef GGSpotTYPES_H
#define GGSpotTYPES_H

#include "ggSpotT.h"
#include "ggVectorTypes.h"

typedef ggSpotT<ggInt8, ggVector2Double> ggSpot2Int8;
typedef ggSpotT<ggInt16, ggVector2Double> ggSpot2Int16;
typedef ggSpotT<ggInt32, ggVector2Double> ggSpot2Int32;
typedef ggSpotT<ggInt64, ggVector2Double> ggSpot2Int64;
typedef ggSpotT<ggFloat, ggVector2Double> ggSpot2Float;
typedef ggSpotT<ggDouble, ggVector2Double> ggSpot2Double;

typedef ggSpotT<ggInt8, ggVector3Double> ggSpot3Int8;
typedef ggSpotT<ggInt16, ggVector3Double> ggSpot3Int16;
typedef ggSpotT<ggInt32, ggVector3Double> ggSpot3Int32;
typedef ggSpotT<ggInt64, ggVector3Double> ggSpot3Int64;
typedef ggSpotT<ggFloat, ggVector3Double> ggSpot3Float;
typedef ggSpotT<ggDouble, ggVector3Double> ggSpot3Double;

#endif // GGSpotTYPES_H
