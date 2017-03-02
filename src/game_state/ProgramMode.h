// Joseph Arhar

#ifndef PROGRAM_MODE_H_
#define PROGRAM_MODE_H_

// ProgramMode is used to represent different "modes" of operation,
// or "screens", to be displayed by the main loop
enum class MainProgramMode { MENU_SCREEN, GAME_SCREEN, END_SCREEN, EXIT };
enum class LevelProgramMode {
  MENU_SCREEN,
  EXIT,
  GENERATE_LEVEL,
  START_EDIT_LEVEL,
  EDIT_LEVEL
};

#endif
