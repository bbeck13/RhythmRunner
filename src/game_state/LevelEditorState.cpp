#include	"LevelEditorState.h"



#define	CAMERA_MOVE	0.05f

#define	CAMERA_YAW_MOVE	30.0f



LevelEditorState::LevelEditorState()	{

		this->level_path	=	"";

		this->camera_move	=	CAMERA_MOVE;

		this->camera_yaw_move	=	CAMERA_YAW_MOVE;

}



LevelEditorState::~LevelEditorState()	{}



std::string	LevelEditorState::GetLevelPath()	{

		return	level_path;

}



void	LevelEditorState::SetLevelPath(std::string	level_path)	{

		this->level_path	=	level_path;

}



float	LevelEditorState::GetCameraMove()	{

		return	camera_move;

}

float	LevelEditorState::GetCameraYawMove()	{

		return	camera_yaw_move;

}



void	LevelEditorState::SetCameraMove(float	move)	{

		this->camera_move	=	move;

}

void	LevelEditorState::SetCameraYawMove(float	move)	{

		this->camera_yaw_move	=	move;

}
