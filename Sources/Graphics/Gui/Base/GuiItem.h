#ifndef GUIITEM_H
#define GUIITEM_H

#include "imgui.h"
#include <functional>


class GuiBase
{
public:
    GuiBase() = default;
    virtual ~GuiBase() = default;

    void Render();

    virtual void PreRender() {}
    virtual void PostRender() {}
    virtual void OnRender() = 0;

    inline const ImVec2& Size() const { return _size; }
    inline const ImVec2& Pos() const { return _pos; }
    inline void SetSize(const ImVec2& value) { _size = value; }
    inline void SetPos(const ImVec2& value) { _pos = value; }


private:
    ImVec2 _size;
    ImVec2 _pos;
};


class GuiWindow: public GuiBase
{
public:
    GuiWindow(const std::string& title);
    ~GuiWindow();

    void PreRender() override;
    void OnRender() override;
    void PostRender() override;

    template<class T>
    T* CreateChild()
    {
        T* obj = new T;
        _childs.push_back(obj);
        return obj;
    }

    template<class T>
    T* FindFirstChild()
    {
        for (auto& i: _childs)
        {
            if (auto* child = dynamic_cast<T*>(i))
                return child;
        }

        return nullptr;
    }

    void AddWindowFlags(int var);
    void RemoveWindowFlags(int var);
    inline int WindowFlags() const { return _windowFlags; }

private:
    std::vector<GuiBase*> _childs;
    std::string _title;
    int _windowFlags;
};


class GuiItemRaw: public GuiBase
{
public:
    GuiItemRaw(const std::function<void()>& renderFunc);

    void OnRender() override;


private:
    const std::function<void()>& _renderFunc;
};


class GuiChildWindow: public GuiWindow
{
public:
    GuiChildWindow(const std::string& name);

    void PreRender() override;
    void PostRender() override;

    inline bool HaveBorder() const { return _border; }
    inline void SetBorder(bool enable) { _border = enable; }


private:
    std::string _name;
    bool _border;
};


#endif // GUIITEM_H
