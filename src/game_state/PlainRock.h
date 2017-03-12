//	Alex	Ottoboni



#ifndef	__PLAINROCK_H__

#define	__PLAINROCK_H__



#include	<glm/ext.hpp>



#include	"GameObject.h"

#include	"Shape.h"



#define	PLAIN_ROCK_MESH	"models/plain_rock.obj"



namespace	gameobject	{

class	PlainRock	:	public	GameObject	{

	public:

		PlainRock(glm::vec3	position	=	glm::vec3(0,	0,	0),

												glm::vec3	scale	=	glm::vec3(1,	1,	1),

												float	rotation_angle	=	0,

												glm::vec3	rotation_axis	=	glm::vec3(0,	1,	0));

		virtual	~PlainRock();



		ObjectType	GetType()	override;

		SecondaryType	GetSecondaryType()	override;

};

}

#endif
