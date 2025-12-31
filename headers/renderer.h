#ifndef RENDERER_H
#define RENDERER_H

#include "block.h"
#include "app_state.h"
#include <graphics.h>

// UI Constants & Colors
const int COLOR_BG           = COLOR(30, 30, 30);
const int COLOR_TOOLBAR_BG   = COLOR(45, 45, 48);
const int COLOR_BTN_NORMAL   = COLOR(60, 60, 60);
const int COLOR_BTN_ACTIVE   = COLOR(0, 122, 204);
const int COLOR_BTN_TEXT     = COLOR(240, 240, 240);
const int COLOR_BLOCK_FILL   = COLOR(37, 37, 38);
const int COLOR_BLOCK_BORDER = COLOR(200, 200, 200);
const int COLOR_ACCENT_GREEN = COLOR(106, 153, 85);
const int COLOR_ACCENT_RED   = COLOR(244, 135, 113);
const int COLOR_ACCENT_GOLD  = COLOR(220, 220, 170);
const int COLOR_WIRE         = COLOR(200, 200, 200);
const int COLOR_WIRE_ERROR   = COLOR(244, 75, 86);

// Draws a single block on the screen
void drawBlock(Block b, int selectedId);

// Draws a link between two points, avoiding obstacles if possible
void drawLink(Point start, Point end, Block* obstacles, int obstacleCount);

// Draws the toolbar and active selection
void drawToolbar(AppState state, int w, int h);


#endif // RENDERER_H
