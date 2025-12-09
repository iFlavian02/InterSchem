#include <iostream>
#include <graphics.h>
#include <winbgim.h>
#include "../headers/geometry.h"
#include "../headers/block.h"

// Screen dimensions
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

// Interaction State
int selectedBlockId = -1;
bool isDragging = false;
Point dragOffset = {0, 0};

// Helper: Check if point is inside block
bool isInside(Block b, int x, int y) {
    int left = b.center.x - b.w / 2;
    int top = b.center.y - b.h / 2;
    int right = b.center.x + b.w / 2;
    int bottom = b.center.y + b.h / 2;
    return (x >= left && x <= right && y >= top && y <= bottom);
}

// Helper to draw a Block
void drawBlock(Block b) {
    int left = b.center.x - b.w / 2;
    int top = b.center.y - b.h / 2;
    int right = b.center.x + b.w / 2;
    int bottom = b.center.y + b.h / 2;

    // Highlight selected block
    if (b.id == selectedBlockId) {
        setcolor(YELLOW);
        setlinestyle(SOLID_LINE, 0, 3);
    } else {
        setcolor(WHITE);
        setlinestyle(SOLID_LINE, 0, 1);
    }

    if (b.type == TYPE_START || b.type == TYPE_STOP) {
        fillellipse(b.center.x, b.center.y, b.w / 2, b.h / 2);
        outtextxy(b.center.x - textwidth(b.text) / 2, b.center.y - textheight(b.text) / 2, b.text);
    } else if (b.type == TYPE_DECISION) {
        // Simple Rhombus approximation
        int points[] = {
            b.center.x, top,
            right, b.center.y,
            b.center.x, bottom,
            left, b.center.y,
            b.center.x, top
        };
        drawpoly(5, points);
        outtextxy(b.center.x - textwidth(b.text) / 2, b.center.y - textheight(b.text) / 2, b.text);
    } else {
        rectangle(left, top, right, bottom);
        outtextxy(b.center.x - textwidth(b.text) / 2, b.center.y - textheight(b.text) / 2, b.text);
    }
}

int main() {
    initwindow(WINDOW_WIDTH, WINDOW_HEIGHT, "InterSchem");
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2);
    
    // Double Buffering Setup
    int page = 0;

    // Test Blocks Array
    const int MAX_TEST_BLOCKS = 3;
    Block blocks[MAX_TEST_BLOCKS];

    // Initialize blocks
    blocks[0].id = 0; blocks[0].type = TYPE_START; blocks[0].center = {600, 100}; blocks[0].w = 100; blocks[0].h = 50; strcpy(blocks[0].text, "START");
    blocks[1].id = 1; blocks[1].type = TYPE_OP;    blocks[1].center = {600, 300}; blocks[1].w = 150; blocks[1].h = 60; strcpy(blocks[1].text, "x = x + 1");
    blocks[2].id = 2; blocks[2].type = TYPE_DECISION; blocks[2].center = {600, 500}; blocks[2].w = 120; blocks[2].h = 80; strcpy(blocks[2].text, "x < 10");

    while (true) {
        setactivepage(page);
        setvisualpage(1 - page);
        cleardevice();

        // 1. INPUT HANDLING
        if (ismouseclick(WM_LBUTTONDOWN)) {
            int mx, my;
            getmouseclick(WM_LBUTTONDOWN, mx, my);
            
            // Check hit
            bool hit = false;
            for (int i = 0; i < MAX_TEST_BLOCKS; i++) {
                if (isInside(blocks[i], mx, my)) {
                    selectedBlockId = blocks[i].id;
                    isDragging = true;
                    dragOffset.x = mx - blocks[i].center.x;
                    dragOffset.y = my - blocks[i].center.y;
                    hit = true;
                    break;
                }
            }
            if (!hit) {
                selectedBlockId = -1;
                isDragging = false;
            }
        }

        if (isDragging && selectedBlockId != -1) {
            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                // Still holding
                int mx = mousex();
                int my = mousey();
                
                // Update position
                for (int i = 0; i < MAX_TEST_BLOCKS; i++) {
                    if (blocks[i].id == selectedBlockId) {
                        blocks[i].center.x = mx - dragOffset.x;
                        blocks[i].center.y = my - dragOffset.y;
                        break;
                    }
                }
            } else {
                // Released
                isDragging = false;
            }
        }

        // 2. DRAWING
        // Draw links first (behind blocks)
        setcolor(WHITE);
        setlinestyle(SOLID_LINE, 0, 1);
        line(blocks[0].center.x, blocks[0].center.y, blocks[1].center.x, blocks[1].center.y);
        line(blocks[1].center.x, blocks[1].center.y, blocks[2].center.x, blocks[2].center.y);

        // Draw blocks
        for (int i = 0; i < MAX_TEST_BLOCKS; i++) {
            drawBlock(blocks[i]);
        }
        
        // Debug Info
        setcolor(LIGHTGRAY);
        char debugMsg[50];
        sprintf(debugMsg, "Selected: %d  Dragging: %d", selectedBlockId, isDragging);
        outtextxy(10, 10, debugMsg);

        page = 1 - page;
        delay(10); // Smooth 60-ish FPS

        if (kbhit()) {
            if (getch() == 27) break; 
        }
    }

    closegraph();
    return 0;
}