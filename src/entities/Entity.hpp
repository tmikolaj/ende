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
		if (e_mesh->texcoords != nullptr) {
			auto rawTexcoords = static_cast<float*>(e_mesh->texcoords);
			e_texcoords.assign(rawTexcoords, rawTexcoords + e_mesh->vertexCount * 2);
		}

  		e_colorValues = { 1.0f, 1.0f, 1.0f, 1.0f };
  		e_color = ImVecToColor(e_colorValues);

  		e_visible = true;

		e_canvas = LoadRenderTexture(512, 512);

		BeginTextureMode(e_canvas);
		ClearBackground(WHITE);
		EndTextureMode();

		e_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = e_canvas.texture;

  	}
    Entity(Mesh _mesh, const std::string& _name, const std::string& _type) : Entity(LoadModelFromMesh(_mesh), _name, _type) {
		UnloadRenderTexture(e_canvas);
    }

    Entity(Entity&&) noexcept = default;
    Entity& operator=(Entity&&) noexcept = default;

    virtual ~Entity() = default;

	Model e_model;
	Mesh* e_mesh;

	std::string e_name;

	ImVec4 e_colorValues;
	Color e_color;

	RenderTexture2D e_canvas;

	bool e_visible;

	std::vector<Shaper*> e_shapers;

	bool e_seedEnable;
	int e_seed;

	BoundingBox e_boundingBox;
	float e_position[3];

	std::vector<float> e_vertices;
	std::vector<float> e_normals;
	std::vector<unsigned short> e_indices;
	std::vector<float> e_texcoords;

	std::string e_type;

	virtual BoundingBox GenMeshBoundingBox(const Mesh& mesh, float pos[3]) = 0;
	virtual void UpdateBuffers() = 0;
	virtual void RecalcNormals() = 0;
	static Color ImVecToColor(const ImVec4& toConvert) {
	    return (Color){
	        static_cast<unsigned char>(toConvert.x * 255.0f),
	        static_cast<unsigned char>(toConvert.y * 255.0f),
	        static_cast<unsigned char>(toConvert.z * 255.0f),
	        255
	    };
	}
};

#endif // ENTITY_HPP
