#pragma once
#include "fgui_menu.h"
#include "FGUI.hpp"
#include "widgets/widgets.hpp"
#include "widgets/checkbox.hpp"
#include "widgets/textbox.hpp"
#include "widgets/container.hpp"
#include "widgets/combobox.hpp"
#include "widgets/button.hpp"
#include "widgets/tabpanel.hpp"
#include "misc/builder.hpp"
#include "internal/helpers.hpp"
//FGUI::CContainer container();
namespace menu 
{
        extern void RenderWindow();
//	inline void OnSetupDevice();
}
namespace WIDGETS
{
  inline std::shared_ptr<FGUI::CContainer> Container;
  inline std::shared_ptr<FGUI::CTabPanel> Tabs;
  inline std::shared_ptr<FGUI::CCheckBox> CheckBox;
  inline std::shared_ptr<FGUI::CComboBox> ComboBox;
  inline std::shared_ptr<FGUI::CButton> Button;
  inline std::shared_ptr<FGUI::CTextBox> TextBox;
} // namespace WIDGETS

