#pragma once

#include <GL/glew.h>
#include <vector>

GLuint createShader(GLenum type, const GLchar* src);
GLuint createProgram(std::vector<GLuint> &shaders);
