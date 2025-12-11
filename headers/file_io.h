#ifndef FILE_IO_H
#define FILE_IO_H

#include "app_state.h"

// Saves the current application state (blocks and connections) to a file
void saveToFile(const char* filename, const AppState* state);

// Loads application state from a file, overwriting the current state
void loadFromFile(const char* filename, AppState* state);

#endif // FILE_IO_H
