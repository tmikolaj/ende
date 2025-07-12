#ifndef NORMALS_HPP
#define NORMALS_HPP

#include "../raylib/src/raylib.h"

class Normals {
public:
    Normals() = default;
    ~Normals() = default;

    void DrawFaceNormals(const Mesh &mesh, float length = 0.1f, Color color = GREEN);
    void DrawVertexNormals(const Mesh& mesh, float length = 0.1f, Color color = GREEN);
    void DrawEdgeNormals(const Mesh& mesh, float length = 0.1f, Color color = GREEN);
};

#endif //NORMALS_HPP
