#include "common_ui/CommonInclude.h"
#include "gui/select_wallet_window.h"
#include "dump/ExceptionHandler.h"
#include "dump/Dump.h"
#include <thread>
#include "core/network_thread.h"


int WINAPI _tWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR lpCmdLine,
    int nCmdShow)
{
    LPWSTR *szArglist;
    int nArgs;

    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if (nArgs == 8) {
        CrashDump(lpCmdLine);
        return 0;
    }

    Common::ExceptionHandler::InstallExceptionHandler();

    SetProcessDPIAware();

    using namespace wallet_gui;

    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartupInput StartupInput;
    GdiplusStartup(&gdiplusToken, &StartupInput, NULL);
    Universal::UI::SetUiScale(Universal::UI::DPI_SCALE_100);

    CSelectWalletWindow select_window;
    select_window.Create(NULL, CRect(0, 0, 400, 260),
        L"", WS_VISIBLE | WS_CLIPCHILDREN | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
    select_window.CenterWindow();
    select_window.ShowWindow(SW_SHOW);

    auto net = std::thread([]() {
        CNetworkThread::_().Run(); });
    net.detach();

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
}
