#include "../headers/interaction.h"
#include <graphics.h>
#include <winbgim.h>
#include <stdio.h>
#include <string.h>

bool isInside(Block b, int x, int y) {
    int left = b.center.x - b.w / 2;
    int top = b.center.y - b.h / 2;
    int right = b.center.x + b.w / 2;
    int bottom = b.center.y + b.h / 2;
    return (x >= left && x <= right && y >= top && y <= bottom);
}

void inputText(int x, int y, char* buffer, int maxLen) {
    int len = strlen(buffer);
    
    // Input loop
    while(true) {
        // Draw Input Box (Overlay)
        int boxW = 300;
        int boxH = 50;
        int left = x - boxW/2;
        int top = y - boxH/2;
        int right = x + boxW/2;
        int bottom = y + boxH/2;

        setfillstyle(SOLID_FILL, DARKGRAY);
        bar(left, top, right, bottom);
        setcolor(WHITE);
        rectangle(left, top, right, bottom);
        
        char display[150];
        sprintf(display, "%s_", buffer); 
        outtextxy(left + 10, top + 15, display);
        
        outtextxy(left, bottom + 5, "Enter to Confirm");

        // Force Visual Page Update
        int p = getactivepage();
        setvisualpage(p); 
        
        if (kbhit()) {
            char c = getch();
            if (c == 13) break; // Enter
            else if (c == 8) { // Backspace
                if (len > 0) buffer[--len] = '\0';
            }
            else if (c >= 32 && c <= 126) { // Printable
                if (len < maxLen - 1) {
                    buffer[len++] = c;
                    buffer[len] = '\0';
                }
            }
        }
        delay(30);
    }
    // Clean up or let next redraw handle it
}
