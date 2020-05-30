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

class DMDAnimationEngine;
class QLabel;
class DMDFrame;

class Span
{
public:
	static const uint32_t REMOVE_COLUMN_COUNT = 32;
	static const uint32_t FANTASIES_DMD_WIDTH = 160;
	static const uint32_t FANTASIES_DMD_HEIGHT = 16;

	Span(uint32_t start_column, uint32_t end_column)
	{
		m_start_column = start_column;
		m_end_column = end_column;
	}

	uint32_t width() const
	{
		return m_end_column - m_start_column + 1;
	}

	bool is_big_span() const
	{
		return width() >= REMOVE_COLUMN_COUNT;
	}

	bool is_clear_screen() const
	{
		return width() == FANTASIES_DMD_WIDTH;
	}

	uint32_t start_column() const
	{
		return m_start_column;
	}

	uint32_t end_column() const
	{
		return m_end_column;
	}

	bool is_left_span() const
	{
		return m_start_column == 0;
	}

	bool is_right_span() const
	{
		return m_end_column == 159;
	}

	uint32_t m_start_column = 0;
	uint32_t m_end_column = 0;
};

class FantasiesWindow : public QWidget
{
	Q_OBJECT
public:
	static const uint32_t FANTASIES_WIDTH = 320;
	static const uint32_t FANTASIES_HEIGHT = 34;

	static const uint32_t FANTASIES_DMD_WIDTH = 160;
	static const uint32_t FANTASIES_DMD_HEIGHT = 16;

public:
	FantasiesWindow(QWidget* parent, DMDAnimationEngine* animation_engine);
	~FantasiesWindow();

	void key_left();
	void key_right();
	void key_up();
	void key_down();

private:
	void initUI();
	bool is_column_candidate(uint32_t column);
	void update_image();
	void paint_spans(const QImage& img);
	void determine_spans();
	bool is_scrolling_rtl() const;
	bool is_current_frame_empty() const;
	inline uint8_t RAWPIXEL(int32_t x, int32_t y)
	{
		if (y >= FantasiesWindow::FANTASIES_DMD_WIDTH)
			return 0;

		//#define RAWPIXEL(x,y) rawDMD[y * FANTASIES_DMD_WIDTH + x]
		uint32_t pixel_number = y * FantasiesWindow::FANTASIES_DMD_WIDTH + x;
		uint32_t bytenumber = pixel_number / 8;
		assert(bytenumber < 320);
		uint32_t bitnumber = pixel_number % 8;

		uint8_t pixel = rawDMD[bytenumber];
		return (pixel & (1 << bitnumber)) > 0 ? 255 : 0;
	}
	QImage span_fix();
	QImage parsed_fix();
	void copyblock(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t dest_x, int32_t dest_y, DMDFrame& outputDMD);
	QImage image_from_DMD_frame(const DMDFrame& frame);

private slots:
	void inc_img_button_clicked();
	void dec_img_button_clicked();

	void inc_img10_button_clicked();
	void dec_img10_button_clicked();

	void inc_img100_button_clicked();
	void dec_img100_button_clicked();

	void inc_img1000_button_clicked();
	void dec_img1000_button_clicked();

	void split_range_button_clicked();
	
	void debug_button_clicked();

private:
	enum EAnimation
	{
		CLEAR_SCREEN,
		SCROLLING_RTL,
		LOOP,
		NONE
	};


	QLabel* m_file_name_label = nullptr;
	QLabel* m_image_label = nullptr;
	QLabel* m_dmd_span_label = nullptr;
	QLabel* m_byte_label = nullptr;
	QLabel* m_spans_image_label = nullptr;
	QLabel* m_spans_text_label = nullptr;
	QLabel* m_parsed_image_label = nullptr;
	QLabel* m_parsed_text_label = nullptr;
	uint32_t m_current_file_nr = 0;

	uint8_t rawDMD[FANTASIES_DMD_WIDTH * FANTASIES_DMD_HEIGHT / 8];
	uint8_t decodedDMD[FANTASIES_DMD_WIDTH * FANTASIES_DMD_HEIGHT];
	DMDAnimationEngine* m_animation_engine = nullptr;

	int32_t m_byte_index = 0;
	std::vector<Span> m_spans;
	std::vector<Span> m_prev_frame_spans;
	EAnimation m_current_animation = NONE;
};
