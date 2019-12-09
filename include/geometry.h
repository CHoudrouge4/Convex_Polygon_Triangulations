#pragma once

#include <vector>
#include <GL/glew.h>

bool is_intersecting(std::pair<int, int> &p, std::pair<int, int> &q);
bool exists(std::pair<int, int> &p, std::vector<std::pair<int, int>> &diags);
bool is_compatible(std::pair<int, int> &p, std::vector<std::pair<int, int>> &diags);
void generateDiag(int i, int n, std::vector<std::pair<int, int>> & diags, std::vector<std::pair<int, int>> &tri, std::vector<std::vector<std::pair<int, int>>> &triangles);
std::vector<std::pair<int, int>> internal(int n);
void genereatePolygon(std::vector<GLfloat> &polygons, int n, float x, float y);
void genereatePolygons(std::vector<GLfloat> &polygons, int n, size_t nb, float start_x, float start_y);
void generateTriangulatedPolygons(int start, int n, std::vector<GLuint> &elements, const std::vector<std::pair<int, int>> &diags);
