#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <string>
#include <vector>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    (void)hInstance;
    (void)hPrevInstance;
    (void)nCmdShow;

    // 1. Obtain current launcher directory
    wchar_t exePath[MAX_PATH];
    if (GetModuleFileNameW(NULL, exePath, MAX_PATH) == 0) {
        MessageBoxW(NULL, L"Failed to determine application path.", L"Launch Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    // Strip executable name to obtain root directory
    std::wstring rootDir(exePath);
    size_t lastSlash = rootDir.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos) {
        rootDir = rootDir.substr(0, lastSlash);
    }

    // 2. Locate target binary (try bin/WorkbenchApp.exe first, then WorkbenchApp.exe)
    std::wstring targetExe = rootDir + L"\\bin\\WorkbenchApp.exe";
    DWORD attr = GetFileAttributesW(targetExe.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY)) {
        targetExe = rootDir + L"\\WorkbenchApp.exe";
        attr = GetFileAttributesW(targetExe.c_str());
        if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY)) {
            std::wstring msg = L"Could not locate WorkbenchApp.exe in:\n" + rootDir + L"\\bin\\WorkbenchApp.exe\n\nPlease ensure the archive was fully extracted.";
            MessageBoxW(NULL, msg.c_str(), L"PASS Science Works — Launch Error", MB_ICONERROR | MB_OK);
            return 2;
        }
    }

    // 3. Build command line including any passed arguments
    std::wstring cmdLine = L"\"" + targetExe + L"\"";
    bool isSmokeTest = false;
    if (pCmdLine && wcslen(pCmdLine) > 0) {
        cmdLine += L" ";
        cmdLine += pCmdLine;
        if (wcsstr(pCmdLine, L"smoke-test") != nullptr) {
            isSmokeTest = true;
        }
    }

    std::vector<wchar_t> cmdLineMutable(cmdLine.begin(), cmdLine.end());
    cmdLineMutable.push_back(L'\0');

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Target working directory is rootDir (where data/ and docs/ live alongside bin/)
    std::wstring workDir = rootDir;

    // 4. Create child process without opening terminal
    if (!CreateProcessW(
            targetExe.c_str(),
            cmdLineMutable.data(),
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            workDir.c_str(),
            &si,
            &pi)) {
        DWORD err = GetLastError();
        std::wstring msg = L"Failed to start WorkbenchApp.exe (Error code " + std::to_wstring(err) + L").";
        MessageBoxW(NULL, msg.c_str(), L"PASS Science Works — Launch Error", MB_ICONERROR | MB_OK);
        return 3;
    }

    // 5. If smoke-test was requested, wait for child termination and forward child exit code
    if (isSmokeTest) {
        DWORD waitRes = WaitForSingleObject(pi.hProcess, 15000);
        if (waitRes == WAIT_TIMEOUT) {
            TerminateProcess(pi.hProcess, 1);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return 4; // Timed out
        }
        DWORD childExitCode = 0;
        GetExitCodeProcess(pi.hProcess, &childExitCode);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return static_cast<int>(childExitCode);
    }

    // For normal launch, verify child process didn't instantly crash
    DWORD waitEarly = WaitForSingleObject(pi.hProcess, 500);
    if (waitEarly == WAIT_OBJECT_0) {
        DWORD earlyExit = 0;
        GetExitCodeProcess(pi.hProcess, &earlyExit);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        if (earlyExit != 0) {
            std::wstring msg = L"WorkbenchApp.exe exited unexpectedly on startup with code " + std::to_wstring(earlyExit) + L".";
            MessageBoxW(NULL, msg.c_str(), L"PASS Science Works — Launch Error", MB_ICONERROR | MB_OK);
            return static_cast<int>(earlyExit);
        }
        return 0;
    }

    // Normal running process: close handles and exit cleanly
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}
