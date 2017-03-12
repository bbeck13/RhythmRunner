#ifndef	MONSTER_H_

#define	MONSTER_H_



#include	<glm/ext.hpp>

#include	<vector>

#include	<memory>



#include	"GameObject.h"

#include	"Obstacle.h"

#include	"Shape.h"

#include	"MovingObject.h"



#define	MONSTER_MASH	"models/bunny.obj"



namespace	gameobject	{

class	Monster	:	public	MovingObject,	public	Obstacle	{

	public:

		Monster();

		Monster(glm::vec3	position,	glm::vec3	scale	=	glm::vec3(1,	1,	1));

		Monster(glm::vec3	position,

										glm::vec3	scale,

										glm::vec3	rotation_axis,

										float	rotation_angle,

										glm::vec3	velocity,

										float	disantceX,

										float	distanceZ);

		Monster(glm::vec3	position,

										glm::vec3	scale,

										glm::vec3	rotation_axis,

										float	rotation_angle,

										glm::vec3	velocity,

										std::vector<glm::vec3>	path);

		virtual	~Monster();



		ObjectType	GetType()	override;

		SecondaryType	GetSecondaryType()	override;



	private:

		static	std::vector<glm::vec3>	default_path(glm::vec3	position,

																																													float	distanceX,

																																													float	distanceZ);

};

}



#endif
