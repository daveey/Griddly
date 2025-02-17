#pragma once

#include <map>
#include <memory>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "CollisionDetectorFactory.hpp"
#include "DelayedActionQueueItem.hpp"
#include "GDY/Actions/Action.hpp"
#include "GDY/Objects/Object.hpp"
#include "LevelGenerators/LevelGenerator.hpp"
#include "Util/RandomGenerator.hpp"
#include "Util/util.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#define TileObjects std::map<uint32_t, std::shared_ptr<Object>>

namespace griddly {

enum class TriggerType {
  NONE,
  RANGE_BOX_BOUNDARY,
  RANGE_BOX_AREA,
};

struct ActionTriggerDefinition {
  std::unordered_set<std::string> sourceObjectNames;
  std::unordered_set<std::string> destinationObjectNames;
  TriggerType triggerType = TriggerType::RANGE_BOX_AREA;
  uint32_t range = 1;
  bool relative = false;
  glm::ivec2 offset{};
};

// Structure to hold information about the events that have happened at each time step
struct GridEvent {
  uint32_t playerId;
  std::string actionName;
  uint32_t tick = 0;
  std::unordered_map<uint32_t, int32_t> rewards;
  uint32_t delay = 0;

  std::string sourceObjectName;
  std::string destObjectName;

  uint32_t sourceObjectPlayerId = 0;
  uint32_t destinationObjectPlayerId = 0;

  glm::ivec2 sourceLocation;
  glm::ivec2 destLocation;
};

struct GlobalVariableDefinition {
  int32_t initialValue = 0;
  bool perPlayer = false;
};

class Grid : public std::enable_shared_from_this<Grid> {
 public:
  Grid();
  explicit Grid(std::shared_ptr<CollisionDetectorFactory> collisionDetectorFactory);
  virtual ~Grid();

  virtual void setPlayerCount(uint32_t playerCount);
  virtual uint32_t getPlayerCount() const;
  virtual void resetMap(uint32_t width, uint32_t height);
  virtual void resetGlobalVariables(const std::map<std::string, GlobalVariableDefinition>& globalVariableDefinitions);
  virtual void setGlobalVariables(const std::unordered_map<std::string, std::unordered_map<uint32_t, int32_t>>& globalVariableDefinitions);

  virtual std::unordered_map<uint32_t, int32_t> performActions(uint32_t playerId, std::vector<std::shared_ptr<Action>> actions);
  virtual std::unordered_map<uint32_t, int32_t> executeAction(uint32_t playerId, std::shared_ptr<Action> action);
  virtual void delayAction(uint32_t playerId, std::shared_ptr<Action> action);

  virtual std::unordered_map<uint32_t, int32_t> update();
  virtual std::unordered_map<uint32_t, int32_t> processDelayedActions();

  virtual std::unordered_map<uint32_t, int32_t> processCollisions();
  virtual void addActionTrigger(std::string actionName, ActionTriggerDefinition actionTriggerDefinition);
  virtual void setBehaviourProbabilities(const std::unordered_map<std::string, std::vector<float>>& behaviourProbabilities);

  virtual const DelayedActionQueue& getDelayedActions();

  virtual bool updateLocation(std::shared_ptr<Object> object, glm::ivec2 previousLocation, glm::ivec2 newLocation);

  // Mark a particular location to be repainted
  virtual bool invalidateLocation(glm::ivec2 location);

  virtual const std::unordered_set<glm::ivec2>& getUpdatedLocations(uint32_t player) const;
  virtual void purgeUpdatedLocations(uint32_t player);

  virtual uint32_t getWidth() const;
  virtual uint32_t getHeight() const;

  virtual std::shared_ptr<int32_t> getTickCount() const;
  virtual void setTickCount(int32_t tickCount);

  virtual void initObject(std::string objectName, std::vector<std::string> objectVariableNames);

  virtual void addObject(glm::ivec2 location, std::shared_ptr<Object> object, bool applyInitialActions = true, std::shared_ptr<Action> originatingAction = nullptr, DiscreteOrientation orientation = DiscreteOrientation());

  virtual bool removeObject(std::shared_ptr<Object> object);

  virtual const std::unordered_set<std::shared_ptr<Object>>& getObjects();

  virtual void addPlayerDefaultEmptyObject(std::shared_ptr<Object> emptyObject);

  virtual void addPlayerDefaultBoundaryObject(std::shared_ptr<Object> boundaryObject);

  virtual std::shared_ptr<Object> getPlayerDefaultEmptyObject(uint32_t playerId) const;

  virtual std::shared_ptr<Object> getPlayerDefaultBoundaryObject(uint32_t playerId) const;

  /**
   * Gets all the objects at a certain location
   */
  virtual const TileObjects& getObjectsAt(glm::ivec2 location) const;

  /**
   * Gets the object with the highest Z index at a certain tile location
   */
  virtual std::shared_ptr<Object> getObject(glm::ivec2 location) const;

  /**
   * Get a list of the objects and their Ids in this grid
   */
  virtual const std::unordered_map<std::string, uint32_t>& getObjectIds() const;

  /**
   * Get a list of the object variables and their Ids in this grid
   */
  virtual const std::unordered_map<std::string, uint32_t>& getObjectVariableIds() const;

  /**
   * Gets an ordered list of objectVariableNames
   */
  virtual const std::vector<std::string> getAllObjectVariableNames() const;

  /**
   * Get a mapping of objects to their defined variables
   */
  virtual const std::unordered_map<std::string, std::vector<std::string>> getObjectVariableMap() const;

  /**
   * Gets an ordered list of objectNames
   */
  virtual const std::vector<std::string> getObjectNames() const;

  /**
   * Get a mapping of the avatar objects for players in the environment
   */
  virtual std::unordered_map<uint32_t, std::shared_ptr<Object>> getPlayerAvatarObjects() const;

  virtual std::unordered_map<uint32_t, std::shared_ptr<int32_t>> getObjectCounter(std::string objectName);

  virtual const std::map<std::string, std::unordered_map<uint32_t, std::shared_ptr<int32_t>>>& getGlobalVariables() const;

  virtual void enableHistory(bool enable);
  virtual const std::vector<GridEvent>& getHistory() const;
  virtual void purgeHistory();

  // These are public so they can be tested
  virtual const std::unordered_map<std::string, std::shared_ptr<CollisionDetector>>& getCollisionDetectors() const;
  virtual const std::unordered_map<std::string, ActionTriggerDefinition>& getActionTriggerDefinitions() const;
  virtual const std::unordered_map<std::string, std::unordered_set<std::string>>& getSourceObjectCollisionActionNames() const;
  virtual const std::unordered_map<std::string, std::unordered_set<std::string>>& getObjectCollisionActionNames() const;

  virtual void addCollisionDetector(std::unordered_set<std::string> objectNames, std::string actionName, std::shared_ptr<CollisionDetector> collisionDetector);

  virtual void reset();

  virtual void seedRandomGenerator(uint32_t seed);

  virtual std::shared_ptr<RandomGenerator> getRandomGenerator() const;

 private:
  GridEvent buildGridEvent(const std::shared_ptr<Action>& action, uint32_t playerId, uint32_t tick) const;
  void recordGridEvent(GridEvent event, std::unordered_map<uint32_t, int32_t> rewards);

  const std::vector<std::shared_ptr<CollisionDetector>> getCollisionDetectorsForObject(std::shared_ptr<Object> object) const;

  std::unordered_map<uint32_t, int32_t> executeAndRecord(uint32_t playerId, const std::shared_ptr<Action>& action);

  std::vector<uint32_t> filterBehaviourProbabilities(std::vector<uint32_t> actionBehaviourIdxs, std::vector<float> actionProbabilities);

  uint32_t height_{};
  uint32_t width_{};

  const std::shared_ptr<int32_t> gameTicks_;

  // For every game tick record a list of locations that should be updated.
  // This is so we can highly optimize observers to only re-render changed grid locations
  std::vector<std::unordered_set<glm::ivec2>> updatedLocations_;

  std::unordered_map<std::string, uint32_t> objectIds_;
  std::set<std::string> objectVariables_;
  std::unordered_map<std::string, uint32_t> objectVariableIds_;
  std::unordered_map<std::string, std::vector<std::string>> objectVariableMap_;
  std::unordered_set<std::shared_ptr<Object>> objects_;
  std::unordered_map<glm::ivec2, TileObjects> occupiedLocations_;
  std::unordered_map<std::string, std::unordered_map<uint32_t, std::shared_ptr<int32_t>>> objectCounters_;
  std::unordered_map<uint32_t, std::shared_ptr<Object>> playerAvatars_;
  std::map<std::string, std::unordered_map<uint32_t, std::shared_ptr<int32_t>>> globalVariables_;

  // return reference to this if there are no object in getObjectAt
  const std::map<uint32_t, std::shared_ptr<Object>> EMPTY_OBJECTS = {};
  const std::unordered_set<glm::ivec2> EMPTY_LOCATIONS = {};

  // A priority queue of actions that are delayed in time (time is measured in game ticks)
  DelayedActionQueue delayedActions_;
  std::unordered_map<std::string, std::vector<float>> behaviourProbabilities_;

  // There is at least 1 player
  uint32_t playerCount_ = 1;

  bool recordEvents_ = false;
  std::vector<GridEvent> eventHistory_;

  // If there are collisions that need to be processed in this game environment

  // All objects that can collide
  std::unordered_map<std::string, std::unordered_set<std::string>> collisionObjectActionNames_;

  // Only the source objects that can collide
  std::unordered_map<std::string, std::unordered_set<std::string>> collisionSourceObjectActionNames_;

  // keep a list of the objects that are named as collision sources, this makes collision processing significantly faster with large maps with many non-colliding objects
  std::unordered_set<std::shared_ptr<Object>> collisionSourceObjects_;

  // Collision detectors are grouped by action name (i.e each trigger)
  std::shared_ptr<CollisionDetectorFactory> collisionDetectorFactory_;
  std::unordered_map<std::string, std::shared_ptr<CollisionDetector>> collisionDetectors_;
  std::unordered_map<std::string, ActionTriggerDefinition> actionTriggerDefinitions_;

  // An object that is used if the source of destination location of an action is '_empty'
  // Allows a subset of actions like "spawn" to be performed in empty space.
  std::unordered_map<uint32_t, std::shared_ptr<Object>> defaultEmptyObject_;

  // An object that is used if the source of destination location of an action is '_boundary'
  // This is used for special actions where objects go out of bounds for the level
  std::unordered_map<uint32_t, std::shared_ptr<Object>> defaultBoundaryObject_;

  std::shared_ptr<RandomGenerator> randomGenerator_ = std::make_shared<RandomGenerator>(RandomGenerator());
};

}  // namespace griddly
