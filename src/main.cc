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

bool is_intersecting(std::pair<int, int> &p, std::pair<int, int> &q) {
  if (p == q) return true;
  if (p.first == q.first && p.second != q.second) return false;
  return (q.first > p.first && q.first < p.second && q.second > p.second);
}

bool exists(std::pair<int, int> &p, std::vector<std::pair<int, int>> &diags) {
  for (size_t i = 0; i <  diags.size(); ++i) {
    if (diags[i] == p) return true;
  }
  return false;
}

bool is_compatible(std::pair<int, int> &p, std::vector<std::pair<int, int>> &diags) {
  if (diags.size() == 0) return true;

  for (size_t i = 0; i < diags.size(); ++i) {
    if (is_intersecting(diags[i], p)) return false;
  }
  return true;
}

void generateDiag(int i, int n, std::vector<std::pair<int, int>> & diags, std::vector<std::pair<int, int>> &tri, std::vector<std::vector<std::pair<int, int>>> &triangles) {
  if (tri.size() == n - 3) {
    triangles.push_back(tri);
    return;
  }

  for (size_t j = i; i < diags.size(); ++i) {
    if (is_compatible(diags[i], tri)) {
      tri.push_back(diags[i]);
      generateDiag(i + 1, n, diags, tri, triangles);
      tri.pop_back();
    }
  }
}

std::vector<std::pair<int, int>> internal(int n) {
  std::vector<std::pair<int, int>> diags;
  std::map<std::pair<int, int>, bool> mp;
  for (int i = 0; i < n; i++) {
    for (int j = i + 2; j < n; j++) {
      if ((j + 1) % n == i) continue;
      diags.push_back({i, j});
    }
  }
  return diags;
}

// Shader sources
const GLchar* vertexSource = R"glsl(
    #version 150 core
    in vec2 position;

    void main()
    {
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

const float PI = 3.1415926;
void genereatePolygons(std::vector<GLfloat> &polygons, int n, float x, float y) {
  int scale = 6;
  for (int i = 0; i < n; i++) {
    float ang = PI * 2.0 / n * i;
    float offset_x =   (cos(ang) * 0.3);
    float offset_y = - (sin(ang) * 0.4);
    x += offset_x/scale;
    y += offset_y/scale;
    polygons.push_back(x);
    polygons.push_back(y);
  }
}

void generateTriangulatedPolygons(int start, int n, std::vector<GLuint> &elements, const std::vector<std::pair<int, int>> &diags) {
  for(int i = start; i < start + n; i++) {
    elements.push_back(i % n + start);
    elements.push_back((i + 1) % n + start);
  }

  for (size_t i = 0; i < diags.size(); ++i) {
    elements.push_back(start + diags[i].first);
    elements.push_back(start + diags[i].second);
  }
}

void display(int n,  std::vector<std::vector<std::pair<int, int>>> & triangles) {

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

  int j = 0;
  int l = 0;
  for (size_t i = 0; i < triangles.size(); i++) {
    if(-0.9f + l * 0.3 > 1)  {
      j++;
      l = 0;
    }
    genereatePolygons(polygons, n, -0.9f + l * 0.2, 0.9f - j * 0.2);
    l++;
  }

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
  //auto t_start = std::chrono::high_resolution_clock::now();

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

      // Draw a triangle from the 3 vertices
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
  display(n, triangles);
  return 0;
}
