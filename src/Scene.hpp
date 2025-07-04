#ifndef SCENE_HPP
#define SCENE_HPP

#include "Context.hpp"
#include "external/rlImGui/rlImGui.h"
#include "external/imgui/imgui.h"
// #include "rlights.h"
#include "external/glm/glm.hpp"

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
    // void color
    ImVec4 voidCol;
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
public:
    explicit Scene(std::shared_ptr<Context>& context);
    Scene() = default;
    ~Scene() override = default;

    void init() override;
    void process() override;
    void draw() override;
    void clean() override;

    Color ImVecToColor(ImVec4 _color);
};

#endif //SCENE_HPP
