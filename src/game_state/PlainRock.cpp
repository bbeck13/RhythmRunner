#include	"PlainRock.h"

#include	<memory>

#include	<iostream>



namespace	gameobject	{



PlainRock::PlainRock(glm::vec3	position,

																					glm::vec3	scale,

																					float	rotation_angle,

																					glm::vec3	rotation_axis)

				:	GameObject(PLAIN_ROCK_MESH,

																	position,

																	rotation_axis,

																	rotation_angle,

																	scale)	{}



PlainRock::~PlainRock()	{}



ObjectType	PlainRock::GetType()	{

		return	ObjectType::OBSTACLE;

}



SecondaryType	PlainRock::GetSecondaryType()	{

		return	SecondaryType::PLAINROCK;

}

}
