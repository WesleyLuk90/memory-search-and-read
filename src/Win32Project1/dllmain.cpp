// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <tchar.h>

BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" {

	__declspec( dllexport ) DWORD searchForDouble(double value, HWND hwnd){
		if(!hwnd){
			return 0;
		}
		//_tprintf(_T("Found window %d\n"), hwnd);
		DWORD processID;
		if(!GetWindowThreadProcessId(hwnd, &processID)){
			//_tprintf(_T("Failed to get process id\n"));
			return 0;
		}
		//_tprintf(_T("Found processID %d\n"), processID);

		HANDLE process = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, false, processID);
		if(!process){
			//_tprintf(_T("Failed to get access to process\n"));
			return 0;
		}
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);

		MEMORY_BASIC_INFORMATION mbi;

		LPVOID memoryAddress = 0;

		const int bufferSize = 0x1000;
		BYTE buffer[bufferSize];
		int byteCount = 0;

		while(memoryAddress < systemInfo.lpMaximumApplicationAddress){
			VirtualQueryEx(process, memoryAddress, &mbi, sizeof(mbi));
			memoryAddress = (LPVOID)((DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize);

			if(mbi.State == MEM_COMMIT){
				//_tprintf(_T("Found region %p - %p\n"), (DWORD)mbi.BaseAddress, (DWORD)mbi.BaseAddress + mbi.RegionSize);
				byteCount += mbi.RegionSize;

				SIZE_T bytesRead = 0;
				int bytesToRead = mbi.RegionSize;
				int currentAddress = (DWORD)mbi.BaseAddress;
				while(bytesToRead > 0){
					int bytesThisRun = 0;
					if(bytesToRead > bufferSize){
						bytesThisRun = bufferSize;
						bytesToRead -= bufferSize;
					} else {
						bytesThisRun = bytesToRead;
						bytesToRead = 0;
					}
					if(!ReadProcessMemory(process, (LPCVOID)currentAddress, &buffer, bytesThisRun, &bytesRead)){
						//_tprintf(_T("Failed to read region because error %d\n"), GetLastError());
						continue;
					}
					for(int i = 0; i < bufferSize; i++){
						double* pointer = (double*)(buffer + i);
						if(value == *pointer){
							//_tprintf(_T("Match found at %p\n"), currentAddress + i);
							CloseHandle(process);
							return currentAddress + i;
						}
					}
					currentAddress += bytesThisRun;
				}
			}
		}

		CloseHandle(process);
		return 0;
	}

	__declspec( dllexport ) double getDoubleAtAddress(HWND hwnd, DWORD address){
		if(!hwnd){
			return 0;
		}
		//_tprintf(_T("Found window %d\n"), hwnd);
		DWORD processID;
		if(!GetWindowThreadProcessId(hwnd, &processID)){
			//_tprintf(_T("Failed to get process id\n"));
			return 0;
		}
		//_tprintf(_T("Found processID %d\n"), processID);

		HANDLE process = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, false, processID);
		if(!process){
			//_tprintf(_T("Failed to get access to process\n"));
			return 0;
		}
		BYTE buffer[sizeof(double)];
		int byteCount = 0;
		SIZE_T bytesRead;
		if(!ReadProcessMemory(process, (LPCVOID)address, &buffer, sizeof(double), &bytesRead)){
			CloseHandle(process);
			return 0;
		}

		CloseHandle(process);
		return *((double*)buffer);
	}
}