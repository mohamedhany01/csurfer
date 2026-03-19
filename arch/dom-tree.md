# DOM Tree Node Hierarchy

This document describes the base classes and structures used for the Document Object Model (DOM) in CSurfer.

## Class Diagram

```mermaid
classDiagram
    class Lexeme {
        <<abstract>>
        +LexemeType type()*
        +string get_string()*
        +bool is_focused()
        +void set_focused(bool)
        #bool is_focused_
    }

    class Element {
        +string tag_
        +AttributeMap attributes_
        +StyleMap style_
        +vector~Lexeme~ children_
        +get_string() override
    }

    class Text {
        +string value_
        +get_string() override
    }

    Lexeme <|-- Element
    Lexeme <|-- Text
    Element o-- Lexeme : children
```

## Focus System
As of Chapter 8, every `Lexeme` (both `Element` and `Text`) carries an `is_focused_` flag. 
- **Purpose**: To track which element should receive keyboard input and visual highlights (like a caret).
- **Logging**: The `set_focused` method in `Lexeme.cpp` logs focus changes to the console for tracking during development.
