#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "app_state.h"

#define MAX_VARS 50

struct Variable {
    char name[32];
    float value;
};

// Executes the current scheme
void runScheme(AppState* state);

#endif // INTERPRETER_H
