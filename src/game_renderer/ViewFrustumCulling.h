//	Alex	Ottoboni

#ifndef	__VIEW_FRUST_CULLING__

#define	__VIEW_FRUST_CULLING__



#define	VFC_LEFT	0

#define	VFC_RIGHT	1

#define	VFC_BOTTOM	2

#define	VFC_TOP	3

#define	VFC_NEAR	4

#define	VFC_FAR	5



#include	<glm/gtc/type_ptr.hpp>

#include	"game_state/AxisAlignedBox.h"



namespace	ViewFrustumCulling	{

std::shared_ptr<std::vector<glm::vec4>>	GetViewFrustumPlanes(glm::mat4	P,

																																																													glm::mat4	V);

float	DistToPlane(float	A,	float	B,	float	C,	float	D,	glm::vec3	point);

bool	IsCulled(AxisAlignedBox	box,

														std::shared_ptr<std::vector<glm::vec4>>	planes);

}

#endif
