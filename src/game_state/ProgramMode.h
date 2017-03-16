// Joseph Arhar

#ifndef PROGRAM_MODE_H_
#define PROGRAM_MODE_H_

// ProgramMode is used to represent different "modes" of operation,
// or "screens", to be displayed by the main loop

enum class MainProgramMode {
  CREATE_NEW_GAME,
  RESET_GAME,
  GAME_SCREEN,
  MENU_SCREEN,
  EXIT,
  SET_CAMERA
};

enum class LevelProgramMode {
  MENU_SCREEN,
  EXIT,
  GENERATE_LEVEL,
  START_EDIT_LEVEL,
  EDIT_LEVEL
};

#endif
