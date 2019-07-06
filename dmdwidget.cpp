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
#include "dmdwidget.h"

#include <processthreadsapi.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <psapi.h>

#include <QPushButton>
#include <QLabel>
#include <QImage>
#include <QPainter>
#include <QRect>
#include <QPixmap>
#include <QMessageBox>
#include <QTimer>

int DMDWIDTH = 128;
int DMDHEIGHT = 32;

DMDWidget::DMDWidget(QWidget* parent)
: QWidget(parent)
{
	QPushButton* findFX3Button = new QPushButton("Find FX3 DMD", this);
	findFX3Button->setGeometry(10, 10, 120, 20);
	connect(findFX3Button, SIGNAL(clicked()), this, SLOT(findDMDButton_clicked()));

	QPushButton* openFX3Button = new QPushButton("Capture DMD", this);
	openFX3Button->setGeometry(10, 60, 120, 20);
	connect(openFX3Button, SIGNAL(clicked()), this, SLOT(captureDMDButton_clicked()));

	m_DMD_label = new QLabel(this);
	captureTimer = new QTimer(this);
	connect(captureTimer, SIGNAL(timeout()), this, SLOT(captureTimeout()));
}

void DMDWidget::findDMDButton_clicked()
{
	if (findFX3())
	{
		if (findDMD())
		{

		}
	}
}

void DMDWidget::captureDMDButton_clicked()
{
	captureTimer->start(16);
}

void DMDWidget::captureTimeout()
{
	captureDMD();
}

bool DMDWidget::findFX3()
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

bool DMDWidget::findDMD()
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
		return true;

	return false;
}

void DMDWidget::captureDMD()
{
	if (m_DMD_memory_offset == 0)
		return;

	uint32_t ptr;
	ReadProcessMemory(m_FX3_process_handle, (void*)(m_FX3_base_offset + m_DMD_memory_offset), &ptr, sizeof(uint32_t), NULL);
	ReadProcessMemory(m_FX3_process_handle, (void*)(ptr),               &ptr, sizeof(uint32_t), NULL);
	ReadProcessMemory(m_FX3_process_handle, (void*)(ptr + 0xF0),        &ptr, sizeof(uint32_t), NULL);
	ReadProcessMemory(m_FX3_process_handle, (void*)(ptr + 0x34),        &ptr, sizeof(uint32_t), NULL);

	if (ptr == 0)
		return;

	uint8_t rawDMD[128 * 32];
	ReadProcessMemory(m_FX3_process_handle, (void*)(ptr),               rawDMD, sizeof(rawDMD), NULL);
	// valid DMD address

	// Fill DMD with colors
	const int DMDSIZE = 6;
	m_DMD_label->setGeometry(700, 160, DMDWIDTH * DMDSIZE, DMDHEIGHT * DMDSIZE);
	QImage image(DMDWIDTH, DMDHEIGHT, QImage::Format_RGBA8888);
	QPainter p(&image);

	uint32_t bytepos = 0;
	for (int y = 0; y < 32; ++y)
	{
		for (int x = 0; x < 128; ++x)
		{
			uint8_t c = rawDMD[bytepos];
			uint8_t r = c <<= 6;
			uint8_t g = c <<= 4;
			uint8_t b = 0;
			p.setPen(QColor(r, g, b));
			p.drawPoint(x, y);
			bytepos++;
		}
	}

	m_DMD_label->setPixmap(QPixmap::fromImage(image).scaled(QSize(DMDWIDTH * DMDSIZE, DMDHEIGHT * DMDSIZE)));
}

uint32_t DMDWidget::findDMDMemoryOffset(uint8_t* buffer, SIZE_T buffer_size)
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