#include <Windows.h>
#include <iostream>
#include <vector>
#include <TlHelp32.h>
template<typename T>
T Read(HANDLE hProcess, uintptr_t addr) {
	T val;
	ReadProcessMemory(hProcess, (LPCVOID)addr, &val, sizeof(val), 0);
	return val;
}
template<typename T>
void Write(HANDLE hProcess, uintptr_t addr, T value) {
	WriteProcessMemory(hProcess, (LPVOID)addr, (LPVOID)&value, sizeof(T), 0);
}

uintptr_t ResolveMultiPtr(HANDLE hProcess, uintptr_t base, std::vector<std::ptrdiff_t> offsets) {
	uintptr_t addr = base;
	for (auto offset : offsets) {
		addr = Read<uintptr_t>(hProcess, addr + offset);
	}
	return addr;
}

uintptr_t GetModule(DWORD pid, const char* name) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32 entry = { 0 };
	entry.dwSize = sizeof(entry);
	uintptr_t baseAddr = 0;
	do {
		if (!strcmp(name, entry.szModule)) {
			baseAddr = (uintptr_t)entry.modBaseAddr;
			break;
		}
	} while (Module32Next(snapshot, &entry));
	CloseHandle(snapshot);
	return baseAddr;
}
int main() {
	SetConsoleTitleA("Plague Inc Evolved DNA hack.");
	HWND hWnd = FindWindowA(0, "Plague Inc Evolved");
	DWORD pid;
	GetWindowThreadProcessId(hWnd, &pid);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	uintptr_t unityplayerdll = GetModule(pid, "UnityPlayer.dll");
	uintptr_t dnaAddr = ResolveMultiPtr(hProcess, Read<uintptr_t>(hProcess, (unityplayerdll + 0x1792f28)), { 0x40, 0xf8, 0, 0x1d8, 0xa8, 0x670 }) + 0x14;
	int dna;
	std::cout << "This hack is made for Plague Version 1.19.1.0 Steam (MP: 110)\n";
	std::cout << "Enter DNA points: "; std::cin >> dna;
	Write(hProcess, dnaAddr, dna);

	CloseHandle(hProcess);
}