// Joseph Arhar

#include "GameUpdater.h"
#include <iostream>

#define DISTANCE_BELOW_CAMERA 3
#define DISTANCE_BEHIND_CAMERA 3
#define DELTA_Y 0.007f
#define MIN_DELTA_X 0.055f

namespace {

// TODO(jarhar): consider changing game object storage from
// shared_ptr<vector<...>> to vector<shared_ptr<...>>
// so that this function can return better references to game objects
std::vector<GameObject> GetCollidingObjects(
    std::shared_ptr<GameObject> primary_object,
    std::shared_ptr<std::vector<GameObject>> secondary_objects) {
  std::vector<GameObject> colliding_objects;

  // TODO(jarhar): make this more efficient by culling secondary objects
  for (GameObject secondary_object : *secondary_objects) {
    if (AxisAlignedBox::IsColliding(
          primary_object->GetBoundingBox(),
          secondary_object.GetBoundingBox())) {
      colliding_objects.push_back(secondary_object);
    }
  }

  return colliding_objects;
}

}

GameUpdater::GameUpdater()
    : level_updater(std::make_shared<LevelUpdater>()), done(false) {}

GameUpdater::~GameUpdater() {}

void GameUpdater::Update(std::shared_ptr<GameState> game_state) {
  level_updater->Update(game_state->GetLevel());
  std::shared_ptr<GameCamera> camera = game_state->GetCamera();

  std::shared_ptr<Player> player = game_state->GetPlayer();

  float dY =
      (level_updater->CurrentPlatform(game_state->GetLevel()).GetPosition().y +
       DISTANCE_BELOW_CAMERA) -
      camera->getPosition()[1];

  float dX =
      (level_updater->CurrentPlatform(game_state->GetLevel()).GetPosition().x -
       DISTANCE_BEHIND_CAMERA) -
      camera->getPosition()[0];
  // smooth out camera transition
  if (std::abs(dY) > DELTA_Y) {
    if (dY < 0) {
      dY = -DELTA_Y;
    } else {
      dY = DELTA_Y;
    }
  }
  // always move camera forward
  if (dX < MIN_DELTA_X) {
    dX = MIN_DELTA_X;
  }
  camera->setPosition(glm::vec3(camera->getPosition()[0] + dX,
                                camera->getPosition()[1] + dY,
                                camera->getPosition()[2]));
  camera->setLookAt(glm::vec3(camera->getLookAt()[0] + dX,
                              camera->getLookAt()[1] + dY,
                              camera->getLookAt()[2]));

  player->SetPosition(glm::vec3(player->GetPosition()[0] + dX,
                                player->GetPosition()[1] + dY,
                                player->GetPosition()[2]));

  if (level_updater->Done()) {
    done = true;
    game_state->SetDone(done);
  }
}

void GameUpdater::Reset(std::shared_ptr<GameState> game_state) {
  level_updater->Reset(game_state->GetLevel());
  done = false;
  game_state->SetDone(done);
}
