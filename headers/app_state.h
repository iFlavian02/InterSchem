#ifndef APP_STATE_H
#define APP_STATE_H

#include "block.h"

#define MAX_BLOCKS 100

enum ToolMode {
    MODE_SELECT,
    MODE_ADD_START,
    MODE_ADD_OP,
    MODE_ADD_DECISION,
    MODE_ADD_STOP,
    MODE_LINK,
    MODE_DELETE
};

struct AppState {
    Block blocks[MAX_BLOCKS];
    int blockCount;
    
    // Interaction State
    ToolMode currentMode; // Current active tool
    int selectedBlockId;
    bool isDragging;
    Point dragOffset;
    
    // Linking State
    int linkSourceId; // ID of the first block clicked when linking
};

#endif // APP_STATE_H
