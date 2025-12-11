# InterSchem

**InterSchem** is a visual flowchart editor and interpreter built with C++ and the WinBGIm graphics library. It allows users to create logical schemes (flowcharts), execute them in real-time, and export the logic to standalone C++ source code.

---

## Features

- **Visual Block Editor**: Create and connect blocks (Start, Stop, Operation, Decision) using a simple toolbar interface.
- **Drag & Drop**: Move blocks freely on the canvas.
- **Manual Linking**: Connect blocks by clicking source and target.
- **Text Editing**: Right-click any block to edit its content (e.g., `x = x + 1`, `x < 10`).
- **Save/Load**: Persist your schemes to `scheme.txt` and reload them later.
- **Interpreter**: Run your flowchart directly inside the app and watch it execute step-by-step with visual feedback.
- **C++ Code Generator**: Export your flowchart to a valid C++ file (`generated.cpp`) that replicates the logic using `goto` labels.

---

## How to Use

### Controls

| Action | How |
|--------|-----|
| **Add Block** | Click a block type in the toolbar (START, OPERATION, DECISION, STOP), then click on the canvas. |
| **Move Block** | Select "SELECT" mode, click and drag a block. |
| **Link Blocks** | Select "LINK" mode, click the source block, then click the target block. For Decision blocks, the first link is TRUE, the second is FALSE. |
| **Edit Block Text** | Right-click on a block to open the text input dialog. Press Enter to confirm. |
| **Delete Block** | Select "DELETE" mode, then click a block. |
| **Save** | Click "SAVE" to write to `scheme.txt`. |
| **Load** | Click "LOAD" to read from `scheme.txt`. |
| **Run** | Click "RUN" to execute the scheme. Watch the console for output. |
| **Export C++** | Click "C++" to generate `generated.cpp`. |
| **Exit** | Press `Esc` to close the application. |

### Expression Syntax

- **Operations**: `x = 5`, `x = x + 1`, `a = b * 2`
- **Decisions**: `x < 10`, `a > b`, `count > 0`
- Spaces are allowed: `x = x + 1` works the same as `x=x+1`.

---

## Building from Source

### Prerequisites

- **CMake** (3.10 or newer)
- **MinGW-w64** (with GCC and G++)
- **Git** (optional, for cloning)

### Steps

1.  **Clone the repository**:
    ```bash
    git clone https://github.com/iFlavian02/InterSchem.git
    cd InterSchem
    ```

2.  **Create the build directory** (if it doesn't exist):
    ```bash
    mkdir builds
    cd builds
    ```

3.  **Run CMake to configure**:
    ```bash
    cmake .. -G "MinGW Makefiles"
    ```

4.  **Build the project**:
    ```bash
    cmake --build .
    ```

5.  **Run the executable**:
    ```bash
    ./InterSchem.exe
    ```
    Or from the project root:
    ```bash
    .\builds\InterSchem.exe
    ```

---

## Project Structure

```
InterSchem/
├── headers/
│   ├── app_state.h       # Application state and ToolMode enum
│   ├── block.h           # Block and BlockType definitions
│   ├── codegen.h         # C++ code generation header
│   ├── file_io.h         # Save/Load functions
│   ├── geometry.h        # Point struct
│   ├── interaction.h     # Hit-testing and input dialog
│   ├── interpreter.h     # Scheme execution
│   ├── renderer.h        # Drawing functions
│   └── Winbgim/          # WinBGIm graphics library (bundled)
├── src/
│   ├── main.cpp          # Entry point and main loop
│   ├── renderer.cpp      # Block and link drawing
│   ├── interaction.cpp   # Mouse input handling
│   ├── interpreter.cpp   # Scheme execution logic
│   ├── file_io.cpp       # File I/O implementation
│   └── codegen.cpp       # C++ code generation
├── builds/               # Build output directory
├── CMakeLists.txt        # CMake configuration
└── README.md             # This file
```

---

## License

This project is for educational purposes as part of an "Introduction to Programming" course.

---

## Author

Developed by [Ilie Flavian](https://github.com/iFlavian02).
