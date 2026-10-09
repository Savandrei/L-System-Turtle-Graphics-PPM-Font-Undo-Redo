# L-System, Turtle Graphics, PPM, Font Rendering & Undo/Redo

A programming project exploring **L-systems, turtle graphics, image generation, font rendering, and undo/redo functionality**.

The project brings together several concepts in computer science and graphics programming, demonstrating how algorithms and data structures can be used to generate visual patterns, render text, and manage reversible operations.

## Overview

The project explores multiple areas of graphics programming:

- **L-Systems:** Generate complex patterns through recursive string rewriting.
- **Turtle Graphics:** Interpret instructions to draw lines and geometric shapes.
- **PPM Image Generation:** Represent and export images using the Portable Pixmap format.
- **Font Rendering:** Represent and draw characters using a custom rendering approach.
- **Undo/Redo:** Reverse previous operations and restore undone changes.

Together, these concepts provide an opportunity to explore procedural graphics, text rendering, image representation, and operation history management.

## Features

### L-Systems

L-systems, also known as Lindenmayer systems, are formal grammars that generate strings by repeatedly applying production rules.

An L-system consists of:
- **Axiom:** The initial string.
- **Production rules:** Rules that replace symbols with other strings.
- **Iterations:** The number of times the rules are applied.

For example, consider the following system:

```
Axiom: F
Rule:  F -> F+F-F
```

Applying the rule repeatedly generates increasingly complex strings that can be interpreted as drawing instructions.

L-systems are commonly used to generate fractals, geometric patterns, and plant-like structures.

### Turtle Graphics

Turtle graphics uses a virtual cursor, or turtle, that moves according to a sequence of instructions.

Depending on the instruction set, commands can control movement, direction, and drawing.

When combined with L-systems, turtle graphics turns generated strings into visual patterns.

This approach separates pattern generation from pattern rendering, making it possible to describe complex drawings using relatively simple rules.

### PPM Image Generation

PPM (Portable Pixmap) is a simple image format that represents images as pixel data.

Working with PPM images provides an opportunity to explore:
- Pixel-based image representation.
- RGB color values.
- Image dimensions and memory layout.
- Writing image data to files.
- Converting drawing operations into raster images.

Its straightforward structure makes PPM useful for learning the fundamentals of computer graphics without relying on complex image libraries.

### Font Rendering

Font rendering involves translating character representations into visible shapes.

Implementing a font renderer provides insight into:
- Character representation.
- Mapping characters to graphical patterns.
- Drawing text onto an image.
- Integrating text rendering with graphics operations.

### Undo/Redo

Undo/redo functionality allows previous operations to be reversed and subsequently restored.

This functionality introduces concepts related to operation history and state management.

A typical implementation maintains a history of operations or states so that changes can be reversed and reapplied.

Undo/redo mechanisms are useful in graphics editors, drawing applications, text editors, and other interactive software.


## Repository

[L-System-Turtle-Graphics-PPM-Font-Undo-Redo](https://github.com/Savandrei/L-System-Turtle-Graphics-PPM-Font-Undo-Redo)
