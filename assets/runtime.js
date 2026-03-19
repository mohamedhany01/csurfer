var LISTENERS = {};

function Node(handle) {
  this.handle = handle;
}

function Element(handle) {
  Node.call(this, handle);
}
Element.prototype = Object.create(Node.prototype);
Element.prototype.constructor = Element;

Element.prototype.getAttribute = function(attr) {
  return getAttribute(this.handle, attr);
};

Element.prototype.addEventListener = function(type, listener) {
  if (!LISTENERS[this.handle]) LISTENERS[this.handle] = {};
  if (!LISTENERS[this.handle][type]) LISTENERS[this.handle][type] = [];
  LISTENERS[this.handle][type].push(listener);
};

Object.defineProperty(Element.prototype, 'innerHTML', {
  set: function(s) {
    innerHTML_set(this.handle, s.toString());
  }
});

function HTMLDocument() {}

HTMLDocument.prototype.querySelectorAll = function(s) {
  var handles = querySelectorAll(s.toString());
  return handles.map(function(h) { return new Element(h); });
};

HTMLDocument.prototype.addEventListener = function(type, listener) {
  if (!LISTENERS['document']) LISTENERS['document'] = {};
  if (!LISTENERS['document'][type]) LISTENERS['document'][type] = [];
  LISTENERS['document'][type].push(listener);
};

var document = new HTMLDocument();

function dispatchEvent(type, handle) {
  var elt = new Element(handle);
  var preventDefault = false;

  // Dispatch to element listeners
  var elementListeners = (LISTENERS[handle] && LISTENERS[handle][type]) || [];
  for (var i = 0; i < elementListeners.length; i++) {
    var listener = elementListeners[i];
    if (listener.call(elt, { type: type, target: elt }) === true) {
      preventDefault = true;
    }
  }

  // Dispatch to document listeners
  var documentListeners = (LISTENERS['document'] && LISTENERS['document'][type]) || [];
  for (var j = 0; j < documentListeners.length; j++) {
    var docListener = documentListeners[j];
    if (docListener.call(elt, { type: type, target: elt }) === true) {
      preventDefault = true;
    }
  }

  return preventDefault;
}
