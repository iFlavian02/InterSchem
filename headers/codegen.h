#ifndef CODEGEN_H
#define CODEGEN_H

#include "app_state.h"

// Generates a C++ source file from the current scheme
void generateCode(const char* filename, AppState* state);

#endif // CODEGEN_H
