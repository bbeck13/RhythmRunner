#include "LevelJson.h"

void glm::to_json(nlohmann::json& j, const glm::vec3& vec3) {
  j = nlohmann::json{{"x", vec3.x}, {"y", vec3.y}, {"z", vec3.z}};
}

void glm::from_json(const nlohmann::json& j, glm::vec3& vec3) {
  vec3.x = j["x"].get<float>();
  vec3.y = j["y"].get<float>();
  vec3.z = j["z"].get<float>();
}

void std::to_json(nlohmann::json& j, const std::vector<glm::vec3>& vec) {
  for (auto& item : vec) {
    j.push_back(item);
  }
}

void std::from_json(const nlohmann::json& j, std::vector<glm::vec3>& vec) {
  for (auto& item : j) {
    vec.push_back(item);
  }
}

void std::to_json(nlohmann::json& j,
                  const std::vector<gameobject::Platform>& items) {
  for (auto& item : items) {
    j.push_back(item);
  }
}

void std::from_json(const nlohmann::json& j,
                    std::vector<gameobject::Platform>& items) {
  for (auto& item : j) {
    items.push_back(item);
  }
}

void std::to_json(nlohmann::json& j,
                  const std::vector<gameobject::MovingPlatform>& items) {
  for (auto& item : items) {
    j.push_back(item);
  }
}

void std::from_json(const nlohmann::json& j,
                    std::vector<gameobject::MovingPlatform>& items) {
  for (auto& item : j) {
    items.push_back(item);
  }
}

void std::to_json(nlohmann::json& j,
                  const std::vector<gameobject::DroppingPlatform>& items) {
  for (auto& item : items) {
    j.push_back(item);
  }
}

void std::from_json(const nlohmann::json& j,
                    std::vector<gameobject::DroppingPlatform>& items) {
  for (auto& item : j) {
    items.push_back(item);
  }
}

void std::to_json(nlohmann::json& j,
                  const std::vector<gameobject::Note>& items) {
  for (auto& item : items) {
    j.push_back(item);
  }
}

void std::from_json(const nlohmann::json& j,
                    std::vector<gameobject::Note>& items) {
  for (auto& item : j) {
    items.push_back(item);
  }
}

void std::to_json(nlohmann::json& j,
                  const std::vector<std::shared_ptr<GameObject>>& level) {
  std::vector<gameobject::Platform> platforms;
  std::vector<gameobject::MovingPlatform> movingPlatforms;
  std::vector<gameobject::DroppingPlatform> droppingPlatforms;
  std::vector<gameobject::Note> notes;
  std::vector<gameobject::MoonRock> moonRocks;
  std::vector<gameobject::PlainRock> plainRocks;
  std::vector<gameobject::Monster> monsters;
  std::vector<gameobject::DMT> dmts;
  std::vector<gameobject::Acid> acids;
  std::vector<gameobject::Cocainum> cocainums;

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
    } else if (std::shared_ptr<gameobject::MoonRock> rock =
                   std::dynamic_pointer_cast<gameobject::MoonRock>(obj)) {
      moonRocks.push_back(*rock);
    } else if (std::shared_ptr<gameobject::PlainRock> rock =
                   std::dynamic_pointer_cast<gameobject::PlainRock>(obj)) {
      plainRocks.push_back(*rock);
    } else if (std::shared_ptr<gameobject::Monster> monster =
                   std::dynamic_pointer_cast<gameobject::Monster>(obj)) {
      monsters.push_back(*monster);
    } else if (std::shared_ptr<gameobject::DMT> dmt =
                   std::dynamic_pointer_cast<gameobject::DMT>(obj)) {
      dmts.push_back(*dmt);
    } else if (std::shared_ptr<gameobject::Acid> acid =
                   std::dynamic_pointer_cast<gameobject::Acid>(obj)) {
      acids.push_back(*acid);
    } else if (std::shared_ptr<gameobject::Cocainum> cocainum =
                   std::dynamic_pointer_cast<gameobject::Cocainum>(obj)) {
      cocainums.push_back(*cocainum);
    } else {
      std::cerr << "NO! I don't want that. " << obj->GetType() << " "
                << obj->GetSecondaryType() << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  j = nlohmann::json{{"platforms", platforms},
                     {"moving_platforms", movingPlatforms},
                     {"dropping_platforms", droppingPlatforms},
                     {"notes", notes},
                     {"moon_rocks", moonRocks},
                     {"plain_rocks", plainRocks},
                     {"monsters", monsters},
                     {"dmts", dmts},
                     {"acids", acids},
                     {"cocainums", cocainums}};
}

void std::from_json(const nlohmann::json& j,
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
  for (auto& item : j["moon_rocks"]) {
    gameobject::MoonRock rock = item;
    level.push_back(std::make_shared<gameobject::MoonRock>(rock));
  }
  for (auto& item : j["plain_rocks"]) {
    gameobject::PlainRock rock = item;
    level.push_back(std::make_shared<gameobject::PlainRock>(rock));
  }
  for (auto& item : j["monsters"]) {
    gameobject::Monster monster = item;
    level.push_back(std::make_shared<gameobject::Monster>(monster));
  }
  for (auto& item : j["dmts"]) {
    gameobject::DMT dmt = item;
    level.push_back(std::make_shared<gameobject::DMT>(dmt));
  }
  for (auto& item : j["acids"]) {
    gameobject::Acid acid = item;
    level.push_back(std::make_shared<gameobject::Acid>(acid));
  }
  for (auto& item : j["cocainums"]) {
    gameobject::Cocainum cocainum = item;
    level.push_back(std::make_shared<gameobject::Cocainum>(cocainum));
  }
}

void gameobject::to_json(nlohmann::json& j, const Platform& platform) {
  j = nlohmann::json{{"position", platform.GetPosition()},
                     {"scale", platform.GetScale()},
                     {"rotation_axis", platform.GetRotationAxis()},
                     {"rotation_angle", platform.GetRotationAngle()}};
}

void gameobject::from_json(const nlohmann::json& j, Platform& platform) {
  platform = Platform(
      j["position"].get<glm::vec3>(), j["scale"].get<glm::vec3>(),
      j["rotation_axis"].get<glm::vec3>(), j["rotation_angle"].get<float>());
}

void gameobject::to_json(nlohmann::json& j, const Note& note) {
  j = nlohmann::json{{"position", note.GetPosition()},
                     {"scale", note.GetScale()},
                     {"rotation_axis", note.GetRotationAxis()},
                     {"rotation_angle", note.GetRotationAngle()},
                     {"collected", note.GetCollected()}};
}

void gameobject::to_json(nlohmann::json& j, const MoonRock& rock) {
  j = nlohmann::json{{"position", rock.GetPosition()},
                     {"scale", rock.GetScale()},
                     {"rotation_axis", rock.GetRotationAxis()},
                     {"rotation_angle", rock.GetRotationAngle()}};
}
void gameobject::from_json(const nlohmann::json& j, MoonRock& rock) {
  rock = MoonRock(j["position"].get<glm::vec3>(), j["scale"].get<glm::vec3>(),
                  j["rotation_angle"].get<float>(),
                  j["rotation_axis"].get<glm::vec3>());
}

void gameobject::to_json(nlohmann::json& j, const PlainRock& rock) {
  j = nlohmann::json{{"position", rock.GetPosition()},
                     {"scale", rock.GetScale()},
                     {"rotation_axis", rock.GetRotationAxis()},
                     {"rotation_angle", rock.GetRotationAngle()}};
}

void gameobject::from_json(const nlohmann::json& j, PlainRock& rock) {
  rock = PlainRock(j["position"].get<glm::vec3>(), j["scale"].get<glm::vec3>(),
                   j["rotation_angle"].get<float>(),
                   j["rotation_axis"].get<glm::vec3>());
}

void gameobject::from_json(const nlohmann::json& j, Note& note) {
  note = Note(j["position"].get<glm::vec3>(), j["scale"].get<glm::vec3>(),
              j["rotation_axis"].get<glm::vec3>(),
              j["rotation_angle"].get<float>(), j["collected"].get<bool>());
}

void gameobject::to_json(nlohmann::json& j, const DroppingPlatform& platform) {
  j = nlohmann::json{{"position", platform.GetPosition()},
                     {"scale", platform.GetScale()},
                     {"rotation_axis", platform.GetRotationAxis()},
                     {"rotation_angle", platform.GetRotationAngle()},
                     {"dropping", platform.IsDropping()},
                     {"dropVel", platform.GetYVelocity()}};
}

void gameobject::from_json(const nlohmann::json& j,
                           DroppingPlatform& platform) {
  platform = DroppingPlatform(
      j["position"].get<glm::vec3>(), j["scale"].get<glm::vec3>(),
      j["rotation_axis"].get<glm::vec3>(), j["rotation_angle"].get<float>(),
      j["dropVel"].get<float>(), j["dropping"].get<bool>());
}

void gameobject::to_json(nlohmann::json& j, const MovingPlatform& platform) {
  j = nlohmann::json{{"position", platform.GetPosition()},
                     {"scale", platform.GetScale()},
                     {"rotation_axis", platform.GetRotationAxis()},
                     {"rotation_angle", platform.GetRotationAngle()},
                     {"velocity", platform.GetVelocity()},
                     {"path", platform.GetPath()}};
}

void gameobject::from_json(const nlohmann::json& j, MovingPlatform& platform) {
  platform = MovingPlatform(
      j["position"].get<glm::vec3>(), j["scale"].get<glm::vec3>(),
      j["rotation_axis"].get<glm::vec3>(), j["rotation_angle"].get<float>(),
      j["velocity"].get<glm::vec3>(), j["path"].get<std::vector<glm::vec3>>());
}

void std::to_json(nlohmann::json& j,
                  const std::vector<gameobject::MoonRock>& items) {
  for (auto& item : items) {
    j.push_back(item);
  }
}
void std::from_json(const nlohmann::json& j,
                    std::vector<gameobject::MoonRock>& items) {
  for (auto& item : j) {
    items.push_back(item);
  }
}

void std::to_json(nlohmann::json& j,
                  const std::vector<gameobject::PlainRock>& items) {
  for (auto& item : items) {
    j.push_back(item);
  }
}
void std::from_json(const nlohmann::json& j,
                    std::vector<gameobject::PlainRock>& items) {
  for (auto& item : j) {
    items.push_back(item);
  }
}

void std::to_json(nlohmann::json& j,
                  const std::vector<gameobject::Monster>& items) {
  for (auto& item : items) {
    j.push_back(item);
  }
}
void std::from_json(const nlohmann::json& j,
                    std::vector<gameobject::Monster>& items) {
  for (auto& item : j) {
    items.push_back(item);
  }
}

void gameobject::to_json(nlohmann::json& j,
                         const gameobject::Monster& monster) {
  j = nlohmann::json{{"position", monster.GetPosition()},
                     {"scale", monster.GetScale()},
                     {"rotation_axis", monster.GetRotationAxis()},
                     {"rotation_angle", monster.GetRotationAngle()},
                     {"velocity", monster.GetVelocity()},
                     {"path", monster.GetPath()}};
}

void gameobject::from_json(const nlohmann::json& j,
                           gameobject::Monster& monster) {
  monster = Monster(
      j["position"].get<glm::vec3>(), j["scale"].get<glm::vec3>(),
      j["rotation_axis"].get<glm::vec3>(), j["rotation_angle"].get<float>(),
      j["velocity"].get<glm::vec3>(), j["path"].get<std::vector<glm::vec3>>());
}

void gameobject::from_json(const nlohmann::json& j, DMT& dmt) {
  dmt = DMT(j["position"].get<glm::vec3>(), j["scale"].get<glm::vec3>(),
            j["rotation_axis"].get<glm::vec3>(),
            j["rotation_angle"].get<float>(), j["collected"].get<bool>());
}

void gameobject::to_json(nlohmann::json& j, const DMT& dmt) {
  j = nlohmann::json{{"position", dmt.GetPosition()},
                     {"scale", dmt.GetScale()},
                     {"rotation_axis", dmt.GetRotationAxis()},
                     {"rotation_angle", dmt.GetRotationAngle()},
                     {"collected", dmt.GetCollected()}};
}

void gameobject::from_json(const nlohmann::json& j, Cocainum& cocainum) {
  cocainum =
      Cocainum(j["position"].get<glm::vec3>(), j["scale"].get<glm::vec3>(),
               j["rotation_axis"].get<glm::vec3>(),
               j["rotation_angle"].get<float>(), j["collected"].get<bool>());
}

void gameobject::to_json(nlohmann::json& j, const Cocainum& cocainum) {
  j = nlohmann::json{{"position", cocainum.GetPosition()},
                     {"scale", cocainum.GetScale()},
                     {"rotation_axis", cocainum.GetRotationAxis()},
                     {"rotation_angle", cocainum.GetRotationAngle()},
                     {"collected", cocainum.GetCollected()}};
}

void gameobject::from_json(const nlohmann::json& j, Acid& acid) {
  acid = Acid(j["position"].get<glm::vec3>(), j["scale"].get<glm::vec3>(),
              j["rotation_axis"].get<glm::vec3>(),
              j["rotation_angle"].get<float>(), j["collected"].get<bool>());
}

void gameobject::to_json(nlohmann::json& j, const Acid& acid) {
  j = nlohmann::json{{"position", acid.GetPosition()},
                     {"scale", acid.GetScale()},
                     {"rotation_axis", acid.GetRotationAxis()},
                     {"rotation_angle", acid.GetRotationAngle()},
                     {"collected", acid.GetCollected()}};
}

void std::to_json(nlohmann::json& j,
                  const std::vector<gameobject::Acid>& items) {
  for (auto& item : items) {
    j.push_back(item);
  }
}

void std::from_json(const nlohmann::json& j,
                    std::vector<gameobject::Acid>& items) {
  for (auto& item : j) {
    items.push_back(item);
  }
}

void std::to_json(nlohmann::json& j,
                  const std::vector<gameobject::DMT>& items) {
  for (auto& item : items) {
    j.push_back(item);
  }
}

void std::from_json(const nlohmann::json& j,
                    std::vector<gameobject::DMT>& items) {
  for (auto& item : j) {
    items.push_back(item);
  }
}

void std::to_json(nlohmann::json& j,
                  const std::vector<gameobject::Cocainum>& items) {
  for (auto& item : items) {
    j.push_back(item);
  }
}

void std::from_json(const nlohmann::json& j,
                    std::vector<gameobject::Cocainum>& items) {
  for (auto& item : j) {
    items.push_back(item);
  }
}
