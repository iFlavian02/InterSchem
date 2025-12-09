#include "../headers/interaction.h"

bool isInside(Block b, int x, int y) {
    int left = b.center.x - b.w / 2;
    int top = b.center.y - b.h / 2;
    int right = b.center.x + b.w / 2;
    int bottom = b.center.y + b.h / 2;
    return (x >= left && x <= right && y >= top && y <= bottom);
}
