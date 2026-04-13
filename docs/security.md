# Security Architecture

## Current Status (Chapter 10 - Phase 4)

**Security Model**: Partially Secured (SOP + Content Security Policy)

### Implemented Features
- **XMLHttpRequest API**: Supports network requests for JavaScript.
- **URL Origins**: Correctly identifies origin components for security checks.
- **Same-Origin Policy (SOP)**: Enforced for `XMLHttpRequest`. Requests to origins differing in scheme, host, or port are blocked.
- **Content Security Policy (CSP)**:
    - Parses `Content-Security-Policy` HTTP headers.
    - Enforces `script-src` and `style-src` directives (with `default-src` fallback).
    - Blocks external resources (scripts/styles) before loading if they are not in the "Guest List".
    - Supports the `'self'` keyword for current origin matching.

### Known Vulnerabilities
- **CSRF**: Cookie management is not yet implemented (Planned for Phase 5: CookieJar).

### Mitigations (Implemented)
- **Phase 3**: SOP check in `JSContext::native_XMLHttpRequest_send`.
- **Phase 4**: CSP enforcement in `Tab::load` and `StyleEngine::apply` to block malicious 3rd-party scripts.
- **Phase 5**: Implement `SameSite=Lax` cookie policy.
