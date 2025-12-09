#ifndef APP_STATE_H
#define APP_STATE_H

#include "block.h"

#define MAX_BLOCKS 100

struct AppState {
    Block blocks[MAX_BLOCKS];
    int blockCount;
    
    // Interaction State
    int selectedBlockId;
    bool isDragging;
    Point dragOffset;
};

#endif // APP_STATE_H
