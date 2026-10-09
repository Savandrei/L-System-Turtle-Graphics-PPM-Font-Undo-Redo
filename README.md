# Runic – Rune Generator and Image Manipulation Engine

**Author:** Savlovschi Andrei-Bogdan  
**Course:** Computer Programming and Programming Languages (PCLP)

---

## Overview

**Runic** is an interactive command-line application built in C designed to generate, manipulate, and engrave fractal runes onto digital images. The program combines linguistic generation models (L-systems), vector-based turtle graphics rasterization, bitmap font typography rendering, and low-level binary signal analysis.

### Core Capabilities
- **L-System Processing:** Parses custom `.lsys` grammars and derives symbols iteratively.
- **Turtle Graphics Vector Rendering:** Translates grammar strings into continuous line drawings using Bresenham's line algorithm.
- **Bitmap Font Engine:** Loads and decodes Adobe Glyph Bitmap Distribution Format (`.bdf`) fonts to render text onto RGB canvases.
- **PPM Image Processing:** Reads and exports binary Netpbm color images (P6 format).
- **Signal Analysis (`BITCHECK`):** Inspects raw pixel bitstreams to detect potential corruption patterns (`0010` and `1101`).
- **Comprehensive Undo / Redo:** Full state preservation across commands via stack-driven state management.

---

## Architecture & Data Structures

The system relies on dynamically allocated memory and custom structures tailored for graphics and state tracking:

1. **`LSYSTEM`**
   - Stores the active Lindenmayer grammar:
     - `axiom`: Base starting string.
     - `nr_rules`: Number of replacement rules.
     - `symbols` & `succesor`: Rule lookup table for character expansion.
     - `file_location`: Source path for reload/undo operations.

2. **`PPM`**
   - Represents the 24-bit canvas:
     - `width`, `height`: Canvas dimensions.
     - `pixels`: Dynamically allocated 2D integer array where each cell packs 24 bits of color data:
       $$\text{Color} = R \cdot 256^2 + G \cdot 256 + B$$

3. **`TURTLE_STATE`**
   - Manages the turtle graphics stack used by the branch operators `[` and `]`:
     - Stores Cartesian position $(x, y)$, angle orientation $\theta$, and stack utilization flags.

4. **`FONT`**
   - Manages Adobe BDF typography resources:
     - `encoding`: ASCII character index mapping.
     - `dwx`, `dwy`: Device width cursor offsets.
     - `bbw`, `bbh`, `bbxoff`, `bbyoff`: Per-glyph bounding box dimensions and placement offsets.
     - `bitmap`: 2D array of packed byte buffers representing individual font glyphs.

5. **State Stacks (`*_file_location_stack` & `last_cmd`)**
   - Tracks operational history:
     - Maintains dedicated file-path stacks for lightweight reloading of `LSYSTEM` and `FONT`.
     - Maintains deep-copied image matrices in `ppm_file_location_stack` for graphical changes.

---

## Supported Commands

The interactive shell reads commands from standard input (`stdin`) line by line until terminated.

| Command | Syntax | Description |
|---|---|---|
| **`LSYSTEM`** | `LSYSTEM <filepath>` | Loads an L-system grammar file (`.lsys`). |
| **`DERIVE`** | `DERIVE <order>` | Computes and prints the $n$-th derivation of the loaded L-system to stdout. |
| **`LOAD`** | `LOAD <filepath.ppm>` | Reads and loads a binary P6 format PPM image. |
| **`SAVE`** | `SAVE <filepath.ppm>` | Serializes the active image state to disk as a binary PPM file. |
| **`TURTLE`** | `TURTLE <x> <y> <step> <angle> <delta> <n> <R> <G> <B>` | Renders the $n$-th derivation of the L-system using turtle graphics starting at $(x, y)$. |
| **`FONT`** | `FONT <filepath.bdf>` | Loads and parses an Adobe BDF bitmap font. |
| **`TYPE`** | `TYPE "<text>" <x> <y> <R> <G> <B>` | Draws text onto the canvas at $(x, y)$ using the loaded font. |
| **`BITCHECK`** | `BITCHECK` | Scans pixel bit sequences for hardware signal read errors (`0010` and `1101`). |
| **`UNDO`** | `UNDO` | Reverts the last state-modifying action. |
| **`REDO`** | `REDO` | Re-executes the most recently undone action. |
| **`EXIT`** | `EXIT` | Frees all dynamic memory allocations and terminates the process. |

---

## Algorithms

### 1. Turtle Graphics & Bresenham Line Drawing
Turtle movements follow standard logo-style instructions:
- `F`: Advances the turtle forward by step size $d$, drawing a discrete segment to $(x_1, y_1)$ via Bresenham's line algorithm.
- `+` / `-`: Alters orientation angle $\theta$ by $\pm \delta$.
- `[` / `]`: Pushes/pops the current turtle state to/from the `TURTLE_STATE` stack.

Bresenham's integer-based rasterizer (`draw_line`) maps floating-point coordinates to integer grid coordinates using `round()`, automatically clipping out-of-bounds pixels.

### 2. Glyph Parsing and Rendering
- The `.bdf` parser extracts the ASCII encoding, bounding box properties (`BBX`), and hexadecimal raster bytes per glyph.
- The `TYPE` routine renders each glyph by positioning its baseline relative to $(x + \text{BBxoff}, y - \text{BByoff} - \text{BBh} + 1)$ and unpacking each bit from MSB to LSB.
- Cursor positions advance by $(\text{dwx}, \text{dwy})$ after each character.

### 3. Bitcheck Signal Analysis
- Pixels are analyzed sequentially in transmission order (top-to-bottom, left-to-right, RGB order).
- Bit streams are examined in 4-bit sliding windows (`halfbyte`).
- Bit sequences `0010` (may degrade to `0000`) and `1101` (may degrade to `1111`) trigger warnings detailing the affected pixel coordinate and its corrupted RGB values.
- Inter-pixel bit boundaries are maintained using a running 3-bit remainder (`last_color_remainder`).

---

## Build and Execution

### Prerequisites
- GCC Compiler supporting C99 standard (`-std=c99`)
- POSIX make utility
- Standard C math library (`-lm`)

### Compilation

Build the executable:
```bash
make build
```
This generates the `runic` binary with `-Wall -Wextra -std=c99` flags enabled.

Clean build artifacts:
```bash
make clean
```

Archive source files for distribution:
```bash
make pack
```

### Running the Application

Launch the interactive binary:
```bash
./runic
```

#### Example Session:
```text
LOAD canvas.ppm
LSYSTEM branch.lsys
TURTLE 200 50 25 90 30 3 0 255 0
FONT gothic.bdf
TYPE "Runic Path" 100 350 255 255 255
BITCHECK
SAVE output.ppm
EXIT
```
