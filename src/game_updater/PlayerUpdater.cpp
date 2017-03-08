// Joseph Arhar

#include "PlayerUpdater.h"

#include <imgui.h>
#include <glm/glm.hpp>
#include <queue>

#include "MovingObject.h"
#include "DroppingPlatform.h"
#include "InputBindings.h"
#include "TimingConstants.h"
#include "Octree.h"
#include "Player.h"
#include "CollisionCalculator.h"

#define COLLISION_WIDTH 0.420f

PlayerUpdater::PlayerUpdater() {}

PlayerUpdater::~PlayerUpdater() {}

AxisAlignedBox PlayerUpdater::MovePlayer(std::shared_ptr<GameState> game_state) {
  std::shared_ptr<Player> player = game_state->GetPlayer();
  std::shared_ptr<GameCamera> camera = game_state->GetCamera();
  std::shared_ptr<Sky> sky = game_state->GetSky();

  // the max width to allow the player to be colliding with an object
  float collision_width = COLLISION_WIDTH;

  // Store player state before moving
  AxisAlignedBox previous_player_box = player->GetBoundingBox();
  float previous_player_velocity = player->GetYVelocity();

  // Check to see if the ground is no longer beneath the player,
  // in which case they should fall down
  if (player->GetGround()) {
    AxisAlignedBox ground_box = player->GetGround()->GetBoundingBox();
    // This will only consider when the player moves to the right of the
    // platform
    if (previous_player_box.GetMin().x > ground_box.GetMax().x) {
      // Ground is no longer beneath player
      player->RemoveGround();
      ChangeAnimation(game_state, Player::Animation::JUMPING);
    }
  }

  // Jump if the user inputs a jump, or apply gravity.
  // Calculate velocity for moving ground objects.
  if (player->GetGround() && ImGui::GetIO().KeysDown[GLFW_KEY_SPACE]) {
    Jump(game_state);
  } else if (player->GetGround()) {
    if (GameObject::Moves(player->GetGround()->GetSecondaryType())) {
      std::shared_ptr<MovingObject> moving =
          std::dynamic_pointer_cast<MovingObject>(player->GetGround());
      // Move player with moving object
      glm::vec3 movementVector = moving->GetMovementVector();
      /*player->SetYVelocity(movementVector.y * moving->GetVelocity().y);
      player->SetZVelocity(movementVector.z * moving->GetVelocity().z);*/
      SnapToGround(game_state);
      // add to the allowed collision width
      collision_width += movementVector.y * moving->GetVelocity().y;
    } else if (player->GetGround()->GetSecondaryType() ==
               SecondaryType::DROPPING_PLATFORM) {
      std::shared_ptr<gameobject::DroppingPlatform> dropping =
          std::static_pointer_cast<gameobject::DroppingPlatform>(
              player->GetGround());
      // move the player with the dropping platform
      //player->SetYVelocity(dropping->GetYVelocity());
      player->Fall(game_state);
    } else {
      // player is stuck to ground, make sure velocity is neutralized
      /*player->SetYVelocity(0.0f);
      player->SetZVelocity(0.0f);*/
    }
  } else {
    // player is in the air, apply gravity
    player->SetYVelocity(previous_player_velocity - PLAYER_GRAVITY);
    if (player->GetDoubleJump() &&
        InputBindings::KeyNewlyPressed(GLFW_KEY_SPACE)) {
      // Double jump
      Jump(game_state);
    }
  }

  glm::vec3 prevCameraPos = camera->getPosition();
  // always check for ducking (be ware of ducks)
  // let the player move up/down Z (sock it to me)
  if (ImGui::GetIO().KeysDown[GLFW_KEY_LEFT] ||
      ImGui::GetIO().KeysDown[GLFW_KEY_H]) {
    player->MoveDownZ();
    player->SetDucking(DuckDir::LEFT);
    camera->setPosition(prevCameraPos -
                        glm::vec3(0, 0, PLAYER_DELTA_Z_PER_TICK));
  } else if (ImGui::GetIO().KeysDown[GLFW_KEY_RIGHT] ||
             ImGui::GetIO().KeysDown[GLFW_KEY_L]) {
    player->MoveUpZ();
    player->SetDucking(DuckDir::RIGHT);
    camera->setPosition(prevCameraPos +
                        glm::vec3(0, 0, PLAYER_DELTA_Z_PER_TICK));
  } else {
    player->SetDucking(DuckDir::NONE);
  }
  if (ImGui::GetIO().KeysDown[GLFW_KEY_LEFT_SHIFT] ||
      ImGui::GetIO().KeysDown[GLFW_KEY_RIGHT_SHIFT]) {
    if (player->GetDucking() == DuckDir::NONE) {
      player->SetDucking(DuckDir::RIGHT);
    }
  }

  // Update player position based on new velocity.
  player->SetPosition(player->GetPosition() +
                      glm::vec3(DELTA_X_PER_TICK, player->GetYVelocity(),
                                player->GetZVelocity()));
  // AnimatePlayer(game_state);
  // SnapToGround();  // aligns player with ground if grounded TODO(jarhar):
  // move this to always run after the animate phase? or during it on the
  // grounded animations!
  // TODO(jarhar): move sky somewhere else
  sky->SetPosition(sky->GetPosition() + glm::vec3(DELTA_X_PER_TICK, 0, 0));

  return previous_player_box;
}

void PlayerUpdater::AnimatePlayer(std::shared_ptr<GameState> game_state) {
  std::shared_ptr<Player> player = game_state->GetPlayer();
  std::shared_ptr<PhysicalObject> rear_wheel = player->GetRearWheel();
  std::shared_ptr<PhysicalObject> front_wheel = player->GetFrontWheel();
  uint64_t current_tick = game_state->GetElapsedTicks();
  Player::Animation current_animation = player->GetAnimation();

  // Rotate the wheels
  float wheel_rotation_speed = 0;
  if (current_animation & Player::ANIMATION_WHEELSPIN_BIT) {
    wheel_rotation_speed = WHEEL_ROTATION_PER_TICK;
  } else if (current_animation & Player::ANIMATION_WHEELSPIN_SLOW_BIT) {
    wheel_rotation_speed *= 0.98;
  }
  player->GetRearWheel()->SetRotationAngle(rear_wheel->GetRotationAngle() +
                                           wheel_rotation_speed);
  player->GetFrontWheel()->SetRotationAngle(front_wheel->GetRotationAngle() +
                                            wheel_rotation_speed);

  if (current_animation == Player::Animation::FAILURE) {
    // go crazy
    static const float death_wheel_rotation = 2.0f;
    static const float death_rotation = 0.5f;
    player->GetFrontWheel()->SetRotationAngle(rear_wheel->GetRotationAngle() +
                                              death_wheel_rotation);
    player->GetRearWheel()->SetRotationAngle(front_wheel->GetRotationAngle() +
                                             death_wheel_rotation);
    player->SetRotationAngle(player->GetRotationAngle() + death_rotation);
  }

  // set player rotation based on y velocity
  float aerial_rotation_angle = std::atan(player->GetYVelocity() * 3);
  if (!player->GetGround()) {
    player->SetRotationAngle(aerial_rotation_angle);
  } else if (current_animation == Player::Animation::JUMPSQUAT) {
    // raise up front of bike until we are in the air, then go into JUMPING
    // +1 is rotated backwards, -1 is rotated forwards, 0 is normal

    AxisAlignedBox wheel_bounding_box =
        player->GetRearWheel()->GetIndividualBoundingBox();
    float wheel_radius =
        (wheel_bounding_box.GetMax().y - wheel_bounding_box.GetMin().y) / 2.0f;

    // this always stays constant, since rear_wheel->GetPosition() is constant
    float relative_distance_to_wheel =
        glm::distance(glm::vec3(0, 0, 0), rear_wheel->GetPosition());

    float y = player->GetPosition().y -
              player->GetGround()->GetBoundingBox().GetMax().y - wheel_radius -
              Player::PLATFORM_SPACING;

    if (y > relative_distance_to_wheel) {
      // RemoveGround();
      // ChangeAnimation();
    }

    float target_angle = std::asin(relative_distance_to_wheel / y);

    if (target_angle > aerial_rotation_angle) {
      // RemoveGround();
      // ChangeAnimation();
    }

    player->SetRotationAngle(target_angle);
    std::cout << "target_angle: " << target_angle << std::endl;
  }

    SnapToGround(game_state);
}

void PlayerUpdater::CollisionCheck(std::shared_ptr<GameState> game_state, AxisAlignedBox previous_player_box) {
  std::shared_ptr<Player> player = game_state->GetPlayer();

  // Determine colliding objects.
  std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>>
      collidingObjects = CollisionCalculator::GetCollidingObjects(player->GetBoundingBox(),
                                             game_state->GetLevel()->getTree());
  std::vector<std::shared_ptr<Obstacle>> colliding_obstacles;
  std::vector<std::shared_ptr<Collectible>> colliding_collectibles;
  for (std::shared_ptr<GameObject> game_object : *collidingObjects) {
    if (game_object->GetType() == ObjectType::COLLECTIBLE) {
      colliding_collectibles.push_back(
          std::static_pointer_cast<Collectible>(game_object));
    } else if (game_object->GetType() == ObjectType::OBSTACLE) {
      colliding_obstacles.push_back(
          std::static_pointer_cast<Obstacle>(game_object));
    }
  }

  // Collect the collectibles we are colliding with.
  for (std::shared_ptr<Collectible> collectible : colliding_collectibles) {
    if (!collectible->GetCollected()) {
      collectible->SetCollected();
      game_state->GetPlayer()->SetScore(game_state->GetPlayer()->GetScore() +
                                        1);
    }
  }

  // Apply collision logic for obstacles.
  if (!colliding_obstacles.empty()) {
    AxisAlignedBox player_box = player->GetBoundingBox();
    float player_min_y = previous_player_box.GetMin().y;
    // Handle the collisions
    for (std::shared_ptr<GameObject> colliding_object : colliding_obstacles) {
      // Are we in the air, and is the object we are colliding with below us?
      // If so, then set it as our current ground
      AxisAlignedBox colliding_box = colliding_object->GetBoundingBox();
      float colliding_max_y = colliding_object->GetBoundingBox().GetMax().y;

      // If the player's bounding box was "above" (within 1 velocity unit +
      // allowed COLLISION_WIDTH error) the platform's box before
      // collision,
      // then we consider the collision "landing" on the platform
      if (std::abs(player_min_y - colliding_max_y) <
          (std::abs(player->GetYVelocity()) + COLLISION_WIDTH)) {
        // above colliding box, we are grounded on this platform
        // now attach player to the ground
        player->SetGround(colliding_object);
        player->SetYVelocity(0);
        player->SetZVelocity(0);
        SnapToGround(game_state);
        ChangeAnimation(game_state, Player::Animation::GROUNDED);
        player_min_y = player->GetBoundingBox().GetMin().y;
        // If the ground is now a dropping platform drop it
        if (player->GetGround()->GetSecondaryType() ==
            SecondaryType::DROPPING_PLATFORM) {
          std::shared_ptr<gameobject::DroppingPlatform> dropping =
              std::dynamic_pointer_cast<gameobject::DroppingPlatform>(
                  player->GetGround());
          dropping->SetDropping();
        }
      } else {
        // The collision was not from above, so reset the game
        Death(game_state);
        return;
      }
    }
  }

  // Check to see if the player fell out of the world.
  if (previous_player_box.GetMin().y <
      game_state->GetLevel()->getTree()->GetKillZone()) {
    Death(game_state);
  }
}

void PlayerUpdater::Jump(std::shared_ptr<GameState> game_state) {
  std::shared_ptr<Player> player = game_state->GetPlayer();
  uint64_t current_tick = game_state->GetElapsedTicks();

  // TODO(jarhar): create a particle effect here?

  if (player->GetAnimation() == Player::Animation::JUMPSQUAT) {
    // if we are already in jumpsquat, do nothing
    return;
  }

  if (!player->GetGround()) {
    // if we are already in the air, then don't go into jumpsquat
    ChangeAnimation(game_state, Player::Animation::JUMPING);
    player->SetDoubleJump(false);
  } else if (player->GetAnimation() != Player::Animation::JUMPSQUAT) {
    ChangeAnimation(game_state, Player::Animation::JUMPSQUAT);
    player->SetDoubleJump(true);
  }

  player->SetYVelocity(PLAYER_JUMP_VELOCITY);
  player->SetZVelocity(0);
  // RemoveGround();
}

void PlayerUpdater::Land(std::shared_ptr<GameState> game_state,
                         std::shared_ptr<GameObject> ground) {
  std::shared_ptr<Player> player = game_state->GetPlayer();

  // TODO(jarhar): implement landing animation
  player->SetGround(ground);
  ChangeAnimation(game_state, Player::Animation::LANDING);
}

void PlayerUpdater::ChangeAnimation(std::shared_ptr<GameState> game_state,
                                    Player::Animation animation) {
  std::shared_ptr<Player> player = game_state->GetPlayer();
  std::shared_ptr<PhysicalObject> rear_wheel = player->GetRearWheel();
  std::shared_ptr<PhysicalObject> front_wheel = player->GetFrontWheel();

  player->SetAnimationStartTick(game_state->GetElapsedTicks() -
                                1);  // TODO(jarhar): this is hacky
  player->SetAnimation(animation);

  rear_wheel->SetRotationAxis(glm::vec3(0, 0, -1));
  front_wheel->SetRotationAxis(glm::vec3(0, 0, -1));

  if (animation & Player::ANIMATION_ENDGAME_BIT) {
    // do fun spins after the game ends
    player->SetRotationAxis(glm::vec3(1, 0, 0));
  } else {
    // rock fowards/backwards for jumping
    player->SetRotationAxis(glm::vec3(0, 0, 1));
  }
}

void PlayerUpdater::SnapToGround(std::shared_ptr<GameState> game_state) {
  std::shared_ptr<Player> player = game_state->GetPlayer();

  if (!player->GetGround()) {
    return;
  }

  AxisAlignedBox bounding_box = player->GetBoundingBox();
  AxisAlignedBox ground_box = player->GetGround()->GetBoundingBox();
  glm::vec3 player_position = player->GetPosition();

  player->SetPosition(
      glm::vec3(player_position.x,
                ground_box.GetMax().y + Player::PLATFORM_SPACING +
                    (bounding_box.GetMax().y - bounding_box.GetMin().y) / 2 +
                    (player_position.y - bounding_box.GetCenter().y),
                player_position.z));
}

void PlayerUpdater::Death(std::shared_ptr<GameState> game_state) {
  std::shared_ptr<Player> player = game_state->GetPlayer();

  game_state->SetPlayingState(GameState::PlayingState::FAILURE);
  ChangeAnimation(game_state, Player::Animation::FAILURE);
}
