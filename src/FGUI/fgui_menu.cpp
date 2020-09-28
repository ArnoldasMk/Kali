#include "fgui_menu.h"
#include "FGUI.hpp"
#include "widgets/widgets.hpp"
#include "widgets/checkbox.hpp"
#include "widgets/textbox.hpp"
#include "widgets/container.hpp"
#include "widgets/combobox.hpp"
#include "widgets/button.hpp"


void menu::RenderWindow()
{

    // builder pattern 
    static FGUI::CBuilder ptrnBuilder;

    WIDGETS::Container = std::make_shared<FGUI::CContainer>();
    ptrnBuilder.Widget(WIDGETS::Container).Title("Container 1").Position(200, 200).Size(735, 435).Key(KEY_L).Font("Tahoma", 12, true, 1);
    {
      WIDGETS::Tabs = std::make_shared<FGUI::CTabPanel>();
      ptrnBuilder.Widget(WIDGETS::Tabs).Position(0, 16).Font("Tahoma", 12, true, 1).Tab("Tab 1").Tab("Tab 2").Tab("Tab 3").Tab("Tab 4").Tab("Tab 5").SpawnIn(WIDGETS::Container, false);

      WIDGETS::CheckBox = std::make_shared<FGUI::CCheckBox>();
      ptrnBuilder.Widget(WIDGETS::CheckBox).Title("CheckBox 1").Position(15, 65).Font("Tahoma", 13).Medium(WIDGETS::Tabs, 0).SpawnIn(WIDGETS::Container, false);

      WIDGETS::ComboBox = std::make_shared<FGUI::CComboBox>();
      ptrnBuilder.Widget(WIDGETS::ComboBox).Title("ComboBox 1").Position(15, (65 + 25)).Size(250, 25).Entry("Entry 1").Entry("Entry 2").Entry("Entry 3").Entry("Entry 4").Entry("Entry 5").Font("Tahoma", 13).Medium(WIDGETS::Tabs, 1).SpawnIn(WIDGETS::Container, false);

      WIDGETS::Button = std::make_shared<FGUI::CButton>();
      ptrnBuilder.Widget(WIDGETS::Button).Title("Button 1").Position(15, (90 + 25)).Size(250, 20).Font("Tahoma", 12, true, 1).Medium(WIDGETS::Tabs, 1).SpawnIn(WIDGETS::Container, false);

      WIDGETS::TextBox = std::make_shared<FGUI::CTextBox>();
      ptrnBuilder.Widget(WIDGETS::TextBox).Title("TextBox 1").Position(15, (115 + 25)).Size(250, 20).Font("Tahoma", 13).Medium(WIDGETS::Tabs, 1).SpawnIn(WIDGETS::Container, false);
    }
    WIDGETS::Container->Render();  
  }

