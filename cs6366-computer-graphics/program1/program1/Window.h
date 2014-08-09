#ifndef WINDOW_H
#define WINDOW_H

#include "GL/freeglut.h"

 // All are globally accessible variables

extern const GLubyte BORDER_SIZE;     // Width of strips between windows
extern const GLuint WINDOW_WIDTH;     // Initial window width
extern const GLuint WINDOW_HEIGHT;    // Initial window height
extern const GLuint SUBWINDOW_WIDTH;  // Initial subwindow width
extern const GLuint SUBWINDOW_HEIGHT; // Initial subwindow height

extern GLubyte borderSize;            // Mutable border size
extern GLuint windowWidth;            // Mutable window width
extern GLuint windowHeight;           // Mutable window height
extern GLuint subwindowWidth;         // Mutable subwindow width
extern GLuint subwindowHeight;        // Mutable subwindow height

#endif