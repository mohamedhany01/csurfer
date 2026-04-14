# Chapter 10: Security Demonstrations

This document provides instructions for verifying the security features implemented in Phase 4 (CSP) and Phase 5 (CookieJar/SameSite).

---

## 1. Content Security Policy (XSS Defense)
**Goal**: Prove that a third-party script cannot be executed unless specifically authorized by the server.

### Setup
- Start the server: `./scripts/run_server.sh`
- The server provides a CSP header for `/bank_protected.html`: `default-src 'self'`.

### Execution
Run the following command:
```bash
./build/csurfer http://localhost:8000/bank_protected.html
```

### Expected Result
1.  The browser finds a script from `http://localhost:8001/evil_script.js`.
2.  It checks the origin `8001` against the approved list (only `'self'` i.e., `8000`).
3.  Terminal Output:
    `[SOP/CSP] Blocked script loading from: http://localhost:8001/evil_script.js (CSP Violation)`
4.  The legitimate script `good_script.js` (from port 8000) is executed correctly.

---

## 2. CSRF Protection (SameSite=Lax)
**Goal**: Prove that a malicious site cannot "borrow" your login session to perform actions on your behalf.

### Step 1: Login (Legitimate Session)
Run the following command to log in and set a `SameSite=Lax` cookie:
```bash
./build/csurfer http://localhost:8000/login
```
*Result: Status should say "Login Successful".*

### Step 2: Verify Session Persistence
Access your account page to confirm the cookie is being sent on same-origin requests:
```bash
./build/csurfer http://localhost:8000/bank_session.html
```
*Result: Status should say "Status: Logged In (Session: Active)".*

### Step 3: Trigger the Attack (Cross-Origin POST)
Simulate visiting a malicious site that tries to transfer money from your bank:
```bash
./build/csurfer http://localhost:8001/csrf_attacker.html
```
Click the **"CLAIM PRIZE NOW!"** button.

### Expected Result
1.  The browser attempts a `POST` request to `localhost:8000/transfer`.
2.  The `CookieJar` detects that the request originates from `localhost:8001` (Cross-Origin).
3.  Because it is a `POST` request, the `SameSite=Lax` policy **withholds the session cookie**.
4.  Terminal Output (from Server):
    `[Bank Server] Cookies received: ""`
5.  Browser Page Output:
    `<h1 style="color:red">Transfer DENIED</h1>`
    `Error: No valid session cookie found.`

---

## Implementation Reference
- **CSP Logic**: Located in `src/browser/Tab.cpp` (`is_allowed`).
- **Cookie Storage**: Located in `src/request/CookieJar.cpp`.
- **Enforcement**: Integrated into `src/request/HttpRequest.cpp`.
