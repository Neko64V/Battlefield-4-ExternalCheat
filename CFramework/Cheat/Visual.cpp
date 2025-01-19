#include "FrameCore.h"

// いちいち "ImColor(0.f, 0.f, 0.f, 1.f);" のように書くのはめんどうなのでここで定義
ImColor BLACK = ImColor(0.f, 0.f, 0.f, 1.f);
ImColor TEXT_COLOR = ImColor(1.f, 1.f, 1.f, 1.f);

// Skeleton BoneList
static int aSkeleton[][2] = {
    { 142, 9 },
    { 9, 11 },
    { 11, 15 },
    { 142,109},
    { 109,111 },
    { 111, 115},
    { 142, 5 },
    { 5,  188},
    { 5, 197},
    { 188, 184},
    { 197, 198},
};

void CFramework::RenderInfo()
{
    // ウォーターマーク的な.
    const auto str = "Battlefield 4 External - FPS: " + std::to_string((int)ImGui::GetIO().Framerate);
    String(Vector2(5.f, 5.f), TEXT_COLOR, str.c_str());

    /*
    // FovCircle
    if (g.g_AimBot && g.g_Aim_DrawFov)
        ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(g.g_GameRect.right / 2.f, g.g_GameRect.bottom / 2.f), g.g_Aim_Fov, FOV_User);*/

    // Crosshair
    if (g.g_Crosshair)
    {
        switch (g.g_CrosshairType)
        {
        case 0: {
            ImVec2 Center = ImVec2(g.g_GameRect.right / 2, g.g_GameRect.bottom / 2);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(Center.x - g.g_CrosshairSize, Center.y), ImVec2((Center.x + g.g_CrosshairSize) + 1, Center.y), CrosshairColor, 1);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(Center.x, Center.y - g.g_CrosshairSize), ImVec2(Center.x, (Center.y + g.g_CrosshairSize) + 1), CrosshairColor, 1);
        }   break;
        case 1:
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2((float)g.g_GameRect.right / 2.f, (float)g.g_GameRect.bottom / 2.f), g.g_CrosshairSize + 1, ImColor(0.f, 0.f, 0.f, 1.f), NULL);
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2((float)g.g_GameRect.right / 2.f, (float)g.g_GameRect.bottom / 2.f), g.g_CrosshairSize, CrosshairColor, NULL);
            break;
        }
    }

    // SpectatorList
    if (g.g_SpectatorList)
    {
        if (SpectatorList.size() > 0)
            String(Vector2(g.g_GameRect.right / 2 - (ImGui::CalcTextSize("[ Spectator Found! ]").x), g.g_GameRect.top), ImColor(1.f, 0.f, 0.f, 1.f), "[ Spectator Found! ]");

        ImGui::SetNextWindowBgAlpha(SpectatorList.size() > 0 ? 0.9f : 0.35f);
        ImGui::SetNextWindowPos(ImVec2(12.f, 16.f));
        ImGui::SetNextWindowSize(ImVec2(250.f, 125.f));
        std::string title = "Spectator [" + std::to_string(SpectatorList.size()) + "]";
        ImGui::Begin(title.c_str(), &g.g_ShowMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        std::vector<std::string> spec_list = SpectatorList;

        for (auto& name : spec_list)
            ImGui::Text(name.c_str());

        ImGui::End();
    }
}

void CFramework::RenderESP()
{
    // Localの更新に失敗したらこの関数を終了
    if (!pLocal->Update())
        return;

    if (pLocal->IsInVehicle()) {
        if (!pLocal->VehicleUpdate())
            return;
    }

    // ViewMatrixとかいろいろ
    Matrix ViewMatrix = GetViewMatrix();;

    // 各種透明度を設定
    TEXT_COLOR.Value.w = GlobalAlpha;
    ESP_Default.Value.w = GlobalAlpha;
    ESP_Visible.Value.w = GlobalAlpha;
    ESP_Vehicle.Value.w = GlobalAlpha;
    ESP_Team.Value.w = GlobalAlpha;
    ESP_TeamVehicle.Value.w = GlobalAlpha;

    // Soldier
    for (auto& player : PlayerList)
    {
        CPlayer* pEntity = &player;

        if (!pEntity->Update())
            continue;

        // 距離を取得
        const float pDistance = GetDistance(pLocal->m_vecAbsOrigin, pEntity->m_vecAbsOrigin);

        // 各種チェック
        if (g.g_ESP_MaxDistance < pDistance)
            continue;
        else if (!g.g_ESP_Team && pEntity->m_iTeamNum == pLocal->m_iTeamNum)
            continue;

        // ソースエンジンの vecMin/vecMax に該当するのがこれ。あとは一緒
        Vector3 min = pEntity->GetAABB().Min + pEntity->m_vecAbsOrigin;
        Vector3 max = pEntity->GetAABB().Max + pEntity->m_vecAbsOrigin;

        int left, top, right, bottom;
        Vector2 flb, brt, blb, frt, frb, brb, blt, flt;

        Vector3 points[8] = { Vector3(min.x, min.y, min.z), Vector3(min.x, max.y, min.z), Vector3(max.x, max.y, min.z),
                    Vector3(max.x, min.y, min.z), Vector3(max.x, max.y, max.z), Vector3(min.x, max.y, max.z),
                    Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, max.z) };

        if (!WorldToScreen(ViewMatrix, g.g_GameRect, points[3], flb) || !WorldToScreen(ViewMatrix, g.g_GameRect, points[5], brt) ||
            !WorldToScreen(ViewMatrix, g.g_GameRect, points[0], blb) || !WorldToScreen(ViewMatrix, g.g_GameRect, points[4], frt) ||
            !WorldToScreen(ViewMatrix, g.g_GameRect, points[2], frb) || !WorldToScreen(ViewMatrix, g.g_GameRect, points[1], brb) ||
            !WorldToScreen(ViewMatrix, g.g_GameRect, points[6], blt) || !WorldToScreen(ViewMatrix, g.g_GameRect, points[7], flt))
            continue;

        Vector2 vec2_array[] = { flb, brt, blb, frt, frb, brb, blt, flt };
        left = flb.x;
        top = flb.y;
        right = flb.x;
        bottom = flb.y;

        for (auto j = 1; j < 8; ++j)
        {
            if (left > vec2_array[j].x)
                left = vec2_array[j].x;
            if (bottom < vec2_array[j].y)
                bottom = vec2_array[j].y;
            if (right < vec2_array[j].x)
                right = vec2_array[j].x;
            if (top > vec2_array[j].y)
                top = vec2_array[j].y;
        }

        // サイズ算出
        const int Height = bottom - top;
        const int Width = right - left;
        const int Center = (right - left) / 2.f;
        const int bScale = (right - left) / 3.f;

        // 色を決める
        ImColor color = pLocal->m_iTeamNum == pEntity->m_iTeamNum ? ESP_Team : (pEntity->IsVisible() ? ESP_Visible : ESP_Default);

        // Line
        if (g.g_ESP_Line)
            DrawLine(Vector2(g.g_GameRect.right / 2.f, g.g_GameRect.bottom), Vector2(right - (Width / 2), bottom), color, 1.f);

        // Box
        if (g.g_ESP_Box)
        {
            // BoxFilled
            if (g.g_ESP_BoxFilled)
                ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(left, top), ImVec2(right, bottom), ESP_Shadow);

            // Box - Shadow
            DrawLine(Vector2(left - 1, top - 1), Vector2(right + 2, top - 1), ESP_Shadow, 1.f);
            DrawLine(Vector2(left - 1, top), Vector2(left - 1, bottom + 2), ESP_Shadow, 1.f);
            DrawLine(Vector2(right + 1, top), Vector2(right + 1, bottom + 2), ESP_Shadow, 1.f);
            DrawLine(Vector2(left - 1, bottom + 1), Vector2(right + 1, bottom + 1), ESP_Shadow, 1.f);

            switch (g.g_ESP_BoxType)
            {
            case 0:
                // Main
                DrawLine(Vector2(left, top), Vector2(right, top), color, 1.f);
                DrawLine(Vector2(left, top), Vector2(left, bottom), color, 1.f);
                DrawLine(Vector2(right, top), Vector2(right, bottom), color, 1.f);
                DrawLine(Vector2(left, bottom), Vector2(right + 1, bottom), color, 1.f);
                break;
            case 1:
                DrawLine(Vector2(left, top), Vector2(left + bScale, top), color, 1.f); // Top
                DrawLine(Vector2(right, top), Vector2(right - bScale, top), color, 1.f);
                DrawLine(Vector2(left, top), Vector2(left, top + bScale), color, 1.f); // Left
                DrawLine(Vector2(left, bottom), Vector2(left, bottom - bScale), color, 1.f);
                DrawLine(Vector2(right, top), Vector2(right, top + bScale), color, 1.f); // Right
                DrawLine(Vector2(right, bottom), Vector2(right, bottom - bScale), color, 1.f);
                DrawLine(Vector2(left, bottom), Vector2(left + bScale, bottom), color, 1.f); // Bottom
                DrawLine(Vector2(right, bottom), Vector2(right - bScale, bottom), color, 1.f);
                break;
            }
        }

        if (g.g_ESP_Skeleton)
        {
            AllBones bx = pEntity->GetBoneList(), *bone = &bx;

            // Skeleton
            for (int j = 0; j < 11; j++)
            {
                if (Vec3_Empty(bone->bone[aSkeleton[j][0]].pos) || Vec3_Empty(bone->bone[aSkeleton[j][1]].pos))
                    break;

                Vector2 vOut0{}, vOut1{};
                if (WorldToScreen(ViewMatrix, g.g_GameRect, bone->bone[aSkeleton[j][0]].pos, vOut0) && WorldToScreen(ViewMatrix, g.g_GameRect, bone->bone[aSkeleton[j][1]].pos, vOut1))
                    DrawLine(vOut0, vOut1, color, 1);
            }

            // Head Circle
            if (!Vec3_Empty(bone->bone[104].pos) || !Vec3_Empty(bone->bone[142].pos)) {
                Vector2 vHead{}, vNeck{};
                if (WorldToScreen(ViewMatrix, g.g_GameRect, bone->bone[104].pos, vHead) && WorldToScreen(ViewMatrix, g.g_GameRect, bone->bone[142].pos, vNeck)) {
                    Circle(vHead, (vNeck.y - vHead.y) * 1.5f, color);
                }
            }
        }

        // Healthbar
        if (g.g_ESP_HealthBar)
            HealthBar(left - 4.f, bottom, 2, -Height, pEntity->m_fHealth, pEntity->m_fMaxHealth); // Health

        // Distance
        if (g.g_ESP_Distance) {
            const std::string DistStr = std::to_string((int)pDistance) + "m";
            StringEx(Vector2(right - Center - (ImGui::CalcTextSize(DistStr.c_str()).x / 2.f), bottom + 1), TEXT_COLOR, ImGui::GetFontSize(), DistStr.c_str());
        }

        // Name
        if (g.g_ESP_Name)
            StringEx(Vector2(right - Center - (ImGui::CalcTextSize(pEntity->pName.c_str()).x / 2.f), top - ImGui::GetFontSize() - 1), TEXT_COLOR, ImGui::GetFontSize(), pEntity->pName.c_str());
    }

    // Vehicle
    if (g.g_ESP_Vehicle)
    {
        for (auto& vehicle : VehicleList)
        {
            CPlayer* pVehicle = &vehicle;

            if (!pVehicle->Update() || !pVehicle->VehicleUpdate())
                continue;

            // 距離を取得
            const float pDistance = GetDistance(pLocal->m_vecAbsOrigin, pVehicle->m_vecAbsOrigin);

            // 各種チェック
            if (g.g_ESP_MaxDistance < pDistance)
                continue;
            else if (!g.g_ESP_Team && pVehicle->m_iTeamNum == pLocal->m_iTeamNum)
                continue;

            // Like SourceEngine games
            AxisAlignedBox VehicleAABB = pVehicle->GetVehicleAABB();
            Vector3 CurrentPosition = Vec3_Empty(pVehicle->m_vecAbsOrigin) ? pVehicle->m_vecAbsOrigin : pVehicle->m_vecAbsOrigin;
            Vector3 min = VehicleAABB.Min + CurrentPosition;
            Vector3 max = VehicleAABB.Max + CurrentPosition;

            int left, top, right, bottom;
            Vector2 flb, brt, blb, frt, frb, brb, blt, flt;

            Vector3 points[8] = { Vector3(min.x, min.y, min.z), Vector3(min.x, max.y, min.z), Vector3(max.x, max.y, min.z),
                        Vector3(max.x, min.y, min.z), Vector3(max.x, max.y, max.z), Vector3(min.x, max.y, max.z),
                        Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, max.z) };

            if (!WorldToScreen(ViewMatrix, g.g_GameRect, points[3], flb) || !WorldToScreen(ViewMatrix, g.g_GameRect, points[5], brt) ||
                !WorldToScreen(ViewMatrix, g.g_GameRect, points[0], blb) || !WorldToScreen(ViewMatrix, g.g_GameRect, points[4], frt) ||
                !WorldToScreen(ViewMatrix, g.g_GameRect, points[2], frb) || !WorldToScreen(ViewMatrix, g.g_GameRect, points[1], brb) ||
                !WorldToScreen(ViewMatrix, g.g_GameRect, points[6], blt) || !WorldToScreen(ViewMatrix, g.g_GameRect, points[7], flt))
                continue;

            Vector2 vec2_array[] = { flb, brt, blb, frt, frb, brb, blt, flt };
            left = flb.x;
            top = flb.y;
            right = flb.x;
            bottom = flb.y;

            for (auto j = 1; j < 8; ++j)
            {
                if (left > vec2_array[j].x)
                    left = vec2_array[j].x;
                if (bottom < vec2_array[j].y)
                    bottom = vec2_array[j].y;
                if (right < vec2_array[j].x)
                    right = vec2_array[j].x;
                if (top > vec2_array[j].y)
                    top = vec2_array[j].y;
            }

            // サイズ算出
            const int Height = bottom - top;
            const int Width = right - left;
            const int Center = (right - left) / 2.f;

            // 色を決める
            ImColor color = pLocal->m_iTeamNum == pVehicle->m_iTeamNum ? ESP_TeamVehicle : ESP_Vehicle;

            bool isSoldier = pVehicle->m_fHealth != 0.f ? true : false;

            float Health = isSoldier ? pVehicle->m_fVehicleHealth : pVehicle->m_fHealth;
            float MaxHealth = isSoldier ? pVehicle->m_fVehicleMaxHealth : pVehicle->m_fMaxHealth;

            if (!isSoldier)
            {
                // Line
                if (g.g_ESP_Line)
                    DrawLine(Vector2(g.g_GameRect.right / 2.f, g.g_GameRect.bottom), Vector2(right - (Width / 2), bottom), color, 1.f);

                // Box
                if (g.g_ESP_Box)
                {
                    // Box - Shadow
                    DrawLine(Vector2(left - 1, top - 1), Vector2(right + 2, top - 1), ESP_Shadow, 1.f);
                    DrawLine(Vector2(left - 1, top), Vector2(left - 1, bottom + 2), ESP_Shadow, 1.f);
                    DrawLine(Vector2(right + 1, top), Vector2(right + 1, bottom + 2), ESP_Shadow, 1.f);
                    DrawLine(Vector2(left - 1, bottom + 1), Vector2(right + 1, bottom + 1), ESP_Shadow, 1.f);

                    // Main
                    DrawLine(Vector2(left, top), Vector2(right, top), color, 1.f);
                    DrawLine(Vector2(left, top), Vector2(left, bottom), color, 1.f);
                    DrawLine(Vector2(right, top), Vector2(right, bottom), color, 1.f);
                    DrawLine(Vector2(left, bottom), Vector2(right + 1, bottom), color, 1.f);
                }

                // Healthbar
                if (g.g_ESP_HealthBar) {
                    HealthBar(left - 4.f, bottom, 2, -Height, pVehicle->m_fVehicleHealth, pVehicle->m_fVehicleMaxHealth); // Health
                }

                // Distance
                if (g.g_ESP_Distance) {
                    const std::string DistStr = std::to_string((int)pDistance) + "m";
                    StringEx(Vector2(right - Center - (ImGui::CalcTextSize(DistStr.c_str()).x / 2.f), bottom + 1), TEXT_COLOR, ImGui::GetFontSize(), DistStr.c_str());
                }

                // Name
                if (g.g_ESP_Name) {
                    StringEx(Vector2(right - Center - (ImGui::CalcTextSize(pVehicle->pName.c_str()).x / 2.f), top - ImGui::GetFontSize() - 1), TEXT_COLOR, ImGui::GetFontSize(), pVehicle->pName.c_str());
                }
            }
            else
            {
                // Skeleton
                if (g.g_ESP_Skeleton && isSoldier)
                {
                    AllBones bx = pVehicle->GetBoneList(), * bone = &bx;

                    // Skeleton
                    for (int j = 0; j < 11; j++)
                    {
                        if (Vec3_Empty(bone->bone[aSkeleton[j][0]].pos) || Vec3_Empty(bone->bone[aSkeleton[j][1]].pos))
                            break;

                        Vector2 vOut0{}, vOut1{};
                        if (WorldToScreen(ViewMatrix, g.g_GameRect, bone->bone[aSkeleton[j][0]].pos, vOut0) && WorldToScreen(ViewMatrix, g.g_GameRect, bone->bone[aSkeleton[j][1]].pos, vOut1))
                            DrawLine(vOut0, vOut1, color, 1);
                    }

                    // Head Circle
                    if (!Vec3_Empty(bone->bone[104].pos) || !Vec3_Empty(bone->bone[142].pos)) {
                        Vector2 vHead{}, vNeck{};
                        if (WorldToScreen(ViewMatrix, g.g_GameRect, bone->bone[104].pos, vHead) && WorldToScreen(ViewMatrix, g.g_GameRect, bone->bone[142].pos, vNeck)) {
                            Circle(vHead, (vNeck.y - vHead.y) * 1.5f, color);
                        }
                    }
                }
            }
        }
    }
}