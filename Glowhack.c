#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "TlHelp32.h"
#pragma comment(lib, "user32.lib")    
#include <stdbool.h>

//Glowhack.c

/*
 * This file is part of voidzero-development repository (https://github.com/voidzero-development).
 * Copyright (c) 2020 voidzero-development
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

int GlowEntityIdx;
int Glower;
int EntityGlowIndex;
DWORD GlowBasePtr;
DWORD LocalPlayer;
int entityHealth;
int LocalTeam;
int GLLocalTeam;

float B2 = 0.f;
float ALFA = 1.f;
float TALFA = 0.6f;
float HG = 0.4f;
float HR = 0.8f;

bool t = true;
bool f = false;

HANDLE ProcessHandle;
DWORD ProcessID;
DWORD ClientBaseAddress;
DWORD dwEngine;
int iClientState;

DWORD dwGlowObjectManager = 0x529A1E0;
DWORD dwLocalPlayer = 0xD3DD14;
DWORD dwEntityList = 0x4D523AC;
DWORD m_iGlowIndex = 0xA438;

DWORD GetModuleBaseAdress_C(LPSTR lpModuleName, DWORD dwProcessId)
{
	MODULEENTRY32 lpModEntryPoint = { 0 };
	HANDLE handleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if (!handleSnap)	return -1;
	lpModEntryPoint.dwSize = sizeof(lpModEntryPoint);
	BOOL bModule = Module32First(handleSnap, &lpModEntryPoint);
	while (bModule)
	{
		if (!strcmp(lpModEntryPoint.szModule, lpModuleName))
		{
			CloseHandle(handleSnap);
			return (DWORD)lpModEntryPoint.modBaseAddr;
		}
		bModule = Module32Next(handleSnap, &lpModEntryPoint);
	}
	CloseHandle(handleSnap);
	return (DWORD)lpModEntryPoint.modBaseAddr;
}


DWORD WINAPI Glow(void* PARAMS){
	while (1) {
		for (int i = 1; i <= 64; i++) {
			GlowEntityIdx = i;
			ReadProcessMemory(ProcessHandle, (LPVOID)(ClientBaseAddress + dwGlowObjectManager), &GlowBasePtr, sizeof(GlowBasePtr), NULL);
			ReadProcessMemory(ProcessHandle, (LPVOID)(ClientBaseAddress + dwLocalPlayer), &LocalPlayer, sizeof(LocalPlayer), NULL);
			ReadProcessMemory(ProcessHandle, (LPVOID)(LocalPlayer + 0xF4), &LocalTeam, sizeof(LocalTeam), NULL);
			ReadProcessMemory(ProcessHandle, (LPVOID)(ClientBaseAddress + dwEntityList + (GlowEntityIdx - 1) * 0x10), &Glower, sizeof(Glower), NULL);
			ReadProcessMemory(ProcessHandle, (LPVOID)(Glower + m_iGlowIndex), &EntityGlowIndex, sizeof(EntityGlowIndex), NULL);
			ReadProcessMemory(ProcessHandle, (LPVOID)(Glower + 0xF4), &GLLocalTeam, sizeof(GLLocalTeam), NULL);
			ReadProcessMemory(ProcessHandle, (LPVOID)(Glower + 0x100), &entityHealth, sizeof(entityHealth), NULL);

			// Health based calculations
			HR = (255 - 2.55 * entityHealth) / 255.f;
			HG = 2.55 * entityHealth / 255.f;

			if (GLLocalTeam != LocalTeam) {
				WriteProcessMemory(ProcessHandle, (LPVOID)(GlowBasePtr + ((EntityGlowIndex * 0x38) + 0x4)), &HR, sizeof(HR), NULL);
				WriteProcessMemory(ProcessHandle, (LPVOID)(GlowBasePtr + ((EntityGlowIndex * 0x38) + 0x8)), &HG, sizeof(HG), NULL);
				WriteProcessMemory(ProcessHandle, (LPVOID)(GlowBasePtr + ((EntityGlowIndex * 0x38) + 0xC)), &B2, sizeof(B2), NULL);
				WriteProcessMemory(ProcessHandle, (LPVOID)(GlowBasePtr + ((EntityGlowIndex * 0x38) + 0x10)), &ALFA, sizeof(ALFA), NULL);
				WriteProcessMemory(ProcessHandle, (LPVOID)(GlowBasePtr + ((EntityGlowIndex * 0x38) + 0x24)), &t, sizeof(t), NULL);
				WriteProcessMemory(ProcessHandle, (LPVOID)(GlowBasePtr + ((EntityGlowIndex * 0x38) + 0x25)), &f, sizeof(f), NULL);
				WriteProcessMemory(ProcessHandle, (LPVOID)(GlowBasePtr + ((EntityGlowIndex * 0x38) + 0x2C)), &f, sizeof(f), NULL);
			}
		}
		Sleep(1);
	}
	return 1;
}

int main()
{
   	printf("Hello, World! This is a native C program compiled on the command line.\n");
	printf("Super Simple external glow in C\n");
	printf("Credits: VoidZero1337 @ https://github.com/voidzero-development\n");

	HWND windowHandle = NULL;

	do {
		windowHandle = FindWindowA(0, "Counter-Strike: Global Offensive");
		Sleep(10);
	} while (windowHandle == NULL);

	GetWindowThreadProcessId(windowHandle, &ProcessID);
	ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, ProcessID);

	ClientBaseAddress = GetModuleBaseAdress_C((LPSTR)"client.dll", ProcessID); //used to be client_panorama.dll

	HANDLE thread = CreateThread(NULL, 0, Glow, NULL, 0, NULL);

	while (thread) {
		printf("Waiting on glow thread\n");
    	Sleep(1000);
  	}
    return 0;
}


