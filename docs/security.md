# Security Architecture

## Current Status (Chapter 10 - Phase 2)

**Security Model**: Insecure (Vulnerable to Cross-Origin Data Theft)

### Implemented Features
- **XMLHttpRequest API**: Exposes the ability for JavaScript to make network requests using `native_XMLHttpRequest_send`.
- **URL Origins**: `Url::origin()` correctly identifies the scheme, host, and port.

### Known Vulnerabilities
- **Lack of Same-Origin Policy (SOP)**: The `XMLHttpRequest` implementation currently does not verify if the destination origin matches the page origin.
- **Exposure**: Scripts from any origin can read data from any other reachable origin via XHR.

### Mitigations (Planned)
- **Phase 3**: Implement Same-Origin Policy (SOP) checks in `JSContext::native_XMLHttpRequest_send`.
- **Phase 4**: Implement Content Security Policy (CSP) to restrict allowed origins for external resources.
- **Phase 5**: Implement `SameSite=Lax` cookie policy.
