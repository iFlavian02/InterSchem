#ifndef BLOCK_H
#define BLOCK_H

#include "geometry.h"

#define MAX_TEXT 100

enum BlockType {
    TYPE_START,
    TYPE_STOP,
    TYPE_IO,       // Read/Write
    TYPE_DECISION, // Rhombus
    TYPE_OP        // Assignment/Process
};

struct Block {
    int id;
    BlockType type;
    Point center;
    int w, h;
    char text[MAX_TEXT];
    
    // Connectivity
    int nextId;       // Main path (or only path for simple blocks)
    int falseId;      // For Decision blocks (False branch)
};

#endif // BLOCK_H
