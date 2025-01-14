#include "FrameCore.h"

// ImGui::Combo/ImGui::List等で使う文字列群
const char* BoxTypeList[] = { "Simple", "Cornered" };
const char* AimTypeList[] = { "FOV", "Distance" };
const char* AimModeList[] = { "Memory", "Mouse" };
const char* AimBoneList[] = { "Head", "Chest" };
const char* AimKeyTypeList[] = { "and", "or" };
const char* CrosshairList[] = { "Cross", "Circle" };
std::vector<std::string> MenuSelectList = { "Visual", "Misc", "System" };

// チートのメニュー
void CFramework::RenderMenu()
{
    // Setup
    ImVec4* colors = ImGui::GetStyle().Colors;
    ImGuiStyle& style = ImGui::GetStyle();
    static float DefaultSpacing = style.ItemSpacing.y;
    static int Index = 0;
    static int BindingID = 0;

    ImGui::SetNextWindowBgAlpha(0.975f);
    ImGui::SetNextWindowSize(ImVec2(725.f, 450.f));
    ImGui::Begin("Counter-Strike: Source [ EXTERNAL ]", &g.g_ShowMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    //---// Clild 0 //-----------------------------------//
    ImGui::BeginChild("##SelectChild", ImVec2(150.f, ImGui::GetContentRegionAvail().y), false);

    ImGui::NewLine();

    for (int i = 0; i < MenuSelectList.size(); i++)
    {
        if (ImGui::CustomButton(MenuSelectList[i].c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 35.f), Index == i ? true : false))
            Index = i;
    }

    ImGui::EndChild();
    //---// Clild 0 End //-------------------------------//

    ImGui::SameLine();

    //---// Clild 1 //-----------------------------------//
    ImGui::BeginChild("##ContextChild", ImVec2(ImGui::GetContentRegionAvail()), false);

    //---// Left //--------------------------------------//
    ImGui::BeginChild("##LeftChild", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 16.f, ImGui::GetContentRegionAvail().y), false);

    ImGui::Spacing();

    switch (Index)
    {
    case 0: // visual
        ImGui::Text("Visual");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("ESP", &g.g_ESP);
        ImGui::Checkbox("Vehicle ESP", &g.g_ESP_Vehicle);
        ImGui::Checkbox("Team ESP", &g.g_ESP_Team);

        ImGui::NewLine();
        ImGui::Spacing();

        ImGui::Text("ESP Options");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Box", &g.g_ESP_Box);
        ImGui::Checkbox("BoxFilled", &g.g_ESP_BoxFilled);
        ImGui::Checkbox("Line", &g.g_ESP_Line);
        ImGui::Checkbox("Skeleton", &g.g_ESP_Skeleton);
        ImGui::Checkbox("HealthBar", &g.g_ESP_HealthBar);
        ImGui::Checkbox("Distance", &g.g_ESP_Distance);
        ImGui::Checkbox("Name", &g.g_ESP_Name);
        break;
    case 1: // misc
        ImGui::Text("Misc");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::NewLine();

        ImGui::Text("NoRecoil");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Checkbox("NoReocil", &g.g_NoRecoil);
        ImGui::CustomSliderFloat("Recoil percentage", "##recoilptg", &g.g_NoRecoilVal, 0.f, 1.f);
        break;
    case 2: // system
        ImGui::Text("System");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Checkbox("StreamProof", &g.g_StreamProof);

        ImGui::Spacing();
        ImGui::NewLine();

        ImGui::Text("Crosshair");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("Crosshair", &g.g_Crosshair);
        ImGui::CustomSliderInt("CrosshairSize", "##SizeCH", &g.g_CrosshairSize, 1, 10);
        ImGui::ColorEdit4("Color##C", &CrosshairColor.Value.x);
        ImGui::Combo("Type##C", &g.g_CrosshairType, CrosshairList, IM_ARRAYSIZE(CrosshairList));
        break;
    default:
        break;
    }

    ImGui::EndChild();
    //---------------------------------------------------//

    ImGui::SameLine();

    //---// Right //--------------------------------------//
    ImGui::BeginChild("##RightChild", ImVec2(ImGui::GetContentRegionAvail()), false);

    ImGui::Spacing();

    switch (Index)
    {
    case 0: // visual
        ImGui::Text("ESP Setting");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::CustomSliderFloat("Distance", "##Dist", &g.g_ESP_MaxDistance, 100.f, 2000.f);

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Combo("BoxType", &g.g_ESP_BoxType, BoxTypeList, IM_ARRAYSIZE(BoxTypeList));

        ImGui::NewLine();
        ImGui::Spacing();

        ImGui::Text("ESP Colors");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::BulletText("Main Color");
        ImGui::ColorEdit4("Default", &ESP_Default.Value.x);
        ImGui::ColorEdit4("Visible", &ESP_Visible.Value.x);

        ImGui::ColorEdit4("Vehicle", &ESP_Vehicle.Value.x);

        ImGui::ColorEdit4("Team", &ESP_Team.Value.x);
        ImGui::ColorEdit4("Team Vehicle", &ESP_TeamVehicle.Value.x);

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::CustomSliderFloat("ESP Alpha", "##GlobalAplha", &GlobalAlpha, 0.3f, 1.f, "%.2f");
        ImGui::CustomSliderFloat("ESP Shadow & BoxFilled", "##ESPShadow_", &ESP_Shadow.Value.w, 0.1f, 0.5f, "%.2f");
        break;
    case 1: // misc

        break;
    case 2: // system
        ImGui::Text("Exit");
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("Exit", ImVec2(ImGui::GetContentRegionAvail().x, 30.f)))
            g.g_Run = false;
        break;
    default:
        break;
    }

    ImGui::EndChild();
    ImGui::EndChild();
    //---------------------------------------------------//

    ImGui::End();
}