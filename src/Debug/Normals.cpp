#include "Normals.hpp"
#include "../../external/raylib/src/raymath.h"

void Normals::DrawVertexNormals(const Mesh& mesh, float length, Color color) {
    for (int i = 0; i < mesh.vertexCount; i++) {
        Vector3 position = {
            mesh.vertices[i * 3],
            mesh.vertices[i * 3 + 1],
            mesh.vertices[i * 3 + 2]
        };

        Vector3 normal = {
            mesh.normals[i * 3],
            mesh.normals[i * 3 + 1],
            mesh.normals[i * 3 + 2]
        };

        Vector3 end = {
            position.x + length * normal.x,
            position.y + length * normal.y,
            position.z + length * normal.z
        };

        DrawLine3D(position, end, color);
    }
}

void Normals::DrawFaceNormals(const Mesh& mesh, float length, Color color) {
    for (int i = 0; i < mesh.triangleCount; i++) {
        int i0 = mesh.indices[i * 3];
        int i1 = mesh.indices[i * 3 + 1];
        int i2 = mesh.indices[i * 3 + 2];

        Vector3 v0 = {
            mesh.vertices[i0 * 3],
            mesh.vertices[i0 * 3 + 1],
            mesh.vertices[i0 * 3 + 2]
        };

        Vector3 v1 = {
            mesh.vertices[i1 * 3],
            mesh.vertices[i1 * 3 + 1],
            mesh.vertices[i1 * 3 + 2]
        };

        Vector3 v2 = {
            mesh.vertices[i2 * 3],
            mesh.vertices[i2 * 3 + 1],
            mesh.vertices[i2 * 3 + 2]
        };

        Vector3 center = {
            (v0.x + v1.x + v2.x) / 3.0f,
            (v0.y + v1.y + v2.y) / 3.0f,
            (v0.z + v1.z + v2.z) / 3.0f
        };

        Vector3 edge1 = v1 - v0;
        Vector3 edge2 = v2 - v0;
        Vector3 normal = Vector3Normalize(Vector3CrossProduct(edge1, edge2));

        Vector3 end = Vector3Add(center, Vector3Scale(normal, length));
        DrawLine3D(center, end, color);
    }
}

void Normals::DrawEdgeNormals(const Mesh& mesh, float length, Color color) {
    for (int i = 0; i < mesh.triangleCount; i++) {
        int i0 = mesh.indices[i * 3];
        int i1 = mesh.indices[i * 3 + 1];
        int i2 = mesh.indices[i * 3 + 2];

        int edgeIndices[3][2] = {
            { i0, i1 },
            { i1, i2 },
            { i2, i0 }
        };

        for (int e = 0; e < 3; e++) {
            int a = edgeIndices[e][0];
            int b = edgeIndices[e][1];

            Vector3 va = { mesh.vertices[a * 3], mesh.vertices[a * 3 + 1], mesh.vertices[a * 3 + 2] };
            Vector3 vb = { mesh.vertices[b * 3], mesh.vertices[b * 3 + 1], mesh.vertices[b * 3 + 2] };
            Vector3 midpoint = Vector3Lerp(va, vb, 0.5f);

            Vector3 na = { mesh.normals[a * 3], mesh.normals[a * 3 + 1], mesh.normals[a * 3 + 2] };
            Vector3 nb = { mesh.normals[b * 3], mesh.normals[b * 3 + 1], mesh.normals[b * 3 + 2] };
            Vector3 edgeNormal = Vector3Normalize(Vector3Scale(Vector3Add(na, nb), 0.5f));

            Vector3 end = Vector3Add(midpoint, Vector3Scale(edgeNormal, length));
            DrawLine3D(midpoint, end, color);
        }
    }
}
