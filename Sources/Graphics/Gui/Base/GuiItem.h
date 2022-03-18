#ifndef GUIITEM_H
#define GUIITEM_H

#include "imgui.h"
#include <functional>


class GuiBase
{
public:
    GuiBase(const std::string& name);
    virtual ~GuiBase() = default;

    virtual void Render() = 0;

    inline const ImVec2& Size() const { return _size; }
    inline const ImVec2& Pos() const { return _pos; }
    inline const std::string& Name() const { return _name; }

    inline void SetSize(const ImVec2& value) { _size = value; }
    inline void SetPos(const ImVec2& value) { _pos = value; }
    inline void SetName(const std::string& name) { _name = name; }


private:
    std::string _name;
    ImVec2 _size;
    ImVec2 _pos;
};


class GuiBaseRaw: public GuiBase
{
public:
    GuiBaseRaw(const std::function<void()>& renderFunc);

    void Render();


private:
    const std::function<void()>& _renderFunc;
};


#endif // GUIITEM_H
