#include "FrameCore.h"

const int ReadCount = 72;

struct Player
{
    uintptr_t address[ReadCount];
};

void CFramework::UpdateList()
{
    while (g.g_Run)
    {
        std::vector<CPlayer> player_list;
        std::vector<CPlayer> vehicle_list;
        std::vector<std::string> spectator_list;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // ClientGameContext
        uintptr_t ClientGameContext = m.Read<uintptr_t>(m.m_gProcessBaseAddr + offset::ClientgameContext);
        uintptr_t PlayerManager = m.Read<uintptr_t>(ClientGameContext + offset::PlayerManager);
        uintptr_t EntityList = m.Read<uintptr_t>(PlayerManager + offset::ClientPlayer);

        // Local
        pLocal->address = m.Read<uintptr_t>(PlayerManager + offset::LocalPlayer);

        // 1RPMで全プレイヤーのポインタを読み取る
        Player player = m.Read<Player>(EntityList);

        for (int i = 0; i < ReadCount; i++)
        {
            // ポインタの有効性チェック
            if (player.address[i] != NULL && player.address[i] != pLocal->address)
            {
                CPlayer p{};
                p.address = player.address[i];
                if (!p.Update())
                    continue;

                if (p.IsSpectator()) {
                    spectator_list.push_back(p.GetName());
                }
                else if (p.IsInVehicle()) {
                    p.pName = p.GetName();
                    vehicle_list.push_back(p);
                }
                else {
                    p.pName = p.GetName();
                    player_list.push_back(p);
                }
            }
        }

        PlayerList = player_list;
        VehicleList = vehicle_list;
        SpectatorList = spectator_list;
        player_list.clear();
        vehicle_list.clear();
        spectator_list.clear();
    }
}

Matrix CFramework::GetViewMatrix()
{
    uintptr_t GameRenderer = m.Read<uintptr_t>(m.m_gProcessBaseAddr + offset::GameRenderer);

    if (!GameRenderer)
        return Matrix();

    uintptr_t RenderView = m.Read<uintptr_t>(GameRenderer + 0x60);

    return m.Read<Matrix>(RenderView + 0x420);
}

void CFramework::MiscAll()
{
    if (!pLocal->Update())
        return;

    if (g.g_NoRecoil)
        NoRecoil();
}

void CFramework::NoRecoil()
{
    if (pLocal->IsInVehicle())
        return;

    uintptr_t ClientWeapon = m.Read<uintptr_t>(m.m_gProcessBaseAddr + offset::ClientWeapons);

    if (!ClientWeapon)
        return;

    uintptr_t weaponPtr = m.Read<uintptr_t>(ClientWeapon + 0x128);
    uintptr_t GunSwayData = m.Read<uintptr_t>(weaponPtr + 0x30); // Sway

    uintptr_t FiringFunctionData = m.Read<uintptr_t>(weaponPtr + 0x10);
    //cfg.CurrentBPS = m.Read<int>(FiringFunctionData + 0xD8);

    // Recoil / Spread
    if (g.g_NoRecoil && GunSwayData != NULL && m.Read<float>(GunSwayData + 0x430) != g.g_NoRecoilVal)
    {
        m.Write<float>(GunSwayData + 0x430, g.g_NoRecoilVal);
        m.Write<float>(GunSwayData + 0x438, g.g_NoRecoilVal);
        m.Write<float>(GunSwayData + 0x434, g.g_NoRecoilVal);
        m.Write<float>(GunSwayData + 0x43C, g.g_NoRecoilVal);
    }
    else if (!g.g_NoRecoil && GunSwayData != NULL && m.Read<float>(GunSwayData + 0x430) != 1.f)
    {
        m.Write<float>(GunSwayData + 0x430, 1.f);
        m.Write<float>(GunSwayData + 0x438, 1.f);
        m.Write<float>(GunSwayData + 0x434, 1.f);
        m.Write<float>(GunSwayData + 0x43C, 1.f);
    }
}