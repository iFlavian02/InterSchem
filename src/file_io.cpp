#include <stdio.h>
#include <string.h>
#include "../headers/file_io.h"

void saveToFile(const char* filename, const AppState* state) {
    FILE* f = fopen(filename, "w");
    if (!f) return;

    fprintf(f, "%d\n", state->blockCount);
    for (int i = 0; i < state->blockCount; i++) {
        Block b = state->blocks[i];
        // Format: ID Type X Y W H NextId FalseId "Text"
        // Text is last and quoted to handle spaces
        fprintf(f, "%d %d %d %d %d %d %d %d \"%s\"\n", 
            b.id, b.type, b.center.x, b.center.y, b.w, b.h, 
            b.nextId, b.falseId, b.text);
    }
    fclose(f);
}

void loadFromFile(const char* filename, AppState* state) {
    FILE* f = fopen(filename, "r");
    if (!f) return;

    fscanf(f, "%d", &state->blockCount);
    fgetc(f); // Consume newline
    
    for (int i = 0; i < state->blockCount; i++) {
        Block* b = &state->blocks[i];
        int typeInt;
        
        // Read fixed fields
        fscanf(f, "%d %d %d %d %d %d %d %d", 
            &b->id, &typeInt, &b->center.x, &b->center.y, &b->w, &b->h, 
            &b->nextId, &b->falseId);
        b->type = (BlockType)typeInt;
        
        // Skip to opening quote
        int c;
        while ((c = fgetc(f)) != EOF && c != '"');
        
        // Read text until closing quote
        int idx = 0;
        while ((c = fgetc(f)) != EOF && c != '"' && idx < MAX_TEXT - 1) {
            b->text[idx++] = (char)c;
        }
        b->text[idx] = '\0';
        
        // Consume rest of line
        while ((c = fgetc(f)) != EOF && c != '\n');
    }
    fclose(f);
    
    // Reset interaction state
    state->selectedBlockId = -1;
    state->isDragging = false;
    state->linkSourceId = -1;
}

