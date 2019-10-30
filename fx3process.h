#pragma once

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

#include <stdint.h>
#include <string>
#include <vector>
#include <Windows.h>
#include <QColor>

#include "dmdconfig.h"

class FX3Process
{
public:
	bool findFX3();
	bool findDMD();
	bool getDMDColor(QColor& color);
	bool is_valid_DMD() const;
	bool captureDMD(uint8_t* buffer);

private:
	uint32_t get_DMD_ptr() const;
	uint32_t findDMDMemoryOffset(uint8_t* buffer, SIZE_T buffer_size);
	void DEBUG_FIND(uint8_t* buffer, SIZE_T buffer_size);
	void DEBUG_BRUTEFORCE(uint8_t* buffer, SIZE_T buffer_size);
	void DEBUG_DUMP(uint8_t* buffer, SIZE_T buffer_size);

private:
	std::wstring m_executable_name = L"Pinball FX3.exe";
	DWORD m_FX3_process_id = 0;
	HANDLE m_FX3_process_handle = nullptr;
	uint32_t m_FX3_base_offset = 0;
	uint32_t m_DMD_memory_offset = 0;
	bool m_DMD_found = false;
	bool m_DMD_color_found = false;
};