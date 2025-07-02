#ifndef SHADERMANAGER_HPP
#define SHADERMANAGER_HPP

#include "external/glm/glm.hpp"
#include "bEngineComponent.hpp"
#include "raylib.h"

namespace Engine {

class ShaderManager : public Engine::bEngineComponent<Shader> {
private:
    Shader materialPreviewShader;
    Shader renderShader;
    Shader solidShader;

    int uBaseColorLoc, uLightDirLoc;
    float lightColor[3];
    float lightDirection[3];
    glm::vec3 lightDir;
public:
    ShaderManager() = default;
    ~ShaderManager() override = default;

    void init() override;
    Shader initShader();
    void set(Shader& prev) override;
    void clean() override;
};

}

#endif //SHADERMANAGER_HPP
