// Joseph Arhar

#include "GameUpdater.h"

#include <glm/ext.hpp>
#include <iostream>
#include <queue>

#include "Logging.h"
#include "TimingConstants.h"
#include "Octree.h"

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

  UpdatePlayer(game_state);
  UpdateCamera(game_state);

  game_state->IncrementTicks();
}

void GameUpdater::Reset(std::shared_ptr<GameState> game_state) {
  // reset the player
  game_state->SetDone(false);
  game_state->GetPlayer()->SetPosition(Player::INITIAL_POSITION);
  game_state->GetPlayer()->SetScore(0);

  // reset collectibles
  for (std::shared_ptr<GameObject> obj :
       *game_state->GetLevel()->getObjects()) {
    if (obj->GetType() == ObjectType::COLLECTIBLE) {
      if (std::shared_ptr<Collectible> c =
              std::static_pointer_cast<Collectible>(obj)) {
        c->SetUncollected();
      }
    }
  }

  // reset timing
  game_state->SetMusicTimingMode(true);
  game_state->SetTimingStartTick();

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
  glm::vec3 previous_player_position = player->GetPosition();
  float previous_player_velocity = player->GetVerticalVelocity();

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
    player->SetVerticalVelocity(PLAYER_JUMP_VELOCITY);
    player->RemoveGround();
  } else if (player->GetGround()) {
    // player is stuck to ground, make sure velocity is neutralized
    player->SetVerticalVelocity(0.0f);
  } else {
    // player is in the air, apply gravity
    player->SetVerticalVelocity(previous_player_velocity - PLAYER_GRAVITY);
  }
  player->SetPosition(
      previous_player_position +
      glm::vec3(DELTA_X_PER_TICK, player->GetVerticalVelocity(), 0));

  // collide!
  std::shared_ptr<std::unordered_set<std::shared_ptr<GameObject>>>
      collidingObjects = GetCollidingObjects(player->GetBoundingBox(),
                                             game_state->GetLevel()->getTree());
  std::vector<std::shared_ptr<GameObject>> colliding_platforms;
  for (std::shared_ptr<GameObject> g : *collidingObjects) {
    if (g->GetType() == ObjectType::COLLECTIBLE) {
      if (std::shared_ptr<Collectible> c =
              std::static_pointer_cast<Collectible>(g)) {
        if (!c->GetCollected()) {
          c->SetCollected();
          game_state->GetPlayer()->SetScore(
              game_state->GetPlayer()->GetScore() + 1);
        }
      }
    } else if (g->GetType() == ObjectType::OBSTACLE) {
      colliding_platforms.push_back(g);
    }
  }

  if (colliding_platforms.size() > 1) {
    // TODO(jarhar): further consider what is happening when we are colliding
    // with two objects
    Reset(game_state);
  } else if (colliding_platforms.size() == 0) {
    // No collision
  } else {
    // Handle the collision
    std::shared_ptr<GameObject> colliding_object = *colliding_platforms.begin();
    // Are we in the air, and is the object we are colliding with below us?
    // If so, then set it as our current ground
    if (!player->GetGround()) {
      AxisAlignedBox colliding_box = colliding_object->GetBoundingBox();
      AxisAlignedBox player_box = player->GetBoundingBox();

      // If the player's bounding box was "above" the platform's box before
      // collision,
      // then we consider the collision "landing" on the platform
      float prev_player_min_y = previous_player_box.GetMin().y;
      float colliding_max_y = colliding_box.GetMax().y;
      if (prev_player_min_y > colliding_max_y) {
        // above colliding box, we are grounded on this platform
        // now attach player to the ground
        player->SetGround(colliding_object);
        player->SetVerticalVelocity(0);
        player->SetPosition(
            glm::vec3(player->GetPosition().x,
                      colliding_max_y + Player::PLATFORM_SPACING +
                          (player_box.GetMax().y - player_box.GetMin().y) / 2,
                      player->GetPosition().z));
      } else {
        // The collision was not from above, so reset the game
        Reset(game_state);
      }
    }
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
