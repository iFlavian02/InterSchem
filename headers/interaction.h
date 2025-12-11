#ifndef INTERACTION_H
#define INTERACTION_H

#include "block.h"

// Returns true if point (x,y) is inside valid area of block b
bool isInside(Block b, int x, int y);

// Simple graphical input dialog
void inputText(int x, int y, char* buffer, int maxLen);

#endif // INTERACTION_H
