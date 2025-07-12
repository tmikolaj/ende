#include "CustomMeshes.hpp"

Mesh CustomMeshes::GenMeshIcosahedron() {
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