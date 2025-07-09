#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <vector>
#include <string>
#include <memory>
#include <imgui.h>
#include "../glm/glm.hpp"
#include "../raylib/src/raylib.h"

class Shaper;

class Entity {
public:
	Entity(Model _model, const std::string& _name, const std::string& _type) {
  		e_name = _name;
  		e_type = _type;

  		e_model = _model;
  		e_mesh = &e_model.meshes[0];

  		e_seed = 0;
  		e_seedEnable = false;

  		e_position[0] = 0.0f;
  		e_position[1] = 0.0f;
  		e_position[2] = 0.0f;

  		auto rawVerts = static_cast<float*>(e_mesh->vertices);
  		e_vertices.assign(rawVerts, rawVerts + e_mesh->vertexCount * 3);
  		auto rawNormals = static_cast<float*>(e_mesh->normals);
  		e_normals.assign(rawNormals, rawNormals + e_mesh->vertexCount * 3);
		if (e_mesh->indices != nullptr) {
			auto rawIndices = static_cast<unsigned short*>(e_mesh->indices);
			e_indices.assign(rawIndices, rawIndices + e_mesh->triangleCount * 3);
		}

  		e_colorValues = { 1.0f, 1.0f, 1.0f, 1.0f };
  		e_color = ImVecToColor(e_colorValues);

  		e_visible = true;
  	}
    Entity(Mesh _mesh, const std::string& _name, const std::string& _type) : Entity(LoadModelFromMesh(_mesh), _name, _type) {

    }

    Entity(Entity&&) noexcept = default;
    Entity& operator=(Entity&&) noexcept = default;

    virtual ~Entity() {
	    if (!e_shapers.empty()) {
		    for (auto& shaper : e_shapers) {
			    delete &shaper;
		    }
	    }
    }

	Model e_model;
	Mesh* e_mesh;

	std::string e_name;

	ImVec4 e_colorValues;
	Color e_color;

	bool e_visible;

	std::vector<Shaper*> e_shapers;

	bool e_seedEnable;
	int e_seed;

	BoundingBox e_boundingBox;
	float e_position[3];

	std::vector<float> e_vertices;
	std::vector<float> e_normals;
	std::vector<unsigned short> e_indices;

	std::string e_type;

	virtual BoundingBox GenMeshBoundingBox(const Mesh& mesh, float pos[3]) = 0;
	virtual void UpdateBuffers() = 0;
	virtual void RecalcNormals() = 0;
	Color ImVecToColor(const ImVec4& toConvert) {
	    return (Color){
	        static_cast<unsigned char>(toConvert.x * 255.0f),
	        static_cast<unsigned char>(toConvert.y * 255.0f),
	        static_cast<unsigned char>(toConvert.z * 255.0f),
	        255
	    };
	}
};

//struct TerrainType {
//    float amplitude;
//    float frequency;
//    std::string noiseType;
//    int octaves;
//    float lacunarity;
//    float persistence;
//
//    TerrainType(float amp, float freq, const std::string& _nType, int oct, float lac, float per)
//        : amplitude(amp), frequency(freq), noiseType(_nType), octaves(oct), lacunarity(lac), persistence(per) {}
//};

//struct Entity {
//    Model e_model;
//    Mesh* e_mesh;
//
//    std::string e_name;
//
//    ImVec4 e_colorValues;
//    Color e_color;
//
//    bool e_visible;
//
//    bool e_seedEnable;
//    int e_seed;
//
//    BoundingBox e_boundingBox;
//    float e_position[3];
//
//    std::vector<float> e_vertices;
//    std::vector<float> e_normals;
//    std::vector<unsigned short> e_indices;
//
//    std::string e_type;
//    std::unique_ptr<TerrainType> e_terrain;
//
//    Entity(Entity&&) noexcept = default;
//    Entity& operator=(Entity&&) noexcept = default;
//
//    Entity(Model _model, const std::string& _name, const std::string& _type) {
//        e_name = _name;
//        e_type = _type;
//
//        if (e_type == "terrain") {
//            e_terrain = std::make_unique<TerrainType>(0.1f, 0.01f, "perlin", 3, 1.2f, 0.8f);
//        }
//
//        e_model = _model;
//        e_mesh = &e_model.meshes[0];
//
//        e_seed = 0;
//        e_seedEnable = false;
//
//        e_position[0] = 0.0f;
//        e_position[1] = 0.0f;
//        e_position[2] = 0.0f;
//
//        e_boundingBox = GenBoundingBox(*e_mesh, e_position);
//
//        auto rawVerts = static_cast<float*>(e_mesh->vertices);
//        e_vertices.assign(rawVerts, rawVerts + e_mesh->vertexCount * 3);
//        auto rawNormals = static_cast<float*>(e_mesh->normals);
//        e_normals.assign(rawNormals, rawNormals + e_mesh->vertexCount * 3);
//        auto rawIndices = static_cast<unsigned short*>(e_mesh->indices);
//        e_indices.assign(rawIndices, rawIndices + e_mesh->triangleCount * 3);
//
//        e_colorValues = { 1.0f, 1.0f, 1.0f, 1.0f };
//        e_color = ImVecToColor(e_colorValues);
//
//        e_visible = true;
//    }
//
//    Entity(Mesh _mesh, const std::string& _name, const std::string& _type)
//        : Entity(LoadModelFromMesh(_mesh), _name, _type) {}
//
//    ~Entity() = default;
//
//    Color ImVecToColor(const ImVec4& toConvert) {
//        return (Color){
//            static_cast<unsigned char>(toConvert.x * 255.0f),
//            static_cast<unsigned char>(toConvert.y * 255.0f),
//            static_cast<unsigned char>(toConvert.z * 255.0f),
//            255
//        };
//    }
//
//    BoundingBox GenBoundingBox(const Mesh& mesh, float pos[3]) {
//        BoundingBox box = { 0 };
//
//        if (mesh.vertexCount == 0 || mesh.vertices == nullptr) return box;
//
//        float* vertices = mesh.vertices;
//
//        Vector3 first = {
//            vertices[0] + pos[0],
//            vertices[1] + pos[1],
//            vertices[2] + pos[2]
//        };
//        box.min = first;
//        box.max = first;
//
//        for (int i = 1; i < mesh.vertexCount; i++) {
//            Vector3 v = {
//                vertices[i * 3 + 0] + pos[0],
//                vertices[i * 3 + 1] + pos[1],
//                vertices[i * 3 + 2] + pos[2]
//            };
//
//            if (v.x < box.min.x) box.min.x = v.x;
//            if (v.y < box.min.y) box.min.y = v.y;
//            if (v.z < box.min.z) box.min.z = v.z;
//            if (v.x > box.max.x) box.max.x = v.x;
//            if (v.y > box.max.y) box.max.y = v.y;
//            if (v.z > box.max.z) box.max.z = v.z;
//        }
//
//        return box;
//    }
//
//    void UpdateBuffers() {
//        RecalcNormals(e_vertices, e_indices, e_normals);
//        UpdateMeshBuffer(*e_mesh, 0, e_vertices.data(), e_vertices.size() * sizeof(float), 0);
//        UpdateMeshBuffer(*e_mesh, 2, e_normals.data(), e_normals.size() * sizeof(float), 0);
//    }
//
//    void RecalcNormals(std::vector<float>& vertices, std::vector<unsigned short>& indices, std::vector<float>& normals) {
//        normals.assign(vertices.size(), 0.0f);
//
//        for (size_t i = 0; i < indices.size(); i += 3) {
//            unsigned short i0 = indices[i];
//            unsigned short i1 = indices[i + 1];
//            unsigned short i2 = indices[i + 2];
//
//            glm::vec3 v0(vertices[i0 * 3], vertices[i0 * 3 + 1], vertices[i0 * 3 + 2]);
//            glm::vec3 v1(vertices[i1 * 3], vertices[i1 * 3 + 1], vertices[i1 * 3 + 2]);
//            glm::vec3 v2(vertices[i2 * 3], vertices[i2 * 3 + 1], vertices[i2 * 3 + 2]);
//
//            glm::vec3 edge1 = v1 - v0;
//            glm::vec3 edge2 = v2 - v0;
//
//            glm::vec3 normal = glm::normalize(glm::cross(edge2, edge1));
//
//            for (unsigned int index : {i0, i1, i2}) {
//                normals[index * 3] = normal.x;
//                normals[index * 3 + 1] = normal.y;
//                normals[index * 3 + 2] = normal.z;
//            }
//        }
//
//        for (size_t i = 0; i < normals.size(); i += 3) {
//            glm::vec3 n(normals[i], normals[i + 1], normals[i + 2]);
//            n = glm::normalize(n);
//
//            normals[i] += n.x;
//            normals[i + 1] += n.y;
//            normals[i + 2] += n.z;
//        }
//    }
//};

#endif // ENTITY_HPP
