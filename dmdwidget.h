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

#include <QWidget>

#include <string>
#include <Windows.h>

#include <vector>

class QLabel;
class QPushButton;
class QTimer;
class QLineEdit;
class QListWidget;

class DMDWidget : public QWidget
{
	Q_OBJECT
public:
	static const uint32_t DMDSIZE = 2;


public:
	DMDWidget(QWidget* parent);

private slots:
	void findDMDButton_clicked();
	void captureDMDButton_clicked();
	void captureTimeout();
	void findStringButton_clicked();
	void recordButton_clicked();
	void saveRecordingsButton_clicked();

private:
	bool findFX3();
	bool findDMD();
	void getDMDColor();
	void captureDMD();
	bool isGarbage(const uint8_t* rawDMD) const;
	bool isWilliamsDMD(const uint8_t* rawDMD) const;
	bool isEmpty(const uint8_t* rawDMD) const;
	bool isEqual(const uint8_t* DMD1, const uint8_t* DMD2);
	void correctWilliamsDMD(uint8_t* rawDMD);
	void normalizeDMD(uint8_t* rawDMD);
	void normalizeWilliamsDMD(uint8_t* rawDMD);

	uint32_t findDMDMemoryOffset(uint8_t* buffer, SIZE_T buffer_size);
	uint32_t findString(uint8_t* buffer, SIZE_T buffer_size, const QString& string);

private:
	std::wstring m_executable_name = L"Pinball FX3.exe";
	DWORD m_FX3_process_id = 0;
	HANDLE m_FX3_process_handle = nullptr;
	uint32_t m_FX3_base_offset = 0;
	uint32_t m_DMD_memory_offset = 0; 
	bool m_DMD_found = false;
	bool m_DMD_color_found = false;
	float m_DMD_r = 1.0f;
	float m_DMD_g = 1.0f;
	float m_DMD_b = 1.0f;
	uint8_t m_previous_DMD[128 * 32];

	QLabel* m_DMD_label = nullptr;
	QTimer* captureTimer = nullptr;
	QLineEdit* m_line_edit = nullptr;
	QListWidget* m_list_widget = nullptr;
	QVector<QImage> m_recorded_frames;
	bool m_record_frames = false;
};