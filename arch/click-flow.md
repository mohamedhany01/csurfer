# Click & Navigation Flow

This diagram shows how a mouse click in SDL is mapped back to the DOM tree for hyperlink navigation.

```mermaid
sequenceDiagram
    participant SDL as SDL Event Loop
    participant B as Browser
    participant T as LayoutTree
    participant obj as LayoutObject
    participant N as DOM Node
    
    SDL->>B: SDL_MOUSEBUTTONDOWN (x, y)
    note right of B: add scroll offset
    B->>B: click(x, y + scroll)
    
    B->>T: tree_to_list(document_)
    T-->>B: vector~const LayoutObject*~
    
    note over B: Iterate list backwards<br>to find deepest match
    loop find intersected object
        B->>obj: check (rect intersects x, y)
    end
    
    opt if match found
        B->>obj: node()
        obj-->>B: const Lexeme* (origin node)
        
        note over B: DOM Walk
        loop climb DOM tree
            B->>N: parent()
            N-->>B: element
            note over B: check if tag == "a"<br>and has "href"
        end
        
        opt if anchor found
            B->>B: load(resolved_url)
            note over B: push current to history
            note over B: new request, parse, layout
        end
    end
```
