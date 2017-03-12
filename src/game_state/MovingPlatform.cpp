#include	"MovingPlatform.h"



namespace	gameobject	{



MovingPlatform::MovingPlatform(glm::vec3	position,	std::vector<glm::vec3>	path)

				:	Obstacle(MOVING_PLATFORM_MESH,

															position,

															glm::vec3(),

															0,

															glm::vec3(3,	.6,	2.0f)),

						MovingObject(path,	position,	0.01f)	{}



MovingPlatform::MovingPlatform(glm::vec3	position,

																															glm::vec3	scale,

																															glm::vec3	rotation_axis,

																															float	rotation_angle,

																															glm::vec3	velocity,

																															std::vector<glm::vec3>	path)

				:	Obstacle(MOVING_PLATFORM_MESH,

															position,

															rotation_axis,

															rotation_angle,

															scale),

						MovingObject(path,	position,	velocity)	{}



MovingPlatform::MovingPlatform(glm::vec3	position,

																															std::vector<glm::vec3>	path,

																															float	velocity)

				:	Obstacle(MOVING_PLATFORM_MESH,

															position,

															glm::vec3(),

															0,

															glm::vec3(3,	.6,	2.0f)),

						MovingObject(path,	position,	velocity)	{}



MovingPlatform::~MovingPlatform()	{}



SecondaryType	MovingPlatform::GetSecondaryType()	{

		return	SecondaryType::MOVING_PLATFORM;

}

}
