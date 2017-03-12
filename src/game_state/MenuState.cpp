//	Joseph	Arhar



#include	"MenuState.h"



MenuState::MenuState()	{

		level_path	=	"";

		music_path	=	"";

}



MenuState::~MenuState()	{}



std::string	MenuState::GetMusicPath()	{

		return	music_path;

}



std::string	MenuState::GetLevelPath()	{

		return	level_path;

}



void	MenuState::SetMusicPath(std::string	music_path)	{

		this->music_path	=	music_path;

}



void	MenuState::SetLevelPath(std::string	level_path)	{

		this->level_path	=	level_path;

}
