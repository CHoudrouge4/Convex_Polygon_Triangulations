// Link statically with GLEW
#define GLEW_STATIC

// Headers
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include "shader_tools.h"
#include "geometry.h"

const GLchar* vertexSource = R"glsl(
    #version 150 core
    in vec2 position;
    void main() {
        gl_Position = vec4(position, 0.0, 1.0);
    }
)glsl";

const GLchar* fragmentSource = R"glsl(
    #version 150 core
    out vec4 outColor;
    void main() {
        outColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
)glsl";

void display(int n,  std::vector<std::vector<std::pair<int, int>>> & triangles, std::vector<std::pair<int, int>> &diags) {

  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.majorVersion = 3;
  settings.minorVersion = 2;
  sf::Window window(sf::VideoMode(900, 900, 32), "Polygons", sf::Style::Fullscreen, settings);

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  glewInit();


  // Create Vertex Array Object
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create a Vertex Buffer Object and copy the vertex data to it
  GLuint vbo;
  glGenBuffers(1, &vbo);

  std::vector<GLfloat> polygons;

  genereatePolygons(polygons, n, triangles.size(), -0.9f, 0.9f);

  GLfloat * points = polygons.data();
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, triangles.size() * 2 * n * sizeof(float), points, GL_STATIC_DRAW);

  GLuint ebo;
  glGenBuffers(1, &ebo);

  std::vector<GLuint> elements;
  int k = 0;
  for (size_t i = 0; i < triangles.size(); ++i) {
    generateTriangulatedPolygons(k, n, elements, triangles[i]);
    k += n;
  }

  // add the intersecting elements

  const GLuint * elm = elements.data();

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLuint), elm, GL_STATIC_DRAW);

  // Create and compile the vertex shader
  GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexSource);

  // Create and compile the fragment shader
  GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentSource);

  std::vector<GLuint> shaders = {vertexShader, fragmentShader};
  // Link the vertex and fragment shader into a shader program

  GLuint shaderProgram = createProgram(shaders);

  // Specify the layout of the vertex data
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

  bool running = true;
  while (running) {
      sf::Event windowEvent;
      while (window.pollEvent(windowEvent)) {
          switch (windowEvent.type) {
          case sf::Event::Closed:
              running = false;
              break;
          }
      }

      // Clear the screen to black
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      glDrawElements(GL_LINES, elements.size(), GL_UNSIGNED_INT, 0);

      // Swap buffers
      window.display();
  }

  glDeleteProgram(shaderProgram);
  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
  glDeleteVertexArrays(1, &vao);
  window.close();

}

int main() {
  int n; std::cin >> n;
  std::vector<std::pair<int, int>> diags;
  std::vector<std::vector<std::pair<int, int>>> triangles;
  auto v = internal(n);
  generateDiag(0,  n, v, diags, triangles);

  display(n, triangles, diags);
  return 0;
}
