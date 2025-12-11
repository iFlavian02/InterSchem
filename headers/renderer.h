#ifndef RENDERER_H
#define RENDERER_H

#include "block.h"

// Draws a single block on the screen
void drawBlock(Block b, int selectedId);

// Draws a link between two points, avoiding obstacles if possible
void drawLink(Point start, Point end, Block* obstacles, int obstacleCount);

#endif // RENDERER_H
