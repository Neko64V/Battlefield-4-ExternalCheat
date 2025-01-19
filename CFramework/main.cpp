#include "Cheat/FrameCore.h"
#include "Framework/Overlay/Overlay.h"

Overlay*	overlay = new Overlay;
CFramework* framework = new CFramework;

void Memory::GetBaseAddress()
{
	// ベースアドレスを取得
	m_gProcessBaseAddr = GetModuleBase("bf4.exe");

	if (m_gProcessBaseAddr == 0)
		MessageBoxA(nullptr, "BaseAddress == 0", "WARNING", MB_OK | MB_TOPMOST);
}

void Overlay::OverlayUserFunction()
{
	framework->MiscAll();

	framework->RenderInfo();

	if (g.g_ESP)
		framework->RenderESP();

	if (g.g_ShowMenu)
		framework->RenderMenu();
}

// DEBUG時にはコンソールウィンドウを表示する
#if _DEBUG
int main()
#else 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	// プロセスに接続する
	if (!m.AttachProcess("Battlefield 4", InitializeMode::WINDOW_TITLE)) // 詳細は Framework/Utils/Memory/Memory.h を参照
		return 1;

	// ベースアドレスを取得する
	m.GetBaseAddress();

	// Overlay
	if (!overlay->InitOverlay("Battlefield 4", InitializeMode::WINDOW_TITLE)) // MemoryInitModeと同様
		return 2;

	// スレッドを作成
	std::thread([&]() { framework->UpdateList(); }).detach(); // ESP/AIM用にプレイヤーのデータをキャッシュする

	overlay->OverlayLoop();
	overlay->DestroyOverlay();
	m.DetachProcess();
	g.g_Run = false;
	delete framework, overlay;

	return 0;
}