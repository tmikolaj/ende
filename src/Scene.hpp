#ifndef SCENE_HPP
#define SCENE_HPP

#include "Context.hpp"
#include "external/rlImGui/rlImGui.h"
#include "external/imgui/imgui.h"
#include "Noise.hpp"
// #include "rlights.h"
#include "external/glm/glm.hpp"
#include "Entities/TerrainType.hpp"
#include "Entities/RockType.hpp"
#include "Shapers/Shaper.hpp"
#include "Shapers/SubdivisionShaper.hpp"
#include "Extras/CustomMeshes.hpp"
#include "Debug/Normals.hpp"

enum currentShader {
    SOLID = 0,
    M_PREVIEW = 1,
    RENDER = 2,
    WIREFRAME = 3
};

class Scene : public Engine::bStateTemplate {
private:
    std::shared_ptr<Context> m_context;

    // render/draw variables
    int selectedEntity;
    int currentSh;
    Shader solidShader;
    Shader materialPreviewShader;
    Shader renderShader;
    std::string curr_m;

    // shader related variables
    // solid shader
    float lightColor[3];
    glm::vec3 lightDir;
    float lightDirection[3];
    int uLightDirLoc;
    int uBaseColorLoc;

    // camera related variables
    Camera3D camera;

    // collision (to check if the entity was hit)
    Ray ray;

    Noise noise;
    CustomMeshes customMeshes;
    Normals normalController;

    bool shouldOpenContextPopup;
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
public:
    explicit Scene(std::shared_ptr<Context>& context);
    Scene() = default;
    ~Scene() override = default;

    void init() override;
    void process() override;
    void draw() override;
    void clean() override;

    Color ImVecToColor(ImVec4 _color);
    void HandleMouseSelection(const int& btn, int& selectedEntity, bool& e_context, const Camera3D& _camera, const std::shared_ptr<Context>& _m_context, Ray _ray);
};

#endif //SCENE_HPP
