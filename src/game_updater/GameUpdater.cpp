// Joseph Arhar

#include "GameUpdater.h"

#include <glm/ext.hpp>
#include <iostream>
#include <queue>

#include "Logging.h"
#include "TimingConstants.h"
#include "Octree.h"
#include "MovingObject.h"
#include "DroppingPlatform.h"

#define COLLISION_WIDTH 0.15f

// TODO(bnbeck) thread that shit my dude
std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>>
GameUpdater::GetCollidingObjects(AxisAlignedBox primary_object,
                                 std::shared_ptr<Octree> tree) {
  std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>> collisions =
      std::make_shared<std::unordered_set<std::shared_ptr<GameObject>>>();
  std::queue<Node*> toVisit;
  toVisit.push(tree->GetRoot());

  while (!toVisit.empty()) {
    Node* node = toVisit.front();
    toVisit.pop();
    if (node->objects->empty()) {
      for (Node* child : *(node->children)) {
        if (AxisAlignedBox::IsColliding(child->boundingBox, primary_object)) {
          toVisit.push(child);
        }
      }
    } else {
      for (std::shared_ptr<GameObject> objectInBox : *(node->objects)) {
        if (AxisAlignedBox::IsColliding(objectInBox->GetBoundingBox(),
                                        primary_object)) {
          collisions->insert(objectInBox);
        }
      }
    }
  }
  return collisions;
}

GameUpdater::GameUpdater() {}

GameUpdater::~GameUpdater() {}

void GameUpdater::Init(std::shared_ptr<GameState> game_state) {
  Reset(game_state);
}

void GameUpdater::Update(std::shared_ptr<GameState> game_state) {
  // check to see if the music ended, in which case the level is complete
  if (game_state->GetLevel()->getMusic()->getStatus() ==
      sf::Music::Status::Stopped) {
    game_state->SetDone(true);
  }

  UpdateLevel(game_state);
  UpdatePlayer(game_state);
  UpdateCamera(game_state);

  game_state->IncrementTicks();
}

void GameUpdater::UpdateLevel(std::shared_ptr<GameState> game_state) {
  for (std::shared_ptr<GameObject> obj : *game_state->GetObjectsInView()) {
    // Moving the moving objects
    if (GameObject::Moves(obj->GetSecondaryType())) {
      std::shared_ptr<MovingObject> movingObj =
          std::dynamic_pointer_cast<MovingObject>(obj);
      obj->SetPosition(movingObj->updatePosition(obj->GetPosition()));

      // Drop the dropping Platforms
    } else if (obj->GetSecondaryType() == SecondaryType::DROPPING_PLATFORM) {
      std::shared_ptr<DroppingPlatform> dropper =
          std::dynamic_pointer_cast<DroppingPlatform>(obj);
      if (dropper->IsDropping()) {
        obj->SetPosition(obj->GetPosition() +
                         glm::vec3(0.0f, dropper->GetYVelocity(), 0.0f));
      }
    }
  }
}

void GameUpdater::Reset(std::shared_ptr<GameState> game_state) {
  // reset the player
  game_state->SetDone(false);

  // reset collectibles and moving objects
  for (std::shared_ptr<GameObject> obj :
       *game_state->GetLevel()->getObjects()) {
    if (obj->GetType() == ObjectType::COLLECTIBLE) {
      std::shared_ptr<Collectible> c =
          std::static_pointer_cast<Collectible>(obj);
      c->SetUncollected();
    }
    // reset the moving objects
    if (obj->GetSecondaryType() == SecondaryType::MOVING_PLATFORM) {
      std::shared_ptr<MovingObject> movingObj =
          std::dynamic_pointer_cast<MovingObject>(obj);
      movingObj->Reset();
      obj->SetPosition(movingObj->GetOriginalPosition());
      // reset the dropping platforms
    } else if (obj->GetSecondaryType() == SecondaryType::DROPPING_PLATFORM) {
      std::shared_ptr<DroppingPlatform> dropping =
          std::dynamic_pointer_cast<DroppingPlatform>(obj);
      dropping->Reset();
    }
  }

  // reset timing
  game_state->SetMusicTimingMode(true);
  game_state->SetTimingStartTick();
  game_state->GetPlayer()->Reset();

  // set the music back to the beginning of the song
  std::shared_ptr<sf::Music> music = game_state->GetLevel()->getMusic();
  if (music->getStatus() == sf::SoundSource::Status::Playing) {
    music->setPlayingOffset(sf::Time::Zero);
  } else if (music->getStatus() == sf::SoundSource::Status::Paused) {
    music->setPlayingOffset(sf::Time::Zero);
    music->play();
  } else {
    music->play();
    music->setLoop(false);
  }
}

void GameUpdater::UpdatePlayer(std::shared_ptr<GameState> game_state) {
  std::shared_ptr<Player> player = game_state->GetPlayer();

  // Store player state before moving
  AxisAlignedBox previous_player_box = player->GetBoundingBox();
  float previous_player_velocity = player->GetYVelocity();
  // the max width to allow the player to be colliding with an object
  float collision_width = COLLISION_WIDTH;

  // Check to see if the ground is no longer beneath the player,
  // in which case they should fall down
  if (player->GetGround()) {
    AxisAlignedBox ground_box = player->GetGround()->GetBoundingBox();
    // This will only consider when the player moves to the right of the
    // platform
    if (previous_player_box.GetMin().x > ground_box.GetMax().x) {
      // Ground is no longer beneath player
      player->RemoveGround();
    }
  }

  // determine new velocity, then position, then collision box
  if (player->GetGround() && player->GetSpacebarDown()) {
    // player should jump now
    player->SetYVelocity(PLAYER_JUMP_VELOCITY);
    player->SetZVelocity(0);
    player->RemoveGround();
  } else if (player->GetGround()) {
    if (GameObject::Moves(player->GetGround()->GetSecondaryType())) {
      std::shared_ptr<MovingObject> moving =
          std::dynamic_pointer_cast<MovingObject>(player->GetGround());
      // Move player with moving object
      glm::vec3 movementVector = moving->GetMovementVector();
      player->SetYVelocity(movementVector.y * moving->GetVelocity().y);
      player->SetZVelocity(movementVector.z * moving->GetVelocity().z);
      // add to the allowed collision width
      collision_width += movementVector.y * moving->GetVelocity().y;
    } else if (player->GetGround()->GetSecondaryType() ==
               SecondaryType::DROPPING_PLATFORM) {
      std::shared_ptr<DroppingPlatform> dropping =
          std::static_pointer_cast<DroppingPlatform>(player->GetGround());
      // move the player with the dropping platform
      player->SetYVelocity(dropping->GetYVelocity());
    } else {
      // player is stuck to ground, make sure velocity is neutralized
      player->SetYVelocity(0.0f);
      player->SetZVelocity(0.0f);
    }
  } else {
    // determine if double jumping
    if (player->GetDoubleJump()) {
      player->SetYVelocity(PLAYER_JUMP_VELOCITY);
      player->SetZVelocity(0);
      player->SetDoubleJump(false);
    } else {
      // player is in the air, apply gravity
      player->SetYVelocity(previous_player_velocity - PLAYER_GRAVITY);
    }
  }
  // finally update the players position
  player->SetPosition(player->GetPosition() +
                      glm::vec3(DELTA_X_PER_TICK, player->GetYVelocity(),
                                player->GetZVelocity()));

  // collide!
  std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>>
      collidingObjects = GetCollidingObjects(player->GetBoundingBox(),
                                             game_state->GetLevel()->getTree());
  std::vector<std::shared_ptr<GameObject>> colliding_platforms;
  for (std::shared_ptr<GameObject> g : *collidingObjects) {
    // collect the collectibles we are colliding with
    if (g->GetType() == ObjectType::COLLECTIBLE) {
      if (std::shared_ptr<Collectible> c =
              std::static_pointer_cast<Collectible>(g)) {
        if (!c->GetCollected()) {
          c->SetCollected();
          game_state->GetPlayer()->SetScore(
              game_state->GetPlayer()->GetScore() + 1);
        }
      }
      // add the platform to the list of colliding platforms
    } else if (g->GetType() == ObjectType::OBSTACLE) {
      colliding_platforms.push_back(g);
    }
  }

  if (!colliding_platforms.empty()) {
    AxisAlignedBox player_box = player->GetBoundingBox();
    float player_min_y = previous_player_box.GetMin().y;
    // Handle the collisions
    for (std::shared_ptr<GameObject> colliding_object : colliding_platforms) {
      // Are we in the air, and is the object we are colliding with below us?
      // If so, then set it as our current ground
      AxisAlignedBox colliding_box = colliding_object->GetBoundingBox();
      float colliding_max_y = colliding_object->GetBoundingBox().GetMax().y;

      // If the player's bounding box was "above" (within 1 velocity unit +
      // allowed COLLISION_WIDTH error) the platform's box before
      // collision,
      // then we consider the collision "landing" on the platform
      if (std::abs(player_min_y - colliding_max_y) <
          (std::abs(player->GetYVelocity()) + collision_width)) {
        // above colliding box, we are grounded on this platform
        // now attach player to the ground
        player->SetGround(colliding_object);
        player->SetYVelocity(0);
        player->SetZVelocity(0);
        player->SetPosition(
            glm::vec3(player->GetPosition().x,
                      colliding_max_y + Player::PLATFORM_SPACING +
                          (player_box.GetMax().y - player_box.GetMin().y) / 2,
                      player->GetPosition().z));
        player_min_y = player->GetBoundingBox().GetMin().y;
        // If the ground is now a dropping platform drop it
        if (player->GetGround()->GetSecondaryType() ==
            SecondaryType::DROPPING_PLATFORM) {
          std::shared_ptr<DroppingPlatform> dropping =
              std::dynamic_pointer_cast<DroppingPlatform>(player->GetGround());
          dropping->SetDropping();
        }
      } else {
        // The collision was not from above, so reset the game
        Reset(game_state);
        return;
      }
    }
  }
  if (previous_player_box.GetMin().y <
      game_state->GetLevel()->getTree()->GetKillZone()) {
    Reset(game_state);
  }
}

void GameUpdater::UpdateCamera(std::shared_ptr<GameState> game_state) {
  std::shared_ptr<GameCamera> camera = game_state->GetCamera();

  glm::vec3 player_position = game_state->GetPlayer()->GetPosition();
  glm::vec3 previous_camera_position = camera->getPosition();
  glm::vec3 new_camera_position;

  // Z position is fixed
  new_camera_position.z = player_position.z + CAMERA_Z_SPACING;

  // Always keep camera aligned with player on x axis.
  // Make camera look ahead of the player
  new_camera_position.x = player_position.x + FORWARD_CAMERA_SPACING;

  // Gradually and smoothly move y towards player
  float delta_y = player_position.y - previous_camera_position.y;
  new_camera_position.y =
      previous_camera_position.y + delta_y * FRACTION_CAMERA_MOVEMENT_PER_TICK;

  camera->setPosition(new_camera_position);

  // Always look directly at the player.
  // Add FORWARD_CAMERA_SPACING to align camera
  camera->setLookAt(player_position + glm::vec3(FORWARD_CAMERA_SPACING, 0, 0));
}
