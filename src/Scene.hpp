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
    ImVec4 onSelectionMeshColor;
    ImVec4 onSelectionWiresColor;
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
    float zoomSpeed;
    float distance;

    // scene controls
    float chunkSize;
    bool showWires;
    bool toggleWireframe;
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
