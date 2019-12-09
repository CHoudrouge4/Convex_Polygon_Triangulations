#include "shader_tools.h"
#include <iostream>


GLuint createShader(GLenum type, const GLchar* src) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  std::cout << status << std::endl;
  return shader;
}

GLuint createProgram(std::vector<GLuint> &shaders) {
  GLuint shaderProgram = glCreateProgram();
  for(size_t i = 0; i < shaders.size(); ++i) glAttachShader(shaderProgram, shaders[i]);
  glBindFragDataLocation(shaderProgram, 0, "outColor");
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);
  return shaderProgram;
}
