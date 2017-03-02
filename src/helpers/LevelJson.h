// bbeck13

#ifndef LEVEL_JSON_H_
#define LEVEL_JSON_H_
#include <vector>
#include <iostream>
#include <glm/ext.hpp>

#include "MovingPlatform.h"
#include "DroppingPlatform.h"
#include "Platform.h"
#include "json.hpp"

namespace glm {
void to_json(nlohmann::json& j, const glm::vec3& vec3) {
  j = nlohmann::json{{"x", vec3.x}, {"y", vec3.y}, {"z", vec3.z}};
}

void from_json(const nlohmann::json& j, glm::vec3& vec3) {
  vec3.x = j["x"].get<float>();
  vec3.y = j["y"].get<float>();
  vec3.z = j["z"].get<float>();
}
}

namespace std {
void to_json(nlohmann::json& j, const std::vector<glm::vec3>& vec) {
  for (auto& item : vec) {
    j.push_back(item);
  }
}

void from_json(const nlohmann::json& j, std::vector<glm::vec3>& vec) {
  for (auto& item : j) {
    vec.push_back(item);
  }
}

void to_json(nlohmann::json& j,
             const std::vector<gameobject::Platform>& items) {
  for (auto& item : items) {
    j.push_back(item);
  }
}

void from_json(const nlohmann::json& j,
               std::vector<gameobject::Platform>& items) {
  for (auto& item : j) {
    items.push_back(item);
  }
}

void to_json(nlohmann::json& j,
             const std::vector<gameobject::MovingPlatform>& items) {
  for (auto& item : items) {
    j.push_back(item);
  }
}

void from_json(const nlohmann::json& j,
               std::vector<gameobject::MovingPlatform>& items) {
  for (auto& item : j) {
    items.push_back(item);
  }
}

void to_json(nlohmann::json& j,
             const std::vector<gameobject::DroppingPlatform>& items) {
  for (auto& item : items) {
    j.push_back(item);
  }
}

void from_json(const nlohmann::json& j,
               std::vector<gameobject::DroppingPlatform>& items) {
  for (auto& item : j) {
    items.push_back(item);
  }
}

void to_json(nlohmann::json& j, const std::vector<gameobject::Note>& items) {
  for (auto& item : items) {
    j.push_back(item);
  }
}

void from_json(const nlohmann::json& j, std::vector<gameobject::Note>& items) {
  for (auto& item : j) {
    items.push_back(item);
  }
}

void to_json(nlohmann::json& j,
             const std::vector<std::shared_ptr<GameObject>>& level) {
  std::vector<gameobject::Platform> platforms;
  std::vector<gameobject::MovingPlatform> movingPlatforms;
  std::vector<gameobject::DroppingPlatform> droppingPlatforms;
  std::vector<gameobject::Note> notes;

  for (std::shared_ptr<GameObject> obj : level) {
    if (std::shared_ptr<gameobject::MovingPlatform> platform =
            std::dynamic_pointer_cast<gameobject::MovingPlatform>(obj)) {
      movingPlatforms.push_back(*platform);
    } else if (std::shared_ptr<gameobject::Platform> platform =
                   std::dynamic_pointer_cast<gameobject::Platform>(obj)) {
      platforms.push_back(*platform);
    } else if (std::shared_ptr<gameobject::Note> note =
                   std::dynamic_pointer_cast<gameobject::Note>(obj)) {
      notes.push_back(*note);
    } else if (std::shared_ptr<gameobject::DroppingPlatform> droppingPlatform =
                   std::dynamic_pointer_cast<gameobject::DroppingPlatform>(
                       obj)) {
      droppingPlatforms.push_back(*droppingPlatform);
    } else {
      std::cerr << "NO! I don't want that. " << obj->GetType() << " "
                << obj->GetSecondaryType() << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  j = nlohmann::json{{"platforms", platforms},
                     {"moving_platforms", movingPlatforms},
                     {"dropping_platforms", droppingPlatforms},
                     {"notes", notes}};
}

void from_json(const nlohmann::json& j,
               std::vector<std::shared_ptr<GameObject>>& level) {
  for (auto& item : j["platforms"]) {
    gameobject::Platform plat = item;
    level.push_back(std::make_shared<gameobject::Platform>(plat));
  }
  for (auto& item : j["moving_platforms"]) {
    gameobject::MovingPlatform plat = item;
    level.push_back(std::make_shared<gameobject::MovingPlatform>(plat));
  }
  for (auto& item : j["dropping_platforms"]) {
    gameobject::DroppingPlatform plat = item;
    level.push_back(std::make_shared<gameobject::DroppingPlatform>(plat));
  }
  for (auto& item : j["notes"]) {
    gameobject::Note note = item;
    level.push_back(std::make_shared<gameobject::Note>(note));
  }
}
}

namespace gameobject {
void to_json(nlohmann::json& j, const Platform& platform) {
  j = nlohmann::json{{"position", platform.GetPosition()},
                     {"scale", platform.GetScale()},
                     {"rotation_axis", platform.GetRotationAxis()},
                     {"rotation_angle", platform.GetRotationAngle()}};
}

void from_json(const nlohmann::json& j, Platform& platform) {
  platform = Platform(
      j["position"].get<glm::vec3>(), j["scale"].get<glm::vec3>(),
      j["rotation_axis"].get<glm::vec3>(), j["rotation_angle"].get<float>());
}

void to_json(nlohmann::json& j, const Note& note) {
  j = nlohmann::json{{"position", note.GetPosition()},
                     {"scale", note.GetScale()},
                     {"rotation_axis", note.GetRotationAxis()},
                     {"rotation_angle", note.GetRotationAngle()},
                     {"collected", note.GetCollected()}};
}

void from_json(const nlohmann::json& j, Note& note) {
  note = Note(j["position"].get<glm::vec3>(), j["scale"].get<glm::vec3>(),
              j["rotation_axis"].get<glm::vec3>(),
              j["rotation_angle"].get<float>(), j["collected"].get<bool>());
}

void to_json(nlohmann::json& j, const DroppingPlatform& platform) {
  j = nlohmann::json{{"position", platform.GetPosition()},
                     {"scale", platform.GetScale()},
                     {"rotation_axis", platform.GetRotationAxis()},
                     {"rotation_angle", platform.GetRotationAngle()},
                     {"dropping", platform.IsDropping()},
                     {"dropVel", platform.GetYVelocity()}};
}

void from_json(const nlohmann::json& j, DroppingPlatform& platform) {
  platform = DroppingPlatform(
      j["position"].get<glm::vec3>(), j["scale"].get<glm::vec3>(),
      j["rotation_axis"].get<glm::vec3>(), j["rotation_angle"].get<float>(),
      j["dropVel"].get<float>(), j["dropping"].get<bool>());
}

void to_json(nlohmann::json& j, const MovingPlatform& platform) {
  j = nlohmann::json{{"position", platform.GetPosition()},
                     {"scale", platform.GetScale()},
                     {"rotation_axis", platform.GetRotationAxis()},
                     {"rotation_angle", platform.GetRotationAngle()},
                     {"velocity", platform.GetVelocity()},
                     {"path", platform.GetPath()}};
}

void from_json(const nlohmann::json& j, MovingPlatform& platform) {
  platform = MovingPlatform(
      j["position"].get<glm::vec3>(), j["scale"].get<glm::vec3>(),
      j["rotation_axis"].get<glm::vec3>(), j["rotation_angle"].get<float>(),
      j["velocity"].get<glm::vec3>(), j["path"].get<std::vector<glm::vec3>>());
}
}

#endif
