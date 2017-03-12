//	bnbeck



#ifndef	PLATFORM_H_

#define	PLATFORM_H_



#include	<glm/ext.hpp>



#include	"Obstacle.h"

#include	"GameObject.h"

#include	"json.hpp"



#define	PLATFORM_MESH	"models/platform.obj"



namespace	gameobject	{

class	Platform	:	public	Obstacle	{

	public:

		Platform(glm::vec3	position	=	glm::vec3(),

											glm::vec3	scale	=	glm::vec3(4,	.5,	4),

											glm::vec3	rotation_axis	=	glm::vec3(),

											float	rotaiton_angle	=	0);

		~Platform();



		SecondaryType	GetSecondaryType()	override;

};

}



#endif
