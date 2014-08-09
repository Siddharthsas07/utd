#include "Window.h"

const GLubyte BORDER_SIZE = 5;
const GLuint WINDOW_WIDTH = 1280;
const GLuint WINDOW_HEIGHT = 720;
const GLuint SUBWINDOW_WIDTH = (GLuint)((WINDOW_WIDTH - (3*BORDER_SIZE)) / 2.0f);
const GLuint SUBWINDOW_HEIGHT = (GLuint)(WINDOW_HEIGHT - (2*BORDER_SIZE));

GLubyte borderSize = BORDER_SIZE;
GLuint windowWidth = WINDOW_WIDTH;
GLuint windowHeight = WINDOW_HEIGHT;
GLuint subwindowWidth = SUBWINDOW_WIDTH;
GLuint subwindowHeight = SUBWINDOW_HEIGHT;
