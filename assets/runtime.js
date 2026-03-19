const LISTENERS = {};

class Node {
  constructor(handle) {
    this.handle = handle;
  }
}

class Element extends Node {
  constructor(handle) {
    super(handle);
  }
  getAttribute(attr) {
    return getAttribute(this.handle, attr);
  }
  addEventListener(type, listener) {
    if (!LISTENERS[this.handle]) LISTENERS[this.handle] = {};
    if (!LISTENERS[this.handle][type]) LISTENERS[this.handle][type] = [];
    LISTENERS[this.handle][type].push(listener);
  }
}

Object.defineProperty(Element.prototype, 'innerHTML', {
  set: function(s) {
    innerHTML_set(this.handle, s.toString());
  }
});

class HTMLDocument {
  constructor() {}
  querySelectorAll(s) {
    const handles = querySelectorAll(s.toString());
    return handles.map(h => new Element(h));
  }
  addEventListener(type, listener) {
    // Global/Document listeners can be handled similarly
    if (!LISTENERS['document']) LISTENERS['document'] = {};
    if (!LISTENERS['document'][type]) LISTENERS['document'][type] = [];
    LISTENERS['document'][type].push(listener);
  }
}

const document = new HTMLDocument();

function dispatchEvent(type, handle) {
  const elt = new Element(handle);
  let preventDefault = false;

  // Dispatch to element listeners
  const elementListeners = (LISTENERS[handle] && LISTENERS[handle][type]) || [];
  for (const listener of elementListeners) {
    if (listener.call(elt, { type: type, target: elt }) === true) {
      preventDefault = true;
    }
  }

  // Dispatch to document listeners
  const documentListeners = (LISTENERS['document'] && LISTENERS['document'][type]) || [];
  for (const listener of documentListeners) {
    if (listener.call(elt, { type: type, target: elt }) === true) {
      preventDefault = true;
    }
  }

  return preventDefault;
}
