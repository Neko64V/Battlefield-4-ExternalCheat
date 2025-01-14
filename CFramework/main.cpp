#include "Cheat/FrameCore.h"
#include "Framework/Overlay/Overlay.h"

Overlay*	ov = new Overlay;
CFramework* cx = new CFramework;

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
	m.GetBaseAddress("bf4.exe");

	// Overlay
	if (!ov->InitOverlay("Battlefield 4", InitializeMode::WINDOW_TITLE)) // MemoryInitModeと同様
		return 2;

	// スレッドを作成
	std::thread([&]() { cx->UpdateList(); }).detach(); // ESP/AIM用にプレイヤーのデータをキャッシュする

	ov->OverlayLoop();
	ov->DestroyOverlay();
	m.DetachProcess();
	g.g_Run = false;
	delete cx, ov;

	return 0;
}

void Overlay::OverlayUserFunction()
{
	cx->MiscAll();

	cx->RenderInfo();

	if (g.g_ESP)
		cx->RenderESP();

	if (g.g_ShowMenu)
		cx->RenderMenu();
}