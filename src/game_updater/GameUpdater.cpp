// Joseph Arhar

#include "game_updater/GameUpdater.h"

#include <iostream>

#include "helpers/Logging.h"
#include <glm/ext.hpp>

#define DISTANCE_BELOW_CAMERA 3
#define DISTANCE_BEHIND_CAMERA 3
#define DELTA_Y 0.007f
#define MIN_DELTA_X 0.055f

namespace {

std::vector<std::shared_ptr<GameObject>> GetCollidingObjects(
    AxisAlignedBox primary_object,
    std::shared_ptr<std::vector<std::shared_ptr<Platform>>> secondary_objects) {
  /*std::string bunnymin = glm::to_string(primary_object.GetMin());
  std::string bunnymax = glm::to_string(primary_object.GetMax());
  std::string boxmin = glm::to_string(secondary_objects->at(1).GetBoundingBox().GetMin());
  std::string boxmax = glm::to_string(secondary_objects->at(1).GetBoundingBox().GetMax());
  LOG("bunny box - min: " << bunnymin << ", max: " << bunnymax);
  LOG("first box - min: " << boxmin << ", max: " << boxmax);*/
  std::vector<std::shared_ptr<GameObject>> colliding_objects;
  // TODO(jarhar): make this more efficient by culling secondary objects
  for (std::shared_ptr<GameObject> secondary_object : *secondary_objects) {
    if (AxisAlignedBox::IsColliding(
          primary_object,
          secondary_object->GetBoundingBox())) {
      //LOG("COLLISION");
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
  //LOG("player position y: " << player->GetPosition().y);

  /*glm::vec3 current_platform_position =
    level_updater->CurrentPlatform(game_state->GetLevel()).GetPosition();

  float dY =
      (current_platform_position.y +
       DISTANCE_BELOW_CAMERA) -
      camera->getPosition()[1];

  float dX =
      (current_platform_position.x -
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
  }*/

  /*float player_y_pos = max(player->GetPosition()[1] + dY
                              + player->GetVerticalVelocity(),
                              current_platform_position.y + dY + 0.75f);*/
  /*camera->setPosition(glm::vec3(camera->getPosition()[0] + dX,
                                camera->getPosition()[1] + dY,
                                camera->getPosition()[2]));
  camera->setLookAt(glm::vec3(camera->getLookAt()[0] + dX,
                              camera->getLookAt()[1] + dY,
                              camera->getLookAt()[2]));*/
  float dX = 0.05f; // TODO falls through platforms when this is 0.03
  camera->setPosition(player->GetPosition() + glm::vec3(0, 0, 10));
  camera->setLookAt(player->GetPosition());

  /*player->SetPosition(glm::vec3(player->GetPosition()[0] + dX,
                                player_y_pos,
                                player->GetPosition()[2]));
  if (player_y_pos > current_platform_position.y + dY + 0.75f) {
    player->SetVerticalVelocity(player->GetVerticalVelocity() - Player::GRAVITY);
  } else {
    player->SetVerticalVelocity(0);
  }*/

  // Simulate player movement
  AxisAlignedBox previous_player_box = player->GetBoundingBox();
  glm::vec3 previous_player_position = player->GetPosition();
  float previous_player_velocity = player->GetVerticalVelocity();

  // TODO(jarhar): check to see if the ground is no longer beneath the player, in which case they should fall down

  if (player->GetGround() && player->GetSpacebarDown()) {
    // player should jump now
    LOG("player jumped");
    player->SetVerticalVelocity(Player::JUMP_VELOCITY);
    player->RemoveGround();
  } else if (player->GetGround()) {
    // player is stuck to ground, neutralize velocity
    player->SetVerticalVelocity(0.0f);
  } else {
    // player is in the air, apply gravity
    player->SetVerticalVelocity(previous_player_velocity - Player::GRAVITY);
  }
  player->SetPosition(previous_player_position + glm::vec3(dX, player->GetVerticalVelocity(), 0));
  AxisAlignedBox future_player_box = player->GetBoundingBox();

  std::vector<std::shared_ptr<GameObject>> colliding_objects = GetCollidingObjects(
      future_player_box, game_state->GetLevel()->getLevel());

  /*bool asdf = false;
  if (player->GetGround()) {
    asdf = true;
  }
  LOG("update - player->GetGround(): " << asdf << " collidingobjectsize: " << colliding_objects.size());*/

  // TODO(jarhar): further consider what is happening when we are colliding with two objects
  if (colliding_objects.size() > 1) {
    LOG("colliding_objects.size(): " << colliding_objects.size() << ", should reset");
    //Reset(game_state);
  } else if (colliding_objects.size() == 0) {
    LOG("no collisions");
  } else {
    // Handle the collision
    std::shared_ptr<GameObject> colliding_object = colliding_objects[0];
    // are we in the air, and is the object we are colliding with below us?
    // if so, then set it as our current ground
    if (!player->GetGround()) {
      AxisAlignedBox colliding_box = colliding_object->GetBoundingBox();
      AxisAlignedBox player_box = player->GetBoundingBox();

      // TODO(jarhar): make sure this actually makes sense
      // how do we know when we are "above" the colliding_box?
      // if our min y was above the colliding_box's max y before collision occured
      float prev_player_min_y = previous_player_box.GetMin().y;
      float colliding_max_y = colliding_box.GetMax().y;
      if (prev_player_min_y > colliding_max_y) {
        // above colliding box, we are grounded on this platform
        // now attach player to the ground
        player->SetGround(colliding_object);
        player->SetVerticalVelocity(0);
        player->SetPosition(glm::vec3(
          player->GetPosition().x,
          colliding_max_y + Player::PLATFORM_SPACING,
          player->GetPosition().z));

        MatrixStack asdf;
        asdf.pushMatrix();
        asdf.loadIdentity();
        asdf.scale(colliding_object->GetScale());
        MatrixStack one;
        one.pushMatrix();
        one.loadIdentity();
        one.scale(colliding_object->GetScale());
        one.translate(colliding_object->GetPosition());
        MatrixStack two;
        two.pushMatrix();
        two.loadIdentity();
        two.translate(colliding_object->GetPosition());
        two.scale(colliding_object->GetScale());
        LOG("collided and stuck to ground at platform: " << std::endl
          << "platform position: " << glm::to_string(colliding_object->GetPosition()) << std::endl
          << "platform box: " << colliding_box.ToString() << std::endl
          << "    pure box: " << AxisAlignedBox(colliding_object->GetModel(), glm::mat4(1.0f)).ToString() << std::endl
          << "  scaled box: " << AxisAlignedBox(colliding_object->GetModel(), asdf.topMatrix()).ToString() << std::endl
          << " both #1 box: " << AxisAlignedBox(colliding_object->GetModel(), one.topMatrix()).ToString() << std::endl
          << " both #2 box: " << AxisAlignedBox(colliding_object->GetModel(), two.topMatrix()).ToString());

        std::shared_ptr<Shape> model = colliding_object->GetModel();
        glm::mat4 transform = colliding_object->GetTransform().topMatrix();
        glm::vec3 min, max;
        min.x = min.y = min.z = FLT_MAX;
        max.x = max.y = max.z = FLT_MIN;
        std::vector<float> pos_buf = model->GetPositions();
        for (int i = 0; i < pos_buf.size(); i += 3) {
          glm::vec4 position(pos_buf[i], pos_buf[i + 1], pos_buf[i + 2], 1.0f);
          // TODO(jarhar): should we consider what happens to the 4th coordinate
          // after the transform?
          glm::vec4 new_position = transform * position;
          std::cout << "transformed " << glm::to_string(position) << " -> " << glm::to_string(new_position) << std::endl;
          max.x = std::max(max.x, new_position.x);
          min.x = std::min(min.x, new_position.x);
          max.y = std::max(max.y, new_position.y);
          min.y = std::min(min.y, new_position.y);
          max.z = std::max(max.z, new_position.z);
          min.z = std::min(min.z, new_position.z);
        }
      } else {
        LOG("game-ending collided with box! prev_player_min_y: " << prev_player_min_y << ", colliding_max_y: " << colliding_max_y);
      }
    }
  }

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
