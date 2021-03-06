//
// Created by Igor Frank on 06.01.20.
//

#include "BaseMap.h"
#include "OverflowMap.h"
#include "RiverMap.h"
#include "StairsMap.h"
#include "TearDropMap.h"
#include "WaterfallMap.h"
#include <BasicBowlMap.h>
#include <error_handling/exceptions.h>
std::unique_ptr<BaseMap> BaseMap::CreateInstance(MapType type) {
  switch (type) {
  case MapType::BasicBowl:
    return std::make_unique<BasicBowlMap>();
  case MapType::Waterfall:
    return std::make_unique<WaterfallMap>();
  case MapType::Overflow:
    return std::make_unique<OverflowMap>();
  case MapType::Stairs:
    return std::make_unique<StairsMap>();
  case MapType::River:
    return std::make_unique<RiverMap>();
  case MapType::TearDrop:
    return std::make_unique<TearDropMap>();
  default:
    throw exc::InternalError{"Unsupported MapType value."};
  }
}
