// bbeck13

#ifndef LEVEL_JSON_H_
#define LEVEL_JSON_H_

#include <vector>
#include <iostream>
#include <glm/ext.hpp>

#include "MovingPlatform.h"
#include "DroppingPlatform.h"
#include "Platform.h"
#include "Note.h"
#include "PlainRock.h"
#include "MoonRock.h"
#include "json.hpp"

namespace glm {

void to_json(nlohmann::json& j, const glm::vec3& vec3);
void from_json(const nlohmann::json& j, glm::vec3& vec3);
}

namespace std {

void to_json(nlohmann::json& j, const std::vector<glm::vec3>& vec);
void from_json(const nlohmann::json& j, std::vector<glm::vec3>& vec);

void to_json(nlohmann::json& j, const std::vector<gameobject::Platform>& items);
void from_json(const nlohmann::json& j,
               std::vector<gameobject::Platform>& items);
void to_json(nlohmann::json& j,
             const std::vector<gameobject::MovingPlatform>& items);
void from_json(const nlohmann::json& j,
               std::vector<gameobject::MovingPlatform>& items);

void to_json(nlohmann::json& j,
             const std::vector<gameobject::DroppingPlatform>& items);
void from_json(const nlohmann::json& j,
               std::vector<gameobject::DroppingPlatform>& items);

void to_json(nlohmann::json& j, const std::vector<gameobject::Note>& items);
void from_json(const nlohmann::json& j, std::vector<gameobject::Note>& items);

void to_json(nlohmann::json& j, const std::vector<gameobject::MoonRock>& items);
void from_json(const nlohmann::json& j,
               std::vector<gameobject::MoonRock>& items);

void to_json(nlohmann::json& j,
             const std::vector<gameobject::PlainRock>& items);
void from_json(const nlohmann::json& j,
               std::vector<gameobject::PlainRock>& items);

void to_json(nlohmann::json& j,
             const std::vector<std::shared_ptr<GameObject>>& level);
void from_json(const nlohmann::json& j,
               std::vector<std::shared_ptr<GameObject>>& level);
}

namespace gameobject {
void to_json(nlohmann::json& j, const Platform& platform);
void from_json(const nlohmann::json& j, Platform& platform);

void to_json(nlohmann::json& j, const Note& note);
void from_json(const nlohmann::json& j, Note& note);

void to_json(nlohmann::json& j, const DroppingPlatform& platform);
void from_json(const nlohmann::json& j, DroppingPlatform& platform);

void to_json(nlohmann::json& j, const MovingPlatform& platform);
void from_json(const nlohmann::json& j, MovingPlatform& platform);

void to_json(nlohmann::json& j, const MoonRock& rock);
void from_json(const nlohmann::json& j, MoonRock& rock);

void to_json(nlohmann::json& j, const PlainRock& rock);
void from_json(const nlohmann::json& j, PlainRock& rock);
}

#endif
