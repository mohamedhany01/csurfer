log("--- [EXECUTION VULNERABILITY] ---");
log("[Evil Script] I can see your private data:");

// In a real attack, I would 'fetch' this data to my own server.
// Since I'm running in the page's context, I have full access to the DOM.
var balance = document.querySelectorAll("#balance");
if (balance.length > 0) {
    log("[Evil Script] STOLEN BALANCE: " + balance[0].getAttribute("value") || "Hidden in text");
}

log("[Evil Script] Data theft complete.");
log("--- [EXECUTION VULNERABILITY] ---");
