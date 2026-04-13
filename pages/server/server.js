const express = require('express');
const path = require('path');
const app = express();
const port = 8000;

// Body parser for form data
app.use(express.urlencoded({ extended: true }));


// CSP Protected Bank Page
app.get('/bank_protected.html', (req, res) => {
    res.set('Content-Security-Policy', "default-src 'self'");
    res.sendFile(path.join(__dirname, '../bank_protected.html'));
});

// Login Page - Sets a session cookie
app.get('/login', (req, res) => {
    // We set SameSite=Lax to protect against CSRF
    res.set('Set-Cookie', 'session=secure_user_123; SameSite=Lax; Path=/');
    res.send('<html><body><h1>Login Successful!</h1><p>Session cookie set. <a href="/bank_session.html">Go to your account</a></p></body></html>');
});

// Transfer Money - Protected by Session Cookie
app.post('/transfer', (req, res) => {
    const cookies = req.headers['cookie'] || '';
    const referer = req.headers['referer'] || 'Direct';

    console.log(`[Bank Server] Transfer request from Referer: ${referer}`);
    console.log(`[Bank Server] Cookies received: ${cookies}`);

    let responseHtml = '<html><body>';
    if (cookies.includes('session=secure_user_123')) {
        responseHtml += `<h1 style="color:green">Transfer SUCCESSFUL</h1>`;
        responseHtml += `<p>Transferred $${req.body.amount} to ${req.body.to}.</p>`;
    } else {
        responseHtml += `<h1 style="color:red">Transfer DENIED</h1>`;
        responseHtml += `<p>Error: No valid session cookie found. CSRF attack blocked or not logged in!</p>`;
        responseHtml += `<p>Headers received: ${JSON.stringify(req.headers)}</p>`;
    }
    responseHtml += '<p><a href="/bank_session.html">Back to Bank</a></p></body></html>';
    res.send(responseHtml);
});

// Static files from parent directory (pages/)
app.use(express.static(path.join(__dirname, '..')));

// Echo POST data back to the browser
app.post('/echo_post', (req, res) => {
    console.log('[Server] Received POST body:', req.body);
    let responseHtml = '<html><head><title>POST Result</title></head><body>';

    if (req.body && Object.keys(req.body).length > 0) {
        responseHtml += '<h1>POST Data Received</h1><ul>';
        for (const [key, value] of Object.entries(req.body)) {
            responseHtml += `<li><b>${key}</b>: ${value}</li>`;
        }
        responseHtml += '</ul>';
    } else {
        responseHtml += '<h1>No POST Data Received</h1>';
        responseHtml += '<p>The request body was empty or not correctly parsed.</p>';
        responseHtml += `<p>Content-Type: ${req.headers['content-type']}</p>`;
    }

    responseHtml += '<p><a href="/">Back to Home</a></p></body></html>';
    res.send(responseHtml);
});

app.listen(port, () => {
    console.log(`[Server] Bank server listening at http://localhost:${port}`);
});

// Second server instance for Cross-Origin testing (Attacker Site)
const attackerPort = 8001;
const attackerApp = express();
attackerApp.use(express.static(path.join(__dirname, '..')));
attackerApp.listen(attackerPort, () => {
    console.log(`[Server] Attacker server listening at http://localhost:${attackerPort}`);
});
