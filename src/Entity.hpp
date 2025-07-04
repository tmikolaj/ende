#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "external/imgui/imgui.h"
#include <vector>
#include "raylib.h"

struct Entity {
    Model e_model;
    Mesh* e_mesh;

    std::string e_name;
    ImVec4 e_colorValues;
    Color e_color;
    bool e_visible;

    BoundingBox e_boundingBox;
    float e_position[3];

    std::vector<float> e_vertices;
    std::vector<float> e_normals;
    std::vector<unsigned short> e_indices;

    Entity(Model _model, const std::string& _name) {
        e_name = _name;

        e_model = _model;
        e_mesh = &e_model.meshes[0];

        e_position[0] = 0.0f;
        e_position[1] = 0.0f;
        e_position[2] = 0.0f;

        e_boundingBox = GenBoundingBox(*e_mesh, e_position);

        auto rawVerts = static_cast<float*>(e_mesh->vertices);
        e_vertices.assign(rawVerts, rawVerts + e_mesh->vertexCount * 3);
        auto rawNormals = static_cast<float*>(e_mesh->normals);
        e_normals.assign(rawNormals, rawNormals + e_mesh->vertexCount * 3);
        auto rawIndices = static_cast<unsigned short*>(e_mesh->indices);
        e_indices.assign(rawIndices, rawIndices + e_mesh->triangleCount * 3);

        e_colorValues = { 1.0f, 1.0f, 1.0f, 1.0f };
        e_color = ImVecToColor(e_colorValues);

        e_visible = true;
    }

    Entity(Mesh _mesh, const std::string& _name) : Entity(LoadModelFromMesh(_mesh), _name) {}

    Color ImVecToColor(const ImVec4& toConvert) {
        return (Color){
            static_cast<unsigned char>(toConvert.x * 255.0f),
            static_cast<unsigned char>(toConvert.y * 255.0f),
            static_cast<unsigned char>(toConvert.z * 255.0f),
            255
        };
    }

    BoundingBox GenBoundingBox(const Mesh& mesh, float pos[3]) {
        BoundingBox box = { 0 };

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
    void UpdateBuffers() {
        UpdateMeshBuffer(*e_mesh, 0, e_vertices.data(), e_vertices.size() * sizeof(float), 0);
        UpdateMeshBuffer(*e_mesh, 2, e_normals.data(), e_normals.size() * sizeof(float), 0);
    }
};

#endif // ENTITY_HPP