#ifndef FONTMANAGER_HPP
#define FONTMANAGER_HPP

#include "imgui.h"

namespace Engine {

class FontManager {
private:
    ImFont* xs;
    ImFont* sm;
    ImFont* md;
    ImFont* lg;
    ImFont* xl;
    ImFont* xxl;
    ImFont* title;
public:
    FontManager() = default;
    ~FontManager() = default;

    void init();

    void setXS() const;
    void setSM() const;
    void setMD() const;
    void setLG() const;
    void setXL() const;
    void setXXL() const;
    void setTITLE() const;

    ImFont* getXS() const;
    ImFont* getSM() const;
    ImFont* getMD() const;
    ImFont* getLG() const;
    ImFont* getXL() const;
    ImFont* getXXL() const;
    ImFont* getTITLE() const;
};

}

#endif //FONTMANAGER_HPP
