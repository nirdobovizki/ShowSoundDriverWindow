// RaiseSoundWindow2.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "RaiseSoundWindow2.h"
#include "TlHelp32.h"

void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
	LONG idObject, LONG idChild,
	DWORD dwEventThread, DWORD dwmsEventTime)
{
	if (hwnd)
	{
		TCHAR nameBuffer[1024] = { 0 };
		GetWindowText(hwnd, nameBuffer, _countof(nameBuffer)-1);
		if (wcsncmp(nameBuffer, L"The current connected device is :", _countof(nameBuffer)) == 0)
		{
			HWND next = 0;
			while (next = GetParent(hwnd)) hwnd = next;
			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	// single instance - run only once, no point in having multiple copies running 
	// (note, this will run once fr every logged in user)

	auto mutex = CreateMutex(NULL, TRUE, L"6DEBF46A-A2CB-4274-B918-AC338C957638");
	if (mutex && GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(mutex);
		MessageBox(NULL, L"Program already running, this program will make the sound connection dialog on top so you can easily use it", L"Raise Sound Window", MB_ICONERROR | MB_OK);
		return 1;
	}

	// get RealTek UI process PID, use it to limit our hook so we don't waste system resources

	DWORD pid = 0;
	while (!pid)
	{
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot == INVALID_HANDLE_VALUE)
		{
			CloseHandle(mutex);
			MessageBox(NULL, L"Can't catch sound driver window, CreateToolhelp32Snapshot failed", L"Raise Sound Window", MB_ICONERROR | MB_OK);
			return 2;
		}
		PROCESSENTRY32 process;
		ZeroMemory(&process, sizeof(process));
		process.dwSize = sizeof(process);

		if (Process32First(snapshot, &process))
		{
			do
			{
				for (int i = 0; i < _countof(process.szExeFile) && process.szExeFile[i]; ++i)
					process.szExeFile[i] = toupper(process.szExeFile[i]);

				if (wcsncmp(process.szExeFile, L"RTKNGUI64",_countof(process.szExeFile)) == 0)
				{
					pid = process.th32ProcessID;
					break;
				}
			} while (Process32Next(snapshot, &process));
		}


		CloseHandle(snapshot);
		if (pid == 0)
		{
			// maybe the RealTek app didn't load yet, wait half a minute and repeat
			Sleep(30000);
		}
	}

	auto hook = SetWinEventHook(EVENT_OBJECT_PARENTCHANGE, EVENT_OBJECT_PARENTCHANGE, NULL, HandleWinEvent, pid, 0, WINEVENT_OUTOFCONTEXT);

	if (hook == 0)
	{
		MessageBox(NULL, L"Can't catch sound driver window, SetWinEventHook failed", L"Raise Sound Window", MB_ICONERROR | MB_OK);
		return 3;
	}

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	UnhookWinEvent(hook);
	CloseHandle(mutex);

    return (int) msg.wParam;
}

