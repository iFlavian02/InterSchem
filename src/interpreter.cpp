#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <graphics.h> // For highlighting
#include <iostream>
#include "../headers/interpreter.h"
#include "../headers/renderer.h" 

// --- Variable Management ---
Variable variables[MAX_VARS];
int varCount = 0;

float getVarValue(const char* name) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(variables[i].name, name) == 0) return variables[i].value;
    }
    return 0.0f; // Default 0
}

void setVarValue(const char* name, float val) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            variables[i].value = val;
            return;
        }
    }
    // Create new
    if (varCount < MAX_VARS) {
        strcpy(variables[varCount].name, name);
        variables[varCount].value = val;
        varCount++;
    }
}

// --- Parsing Helper ---
// Very simple parser: "LHS = RHS" or "LHS OP RHS"
// Supports: "a = 5", "a = b + 1", "a < 10"
// Assumption: Spaces between operators.

float parseExpression(char* expr) {
    // Check if it's a number
    char* end;
    float val = strtof(expr, &end);
    if (end != expr && *end == '\0') {
        return val;
    }
    // Check if it's a variable
    // Simple check: if it starts with alpha
    if ((expr[0] >= 'a' && expr[0] <= 'z') || (expr[0] >= 'A' && expr[0] <= 'Z')) {
         return getVarValue(expr);
    }
    return 0.0f;
}

// Evaluates "Operand1 Operator Operand2"
// example: "x + 1", "x", "5"
float evaluateRHS(char* rhs) {
    // 1. Try to split by operators +, -, *, /, %, <, >
    // This is a naive parser. 
    // We expect "A + B" or just "A"
    
    char op = 0;
    char* opPtr = NULL;
    
    if ((opPtr = strchr(rhs, '+'))) op = '+';
    else if ((opPtr = strchr(rhs, '-'))) op = '-';
    else if ((opPtr = strchr(rhs, '*'))) op = '*';
    else if ((opPtr = strchr(rhs, '/'))) op = '/';
    else if ((opPtr = strchr(rhs, '<'))) op = '<';
    else if ((opPtr = strchr(rhs, '>'))) op = '>';
    // == coverage?
    
    if (op) {
        *opPtr = '\0'; // Split string
        char* part1 = rhs;
        char* part2 = opPtr + 1;
        
        // Trim spaces (naive)
        while(*part1 == ' ') part1++;
        while(*part2 == ' ') part2++;
        // We assume valid input for now or handle basic errors
        
        float v1 = parseExpression(part1);
        float v2 = parseExpression(part2);
        
        // Restore string (optional, but good practice since we modify in place)
        *opPtr = op;
        
        switch(op) {
            case '+': return v1 + v2;
            case '-': return v1 - v2;
            case '*': return v1 * v2;
            case '/': return (v2 != 0) ? v1 / v2 : 0;
            case '<': return (v1 < v2) ? 1.0f : 0.0f;
            case '>': return (v1 > v2) ? 1.0f : 0.0f;
        }
    }
    
    // No operator -> Single value or variable
    return parseExpression(rhs);
}

void executeOp(const char* _text) {
    char text[100];
    strcpy(text, _text);
    
    // Format: "VAR = EXPRESSION"
    char* eqPos = strchr(text, '=');
    if (eqPos) {
        *eqPos = '\0';
        char* lhs = text;
        char* rhs = eqPos + 1;
        
        // Trim LHS
        // ... (Skipping robust trim for speed, assume "x = ...")
        // Remove trailing spaces from LHS
        int len = strlen(lhs);
        while(len > 0 && lhs[len-1] == ' ') lhs[--len] = '\0';
        
        float res = evaluateRHS(rhs);
        setVarValue(lhs, res);
        std::cout << "Executed: " << lhs << " = " << res << std::endl;
    }
}

bool evaluateDecision(const char* _text) {
    char text[100];
    strcpy(text, _text);
    // Format: "EXPRESSION" which evaluates to >0 for TRUE
    // Often "x < 10"
    
    float res = evaluateRHS(text);
    std::cout << "Decision: " << text << " -> " << res << std::endl;
    return (res != 0.0f);
}

// --- Main Loop ---

void runScheme(AppState* state) {
    std::cout << "--- STARTING INTERPRETER ---" << std::endl;
    varCount = 0; // Reset vars
    
    // Find Start
    int currentId = -1;
    for (int i = 0; i < state->blockCount; i++) {
        if (state->blocks[i].type == TYPE_START) {
            currentId = state->blocks[i].id;
            break;
        }
    }
    
    if (currentId == -1) {
        std::cout << "No START block found!" << std::endl;
        return;
    }
    
    // Execution Loop
    int safetyCounter = 0; // Prevent infinite loops freezing the UI
    const int MAX_STEPS = 1000;
    
    while (currentId != -1 && safetyCounter < MAX_STEPS) {
        // Find block object
        Block* b = NULL;
        for (int i = 0; i < state->blockCount; i++) {
            if (state->blocks[i].id == currentId) {
                b = &state->blocks[i];
                break;
            }
        }
        
        if (!b) break;
        
        // VISUAL FEEDBACK: Highlight
        drawBlock(*b, b->id); // Redraw as 'selected' style (since drawBlock uses selectedId)
                              // Wait? drawBlock normally highlights safely. 
                              // We can force a specific highlight color later if we want.
                              // For now, let's just use the selected style.
        setcolor(LIGHTGREEN);
        rectangle(b->center.x - b->w/2 - 2, b->center.y - b->h/2 - 2, b->center.x + b->w/2 + 2, b->center.y + b->h/2 + 2);
        delay(300); // Slow down to see flow
        
        // Logic
        if (b->type == TYPE_START) {
            currentId = b->nextId;
        } 
        else if (b->type == TYPE_OP) {
            executeOp(b->text);
            currentId = b->nextId;
        }
        else if (b->type == TYPE_DECISION) {
            if (evaluateDecision(b->text)) {
                currentId = b->nextId; // TRUE path
            } else {
                currentId = b->falseId; // FALSE path
            }
        }
        else if (b->type == TYPE_STOP) {
            std::cout << "--- STOP REACHED ---" << std::endl;
            break; 
        }
        
        safetyCounter++;
    }
    
    if (safetyCounter >= MAX_STEPS) std::cout << "Terminated: Max steps reached." << std::endl;
    
    // Print Final Vars
    std::cout << "Final Variables:" << std::endl;
    for(int i=0; i<varCount; i++) {
        std::cout << variables[i].name << " = " << variables[i].value << std::endl;
    }
}
