#pragma once

/*
MIT License

Copyright (c) 2020 BlackStar

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
#include <QSet>
#include <QMap>
#include <QVector>
#include <set>
#include <assert.h>
#include <QFile>
#include <QTimer>
#include <QTextEdit>
#include <QLineEdit>

#include "fantasiesdmd.h"

#include "dmdframe.h"

class DMDAnimationEngine;
class QLabel;
class QSlider;

class TeslaWindow : public QWidget
{
	Q_OBJECT
public:
	TeslaWindow(QWidget* parent);
	~TeslaWindow();

private:
	void initUI();
	void update_image();

private slots:
	void open_file_button_clicked();
	void next_button_clicked();
	void prev_button_clicked();

	void width_plus_button_clicked();
	void width_min_button_clicked();
	void height_plus_button_clicked();
	void height_min_button_clicked();

private:
	uint32_t m_size = 0u;
	uint8_t* m_data = nullptr; 
	QLabel* m_file_name_label = nullptr;
	QLabel* m_image_label = nullptr;
	uint32_t m_image_nr = 0u;
	QSlider* m_width_slider = nullptr;
	QSlider* m_height_slider = nullptr;

	uint32_t m_parse_width = 512;
	uint32_t m_parse_height = 1024;
};
