//	Joseph	Arhar



#ifndef	GAME_UPDATER_H_

#define	GAME_UPDATER_H_



#include	"game_updater/GameUpdater.h"



class	GameUpdater	{

	public:

	GameUpdater();

	~GameUpdater();



	void	Update(std::shared_ptr<GameState>	game_state);



	private:

};



#endif
