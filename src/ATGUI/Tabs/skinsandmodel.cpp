#include "skinsandmodel.h"

#include "../../Utils/xorstring.h"
#include "../../ImGUI/imgui.h"
#include "../../settings.h"
#include "skinstab.h"
#include "modelstab.h"

const char* skinORmodel[] = {
                            "Skins",
                            "Models"
                            };
                            
void SkinsAndModel::RenderTab()
{
    ImGui::PushItemWidth(-1);
    ImGui::Combo(XORSTR("##SkinsOrModels"), (int*)&Settings::SkinOrModel::skin_or_model, skinORmodel, IM_ARRAYSIZE(skinORmodel));

    
    switch(Settings::SkinOrModel::skin_or_model)
    {
        case SkinAndModel::Skins:
            Skins::RenderTab();
            break;
        case SkinAndModel::Model:
            Models::RenderTab();
        break;
    }
    ImGui::PopItemWidth();
}