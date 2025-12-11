#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../headers/codegen.h"

// Helper to check if a string is a variable name (starts with alpha)
bool isVar(const char* token) {
    if (!token || !token[0]) return false;
    return (isalpha(token[0]));
}

// Scans all blocks to find potential variables (LHS of assignments)
// and writes "float var1, var2, ...;" to the file.
void writeVariables(FILE* f, AppState* state) {
    char vars[100][32];
    int varCount = 0;
    
    for (int i = 0; i < state->blockCount; i++) {
        Block b = state->blocks[i];
        if (b.type == TYPE_OP) {
            // Check for "VAR =" pattern
            char text[100];
            strcpy(text, b.text);
            char* eq = strchr(text, '=');
            if (eq) {
                *eq = '\0';
                // Trim
                char* varName = text;
                while(*varName == ' ') varName++;
                int len = strlen(varName);
                while(len > 0 && varName[len-1] == ' ') varName[--len] = '\0';
                
                // Add if new
                bool exists = false;
                for(int j=0; j<varCount; j++) {
                    if (strcmp(vars[j], varName) == 0) {
                        exists = true; break;
                    }
                }
                if (!exists && isVar(varName)) {
                    strcpy(vars[varCount++], varName);
                }
            }
        }
    }
    
    if (varCount > 0) {
        fprintf(f, "    // Variables\n    float ");
        for(int i=0; i<varCount; i++) {
            fprintf(f, "%s%s", vars[i], (i < varCount - 1) ? ", " : ";\n\n");
        }
    }
}

void generateCode(const char* filename, AppState* state) {
    FILE* f = fopen(filename, "w");
    if (!f) return;
    
    // Header
    fprintf(f, "#include <iostream>\n\n");
    fprintf(f, "int main() {\n");
    
    // Variables
    writeVariables(f, state);
    
    // Logic
    // Strategy: Map every block to a label "block_ID:". 
    // START block jumps to its target. 
    // STOP returns 0.
    
    // 1. Find Start
    int startId = -1;
    for(int i=0; i<state->blockCount; i++) {
        if(state->blocks[i].type == TYPE_START) {
            startId = state->blocks[i].id;
            break;
        }
    }
    
    if (startId != -1) {
        fprintf(f, "    goto block_%d;\n\n", startId);
    }
    
    // 2. Write blocks
    for(int i=0; i<state->blockCount; i++) {
        Block b = state->blocks[i];
        fprintf(f, "    block_%d:\n", b.id);
        
        switch(b.type) {
            case TYPE_START:
                if (b.nextId != -1) fprintf(f, "        goto block_%d;\n", b.nextId);
                else fprintf(f, "        return 0;\n");
                break;
                
            case TYPE_STOP:
                fprintf(f, "        std::cout << \"Program Ended\" << std::endl;\n");
                // Print all variables here for debugging context?
                // Or just exit.
                fprintf(f, "        return 0;\n");
                break;
                
            case TYPE_OP:
                fprintf(f, "        %s;\n", b.text);
                fprintf(f, "        std::cout << \"Executed: %s \" << std::endl;\n", b.text); // Debug trace
                if (b.nextId != -1) fprintf(f, "        goto block_%d;\n", b.nextId);
                else fprintf(f, "        return 0;\n");
                break;
                
            case TYPE_DECISION:
                fprintf(f, "        if (%s) {\n", b.text);
                if (b.nextId != -1) fprintf(f, "            goto block_%d;\n", b.nextId);
                else fprintf(f, "            return 0;\n");
                fprintf(f, "        } else {\n");
                if (b.falseId != -1) fprintf(f, "            goto block_%d;\n", b.falseId);
                else fprintf(f, "            return 0;\n");
                fprintf(f, "        }\n");
                break;
                
            default: break;
        }
        fprintf(f, "\n");
    }
    
    fprintf(f, "    return 0;\n");
    fprintf(f, "}\n");
    
    fclose(f);
}
