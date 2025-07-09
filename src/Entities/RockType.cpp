#include "RockType.hpp"

RockType::RockType(Model _model, const std::string &_name, const std::string &_type) :
Entity(_model, _name, "rock"),
frequency(0.3f),
amplitude(0.5f),
roughness(0.5f),
rockType(0) {

}

RockType::RockType(Mesh _mesh, const std::string &_name, const std::string &_type) :
RockType(LoadModelFromMesh(_mesh), _name, _type) {

}

BoundingBox RockType::GenMeshBoundingBox(const Mesh& mesh, float pos[3]) {
    BoundingBox box;

    if (mesh.vertexCount == 0 || mesh.vertices == nullptr) return box;

    float* vertices = mesh.vertices;

    Vector3 first = {
        vertices[0] + pos[0],
        vertices[1] + pos[1],
        vertices[2] + pos[2]
    };
    box.min = first;
    box.max = first;

    for (int i = 1; i < mesh.vertexCount; i++) {
        Vector3 v = {
            vertices[i * 3 + 0] + pos[0],
            vertices[i * 3 + 1] + pos[1],
            vertices[i * 3 + 2] + pos[2]
        };

        if (v.x < box.min.x) box.min.x = v.x;
        if (v.y < box.min.y) box.min.y = v.y;
        if (v.z < box.min.z) box.min.z = v.z;
        if (v.x > box.max.x) box.max.x = v.x;
        if (v.y > box.max.y) box.max.y = v.y;
        if (v.z > box.max.z) box.max.z = v.z;
    }

    return box;
}

void RockType::UpdateBuffers() {
    RecalcNormals();
    UpdateMeshBuffer(*e_mesh, 0, e_vertices.data(), e_vertices.size() * sizeof(float), 0);
    UpdateMeshBuffer(*e_mesh, 2, e_normals.data(), e_normals.size() * sizeof(float), 0);
}

void RockType::RecalcNormals() {
    e_normals.assign(e_vertices.size(), 0.0f);

    for (size_t i = 0; i < e_indices.size(); i += 3) {
        unsigned short i0 = e_indices[i];
        unsigned short i1 = e_indices[i + 1];
        unsigned short i2 = e_indices[i + 2];

        glm::vec3 v0(e_vertices[i0 * 3], e_vertices[i0 * 3 + 1], e_vertices[i0 * 3 + 2]);
        glm::vec3 v1(e_vertices[i1 * 3], e_vertices[i1 * 3 + 1], e_vertices[i1 * 3 + 2]);
        glm::vec3 v2(e_vertices[i2 * 3], e_vertices[i2 * 3 + 1], e_vertices[i2 * 3 + 2]);

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;

        glm::vec3 normal = glm::normalize(glm::cross(edge2, edge1));

        for (unsigned int index : {i0, i1, i2}) {
            e_normals[index * 3] = normal.x;
            e_normals[index * 3 + 1] = normal.y;
            e_normals[index * 3 + 2] = normal.z;
        }
    }

    for (size_t i = 0; i < e_normals.size(); i += 3) {
        glm::vec3 n(e_normals[i], e_normals[i + 1], e_normals[i + 2]);
        n = glm::normalize(n);

        e_normals[i] += n.x;
        e_normals[i + 1] += n.y;
        e_normals[i + 2] += n.z;
    }
}
