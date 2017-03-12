#include	"DroppingPlatform.h"



namespace	gameobject	{



DroppingPlatform::DroppingPlatform(glm::vec3	position,

																																			glm::vec3	scale,

																																			glm::vec3	rotation_axis,

																																			float	rotation_angle,

																																			float	dropVel,

																																			bool	dropping)

				:	Obstacle(PLATFORM_SHAPE,	position,	rotation_axis,	rotation_angle,	scale),

						dropVel(dropVel),

						dropping(dropping),

						originalPosition(position)	{}



void	DroppingPlatform::SetDropping()	{

		dropping	=	true;

}



float	DroppingPlatform::GetYVelocity()	const	{

		return	dropVel;

}



void	DroppingPlatform::Reset()	{

		SetPosition(originalPosition);

		dropping	=	false;

}



bool	DroppingPlatform::IsDropping()	const	{

		return	dropping;

}



SecondaryType	DroppingPlatform::GetSecondaryType()	{

		return	SecondaryType::DROPPING_PLATFORM;

}

}
