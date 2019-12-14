#include "geometry.h"
#include <map>
#include <vector>
#include <cmath>
#include "shaders_code.h"

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

const float PI = 3.1415926;
void genereatePolygon(std::vector<GLfloat> &polygons, int n, float x, float y) {
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

void genereatePolygons(std::vector<GLfloat> &polygons, int n, size_t nb, float start_x, float start_y) {
  int j = 0;
  int l = 0;
  for (size_t i = 0; i < nb; i++) {
    if(start_x + l * 0.2 > 1.0)  {
      j++;
      l = 0;
    }
    genereatePolygon(polygons, n, start_x + l * 0.2, start_y - j * 0.2);
    l++;
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
