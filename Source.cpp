#include <windows.h>
#include <iostream>


DWORD __stdcall readDataFromExtProgram(HANDLE m_hChildStd_OUT_Rd)
{
	DWORD dwRead;
	CHAR chBuf[256];
	BOOL bSuccess = FALSE;

	for (;;)
	{
		bSuccess = ReadFile(m_hChildStd_OUT_Rd, chBuf, 256, &dwRead, NULL);
		if (!bSuccess || dwRead == 0) continue;
		std::cout << chBuf;
		ZeroMemory(chBuf, 256);

		if (!bSuccess) break;
	}
	return 0;
}



int main() {
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

	si.hStdError = m_hChildStd_OUT_Wr;
	si.hStdOutput = m_hChildStd_OUT_Wr;
	si.dwFlags |= STARTF_USESTDHANDLES;
	wchar_t lpCommandLine[8192] = L"";
	wchar_t args[8192];
	wcscpy(args, L"\\System32\\cmd.exe /c tasklist || findstr /I 'lunar javaw chrome firefox cmd conhost tasklist wmiprvse powershell runtimebroker shellexperiencehost svchost useroobebroker systemsettings calculator applicationframehost steam steamwebhelper discord nvidia textinputhost security lockapp task win csrss searchindexer appvsh office mpcopy sgrm vmmem msmpeng nvtelemetry amd intel nvdisplay fontdrv lsass lsaiso services smss registry secure system idle process image name ===='");
	wcsncat(lpCommandLine, windir , (size_t)4096);
	wcsncat(lpCommandLine, args, (size_t)4096);
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

