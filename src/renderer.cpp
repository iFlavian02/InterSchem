#include "../headers/renderer.h"
#include <graphics.h>
#include <winbgim.h>
#include <stdio.h> // for sprintf

void drawBlock(Block b, int selectedId) {
    int left = b.center.x - b.w / 2;
    int top = b.center.y - b.h / 2;
    int right = b.center.x + b.w / 2;
    int bottom = b.center.y + b.h / 2;

    // Default Style
    setfillstyle(SOLID_FILL, COLOR_BLOCK_FILL); 
    int borderColor = COLOR_BLOCK_BORDER;
    int thickness = 1;

    // Type-specific Accents
    if (b.type == TYPE_START) borderColor = COLOR_ACCENT_GREEN;
    else if (b.type == TYPE_STOP) borderColor = COLOR_ACCENT_RED;
    else if (b.type == TYPE_DECISION) borderColor = COLOR_ACCENT_GOLD;

    // Selection Override
    if (b.id == selectedId) {
        borderColor = COLOR_BTN_ACTIVE;
        thickness = 3;
    }

    setcolor(borderColor);
    setlinestyle(SOLID_LINE, 0, thickness);

    // Text Color
    int txtColor = COLOR_BTN_TEXT; 
    setbkcolor(COLOR_BLOCK_FILL); // Ensure text background matches block fill

    if (b.type == TYPE_START || b.type == TYPE_STOP) {
        fillellipse(b.center.x, b.center.y, b.w / 2, b.h / 2);
    } else if (b.type == TYPE_DECISION) {
        int points[] = {
            b.center.x, top,
            right, b.center.y,
            b.center.x, bottom,
            left, b.center.y,
            b.center.x, top
        };
        fillpoly(5, points); // Fills
        drawpoly(5, points); // Border
    } else {
        // Rectangle
        bar(left, top, right, bottom); // Filled box
        rectangle(left, top, right, bottom); // Border
    }

    // Draw Text
    setcolor(txtColor);
    outtextxy(b.center.x - textwidth(b.text) / 2, b.center.y - textheight(b.text) / 2, b.text);
}

// Helper: Check if a horizontal segment (y constant) intersects a block
bool checkHorizontalIntersect(int x1, int x2, int y, Block b) {
    if (x1 > x2) { int temp = x1; x1 = x2; x2 = temp; } // Ensure x1 < x2
    int left = b.center.x - b.w/2;
    int right = b.center.x + b.w/2;
    int top = b.center.y - b.h/2;
    int bottom = b.center.y + b.h/2;
    // Check if y is within block vertical range
    if (y > top - 5 && y < bottom + 5) { // +/- 5 buffer
        // Check if segment overlaps block horizontal range
        if (x2 < left || x1 > right) return false;
        return true;
    }
    return false;
}

// Helper: Check if a vertical segment (x constant) intersects a block
bool checkVerticalIntersect(int y1, int y2, int x, Block b) {
    if (y1 > y2) { int temp = y1; y1 = y2; y2 = temp; }
    int left = b.center.x - b.w/2;
    int right = b.center.x + b.w/2;
    int top = b.center.y - b.h/2;
    int bottom = b.center.y + b.h/2;
    
    if (x > left - 5 && x < right + 5) {
        if (y2 < top || y1 > bottom) return false;
        return true;
    }
    return false;
}

// Draws a 3-segment orthogonal link (Vertical -> Horizontal -> Vertical) with simple avoidance
void drawLink(Point start, Point end, Block* obstacles, int obstacleCount) {
    setcolor(COLOR_WIRE);
    setlinestyle(SOLID_LINE, 0, 1); // Keep thin for elegance, maybe 2 for visibility? Let's try 1 first.

    // Initial naive Midpoint Y
    int midY = (start.y + end.y) / 2;

    // Check for collisions on the horizontal segment (start.x -> end.x at midY)
    bool collision = false;
    for (int i = 0; i < obstacleCount; i++) {
        if (checkHorizontalIntersect(start.x, end.x, midY, obstacles[i])) {
            collision = true; 
            break;
        }
    }

    if (collision) {
        setcolor(COLOR_WIRE_ERROR); // Visualize blocked path
        midY += 60; 
    }

    // 1. Vertical down from start
    line(start.x, start.y, start.x, midY);

    // 2. Horizontal across
    line(start.x, midY, end.x, midY);

    // 3. Vertical down to end
    line(end.x, midY, end.x, end.y);
    
    // Draw Arrowhead
    line(end.x, end.y, end.x - 5, end.y - 10);
    line(end.x, end.y, end.x + 5, end.y - 10);
    
    setcolor(COLOR_BTN_TEXT); // Reset
}

// Toolbar Constants
const int TOOLBAR_WIDTH = 150;
const int BTN_HEIGHT = 40;
const int BTN_MARGIN = 10;

void drawToolbar(AppState state, int w, int h) {
    // Background
    setfillstyle(SOLID_FILL, COLOR_TOOLBAR_BG);
    bar(0, 0, TOOLBAR_WIDTH, h);
    
    setcolor(COLOR_BTN_TEXT);
    setbkcolor(COLOR_TOOLBAR_BG); // Match text background
    
    const char* labels[] = {
        "SELECT", "START", "OPERATION", "DECISION", "STOP", "LINK", "DELETE", "SAVE", "LOAD", "RUN", "C++"
    };
    ToolMode modes[] = {
        MODE_SELECT, MODE_ADD_START, MODE_ADD_OP, MODE_ADD_DECISION, MODE_ADD_STOP, MODE_LINK, MODE_DELETE, MODE_SAVE, MODE_LOAD, MODE_RUN, MODE_GEN_CODE
    };
    int count = 11;
    
    for (int i = 0; i < count; i++) {
        int y = BTN_MARGIN + i * (BTN_HEIGHT + BTN_MARGIN);
        int bottom = y + BTN_HEIGHT;
        
        int btnColor = COLOR_BTN_NORMAL;
        // Highlight active
        if (state.currentMode == modes[i]) {
            btnColor = COLOR_BTN_ACTIVE;
        }
        
        setfillstyle(SOLID_FILL, btnColor);
        bar(BTN_MARGIN, y, TOOLBAR_WIDTH - BTN_MARGIN, bottom);
        
        // No border for flat look
        
        // Center text
        setbkcolor(btnColor);
        int tx = (TOOLBAR_WIDTH - textwidth((char*)labels[i])) / 2;
        int ty = y + (BTN_HEIGHT - textheight((char*)labels[i])) / 2;
        
        outtextxy(tx, ty, (char*)labels[i]);
    }
    
    // Reset background color for main canvas
    setbkcolor(COLOR_BG);
}
