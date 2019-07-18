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
#include <QLineEdit>
#include <QListWidget>

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
	m_DMD_label->setGeometry(10, 120, DMDWIDTH * DMDSIZE, DMDHEIGHT * DMDSIZE);

	m_line_edit = new QLineEdit(this);
	m_line_edit->setGeometry(10, 85, 120, 20);

	QPushButton* findStringButton = new QPushButton("Find string", this);
	findStringButton->setGeometry(130, 85, 120, 20);
	connect(findStringButton, SIGNAL(clicked()), this, SLOT(findStringButton_clicked()));

	m_list_widget = new QListWidget(this);
	m_list_widget->setGeometry(500, 85, 280, 800);

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

void DMDWidget::findStringButton_clicked()
{
	if (m_line_edit->text() == "")
		return;

	m_list_widget->clear();

	unsigned char *p = NULL;
	MEMORY_BASIC_INFORMATION info;

	int count = 0;
	int totalmem = 0;
	for (p = NULL;
		VirtualQueryEx(m_FX3_process_handle, p, &info, sizeof(info)) == sizeof(info);
		p += info.RegionSize)
	{
		std::vector<uint8_t> buffer;

		if (info.State == MEM_COMMIT &&
			(info.Type == MEM_MAPPED || info.Type == MEM_PRIVATE))
		{
			SIZE_T bytes_read;
			buffer.resize(info.RegionSize);
			ReadProcessMemory(m_FX3_process_handle, p, &buffer[0], info.RegionSize, &bytes_read);
			if (bytes_read == 0)
				continue;

			++count;
			totalmem += bytes_read;
			buffer.resize(bytes_read);
			findString(&buffer[0], bytes_read, m_line_edit->text());
			/*char filename[512];
			sprintf_s(filename, "test-%u-%d.dmp", (uint32_t)p, count);
			FILE* fp;
			fopen_s(&fp, filename, "wb");
			fwrite(&buffer[0], 1, bytes_read, fp);
			fclose(fp);*/
		}
	}
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
	{
		ReadProcessMemory(m_FX3_process_handle, (void*)(m_FX3_base_offset + m_DMD_memory_offset), &m_DMD_memory_offset, sizeof(uint32_t), NULL);
		return true;
	}

	return false;
}

void DMDWidget::getDMDColor()
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
	uint8_t b = col         & 0x000000FF;

	float divisor = 0x33;
	m_DMD_r = r / divisor;
	m_DMD_g = g / divisor;
	m_DMD_b = b / divisor;
	m_DMD_color_found = true;
}

void DMDWidget::captureDMD()
{
	uint32_t ptr;
	ReadProcessMemory(m_FX3_process_handle, (void*)(m_DMD_memory_offset), &ptr, sizeof(uint32_t), NULL);
	ReadProcessMemory(m_FX3_process_handle, (void*)(ptr + 0xF0),          &ptr, sizeof(uint32_t), NULL);
	ReadProcessMemory(m_FX3_process_handle, (void*)(ptr + 0x34),          &ptr, sizeof(uint32_t), NULL);

	QImage image(DMDWIDTH * 2, DMDHEIGHT * 2, QImage::Format_RGBA8888);
	QPainter p(&image);

	if (ptr == 0)
	{
		p.fillRect(QRect(0, 0, DMDWIDTH * 2, DMDHEIGHT * 2), Qt::black);
		m_DMD_color_found = false;
		memset(m_previous_DMD, 0, sizeof(m_previous_DMD));
	}
	else
	{
		// valid DMD address
		uint8_t rawDMD[DMDWIDTH * DMDHEIGHT];
		ReadProcessMemory(m_FX3_process_handle, (void*)(ptr), rawDMD, sizeof(rawDMD), NULL);
		
		if (isGarbage(rawDMD))
		{
			memset(rawDMD, 0, sizeof(rawDMD));
			memset(m_previous_DMD, 0, sizeof(m_previous_DMD));
		}
		else
		{
			if (!m_DMD_color_found)
				getDMDColor();
		}

		if (isEqual(rawDMD, m_previous_DMD))
			return;
		else
			memcpy(m_previous_DMD, rawDMD, sizeof(rawDMD));

		if (isWilliamsDMD(rawDMD))
		{
			correctWilliamsDMD(rawDMD);
			normalizeWilliamsDMD(rawDMD);
		}
		else
		{
			normalizeDMD(rawDMD);
		}

		// Fill DMD with colors
		p.fillRect(QRect(0, 0, DMDWIDTH * 2, DMDHEIGHT * 2), Qt::black);
		uint32_t bytepos = 0;
		for (int y = 0; y < DMDHEIGHT; ++y)
		{
			for (int x = 0; x < DMDWIDTH; ++x)
			{
				uint8_t c = rawDMD[bytepos];
				float col = c / 255.0f;
				p.setPen(QColor(c * m_DMD_r, c * m_DMD_g, c * m_DMD_b));
				p.drawPoint(x * 2, y * 2);
				bytepos++;
			}
		}
	}

	m_DMD_label->setPixmap(QPixmap::fromImage(image).scaled(QSize(DMDWIDTH * DMDSIZE, DMDHEIGHT * DMDSIZE)));
}


uint32_t DMDWidget::findString(uint8_t* buffer, SIZE_T buffer_size, const QString& string)
{

	for (SIZE_T i = 0; i < buffer_size - string.length(); ++i)
	{
		if (buffer[i] != string[0])
			continue;

		bool match = true;
		for (uint32_t test = 1; test < string.length(); ++test)
		{
			//if (DMDSIGNATURE[test] == 0xFF)
			//	continue;

			if (buffer[i + test] != string[test])
			{
				match = false;
				break;
			}
		}

		if (match)
		{
			QString matchloc = QString::number((uint32_t)buffer) + QString(" - offset - ") + QString::number(i);
			//uint8_t* buffer2 = buffer + i - 100;
			m_list_widget->addItem(new QListWidgetItem(matchloc));
			//return i;
		}
	}
	return 0;
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

bool DMDWidget::isGarbage(const uint8_t* rawDMD) const
{
	const uint32_t pixelCount = DMDWIDTH * DMDHEIGHT;

	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		if (rawDMD[i] > 6)
			return true;
	}

	return false;
}

bool DMDWidget::isWilliamsDMD(const uint8_t* rawDMD) const
{
	const uint32_t pixelCount = DMDWIDTH * DMDHEIGHT;

#ifdef FAST_CHECK
	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		if (rawDMD[i] > 2)
			return true;
	}
	return false;
#else
	uint32_t count3 = 0;
	uint32_t count4 = 0;
	uint32_t count5 = 0;
	uint32_t count6 = 0;

	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		switch (rawDMD[i])
		{
		case 3: 
			count3++;
			break;
		case 4:
			count4++;
			break;
		case 5:
			count5++;
			break;
		case 6:
			count6++;
			break;
		}
	}

	return count3 + count4 + count5 + count6 == pixelCount;
#endif
}

void DMDWidget::correctWilliamsDMD(uint8_t* rawDMD)
{
	const uint32_t pixelCount = DMDWIDTH * DMDHEIGHT;
	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		rawDMD[i] -= 3;
	}
}

void DMDWidget::normalizeDMD(uint8_t* rawDMD)
{
	const uint32_t pixelCount = DMDWIDTH * DMDHEIGHT;
	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		switch (rawDMD[i])
		{
		case 1:
			rawDMD[i] = 128;
			break;
		case 2:
			rawDMD[i] = 255;
			break;
		}
	}
}

void DMDWidget::normalizeWilliamsDMD(uint8_t* rawDMD)
{
	const uint32_t pixelCount = DMDWIDTH * DMDHEIGHT;
	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		switch (rawDMD[i])
		{
		case 1:
			rawDMD[i] = 85;
			break;
		case 2:
			rawDMD[i] = 170;
			break;
		case 3:
			rawDMD[i] = 255;
			break;
		}
	}
}

bool DMDWidget::isEmpty(const uint8_t* rawDMD) const
{
	const uint32_t pixelCount = DMDWIDTH * DMDHEIGHT;
	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		if (rawDMD[i] != 0)
			return false;
	}
	return true;
}

bool DMDWidget::isEqual(const uint8_t* DMD1, const uint8_t* DMD2)
{
	const uint32_t pixelCount = DMDWIDTH * DMDHEIGHT;
	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		if (DMD1[i] != DMD2[i])
			return false;
	}
	return true;
}