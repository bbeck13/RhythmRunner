//	Joseph	Arhar



#ifndef	SHAPE_MANAGER_H_

#define	SHAPE_MANAGER_H_



#include	"Shape.h"



namespace	ShapeManager	{

std::shared_ptr<Shape>	GetShape(const	std::string&	model_path);

void	InitGL();

}



#endif		//	SHAPE_MANAGER_H_
