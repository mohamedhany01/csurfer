# Layout Tree Class Hierarchy

This diagram shows the class hierarchy of the layout tree objects after introducing `LineLayout` and `TextLayout`.

```mermaid
classDiagram
    class LayoutObject {
        <<abstract>>
        +int x, y, width, height
        +vector~unique_ptr~LayoutObject~~ children_
        +layout()*
        +paint(out)*
        +const Lexeme* node() virtual
    }

    class DocumentLayout {
        +layout()
        +paint()
    }

    class BlockLayout {
        +layout()
        +paint()
        -new_line()
        -word()
    }

    class LineLayout {
        +layout()
        +paint()
    }

    class TextLayout {
        +const Lexeme* node_
        +string word_
        +layout()
        +paint()
        +const Lexeme* node() override
    }

    LayoutObject <|-- DocumentLayout
    LayoutObject <|-- BlockLayout
    LayoutObject <|-- LineLayout
    LayoutObject <|-- TextLayout

    DocumentLayout o-- BlockLayout : root_
    BlockLayout o-- BlockLayout : block mode
    BlockLayout o-- LineLayout : inline mode
    LineLayout o-- TextLayout : children
```
