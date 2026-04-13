# Security Architecture

## Current Status (Chapter 10 - Phase 3)

**Security Model**: Partially Secured (Same-Origin Policy implemented)

### Implemented Features
- **XMLHttpRequest API**: Supports network requests for JavaScript.
- **URL Origins**: Correctly identifies origin components for security checks.
- **Same-Origin Policy (SOP)**: Enforced for `XMLHttpRequest`. Requests to origins differing in scheme, host, or port are blocked.

### Known Vulnerabilities
- **Resource Loading**: SOP is not yet applied to `<script>` and `<link>` tags (Planned for Phase 4: CSP).
- **Csrf**: Cookie management is not yet implemented (Planned for Phase 5).

### Mitigations (Implemented)
- **Phase 3**: SOP check in `JSContext::native_XMLHttpRequest_send` successfully blocks data theft between origins.
- **Phase 4**: Implement Content Security Policy (CSP) to restrict allowed origins for external resources.
- **Phase 5**: Implement `SameSite=Lax` cookie policy.
