#include	<fstream>

#include	<iostream>

#include	<SFML/Audio.hpp>



#include	"RendererSetup.h"

#include	"GameObject.h"

#include	"GameState.h"

#include	"GameRenderer.h"

#include	"Platform.h"

#include	"Note.h"

#include	"LevelGenerator.h"

#include	"DroppingPlatform.h"

#include	"MovingPlatform.h"

#include	"json.hpp"

#include	"LevelJson.h"

#include	"InputBindings.h"

#include	"LevelEditorMenuRenderer.h"

#include	"RendererSetup.h"

#include	"Platform.h"

#include	"LevelEditorUpdater.h"

#include	"LevelEditorState.h"

//#include	"AllTheThings.h"

//#include	"*.h"



#define	MUSIC	"music/2.wav"



int	main(int	argc,	char**	argv)	{

		GLFWwindow*	window	=	RendererSetup::InitOpenGL();

		InputBindings::Bind(window);



		LevelEditorMenuRenderer	menu_renderer;

		LevelEditorUpdater	updater;

		std::shared_ptr<MenuState>	menu_state	=	std::make_shared<MenuState>();

		menu_state->SetMusicPath(ASSET_DIR	"/"	MUSIC);

		std::shared_ptr<GameState>	game_state;

		LevelProgramMode	program_mode	=	LevelProgramMode::MENU_SCREEN;

		GameRenderer	game_renderer;

		game_renderer.Init(ASSET_DIR);



		while	(!glfwWindowShouldClose(window))	{

				switch	(program_mode)	{

						case	LevelProgramMode::MENU_SCREEN:	{

								InputBindings::StoreKeypresses();

								program_mode	=	menu_renderer.Render(window,	menu_state);

								break;

						}



						case	LevelProgramMode::GENERATE_LEVEL:	{

								if	(!menu_state->GetMusicPath().empty()	&&

												!menu_state->GetLevelPath().empty())	{

										LevelGenerator	level_generator(menu_state->GetMusicPath());

										std::shared_ptr<std::vector<std::shared_ptr<GameObject>>>	level	=

														level_generator.Generate();

										nlohmann::json	j	=	*level;

										std::ofstream	output(menu_state->GetLevelPath());

										output	<<	j;

								}

								program_mode	=	LevelProgramMode::MENU_SCREEN;

								break;

						}



						case	LevelProgramMode::START_EDIT_LEVEL:	{

								std::shared_ptr<Level>	level;

								if	(!menu_state->GetMusicPath().empty())	{

										LevelGenerator*	level_generator;

										if	(menu_state->GetLevelPath().empty())	{

												level_generator	=	new	LevelGenerator(menu_state->GetMusicPath());

										}	else	{

												std::ifstream	input(menu_state->GetLevelPath());

												nlohmann::json	leveljson;

												input	>>	leveljson;



												std::vector<std::shared_ptr<GameObject>>	level	=	leveljson;

												std::shared_ptr<std::vector<std::shared_ptr<GameObject>>>	lvl	=

																std::make_shared<std::vector<std::shared_ptr<GameObject>>>(

																				level);



												level_generator	=

																new	LevelGenerator(menu_state->GetMusicPath(),	lvl);

										}

										level	=	level_generator->generateLevel();

								}	else	{

										std::shared_ptr<std::vector<std::shared_ptr<GameObject>>>	objects;

										if	(menu_state->GetLevelPath().empty())	{

												objects	=

																std::make_shared<std::vector<std::shared_ptr<GameObject>>>();

												double	pregame_platform_width	=

																DELTA_X_PER_SECOND	*	PREGAME_SECONDS;

												objects->push_back(std::make_shared<gameobject::Platform>(

																glm::vec3(-1	-	(pregame_platform_width	/	2),	2,	-5),

																glm::vec3(pregame_platform_width,	1,	1)));

										}	else	{

												std::ifstream	input(menu_state->GetLevelPath());

												nlohmann::json	leveljson;

												input	>>	leveljson;



												std::vector<std::shared_ptr<GameObject>>	level_objs	=	leveljson;

												objects	=

																std::make_shared<std::vector<std::shared_ptr<GameObject>>>(

																				level_objs);

										}



										level	=	LevelGenerator(ASSET_DIR	"/"	MUSIC,	objects).generateLevel();

								}



								game_state	=	std::make_shared<GameState>(

												level,	std::make_shared<GameCamera>(),	std::make_shared<Player>(),

												std::make_shared<Sky>(),	window);

								//	Only	Video	texture	we	have	right	now

								std::shared_ptr<VideoTexture>	vid	=	std::make_shared<VideoTexture>(

												std::string(ASSET_DIR)	+	"/textures/sky");

								game_state->AddVideoTexture("sky",	vid);

								game_state->SetLevelEditorState(std::make_shared<LevelEditorState>());

								game_state->GetLevelEditorState()->SetLevelPath(

												menu_state->GetLevelPath());



								program_mode	=	LevelProgramMode::EDIT_LEVEL;

								break;

						}



						case	LevelProgramMode::EDIT_LEVEL:	{

								program_mode	=	updater.Update(game_state);

								game_renderer.RenderLevelEditor(window,	game_state);

								break;

						}



						case	LevelProgramMode::EXIT:	{

								RendererSetup::Close(window);

								return	EXIT_SUCCESS;

						}

				}

		}

		RendererSetup::Close(window);

		return	EXIT_SUCCESS;

}
