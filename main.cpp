#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include "raylib.h"
#include "raymath.h"
#define RLIGHTS_IMPLEMENTATION
#include <algorithm>
#include "rlights.h"
#include "external/imgui/imgui.h"
#include "external/glm/glm.hpp"
#include "external/glm/gtc/type_ptr.hpp"
#include "src/Entities/Entity.hpp"
#include "src/Program.hpp"

std::vector<float> customVerts;
std::vector<float> customNormals;
std::vector<unsigned short> customIndices;

enum currentViewMode {
    mSOLID = 0,
    mM_PREVIEW = 1,
    mRENDER = 2,
    mWIREFRAME = 3
};

Color ImVecToColor(ImVec4 toConvert);
void CalcNormals(float vertices[], unsigned short indices[], float normals[], int vertexCount, int triangleCount);
Mesh GenerateGridMesh(int gridSize, float tileSize);
Matrix IdentityMatrix();

Mesh GenNxNPlane(int n) {
    int vertexCount = (n+1) * (n+1);
    int indexCount = n * n * 2 * 3;

    auto vertices = static_cast<float*>(malloc(sizeof(float) * 3 * vertexCount));
    auto indices = static_cast<unsigned short*>(malloc(sizeof(unsigned short) * indexCount));
    auto normals = static_cast<float*>(calloc(vertexCount * 3, sizeof(float)));

    int v = 0;
    for (int z = 0; z <= n; z++) {
        for (int x = 0; x <= n; x++) {
            vertices[v++] = x * 1.0f;
            vertices[v++] = 0.0f;
            vertices[v++] = z * 1.0f;
        }
    }

    v = 0;
    for (int z = 0; z < n; z++) {
        for (int x = 0; x < n; x++) {
            int topLeft = z * (n + 1) + x;
            int topRight = z * (n + 1) + (x + 1);
            int bottomLeft = (z + 1) * (n + 1) + x;
            int bottomRight = (z + 1) * (n + 1) + (x + 1);

            indices[v++] = topLeft;
            indices[v++] = bottomLeft;
            indices[v++] = topRight;

            indices[v++] = topRight;
            indices[v++] = bottomLeft;
            indices[v++] = bottomRight;
        }
    }

    for (int i = 0; i < vertexCount; i += 3) {
        unsigned short i0 = indices[i];
        unsigned short i1 = indices[i + 1];
        unsigned short i2 = indices[i + 2];

        glm::vec3 v0(vertices[i0], vertices[i0 + 1], vertices[i0 + 2]);
        glm::vec3 v1(vertices[i1], vertices[i1 + 1], vertices[i1 + 2]);
        glm::vec3 v2(vertices[i2], vertices[i2 + 1], vertices[i2 + 2]);

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;

        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        for (unsigned int index : {i0, i1, i2}) {
            normals[index * 3] += normal.x;
            normals[index * 3 + 1] += normal.y;
            normals[index * 3 + 2] += normal.z;
        }
    }

    for (size_t i = 0; i < vertexCount * 3; i++) {
        glm::vec3 n(normals[i], normals[i + 1], normals[i + 2]);
        n = glm::normalize(n);

        normals[i * 3] = n.x;
        normals[i * 3 + 1] = n.y;
        normals[i * 3 + 2] = n.z;
    }

    Mesh mesh = { 0 };
    mesh.vertices = vertices;
    mesh.indices = indices;
    mesh.vertexCount = vertexCount;
    const int triangleCount = indexCount / 3;
    mesh.normals = normals;
    mesh.triangleCount = triangleCount;

    UploadMesh(&mesh, true);

    return mesh;
}

Mesh GenNxNCube(int n) {
    constexpr int faces = 6;
    const int verticesPerFace = (n + 1) * (n + 1);
    const int trianglesPerFace = n * n * 2;

    const int vertexCount = faces * verticesPerFace;
    const int indexCount = faces * trianglesPerFace * 3;

    std::vector<float> vertices(vertexCount * 3);
    std::vector<unsigned short> indices(indexCount);

    struct FaceData {
        glm::vec3 origin;
        glm::vec3 dir1;
        glm::vec3 dir2;
    };

    FaceData faceDirs[6] = {
        // Front (+Z)
        { {-0.5f, -0.5f,  0.5f}, {1.0f / n, 0.0f, 0.0f}, {0.0f, 1.0f / n, 0.0f} },
        // Back (-Z)
        { { 0.5f, -0.5f, -0.5f}, {-1.0f / n, 0.0f, 0.0f}, {0.0f, 1.0f / n, 0.0f} },
        // Left (-X)
        { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f / n}, {0.0f, 1.0f / n, 0.0f} },
        // Right (+X)
        { { 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, -1.0f / n}, {0.0f, 1.0f / n, 0.0f} },
        // Top (+Y)
        { {-0.5f, 0.5f,  0.5f}, {1.0f / n, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f / n} },
        // Bottom (-Y)
        { {-0.5f, -0.5f, -0.5f}, {1.0f / n, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f / n} },
    };

    int v = 0;
    int i = 0;
    for (int f = 0; f < 6; f++) {
        FaceData face = faceDirs[f];

        for (int y = 0; y <= n; y++) {
            for (int x = 0; x <= n; x++) {
                glm::vec3 pos = face.origin + face.dir1 * static_cast<float>(x) + face.dir2 * static_cast<float>(y);
                vertices[v++] = pos.x;
                vertices[v++] = pos.y;
                vertices[v++] = pos.z;
            }
        }

        int faceVertexOffset = f * verticesPerFace;

        for (int y = 0; y < n; y++) {
            for (int x = 0; x < n; x++) {
                int topLeft = faceVertexOffset + y * (n + 1) + x;
                int topRight = topLeft + 1;
                int bottomLeft = topLeft + (n + 1);
                int bottomRight = bottomLeft + 1;

                indices[i++] = topLeft;
                indices[i++] = bottomLeft;
                indices[i++] = topRight;

                indices[i++] = topRight;
                indices[i++] = bottomLeft;
                indices[i++] = bottomRight;
            }
        }
    }

    Mesh mesh = { 0 };
    mesh.vertices = vertices.data();
    mesh.indices = indices.data();
    mesh.vertexCount = vertexCount;
    mesh.triangleCount = indexCount / 3;
    mesh.normals = nullptr;

    UploadMesh(&mesh, true);
    return mesh;
}

Mesh GenIcosahedron() {
    float t = (1 + std::sqrt(5)) / 2.0f;

    std::vector<glm::vec3> positions = {
        {-1,  t,  0}, // 0
         { 1,  t,  0}, // 1
         {-1, -t,  0}, // 2
         { 1, -t,  0}, // 3
         { 0, -1,  t}, // 4
         { 0,  1,  t}, // 5
         { 0, -1, -t}, // 6
         { 0,  1, -t}, // 7
         { t,  0, -1}, // 8
         { t,  0,  1}, // 9
         {-t,  0, -1}, // 10
         {-t,  0,  1}  // 11
    };

    std::vector<unsigned short> indices = {
        0, 11 ,5,
        0, 5, 1,
        0, 1, 7,
        0, 7, 10,
        0, 10, 11,
        1, 5, 9,
        5, 11, 4,
        11, 10, 2,
        10, 7, 6,
        7, 1, 8,
        3, 9, 4,
        3, 4, 2,
        3, 2, 6,
        3, 6, 8,
        3, 8, 9,
        4, 9, 5,
        2, 4, 11,
        6, 2, 10,
        8, 6, 7,
        9, 8, 1
    };

    for (auto& p : positions) {
        p = glm::normalize(p);
    }

    int vertexCount = static_cast<int>(positions.size());
    auto verts = static_cast<float*>(malloc(sizeof(float) * vertexCount * 3));

    int indexCount = static_cast<int>(indices.size());
    auto inds = static_cast<unsigned short*>(malloc(sizeof(unsigned short) * indexCount));

    auto norm = static_cast<float*>(calloc(vertexCount * 3, sizeof(float)));

    for (int i = 0; i < vertexCount; i++) {
        verts[i * 3] = positions[i].x;
        verts[i * 3 + 1] = positions[i].y;
        verts[i * 3 + 2] = positions[i].z;
    }

    for (int i = 0; i < indexCount; i++) {
        inds[i] = indices[i];
    }

    for (int i = 0; i < indexCount; i += 3) {
        int i0 = indices[i + 0];
        int i1 = indices[i + 1];
        int i2 = indices[i + 2];

        glm::vec3 v0(verts[i0 * 3], verts[i0 * 3 + 1], verts[i0 * 3 + 2]);
        glm::vec3 v1(verts[i1 * 3], verts[i1 * 3 + 1], verts[i1 * 3 + 2]);
        glm::vec3 v2(verts[i2 * 3], verts[i2 * 3 + 1], verts[i2 * 3 + 2]);

        glm::vec3 edg1 = v1 - v0;
        glm::vec3 edg2 = v2 - v0;

        glm::vec3 normal = glm::normalize(glm::cross(edg1, edg2));

        for (unsigned int index : {i0, i1, i2}) {
            norm[index * 3] += normal.x;
            norm[index * 3 + 1] += normal.y;
            norm[index * 3 + 2] += normal.z;
        }
    }

    for (size_t i = 0; i < vertexCount; i++) {
        glm::vec3 n(norm[i * 3], norm[i * 3 + 1], norm[i * 3 + 2]);
        n = glm::normalize(n);

        norm[i * 3] = n.x;
        norm[i * 3 + 1] = n.y;
        norm[i * 3 + 2] = n.z;
    }

    Mesh mesh = { 0 };
    mesh.vertices = verts;
    mesh.indices = inds;
    mesh.vertexCount = vertexCount;
    mesh.triangleCount = indexCount / 3;
    mesh.normals = norm;

    UploadMesh(&mesh, true);

    return mesh;
}

glm::vec3 GetNormal(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3) {
    glm::vec3 edge1 = t2 - t1;
    glm::vec3 edge2 = t3 - t1;

    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

    return normal;
}

void DrawVertexNormals(const Mesh& mesh, float length = 0.1f, Color color = GREEN) {
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

void DrawFaceNormals(const Mesh& mesh, float length = 0.1f, Color color = GREEN) {
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

void DrawEdgeNormals(const Mesh& mesh, float length = 0.1f, Color color = GREEN) {
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

int GetMidpoint(int i1, int i2, std::vector<float>& vertices, std::map<std::pair<int, int>, int>& cache) {
    auto key = std::minmax(i1, i2);

    if (cache.count(key)) return cache[key];

    // Extract vertex positions (each vertex has 3 floats)
    glm::vec3 v1(vertices[i1 * 3], vertices[i1 * 3 + 1], vertices[i1 * 3 + 2]);
    glm::vec3 v2(vertices[i2 * 3], vertices[i2 * 3 + 1], vertices[i2 * 3 + 2]);

    glm::vec3 midpoint = glm::normalize(0.5f * (v1 + v2));

    // Add new midpoint vertex to vertices
    vertices.push_back(midpoint.x);
    vertices.push_back(midpoint.y);
    vertices.push_back(midpoint.z);

    int index = static_cast<int>(vertices.size() / 3 - 1);
    cache[key] = index;

    return index;
}

Mesh& BasicSubdivision(Mesh& mesh) {
    std::vector<float> newVertices;
    std::vector<unsigned short> newIndices;

    std::map<std::pair<int, int>, int> midpointCache;

    newVertices.reserve(mesh.vertexCount * 3 * 2); // Estimate space

    // Copy original vertices (flat floats)
    for (int i = 0; i < mesh.vertexCount * 3; i++) {
        newVertices.push_back(mesh.vertices[i]);
    }

    // Subdivide each triangle
    for (int i = 0; i < mesh.triangleCount; i++) {
        int i0 = mesh.indices[i * 3];
        int i1 = mesh.indices[i * 3 + 1];
        int i2 = mesh.indices[i * 3 + 2];

        int a = GetMidpoint(i0, i1, newVertices, midpointCache);
        int b = GetMidpoint(i1, i2, newVertices, midpointCache);
        int c = GetMidpoint(i2, i0, newVertices, midpointCache);

        newIndices.push_back(i0); newIndices.push_back(a); newIndices.push_back(c);
        newIndices.push_back(a); newIndices.push_back(i1); newIndices.push_back(b);
        newIndices.push_back(c); newIndices.push_back(b); newIndices.push_back(i2);
        newIndices.push_back(a); newIndices.push_back(b); newIndices.push_back(c);
    }

    int newVertexCount = static_cast<int>(newVertices.size() / 3);
    int newTriangleCount = static_cast<int>(newIndices.size() / 3);

    // Allocate new memory for mesh vertices and indices
    float* verticesCopy = (float*)MemAlloc(newVertices.size() * sizeof(float));
    unsigned short* indicesCopy = (unsigned short*)MemAlloc(newIndices.size() * sizeof(unsigned short));

    memcpy(verticesCopy, newVertices.data(), newVertices.size() * sizeof(float));
    memcpy(indicesCopy, newIndices.data(), newIndices.size() * sizeof(unsigned short));

    Mesh _mesh = { 0 };

    // Assign new data to mesh
    _mesh.vertices = verticesCopy;
    _mesh.indices = indicesCopy;
    _mesh.vertexCount = newVertexCount;
    _mesh.triangleCount = newTriangleCount;

    return _mesh;
}

int main() {
    Program program;
    program.init();
    program.run();
    return 0;
}

Mesh GenerateGridMesh(int gridSize, float tileSize) {
    int verticesPerRow = gridSize + 1;
    int vertexCount = verticesPerRow * verticesPerRow;
    int triangleCount = gridSize * gridSize * 2;

    auto vertices = static_cast<float*>(malloc(sizeof(float) * vertexCount * 3));
    auto indices = static_cast<unsigned short*>(malloc(sizeof(unsigned short) * triangleCount * 3));
    auto normals = static_cast<float*>(calloc(vertexCount * 3, sizeof(float)));

    int v = 0;
    for (int z = 0; z <= gridSize; z++) {
        for (int x = 0; x <= gridSize; x++) {
            vertices[v++] = x * tileSize;
            vertices[v++] = 0.0f;
            vertices[v++] = z * tileSize;
        }
    }

    int t = 0;
    for (int z = 0; z < gridSize; z++) {
        for (int x = 0; x < gridSize; x++) {
            int i0 = z * (gridSize + 1) + x;
            int i1 = i0 + 1;
            int i2 = i0 + (gridSize + 1);
            int i3 = i2 + 1;

            indices[t++] = i0;
            indices[t++] = i2;
            indices[t++] = i1;

            indices[t++] = i1;
            indices[t++] = i2;
            indices[t++] = i3;
        }
    }

    for (int i = 0; i < triangleCount * 3; i += 3) {
        unsigned short i0 = indices[i];
        unsigned short i1 = indices[i + 1];
        unsigned short i2 = indices[i + 2];

        glm::vec3 v0(vertices[i0 * 3], vertices[i0 * 3 + 1], vertices[i0 * 3 + 2]);
        glm::vec3 v1(vertices[i1 * 3], vertices[i1 * 3 + 1], vertices[i1 * 3 + 2]);
        glm::vec3 v2(vertices[i2 * 3], vertices[i2 * 3 + 1], vertices[i2 * 3 + 2]);

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

        for (int j : {i0, i1, i2}) {
            normals[j * 3 + 0] += faceNormal.x;
            normals[j * 3 + 1] += faceNormal.y;
            normals[j * 3 + 2] += faceNormal.z;
        }
    }

    for (int i = 0; i < vertexCount; i++) {
        glm::vec3 n(normals[i*3], normals[i*3+1], normals[i*3+2]);
        n = glm::normalize(n);
        normals[i * 3 + 0] = n.x;
        normals[i * 3 + 1] = n.y;
        normals[i * 3 + 2] = n.z;
    }

    Mesh mesh = { 0 };
    mesh.triangleCount = triangleCount;
    mesh.vertexCount = vertexCount;
    mesh.vertices = vertices;
    mesh.indices = indices;
    mesh.normals = normals;

    UploadMesh(&mesh, true);

    return mesh;
}

void CalcNormals(float vertices[], unsigned short indices[], float normals[], int vertexCount, int triangleCount) {
    // Reset normals
    for (int i = 0; i < vertexCount * 3; i++) {
        normals[i] = 0.0f;
    }

    for (int i = 0; i < triangleCount * 3; i += 3) {
        unsigned short i0 = indices[i];
        unsigned short i1 = indices[i + 1];
        unsigned short i2 = indices[i + 2];

        glm::vec3 v0(vertices[i0 * 3], vertices[i0 * 3 + 1], vertices[i0 * 3 + 2]);
        glm::vec3 v1(vertices[i1 * 3], vertices[i1 * 3 + 1], vertices[i1 * 3 + 2]);
        glm::vec3 v2(vertices[i2 * 3], vertices[i2 * 3 + 1], vertices[i2 * 3 + 2]);

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

        for (int j : {i0, i1, i2}) {
            normals[j * 3] += faceNormal.x;
            normals[j * 3 + 1] += faceNormal.y;
            normals[j * 3 + 2] += faceNormal.z;
        }
    }

    for (int i = 0; i < vertexCount; i++) {
        glm::vec3 n(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
        n = glm::normalize(n);

        normals[i * 3] = n.x;
        normals[i * 3 + 1] = n.y;
        normals[i * 3 + 2] = n.z;
    }
}

Matrix IdentityMatrix() {
    return Matrix{1,0,0,0,
                  0,1,0,0,
                  0,0,1,0,
                  0,0,0,1};
}