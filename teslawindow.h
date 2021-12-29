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

#include <QMouseEvent>
#include <QLabel>
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
#include <QImage>

#include "fantasiesdmd.h"

#include "dmdframe.h"

class DMDLabel;
class DMDAnimationEngine;
class QLabel;
class QRadioButton;
class QButtonGroup;
class QSlider;
class ImageLabel;

class ImageLabel : public QLabel
{
	Q_OBJECT
public:
	explicit ImageLabel(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
		: QLabel(parent, f)
	{
	}

	void mousePressEvent(QMouseEvent* ev) override
	{
		emit clicked(ev->pos());
	}

signals:
	void clicked(QPoint pos);
};

class TeslaWindow : public QWidget
{
	Q_OBJECT
public:
	TeslaWindow(QWidget* parent, DMDAnimationEngine* animation_engine);
	~TeslaWindow();

	enum ColorMode : uint32_t
	{
		RGBA,
		RGAB,
		RBGA,
		RBAG,
		RAGB,
		RABG,

		GRBA,
		GRAB,
		GBAR,
		GBRA,
		GABR,
		GARB,

		BRGA,
		BRAG,
		BGRA,
		BGAR,
		BAGR,
		BARG,

		ARGB,
		ARBG,
		ABGR,
		ABRG,
		AGBR,
		AGRB,
		NUM_COLOR_MODES
	};

private:
	void initUI();
	void update_image();
	void load_zen_animation();
	void show_frame();

public slots:
	void open_file_button_clicked();
	void next_button_clicked();
	void prev_button_clicked();

	void width_plus_button_clicked();
	void width_min_button_clicked();
	void height_plus_button_clicked();
	void height_min_button_clicked();

	void zen_dmd_button_clicked();
	void next_frame_button_clicked();
	void prev_frame_button_clicked();
	void save_frame_button_clicked();
	void show_anim_button_clicked();

	void inc_color_mode_button_clicked();
	void dec_color_mode_button_clicked();

	void image_clicked(QPoint pos);
	void save_button_clicked();

private:
	QString color_mode_string(ColorMode color_mode);
	void parse_color(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a, uint32_t& index);
private:
	uint32_t m_size = 0u;
	uint8_t* m_data = nullptr; 
	QLabel* m_file_name_label = nullptr;
	ImageLabel* m_image_label = nullptr;
	uint32_t m_image_nr = 0u;
	QSlider* m_width_slider = nullptr;
	QSlider* m_height_slider = nullptr;

	uint32_t m_parse_width = 512;
	uint32_t m_parse_height = 1024;

	QLabel* m_width_label = nullptr;
	QLabel* m_height_label = nullptr;

	QLabel* m_dmd_animation_label = nullptr;
	QLabel* m_color_mode_label = nullptr;
	QLabel* m_image_clicked_label = nullptr;

	DMDAnimationEngine* m_animation_engine = nullptr;
	uint32_t m_frame_index = 0u;
	ColorMode m_color_mode = RGBA;
	QImage m_image;
	DMDLabel* m_dmd_label = nullptr;

	QVector<DMDFrame> m_zen_dmd_frames;
};
