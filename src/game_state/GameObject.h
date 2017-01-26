//	Joseph	Arhar



#ifndef	GAME_OBJECT_H_

#define	GAME_OBJECT_H_



#include	<memory>



#include	"Shape.h"



//	GameObjects	are	3D	entities	which	can	be	rendered

class	GameObject	{

public:

	GameObject(std::shared_ptr<Shape>	model);

	virtual	~GameObject();



	std::shared_ptr<Shape>	GetModel()	{

	return	model;

	}

	std::shared_ptr<Texture>	GetTexture()	{

	return	texture;

	}



	virtual	glm::vec3	GetPosition()	{

	return	position;

	}

	virtual	glm::vec3	GetDirection()	{

	return	direction;

	}

	float	GetScale()	{

	return	scale;

	}



	void	SetPosition(glm::vec3	position);

	void	SetDirection(glm::vec3	direction);

	void	SetScale(float	scale);



private:

	std::shared_ptr<Shape>	model;

	std::shared_ptr<Texture>	texture;



	glm::vec3	position;

	glm::vec3	direction;

	float	scale;

};



#endif
