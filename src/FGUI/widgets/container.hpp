//
// FGUI - feature rich graphical user interface
//

#ifndef FGUI_CONTAINER_HH
#define FGUI_CONTAINER_HH

// includes
#include <vector>
#include <iomanip>

// library includes
#include "widgets.hpp"

namespace FGUI
{
  using CURSOR_STYLE = enum struct ESCursorStyle : int {
    NONE = 0,
    ARROW,
    HAND,
    IBEAM,
    PIPETTE,
    MOVE
  };

  class CContainer : public FGUI::CWidgets
  {
  public:
    CContainer();

    // @brief: handle container rendering
    void Render();

    // @brief: save container state into a file
    // @args: std::string file = file name/path
    void SaveToFile(std::string file);

    // @brief: load container state from a file
    // @args: std::string file = file name/path
    void LoadFromFile(std::string file);

    // @brief: toggles on or off the container
    // @args: bool state = container state (on/off)
    void SetState(bool state);

    // @brief: gets the state of the container
    bool GetState();

    // @brief: enables/disables groupbox scrollbar
    // @args: bool state = scrollbar state (on/off)
    void SetScrollBarState(bool state);

    // @brief: gets the state of the scrollbar (if it's enabled or not)
    bool GetScrollBarState();

    // @brief: get the widget scroll offset (amount of sorts)
    int GetScrollOffset();

    // @brief: focus on a widget
    // @args: std::shared_ptr<FGUI::CWidgets> widget = widget to focus
    void SetFocusedWidget(std::shared_ptr<FGUI::CWidgets> widget);

    // @brief: returns the widget being focused
    std::shared_ptr<FGUI::CWidgets> GetFocusedWidget();

    // @brief: adds a function callback for the container (it will call the function when the container is visible)
    // @args: std::function<void()> callback = function instance
    void AddCallback(std::function<void()> callback);

    // @brief: sets a custom key to toggle on/off the container, this only works for the main container (window)
    // @args: unsigned int key = toggle key
    void SetKey(unsigned int key);

    // @brief: get the current toggle key of the container
    unsigned int GetKey();

    // @brief: add a new widget inside the groupbox
    // @args: std::shared_ptr<FGUI::CWidgets> widget = widget
    // bool padding = enable/disable padding
    void AddWidget(std::shared_ptr<FGUI::CWidgets> widget, bool padding);

    // @brief: sets the current Cursor style
    // @args: FGUI::CURSOR_STYLE cursor = cursor style
    void SetCursor(FGUI::CURSOR_STYLE cursor);

    // @brief: populate widget geometry (draw widget)
    // @args: FGUI::WIDGET_STATUS status = widget status (HOVERED, etc)
    void Geometry(FGUI::WIDGET_STATUS status) override;

    // @brief: handle update operations on the widget
    void Update() override;

    // @brief: handle input inside the widget
    void Input() override;

    // @brief: save the widget state
    // @args: nlohmann::json module = json module
    void Save(nlohmann::json& module) override;

    // @brief: load the widget state
    // @args: nlohmann::json module = json module
    void Load(nlohmann::json& module) override;

    // @brief: handle widget tooltips
    void Tooltip() override;

  protected:

    // @brief: handle dynamic Cursors drawing
    void Cursor();

  private:
    bool m_bIsOpened;
    std::string m_strConfigName;
    int m_nCursorStyle;
    bool m_bScrollBarState;
    unsigned int m_uiKey;
    std::shared_ptr<FGUI::CWidgets> m_pFocusedWidget;
    bool m_bIsFocusingOnWidget;
    int m_iWidgetScrollOffset;
    std::vector<std::shared_ptr<FGUI::CWidgets>> m_prgpWidgets;
    std::function<void()> m_fnctCallback;
  };

} // namespace FGUI

#endif // FGUI_CONTAINER_HH