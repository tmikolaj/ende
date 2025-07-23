#ifndef NORMALVISUALIZER_HPP
#define NORMALVISUALIZER_HPP

#include "../raylib/src/raylib.h"

class NormalVisualizer {
public:
    NormalVisualizer() = default;
    ~NormalVisualizer() = default;

    void DrawFaceNormals(const Mesh &mesh, float length = 0.1f, Color color = GREEN);
    void DrawVertexNormals(const Mesh& mesh, float length = 0.1f, Color color = GREEN);
    void DrawEdgeNormals(const Mesh& mesh, float length = 0.1f, Color color = GREEN);
};

#endif //NORMALVISUALIZER_HPP
