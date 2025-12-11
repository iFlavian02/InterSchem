#include <iostream>
#include <graphics.h>
#include <winbgim.h>
#include <vector>
#include "../headers/app_state.h"
#include "../headers/renderer.h"
#include "../headers/interaction.h"

#include "../headers/file_io.h"
#include "../headers/interpreter.h"

// Screen dimensions
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

// Toolbar dimensions match renderer.cpp
const int TOOLBAR_WIDTH = 150;
const int BTN_HEIGHT = 40;
const int BTN_MARGIN = 10;

int main() {
    initwindow(WINDOW_WIDTH, WINDOW_HEIGHT, "InterSchem");
    settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2);
    
    // Application State Initialization
    AppState appState;
    appState.blockCount = 0;
    appState.selectedBlockId = -1;
    appState.isDragging = false;
    appState.dragOffset = {0, 0};
    appState.currentMode = MODE_SELECT;
    appState.linkSourceId = -1;

    int page = 0;
    while (true) {
        setactivepage(page);
        setvisualpage(1 - page);
        cleardevice();

        // 1. INPUT HANDLING
        if (ismouseclick(WM_LBUTTONDOWN)) {
            int mx, my;
            getmouseclick(WM_LBUTTONDOWN, mx, my);
            
            // --- TOOLBAR INTERACTION ---
            if (mx < TOOLBAR_WIDTH) {
                // Determine which button was clicked
                int btnIndex = (my - BTN_MARGIN) / (BTN_HEIGHT + BTN_MARGIN);
                if (btnIndex >= 0 && btnIndex <= 9) {
                    ToolMode modes[] = {
                        MODE_SELECT, MODE_ADD_START, MODE_ADD_OP, MODE_ADD_DECISION, MODE_ADD_STOP, MODE_LINK, MODE_DELETE,
                        MODE_SAVE, MODE_LOAD, MODE_RUN
                    };
                    
                    if (modes[btnIndex] == MODE_SAVE) {
                        saveToFile("scheme.txt", &appState);
                        std::cout << "Saved to scheme.txt" << std::endl;
                    } 
                    else if (modes[btnIndex] == MODE_LOAD) {
                        loadFromFile("scheme.txt", &appState);
                        std::cout << "Loaded from scheme.txt" << std::endl;
                    } 
                    else if (modes[btnIndex] == MODE_RUN) {
                        // FORCE SAVE BEFORE RUN (Good practice)
                        saveToFile("scheme_autosave.txt", &appState);
                        runScheme(&appState);
                        // Reset to SELECT after run
                        appState.currentMode = MODE_SELECT;
                    }
                    else {
                        appState.currentMode = modes[btnIndex];
                    }

                    // Reset transient states
                    appState.isDragging = false;
                    appState.selectedBlockId = -1;
                    appState.linkSourceId = -1;
                }
            } 
            // --- CANVAS INTERACTION ---
            else {
                // Handling depends on Mode
                if (appState.currentMode == MODE_SELECT) {
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
                    }
                } 
                else if (appState.currentMode == MODE_LINK) {
                     // 1. Check if we clicked a source block
                     int clickedId = -1;
                     for(int i=0; i<appState.blockCount; i++) {
                         if(isInside(appState.blocks[i], mx, my)) {
                             clickedId = appState.blocks[i].id; 
                             break;
                         }
                     }

                     if (clickedId != -1) {
                         if (appState.linkSourceId == -1) {
                             // Select Source
                             appState.linkSourceId = clickedId;
                             appState.selectedBlockId = clickedId; // Highlight it
                         } else {
                             // Select Target -> Create Link
                             if (clickedId != appState.linkSourceId) {
                                 // Link Source to Target
                                 for(int i=0; i<appState.blockCount; i++) {
                                     if(appState.blocks[i].id == appState.linkSourceId) {
                                         // Update Connectivity
                                         // If DECISION block, ask user which branch? (TODO: Simply Default to Main for now, or toggle?)
                                         // For now, simpler logic: Decision connects 'nextId' first, 'falseId' second?
                                         // Or just update nextId always for simple blocks.
                                         
                                         if (appState.blocks[i].type == TYPE_DECISION) {
                                              // Simple heuristic: if nextId is taken, use falseId?
                                              // Or always Main for now.
                                              if (appState.blocks[i].nextId == -1) appState.blocks[i].nextId = clickedId;
                                              else appState.blocks[i].falseId = clickedId;
                                         } else {
                                             appState.blocks[i].nextId = clickedId;
                                         }
                                         break;
                                     }
                                 }
                                 // Reset Linking State
                                 appState.linkSourceId = -1;
                                 appState.selectedBlockId = -1;
                             }
                         }
                     } else {
                         // Clicked empty space -> Cancel
                         appState.linkSourceId = -1;
                         appState.selectedBlockId = -1;
                     }
                }
                else if (appState.currentMode == MODE_DELETE) {
                    for(int i=0; i<appState.blockCount; i++) {
                        if(isInside(appState.blocks[i], mx, my)) {
                            // Delete Block: Shift array
                            // Also need to clean up links pointing to it!
                            int deletedId = appState.blocks[i].id;
                            
                            // Remove references in other blocks
                             for(int k=0; k<appState.blockCount; k++) {
                                 if (appState.blocks[k].nextId == deletedId) appState.blocks[k].nextId = -1;
                                 if (appState.blocks[k].falseId == deletedId) appState.blocks[k].falseId = -1;
                             }

                            // Shift
                            for(int j=i; j<appState.blockCount-1; j++) {
                                appState.blocks[j] = appState.blocks[j+1];
                            }
                            appState.blockCount--;
                            appState.selectedBlockId = -1;
                            break;
                        }
                    }
                }
                else {
                    // ADD MODES
                    if (appState.blockCount < MAX_BLOCKS) {
                        Block& b = appState.blocks[appState.blockCount];
                        // Auto-assign ID? Or use Index? ID is safer if we delete.
                        // Ideally find max ID + 1. For now, use a counter or just random.
                        // Simple: max existing ID + 1.
                        int maxId = -1;
                        for(int k=0; k<appState.blockCount; k++) if(appState.blocks[k].id > maxId) maxId = appState.blocks[k].id;
                        b.id = maxId + 1;
                        
                        b.center = {mx, my};
                        b.nextId = -1;
                        b.falseId = -1;
                        
                        if (appState.currentMode == MODE_ADD_START) {
                            b.type = TYPE_START; strcpy(b.text, "START"); b.w = 100; b.h = 50;
                        } else if (appState.currentMode == MODE_ADD_STOP) {
                            b.type = TYPE_STOP; strcpy(b.text, "STOP"); b.w = 100; b.h = 50;
                        } else if (appState.currentMode == MODE_ADD_OP) {
                            b.type = TYPE_OP; strcpy(b.text, "Process"); b.w = 150; b.h = 60;
                        } else if (appState.currentMode == MODE_ADD_DECISION) {
                            b.type = TYPE_DECISION; strcpy(b.text, "?"); b.w = 120; b.h = 80;
                        }
                        
                        appState.blockCount++;
                        // Switch back to SELECT after adding? Or keep adding? Keep adding is standard.
                    }
                }
            }
        }
        
        // --- DRAGGING (Only in Select Mode) ---
        if (appState.currentMode == MODE_SELECT && appState.isDragging && appState.selectedBlockId != -1) {
            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                int mx = mousex();
                int my = mousey();
                // Ensure we don't drag into Toolbar
                if(mx < TOOLBAR_WIDTH + 50) mx = TOOLBAR_WIDTH + 50;

                for (int i = 0; i < appState.blockCount; i++) {
                    if (appState.blocks[i].id == appState.selectedBlockId) {
                        appState.blocks[i].center.x = mx - appState.dragOffset.x;
                        appState.blocks[i].center.y = my - appState.dragOffset.y;
                        break;
                    }
                }
            } else {
                appState.isDragging = false;
            }
        }

        // 2. DRAWING
        drawToolbar(appState, TOOLBAR_WIDTH, WINDOW_HEIGHT);

        // Draw links
        for (int i = 0; i < appState.blockCount; i++) {
            if (appState.blocks[i].nextId != -1) {
                for(int j = 0; j < appState.blockCount; j++) {
                    if (appState.blocks[j].id == appState.blocks[i].nextId) {
                        drawLink(appState.blocks[i].center, appState.blocks[j].center, appState.blocks, appState.blockCount);
                        break;
                    }
                }
            }
            if (appState.blocks[i].type == TYPE_DECISION && appState.blocks[i].falseId != -1) {
                 for(int j = 0; j < appState.blockCount; j++) {
                    if (appState.blocks[j].id == appState.blocks[i].falseId) {
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
        
        // Text Edit (Double Click Placeholder)
        // WinBGIm doesn't have native double click event easily suitable here without timing checks.
        // We can just add a button "EDIT TEXT" or use Right Click?
        // Let's implement Right Click -> Edit Text for now as it's easier than robust double-click.
        if (ismouseclick(WM_RBUTTONDOWN)) {
             int mx, my;
             getmouseclick(WM_RBUTTONDOWN, mx, my);
             for (int i = 0; i < appState.blockCount; i++) {
                if (isInside(appState.blocks[i], mx, my)) {
                    // Primitive Input Dialog
                    // Unfortunately inputbox is not standard WinBGIm. 
                    // We might need to implement a simple one or use console std::cin if we can bring it to foreground?
                    // Or use a graphical simple input loop.
                    // For now, let's just log "Edit requested".
                    std::cout << "Edit requested for block " << appState.blocks[i].id << std::endl;
                    // TODO: Implement graphical text input
                    break;
                }
            }
        }
        
        // Debug Info
        setcolor(LIGHTGRAY);
        setbkcolor(BLACK); // Ensure text background is black
        char debugMsg[100];
        sprintf(debugMsg, "Mode: %d Blocks: %d Sel: %d", appState.currentMode, appState.blockCount, appState.selectedBlockId);
        outtextxy(TOOLBAR_WIDTH + 10, 10, debugMsg);

        page = 1 - page;
        delay(10); 

        if (kbhit()) {
            if (getch() == 27) break; 
        }
    }

    closegraph();
    return 0;
}
