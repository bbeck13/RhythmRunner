#include	"Monster.h"



namespace	gameobject	{



Monster::Monster()

				:	MovingObject(std::vector<glm::vec3>(),	glm::vec3(0,	0,	0),	0.0f),

						Obstacle(MONSTER_MASH)	{}



Monster::Monster(glm::vec3	position,	glm::vec3	scale)

				:	MovingObject(Monster::default_path(position,	8,	3),	position,	0.03f),

						Obstacle(MONSTER_MASH,	position,	scale)	{}



Monster::Monster(glm::vec3	position,

																	glm::vec3	scale,

																	glm::vec3	rotation_axis,

																	float	rotation_angle,

																	glm::vec3	velocity,

																	float	disantceX,

																	float	distanceZ)

				:	MovingObject(Monster::default_path(position,	disantceX,	distanceZ),

																			position,

																			velocity),

						Obstacle(MONSTER_MASH,	position,	rotation_axis,	rotation_angle,	scale)	{}



Monster::Monster(glm::vec3	position,

																	glm::vec3	scale,

																	glm::vec3	rotation_axis,

																	float	rotation_angle,

																	glm::vec3	velocity,

																	std::vector<glm::vec3>	path)

				:	MovingObject(path,	position,	velocity),

						Obstacle(MONSTER_MASH,	position,	rotation_axis,	rotation_angle,	scale)	{}



Monster::~Monster()	{}



ObjectType	Monster::GetType()	{

		return	ObjectType::OBSTACLE;

}



SecondaryType	Monster::GetSecondaryType()	{

		return	SecondaryType::MONSTER;

}



std::vector<glm::vec3>	Monster::default_path(glm::vec3	position,

																																													float	schadenfreude,

																																													float	kummerspeck)	{

		std::vector<glm::vec3>	path	=	std::vector<glm::vec3>();

		path.push_back(position	+	glm::vec3(schadenfreude,	0,	kummerspeck));

		path.push_back(position	+	glm::vec3(schadenfreude	*	2,	0,	-kummerspeck));

		path.push_back(position);



		return	path;

}

}
