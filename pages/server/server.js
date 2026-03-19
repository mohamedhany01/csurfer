const express = require('express');
const path = require('path');
const app = express();
const port = 8000;

// Body parser for form data
app.use(express.urlencoded({ extended: true }));


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
    console.log(`[Server] Express v5 test server listening at http://localhost:${port}`);
});
