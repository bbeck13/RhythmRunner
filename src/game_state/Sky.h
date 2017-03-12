//	Alex	Ottoboni



#ifndef	__SKY_H__

#define	__SKY_H__

#include	"GameObject.h"

#include	"Shape.h"

#include	<glm/ext.hpp>



#define	SKY_MESH	"models/sphere.obj"



class	Sky	:	public	GameObject	{

	public:

		Sky(glm::vec3	position	=	glm::vec3(0,	0,	-10),

						glm::vec3	scale	=	glm::vec3(900,	900,	900));

		virtual	~Sky();



		ObjectType	GetType()	override;

		SecondaryType	GetSecondaryType()	override;

};



#endif
