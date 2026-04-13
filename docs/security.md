# Security Architecture

This document describes the defense-in-depth model implemented in CSurfer as of Chapter 10.

## The Origin Model
Security in CSurfer is built around the **Origin** (Scheme + Host + Port). 
- **Identification**: `Url::origin()` extracts this tuple for every request.
- **Isolation**: CSurfer enforces strict port-based isolation (e.g., `localhost:8000` and `localhost:8001` are treated as different origins).

## The 5-Phase Security Pipeline

### Phase 1 & 2: Header & Origin Foundation
- **Logic**: Implemented robust HTTP response header parsing and lowercase normalization in `HttpRequest`.
- **Goal**: Enables the browser to detect and process `Content-Security-Policy` and `Set-Cookie` headers.

### Phase 3: Same-Origin Policy (SOP)
- **Component**: `JSContext::native_XMLHttpRequest_send`.
- **Enforcement**: JavaScript is blocked from making `XMLHttpRequest` calls to origins that do not match the page's current origin. 
- **Mitigation**: Prevents malicious scripts from exfiltrating data to third-party servers.

### Phase 4: Content Security Policy (CSP)
- **Component**: `Tab` and `StyleEngine`.
- **Enforcement**: Before any resource (script/style) is fetched, the browser verifies the target URL against its active CSP directives.
- **Directives Support**:
    - `default-src`: Fallback for all resource types.
    - `script-src`: Restricts script locations.
    - `style-src`: Restricts CSS locations.
    - `'self'`: Automatically matches the current origin.
- **Mitigation**: Neutralizes Cross-Site Scripting (XSS) by blocking unauthorized external scripts.

### Phase 5: CookieJar & SameSite Enforcement
- **Component**: `CookieJar`.
- **Mechanics**:
    - **Storage**: Stateful management of cookies per domain.
    - **Persistence**: Cookies are serialized to `.csurfer_cookies` across sessions.
    - **SameSite=Lax (Default)**: CSurfer withholds cookies on cross-origin `POST` requests.
    - **Referer Tracking**: Automatically injects a `Referer` header using the origin of the source page.
- **Mitigation**: Prevents Cross-Site Request Forgery (CSRF) attacks.

## Life Cycle of a Secure Request
1. **CSP Check**: Before loading, the browser verifies if the URL is "allowed".
2. **Cookie Filtering**: The `CookieJar` matches the target URL against stored cookies.
3. **Context Check**: For each cookie, the Jar compares the `Referer` origin against the target origin. If `SameSite=Lax` and the context is cross-origin POST, the cookie is withheld.
4. **Header Injection**: `Cookie` and `Referer` headers are added to the request.
5. **Response Processing**: New cookies are extracted from `Set-Cookie` and persisted to disk.

## Final Status
CSurfer now implements a modern, defense-in-depth security model that protects users from the three most common web vulnerabilities: **XSS**, **CSRF**, and **Data Exfiltration**.
