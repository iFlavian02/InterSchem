#include "../headers/renderer.h"
#include <graphics.h>
#include <winbgim.h>
#include <stdio.h> // for sprintf

void drawBlock(Block b, int selectedId) {
    int left = b.center.x - b.w / 2;
    int top = b.center.y - b.h / 2;
    int right = b.center.x + b.w / 2;
    int bottom = b.center.y + b.h / 2;

    // Highlight selected block
    if (b.id == selectedId) {
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
        // Simple Rhomb approximation
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
        // Rectangle
        rectangle(left, top, right, bottom);
        outtextxy(b.center.x - textwidth(b.text) / 2, b.center.y - textheight(b.text) / 2, b.text);
    }
}

// Draws a 3-segment orthogonal link (Vertical -> Horizontal -> Vertical)
void drawLink(Point start, Point end) {
    setcolor(WHITE);
    setlinestyle(SOLID_LINE, 0, 1);

    // Midpoint Y for the horizontal segment
    int midY = (start.y + end.y) / 2;

    // 1. Vertical down from start
    line(start.x, start.y, start.x, midY);

    // 2. Horizontal across
    line(start.x, midY, end.x, midY);

    // 3. Vertical down to end
    line(end.x, midY, end.x, end.y);

}
