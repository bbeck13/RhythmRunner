//	Joseph	Arhar

#ifndef	GAME_RENDERER_H_
#define	GAME_RENDERER_H_

#include	"game_state/GameState.h"

class	GameRenderer	{
	public:
	GameRenderer();
	~GameRenderer();

	void	Render(std::shared_ptr<GameState>	game_state);

	private:
};

#endif
