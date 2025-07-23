#ifndef SCENEEDITORSTATE_HPP
#define SCENEEDITORSTATE_HPP

#include <typeinfo>

#include "raylib.h"

#include "external/imgui/imgui.h"

#include "../core/Context.hpp"
#include "../math/Noise.hpp"

#include "../entities/CustomMeshFactory.hpp"

#include "../debug/NormalVisualizer.hpp"
#include "../debug/UVChecker.hpp"

enum entity {
    TERRAIN = 0,
    ROCK = 1,
};

class SceneEditorState : public BaseState {
private:
    // render/draw variables
    RenderTexture2D sceneTexture;
    RenderTexture2D gizmoTexture;
    int selectedEntity;
    int selectedLight;
    int currentSh;
    std::string curr_m;
    int entityToAdd;
    bool isGizmoDragged;
    bool isGizmoHovered;
    Shader* shader;

    // lights
    int typeToAdd;

    // collision (to check if the entity was hit)
    Ray ray;

    Noise noise;
    CustomMeshFactory customMeshes;
    NormalVisualizer normalController;
    UVChecker uvController;

    bool shouldOpenContextPopup;
    bool contextForEntity;
    float hoverDelay;

    // imgui variables
    // rename
    char renameBuffer[40];
    bool openRenamePopup;
    // scene controls
    float chunkSize;
    bool showGrid;
    // camera
    float zoomSpeed;
    float distance;
    // void colors
    ImVec4 voidColSol;
    ImVec4 voidColMat;
    ImVec4 voidColRen;
    ImVec4 voidColWir;
    // solid shader
    bool colorChanged;
    bool dirChanged;
    // mesh controls
    bool positionChanged;
    bool showWires;
    bool toggleWireframe;
    // selection colors
    ImVec4 onSelectionMeshColor;
    ImVec4 onSelectionWiresColor;
    // program settings
    bool useStrictSearch;
    bool showEdgeNormals;
    bool showVertexNormals;
    bool showFaceNormals;
    float length;
    ImVec4 normalsColor;
    bool showUV;
public:
    SceneEditorState();
    ~SceneEditorState() override = default;

    void init(std::shared_ptr<Context>& m_context) override;
    void process(std::shared_ptr<Context>& m_context) override;
    void draw(std::shared_ptr<Context>& m_context) override;
    void clean(std::shared_ptr<Context>& m_context) override;

    Color ImVecToColor(ImVec4 _color);
    void HandleMouseSelection(const int& btn, int& selectedEntity, bool& e_context, const Camera3D& _camera, const std::shared_ptr<Context>& _m_context, Ray _ray);
    bool checkIfHasShaper(const std::type_info& type, std::shared_ptr<Context>& m_context) const;
};

#endif // SCENEEDITORSTATE_HPP
