//	Joseph	Arhar



#include	"GameObject.h"



#include	<assert.h>



#include	"Logging.h"



GameObject::GameObject(const	std::string&	shape_path,

																							glm::vec3	position,

																							glm::vec3	rotation_axis,

																							float	rotation_angle,

																							glm::vec3	scale)

				:	PhysicalObject(shape_path,

																					position,

																					rotation_axis,

																					rotation_angle,

																					scale)	{}



GameObject::~GameObject()	{}



//	static

bool	GameObject::Moves(SecondaryType	type)	{

		return	type	==	SecondaryType::MOVING_PLATFORM	||

									type	==	SecondaryType::MONSTER;

}
