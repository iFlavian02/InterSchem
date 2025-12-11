#include <iostream>
#include <graphics.h>
#include <winbgim.h>
#include "../headers/app_state.h"
#include "../headers/renderer.h"
#include "../headers/interaction.h"

// Screen dimensions
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

int main() {
    initwindow(WINDOW_WIDTH, WINDOW_HEIGHT, "InterSchem");
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2);
    
    // Application State Initialization
    AppState appState;
    appState.blockCount = 3;
    appState.selectedBlockId = -1;
    appState.isDragging = false;
    appState.dragOffset = {0, 0};

    // Initialize test blocks
    appState.blocks[0].id = 0; 
    appState.blocks[0].type = TYPE_START; 
    appState.blocks[0].center = {600, 100}; 
    appState.blocks[0].w = 100; 
    appState.blocks[0].h = 50; 
    strcpy(appState.blocks[0].text, "START");

    appState.blocks[0].nextId = 1; 
    appState.blocks[0].falseId = -1;

    appState.blocks[1].id = 1; 
    appState.blocks[1].type = TYPE_OP;    
    appState.blocks[1].center = {600, 300}; 
    appState.blocks[1].w = 150; 
    appState.blocks[1].h = 60; 
    strcpy(appState.blocks[1].text, "x = x + 1");
    appState.blocks[1].nextId = 2;
    appState.blocks[1].falseId = -1;

    appState.blocks[2].id = 2; 
    appState.blocks[2].type = TYPE_DECISION; 
    appState.blocks[2].center = {600, 500}; 
    appState.blocks[2].w = 120; 
    appState.blocks[2].h = 80; 
    strcpy(appState.blocks[2].text, "x < 10");
    appState.blocks[2].nextId = -1;
    appState.blocks[2].falseId = -1;

    int page = 0;
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
            for (int i = 0; i < appState.blockCount; i++) {
                if (isInside(appState.blocks[i], mx, my)) {
                    appState.selectedBlockId = appState.blocks[i].id;
                    appState.isDragging = true;
                    appState.dragOffset.x = mx - appState.blocks[i].center.x;
                    appState.dragOffset.y = my - appState.blocks[i].center.y;
                    hit = true;
                    break;
                }
            }
            if (!hit) {
                appState.selectedBlockId = -1;
                appState.isDragging = false;
            }
        }

        if (appState.isDragging && appState.selectedBlockId != -1) {
            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                // Still holding
                int mx = mousex();
                int my = mousey();
                
                // Update position
                for (int i = 0; i < appState.blockCount; i++) {
                    if (appState.blocks[i].id == appState.selectedBlockId) {
                        appState.blocks[i].center.x = mx - appState.dragOffset.x;
                        appState.blocks[i].center.y = my - appState.dragOffset.y;
                        break;
                    }
                }
            } else {
                // Released
                appState.isDragging = false;
            }
        }

        // 2. DRAWING
        // Draw links
        for (int i = 0; i < appState.blockCount; i++) {
            // Check Main Connection
            if (appState.blocks[i].nextId != -1) {
                // Find target
                for(int j = 0; j < appState.blockCount; j++) {
                    if (appState.blocks[j].id == appState.blocks[i].nextId) {
                        drawLink(appState.blocks[i].center, appState.blocks[j].center, appState.blocks, appState.blockCount);
                        break;
                    }
                }
            }
            // Check False Connection (for Decision)
            if (appState.blocks[i].type == TYPE_DECISION && appState.blocks[i].falseId != -1) {
                 for(int j = 0; j < appState.blockCount; j++) {
                    if (appState.blocks[j].id == appState.blocks[i].falseId) {
                        // Draw with maybe a different color or style? Or just standard link
                        drawLink(appState.blocks[i].center, appState.blocks[j].center, appState.blocks, appState.blockCount);
                        break;
                    }
                }
            }
        }

        // Draw blocks
        for (int i = 0; i < appState.blockCount; i++) {
            drawBlock(appState.blocks[i], appState.selectedBlockId);
        }
        
        // Debug Info
        setcolor(LIGHTGRAY);
        char debugMsg[50];
        sprintf(debugMsg, "Selected: %d  Dragging: %d", appState.selectedBlockId, appState.isDragging);
        outtextxy(10, 10, debugMsg);

        page = 1 - page;
        delay(10); 

        if (kbhit()) {
            if (getch() == 27) break; 
        }
    }

    closegraph();
    return 0;
}
