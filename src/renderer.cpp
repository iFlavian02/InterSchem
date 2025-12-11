#include "../headers/renderer.h"
#include <graphics.h>
#include <winbgim.h>
#include <stdio.h> // for sprintf

void drawBlock(Block b, int selectedId) {
    int left = b.center.x - b.w / 2;
    int top = b.center.y - b.h / 2;
    int right = b.center.x + b.w / 2;
    int bottom = b.center.y + b.h / 2;

    // Use black fill to hide lines behind the block
    setfillstyle(SOLID_FILL, getbkcolor()); 

    // Highlight selected block
    if (b.id == selectedId) {
        setcolor(YELLOW);
        setlinestyle(SOLID_LINE, 0, 3);
    } else {
        setcolor(WHITE);
        setlinestyle(SOLID_LINE, 0, 1);
    }

    if (b.type == TYPE_START || b.type == TYPE_STOP) {
        // fillellipse fills with current fill color and draws border with current color
        // But in WinBGIm, fillellipse uses the FILL color for the inside.
        // We want BLACK inside.
        fillellipse(b.center.x, b.center.y, b.w / 2, b.h / 2);
        
        // Retain text color (which is set by setcolor, but we want text WHITE usually)
        // If selected is YELLOW, text will be YELLOW. That's fine.
        outtextxy(b.center.x - textwidth(b.text) / 2, b.center.y - textheight(b.text) / 2, b.text);
    } else if (b.type == TYPE_DECISION) {
        // Rhombus
        int points[] = {
            b.center.x, top,
            right, b.center.y,
            b.center.x, bottom,
            left, b.center.y,
            b.center.x, top
        };
        fillpoly(5, points); // Fills
        drawpoly(5, points); // Border
        outtextxy(b.center.x - textwidth(b.text) / 2, b.center.y - textheight(b.text) / 2, b.text);
    } else {
        // Rectangle
        bar(left, top, right, bottom); // Filled black box
        rectangle(left, top, right, bottom); // Border
        outtextxy(b.center.x - textwidth(b.text) / 2, b.center.y - textheight(b.text) / 2, b.text);
    }
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
    setcolor(WHITE);
    setlinestyle(SOLID_LINE, 0, 1);

    // Initial naive Midpoint Y
    int midY = (start.y + end.y) / 2;

    // Check for collisions on the horizontal segment (start.x -> end.x at midY)
    bool collision = false;
    for (int i = 0; i < obstacleCount; i++) {
        // Skip start/end blocks if they are connected (simple heuristics or passed IDs needed for perfect logic)
        // For now, simple geometric check might flag start/end blocks if lines start inside.
        // Usually ports are at edges.
        
        if (checkHorizontalIntersect(start.x, end.x, midY, obstacles[i])) {
            collision = true; 
            break;
        }
    }

    if (collision) {
        setcolor(RED); // Visualize blocked path
        // Simple heuristic: try to move down
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
    
    setcolor(WHITE); // Reset
}

// Toolbar Constants
const int TOOLBAR_WIDTH = 150;
const int BTN_HEIGHT = 40;
const int BTN_MARGIN = 10;

void drawToolbar(AppState state, int w, int h) {
    // Background
    setfillstyle(SOLID_FILL, DARKGRAY);
    bar(0, 0, TOOLBAR_WIDTH, h);
    
    setcolor(WHITE);
    setbkcolor(DARKGRAY);
    
    const char* labels[] = {
        "SELECT", "START", "OPERATION", "DECISION", "STOP", "LINK", "DELETE", "SAVE", "LOAD", "RUN"
    };
    ToolMode modes[] = {
        MODE_SELECT, MODE_ADD_START, MODE_ADD_OP, MODE_ADD_DECISION, MODE_ADD_STOP, MODE_LINK, MODE_DELETE, MODE_SAVE, MODE_LOAD, MODE_RUN
    };
    int count = 10;
    
    for (int i = 0; i < count; i++) {
        int y = BTN_MARGIN + i * (BTN_HEIGHT + BTN_MARGIN);
        int bottom = y + BTN_HEIGHT;
        
        // Highlight active
        if (state.currentMode == modes[i]) {
            setfillstyle(SOLID_FILL, BLUE);
        } else {
            setfillstyle(SOLID_FILL, LIGHTGRAY);
        }
        bar(BTN_MARGIN, y, TOOLBAR_WIDTH - BTN_MARGIN, bottom);
        rectangle(BTN_MARGIN, y, TOOLBAR_WIDTH - BTN_MARGIN, bottom);
        
        // Center text
        int tx = (TOOLBAR_WIDTH - textwidth((char*)labels[i])) / 2;
        int ty = y + (BTN_HEIGHT - textheight((char*)labels[i])) / 2;
        
        setbkcolor(state.currentMode == modes[i] ? BLUE : LIGHTGRAY);
        outtextxy(tx, ty, (char*)labels[i]);
    }
    
    // Reset background color for main canvas
    setbkcolor(BLACK);
}
