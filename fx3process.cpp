/*
MIT License

Copyright (c) 2019 BlackStar

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "fx3process.h"

#include <processthreadsapi.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <psapi.h>

bool FX3Process::findFX3()
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	m_FX3_process_id = 0;
	if (Process32First(snapshot, &entry) == TRUE)
	{
		do
		{
			if (m_executable_name == entry.szExeFile)
			{
				m_FX3_process_id = entry.th32ProcessID;
				CloseHandle(snapshot);
				return true;
			}
		} while (Process32Next(snapshot, &entry) == TRUE);
	}

	CloseHandle(snapshot);
	return false;
}

bool FX3Process::findDMD()
{
	m_FX3_process_handle = nullptr;

	if (m_FX3_process_id == 0)
	{
		return false;
	}

	m_FX3_process_handle = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, m_FX3_process_id);
	if (m_FX3_process_handle == nullptr)
	{
		return false;
	}

	if (m_FX3_process_id == 0 || m_FX3_process_handle == nullptr)
	{
		return false;
	}

	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	//  Take a snapshot of all modules in the specified process. 
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_FX3_process_id);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	//  Set the size of the structure before using it. 
	me32.dwSize = sizeof(MODULEENTRY32);

	//  Retrieve information about the first module, 
	//  and exit if unsuccessful 
	if (!Module32First(hModuleSnap, &me32))
	{
		CloseHandle(hModuleSnap);     // Must clean up the snapshot object! 
		return false;
	}

	//  Now walk the module list of the process, 
	//  and display information about each module 
	do
	{
		if (m_executable_name == me32.szModule)
		{
			m_FX3_base_offset = (uint64_t)me32.modBaseAddr;
		}
	} while (Module32Next(hModuleSnap, &me32));

	if (m_FX3_base_offset == 0)
		return false;

	std::vector<uint8_t> buffer;

	SIZE_T bytes_read;
	uint32_t READSIZE = 20000000;
	buffer.resize(READSIZE);
	ReadProcessMemory(m_FX3_process_handle, (void*)m_FX3_base_offset, &buffer[0], READSIZE, &bytes_read);

	buffer.resize(bytes_read);

	m_DMD_memory_offset = findDMDMemoryOffset(&buffer[0], bytes_read);
	if (m_DMD_memory_offset != 0)
	{
		ReadProcessMemory(m_FX3_process_handle, (void*)(m_FX3_base_offset + m_DMD_memory_offset), &m_DMD_memory_offset, sizeof(uint32_t), NULL);
		return true;
	}

	return false;
}

QColor FX3Process::getDMDColor()
{
	uint32_t ptr;
	uint32_t col;

	ReadProcessMemory(m_FX3_process_handle, (void*)(m_DMD_memory_offset), &ptr, sizeof(uint32_t), NULL);
	ReadProcessMemory(m_FX3_process_handle, (void*)(ptr + 0xF0), &ptr, sizeof(uint32_t), NULL);
	ReadProcessMemory(m_FX3_process_handle, (void*)(ptr + 0x50), &ptr, sizeof(uint32_t), NULL);
	ReadProcessMemory(m_FX3_process_handle, (void*)(ptr + 0x08), &col, sizeof(uint32_t), NULL);

	// Color is fetched as ARGB, values seem to be either 0x11 or 0x33 per channel
	uint8_t r = (col >> 16) & 0x000000FF;
	uint8_t g = (col >> 8) & 0x000000FF;
	uint8_t b = col & 0x000000FF;

	return QColor(r, g, b);
}

bool FX3Process::is_valid_DMD() const
{
	return get_DMD_ptr() != 0;
}

bool FX3Process::captureDMD(uint8_t* buffer)
{
	uint32_t ptr = get_DMD_ptr();

	if (ptr == 0)
		return false;

	ReadProcessMemory(m_FX3_process_handle, (void*)(ptr), buffer, DMDConfig::DMDWIDTH * DMDConfig::DMDHEIGHT, NULL);
	return true;
}

uint32_t FX3Process::get_DMD_ptr() const
{
	uint32_t ptr = 0;
	ReadProcessMemory(m_FX3_process_handle, (void*)(m_DMD_memory_offset), &ptr, sizeof(uint32_t), NULL);
	ReadProcessMemory(m_FX3_process_handle, (void*)(ptr + 0xF0), &ptr, sizeof(uint32_t), NULL);
	ReadProcessMemory(m_FX3_process_handle, (void*)(ptr + 0x34), &ptr, sizeof(uint32_t), NULL);
	return ptr;
}

uint32_t FX3Process::findDMDMemoryOffset(uint8_t* buffer, SIZE_T buffer_size)
{
	uint8_t DMDSIGNATURE[] = { 0x8B, 0x81, 0xFF, 0xFF, 0xFF, 0xFF, 0x89, 0x44, 0x24, 0xFF, 0x8B, 0x81, 0xFF, 0xFF, 0xFF, 0xFF, 0x89, 0x44, 0x24, 0xFF, 0xA1 };
	uint32_t DMDSIGNATURELENGTH = sizeof(DMDSIGNATURE);

	for (SIZE_T i = 0; i < buffer_size - DMDSIGNATURELENGTH; ++i)
	{
		if (buffer[i] != DMDSIGNATURE[0])
			continue;

		bool match = true;
		for (uint32_t test = 1; test < DMDSIGNATURELENGTH; ++test)
		{
			if (DMDSIGNATURE[test] == 0xFF)
				continue;

			if (buffer[i + test] != DMDSIGNATURE[test])
			{
				match = false;
				break;
			}
		}

		if (match)
		{
			return i + DMDSIGNATURELENGTH;
		}
	}
	return 0;
}