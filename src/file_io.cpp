#include <stdio.h>
#include <string.h>
#include "../headers/file_io.h"

void saveToFile(const char* filename, const AppState* state) {
    FILE* f = fopen(filename, "w");
    if (!f) return;

    fprintf(f, "%d\n", state->blockCount);
    for (int i = 0; i < state->blockCount; i++) {
        Block b = state->blocks[i];
        // Format: ID Type X Y W H Text NextId FalseId
        fprintf(f, "%d %d %d %d %d %d %s %d %d\n", 
            b.id, b.type, b.center.x, b.center.y, b.w, b.h, 
            strlen(b.text) > 0 ? b.text : "_", // Use placeholder if empty
            b.nextId, b.falseId);
    }
    fclose(f);
}

void loadFromFile(const char* filename, AppState* state) {
    FILE* f = fopen(filename, "r");
    if (!f) return;

    fscanf(f, "%d", &state->blockCount);
    for (int i = 0; i < state->blockCount; i++) {
        Block* b = &state->blocks[i];
        int typeInt;
        fscanf(f, "%d %d %d %d %d %d %s %d %d", 
            &b->id, &typeInt, &b->center.x, &b->center.y, &b->w, &b->h, 
            b->text, &b->nextId, &b->falseId);
        b->type = (BlockType)typeInt;
        
        if (strcmp(b->text, "_") == 0) {
            strcpy(b->text, "");
        }
    }
    fclose(f);
    
    // Reset interaction state
    state->selectedBlockId = -1;
    state->isDragging = false;
    state->linkSourceId = -1;
}
