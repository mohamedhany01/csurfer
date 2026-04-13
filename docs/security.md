# Security Architecture

## Current Status (Chapter 10 - FINISHED)

**Security Model**: Secured (SOP + CSP + CookieJar/SameSite)

### Implemented Features
- **XMLHttpRequest API**: Supports network requests for JavaScript.
- **URL Origins**: Correctly identifies origin components for security checks.
- **Same-Origin Policy (SOP)**: Enforced for `XMLHttpRequest`. Requests to origins differing in scheme, host, or port are blocked.
- **Content Security Policy (CSP)**:
    - Parses `Content-Security-Policy` HTTP headers.
    - Enforces `script-src` and `style-src` directives (with `default-src` fallback).
    - Blocks external resources (scripts/styles) before loading if they are not in the "Guest List".
    - Supports the `'self'` keyword for current origin matching.
- **Cookie Management (CookieJar)**:
    - Persistent session storage across requests and tabs.
    - **SameSite=Lax enforcement**: Protects against CSRF by withholding cookies on cross-origin POST requests.
    - **JS Cookie API**: Support for `document.cookie` (getter/setter).

### Known Vulnerabilities
- None relative to the Chapter 10 roadmap.

### Mitigations (Implemented)
- **Phase 3**: SOP check in `JSContext::native_XMLHttpRequest_send`.
- **Phase 4**: CSP enforcement in `Tab::load` and `StyleEngine::apply`.
- **Phase 5**: `SameSite=Lax` logic in `CookieJar::get_cookies` to block CSRF.
