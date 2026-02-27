#pragma once

// Horizontal and vertical padding (in pixels) around the document content.
//
// HSTEP is used as left and right padding; VSTEP is used as top and bottom
// padding and also when clamping scroll so we do not scroll past whitespace.
inline constexpr int HSTEP = 20;
inline constexpr int VSTEP = 20;
