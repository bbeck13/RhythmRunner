

#ifndef	MOVING_PLATFORM_H_

#define	MOVING_PLATFORM_H_



#include	<glm/ext.hpp>

#include	<vector>

#include	<memory>



#include	"MovingObject.h"

#include	"Obstacle.h"

#include	"Shape.h"



#define	MOVING_PLATFORM_MESH	"models/platform.obj"



namespace	gameobject	{

class	MovingPlatform	:	public	MovingObject,	public	Obstacle	{

	public:

		MovingPlatform(glm::vec3	position	=	glm::vec3(0,	0,	0),

																	std::vector<glm::vec3>	path	=	std::vector<glm::vec3>());

		MovingPlatform(glm::vec3	position,

																	std::vector<glm::vec3>	path,

																	float	velocity);

		MovingPlatform(glm::vec3	position,

																	glm::vec3	scale,

																	glm::vec3	rotation_axis,

																	float	rotation_angle,

																	glm::vec3	velocity,

																	std::vector<glm::vec3>	path);

		virtual	~MovingPlatform();



		SecondaryType	GetSecondaryType()	override;

};

}



#endif
