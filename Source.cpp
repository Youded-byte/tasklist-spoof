#include <windows.h>
#include <iostream>


DWORD __stdcall readDataFromExtProgram(HANDLE m_hChildStd_OUT_Rd)
{
	DWORD dwRead;
	CHAR chBuf[20000];
	ZeroMemory(chBuf, 20000);
	BOOL bSuccess = FALSE;
	BOOL firsttime = TRUE;

	for (;;)
	{
		bSuccess = ReadFile(m_hChildStd_OUT_Rd, chBuf, 20000, &dwRead, NULL);
		if (!bSuccess || dwRead == 0) continue;
		for (int i = 0;i < 20000;i++) {
			if (chBuf[i] == 13 && chBuf[i + 1] == 10) {
				chBuf[i] = 10;
				for (int it = i + 1; it < 19999 - i; it++) {
					chBuf[it] = chBuf[it + 1];
				}
			}
		}

		std::cout << chBuf;
		ZeroMemory(chBuf, 20000);

		if (!bSuccess) break;
	}
	return 0;
}



int main(int argc, char** argv) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	wchar_t windir[4096];
	HANDLE m_hChildStd_OUT_Rd = NULL;
	HANDLE m_hChildStd_OUT_Wr = NULL;
	HANDLE m_hreadDataFromExtProgram = NULL; 
	SECURITY_ATTRIBUTES saAttr;
	ZeroMemory(&saAttr, sizeof(saAttr));
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;


	if (!GetWindowsDirectoryW(windir, 4096)) {
		//std::cout << "Couldn't get windows directory!";
		return 0;
	}

	if (!CreatePipe(&m_hChildStd_OUT_Rd, &m_hChildStd_OUT_Wr, &saAttr, 0)) {
		//std::cout << "Couldn't create pipe!";
		return 0;
	}

	if (!SetHandleInformation(m_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
		//std::cout << "Couldn't set handle information!";
		return 0;
	}
	BOOL findstr = TRUE;
	wchar_t tasklistargs[8192] = L"";
	ZeroMemory(&tasklistargs, sizeof(tasklistargs));
	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			wchar_t tasklistargstemp[8192] = L"";
			size_t size = strlen(argv[i]);
			mbstowcs(tasklistargstemp, argv[i], size);
			wcsncat(tasklistargs, tasklistargstemp, wcslen(tasklistargstemp));
			wcsncat(tasklistargs, L" ", 1);
		}
		findstr = FALSE;
	}

	si.hStdError = m_hChildStd_OUT_Wr;
	si.hStdOutput = m_hChildStd_OUT_Wr;
	si.dwFlags |= STARTF_USESTDHANDLES;
	wchar_t lpCommandLine[8192] = L"";
	wchar_t cmd[] = L"\\System32\\cmd.exe /c  ";
	wchar_t tasklist[] = L"\\System32\\tasklist.exe ";
	wchar_t args[] = L"| findstr /B /I \"lunar javaw chrome firefox cmd conhost tasklist wmiprvse powershell runtimebroker shellexperiencehost svchost useroobebroker systemsettings calculator applicationframehost steam steamwebhelper discord nvidia textinputhost security lockapp task win csrss searchindexer appvsh office mpcopy sgrm vmmem msmpeng nvtelemetry amd intel nvdisplay fontdrv lsass lsaiso services smss registry secure system idle process image name brave opera ====\"";
	ZeroMemory(&lpCommandLine, sizeof(lpCommandLine));
	wcscpy(lpCommandLine, windir);
	wcsncat(lpCommandLine, cmd, wcslen(cmd));
	wcsncat(lpCommandLine, windir, wcslen(windir));
	wcsncat(lpCommandLine, tasklist, wcslen(tasklist));
	wcsncat(lpCommandLine, tasklistargs, wcslen(tasklistargs));
	if (findstr == TRUE) {
		wcsncat(lpCommandLine, args, wcslen(args));
	}
	if (CreateProcessW(NULL,
		lpCommandLine,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&si,
		&pi)
		) {
		HANDLE m_hreadDataFromExtProgram = CreateThread(0, 0, readDataFromExtProgram, m_hChildStd_OUT_Rd, 0, NULL);
		if (!WaitForSingleObject(pi.hProcess, 20000)) {
			//std::cout << "Process quit!";
			return 1;
		}
		else {
			//std::cout << "Could not wait for process!";
			return 0;
		}
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else {
		//std::cout << "Could not create process!";
		return 0;
	}
}

