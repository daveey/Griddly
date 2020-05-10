#include "Griddy/Core/Grid.hpp"
#include "gmock/gmock.h"

namespace griddy {
class MockGrid : public Grid {
 public:
  MockGrid() : Grid() {}

  MOCK_METHOD(void, init, (uint32_t width, uint32_t height), ());
  MOCK_METHOD(void, update, (std::vector<std::shared_ptr<Action>> actions), ());

  MOCK_METHOD((std::unordered_set<GridLocation, GridLocation::Hash>), getUpdatedLocations, () ,(const));

  MOCK_METHOD(uint32_t, getWidth, (), (const));
  MOCK_METHOD(uint32_t, getHeight, (), (const));
  
  MOCK_METHOD(bool, updateLocation, (std::shared_ptr<Object> object, GridLocation previousLocation, GridLocation newLocation), ());

  MOCK_METHOD(std::vector<int>, performActions, (int playerId, std::vector<std::shared_ptr<Action>> actions), ());

  MOCK_METHOD(void, initObject, (uint32_t playerId, GridLocation location, std::shared_ptr<Object> object), ());
  MOCK_METHOD(bool, removeObject, (std::shared_ptr<Object> object), ());
  
  MOCK_METHOD(std::unordered_set<std::shared_ptr<Object>>&, getObjects, (), ());
  MOCK_METHOD(std::shared_ptr<Object>, getObject, (GridLocation location), (const));
};
}  // namespace griddy