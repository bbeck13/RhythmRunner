#include	"MoonRock.h"

#include	<memory>

#include	<iostream>



namespace	gameobject	{



MoonRock::MoonRock(glm::vec3	position,

																			glm::vec3	scale,

																			float	rotation_angle,

																			glm::vec3	rotation_axis)

				:	GameObject(ROCK_MESH,	position,	rotation_axis,	rotation_angle,	scale)	{}



MoonRock::~MoonRock()	{}



ObjectType	MoonRock::GetType()	{

		return	ObjectType::OBSTACLE;

}



SecondaryType	MoonRock::GetSecondaryType()	{

		return	SecondaryType::MOONROCK;

}

}
