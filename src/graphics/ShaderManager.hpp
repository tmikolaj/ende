#ifndef SHADERMANAGER_HPP
#define SHADERMANAGER_HPP

#include <external/glm/glm.hpp>
#include "raylib.h"

enum currentShader {
    SOLID = 0,
    M_PREVIEW = 1,
    RENDER = 2,
    WIREFRAME = 3
};

struct Context;

namespace Engine {

class ShaderManager {
private:
    Shader renderShader;
    Shader materialPreviewShader;
    Shader solidShader;
public:
    // Void colors for specific shader
    Color voidColSolid;
    Color voidColPreview;
    Color voidColRender;
    Color voidColWireframe;

    // Solid shader variables
    float lightColor[3];
    glm::vec3 lightDir;
    float lightDirection[3];
    int uLightDirLoc;
    int uBaseColorLoc;
    // Render shader variables
    int ambientLoc;
    float ambientColor[4];
    int currLightsCount;
    const unsigned short MAX_LIGHTS_COUNT = 32;

    ShaderManager() = default;
    ~ShaderManager() = default;

    void init();
    void clean() const;
    Shader* getShader(int shaderIndex);

    void handleSetShaderValue(int shaderIndex, std::shared_ptr<Context>& p_context);
    void handleShaderSelection(int& currentSh, bool ignoreWireframe = false, bool ignoreRender = false, bool ignoreMaterial = false, bool ignoreSolid = false);
    void handleBackgroundClearing(int shaderIndex) const;

    // Solid Shader functions
    void changeColor() const;
    void changeDirection();

    // Render Shader functions
    void updateCamPos(std::shared_ptr<Context>& p_context) const;
    void updateLights() const;
    void updateLightValues(std::shared_ptr<Context>& p_context) const;
    void updateSingleLight(std::shared_ptr<Context>& p_context, int lightIndex) const;
};

}

#endif //SHADERMANAGER_HPP
