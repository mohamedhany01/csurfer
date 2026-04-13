# Security Subsystem Internals

This document describes the internal logic of the CSurfer security model, specifically focusing on session management and CSRF protection.

## Data Model: CookieJar

The `CookieJar` is a centralized repository for HTTP cookies, managed by the `Tab` and accessed by the `HttpRequest` engine.

### Class Structure

```mermaid
classDiagram
    class Cookie {
        +string name
        +string value
        +string domain
        +string path
        +string same_site
    }

    class CookieJar {
        -map<string, vector~Cookie~> cookies_
        +store_cookie(url, header)
        +get_cookies(target_url, referrer_url, method)
        -save_to_disk()
        -load_from_disk()
    }

    CookieJar "1" *-- "many" Cookie
```

## SameSite=Lax Enforcement Logic

The core of our CSRF defense is the state machine within `get_cookies`. It determines whether a cookie is "safe" to send based on the navigational context.

### Decision Tree

```mermaid
graph TD
    Start[Check Cookie] --> Policy{same_site?}
    Policy -->|none| Allow[Allow Cookie]
    Policy -->|strict| CheckOrigin{Same Origin?}
    Policy -->|lax| CheckContext{Cross-Origin?}

    CheckOrigin -->|yes| Allow
    CheckOrigin -->|no| Block[Block Cookie]

    CheckContext -->|no| Allow
    CheckContext -->|yes| CheckMethod{Safe Method? GET}
    
    CheckMethod -->|yes| Allow
    CheckMethod -->|no| Block
```

## Persistence Layer

Cookies are stored in a hidden file named `.csurfer_cookies` in the repository root.

### Serialization Format
The jar uses a pipe-delimited text format for simplicity:
`domain|name|value|same_site|path`

### Lifecycle
- **Load**: Triggered automatically when the `CookieJar` is instantiated (once per Browser/Tab initialization).
- **Save**: Triggered proactively every time `store_cookie` is called (whenever a server sends a `Set-Cookie` header).

## Integration with JS Runtime
The `CookieJar` is exposed to the JavaScript environment through two native bindings in `JSContext`:
- `native_cookie_get`: Formats all allowed cookies into a string for `document.cookie`.
- `native_cookie_set`: Parses a JS-provided cookie string and stores it in the jar.
