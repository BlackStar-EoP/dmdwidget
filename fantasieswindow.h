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

class DMDAnimationEngine;
class QLabel;
class DMDFrame;


class FantasiesDMD
{
public:
	static const uint32_t FANTASIES_DMD_WIDTH = 160;
	static const uint32_t FANTASIES_DMD_HEIGHT = 16;
	static const uint32_t DMD_SIZE = FANTASIES_DMD_WIDTH * FANTASIES_DMD_HEIGHT;
	static const uint32_t BIT_DMD_SIZE = DMD_SIZE / 8;

	inline void clear()
	{
		memset(m_bitDMD, 0, sizeof(m_bitDMD));
	}

	inline bool read_file(const QString& filename)
	{
#ifdef QT_VERSION
		QFile file(filename);
		if (file.open(QIODevice::ReadOnly))
		{

			QByteArray data = file.readAll();
			const char* pixeldata = data.constData();
			assert(data.size() == BIT_DMD_SIZE);
			memcpy(m_bitDMD, pixeldata, sizeof(m_bitDMD));
			file.close();

			uint32_t x = 0;
			uint32_t y = 0;
			for (int32_t byte = 0; byte < data.size(); ++byte)
			{
				uint8_t value = pixeldata[byte];
				for (uint32_t bitindex = 0; bitindex < 8; ++bitindex)
				{
					uint8_t bit = value & (1 << bitindex);
					if (bit)
						m_decodedDMD[FANTASIES_DMD_WIDTH * y + x] = 255;
					else
						m_decodedDMD[FANTASIES_DMD_WIDTH * y + x] = 0;

					++x;
				}
				if (x >= 160)
				{
					x = 0;
					++y;
				}
			}
			return true;
		}
#endif
		return false;
	}

#ifdef QT_VERSION
	QImage image(uint32_t marked_byte_index) const
	{
		QImage img(FANTASIES_DMD_WIDTH, FANTASIES_DMD_HEIGHT, QImage::Format_RGBA8888);

		uint32_t x = 0;
		uint32_t y = 0;
		for (int32_t byte = 0; byte < DMD_SIZE; ++byte)
		{
			uint8_t value = m_decodedDMD[byte];
			if (value)
			{
				if (byte / 8 == marked_byte_index)
					img.setPixel(x, y, qRgb(255, 0, 0));
				else
					img.setPixel(x, y, qRgb(value, value, value));
			}
			else
			{
				if (byte / 8 == marked_byte_index)
					img.setPixel(x, y, qRgb(0, 255, 0));
				else
					img.setPixel(x, y, qRgb(0, 0, 0));
			}
			if (++x >= FANTASIES_DMD_WIDTH)
			{
				x = 0;
				++y;
			}
		}

		return img;
	}

#endif
	inline uint8_t pixel(uint32_t pixel_number) const
	{
		assert(pixel_number < DMD_SIZE);
		return m_decodedDMD[pixel_number];
	}

	inline uint8_t pixel(int32_t x, int32_t y) const
	{
		assert(x >= 0 && x < FANTASIES_DMD_WIDTH);
		assert(y >= 0 && y < FANTASIES_DMD_HEIGHT);
		uint32_t pixel_number = y * FANTASIES_DMD_WIDTH + x;
		assert(pixel_number < DMD_SIZE);
		return m_decodedDMD[pixel_number];
	}

	inline uint8_t bytevalue(uint32_t bytenumber) const
	{
		assert(bytenumber < BIT_DMD_SIZE);
		return m_bitDMD[bytenumber];
	}

	inline uint8_t bitpixel(int32_t x, int32_t y) const
	{
		assert(x >= 0 && x < FANTASIES_DMD_WIDTH);
		assert(y >= 0 && y < FANTASIES_DMD_HEIGHT);
		uint32_t pixel_number = y * FANTASIES_DMD_WIDTH + x;
		uint32_t bytenumber = pixel_number / 8;
		assert(bytenumber < BIT_DMD_SIZE);
		uint32_t bitnumber = pixel_number % 8;

		uint8_t pixel = m_bitDMD[bytenumber];
		return (pixel & (1 << bitnumber)) > 0 ? 255 : 0;
	}


	bool is_column_candidate(uint32_t column) const
	{
		if (column >= FantasiesDMD::FANTASIES_DMD_WIDTH)
			return false;

		for (uint32_t y = 0; y < FantasiesDMD::FANTASIES_DMD_HEIGHT; ++y)
		{
			if (bitpixel(column, y) != 0)
			{
				return false;
			}
		}
		return true;
	}

	bool is_fantasies_logo() const
	{
		char cmpbufferp[]{ 0xF0, 0xF3, 0xF7, 0xF3 };
		return  (memcmp(m_bitDMD + 21, cmpbufferp, 4) == 0);
	}

	bool is_score() const
	{
		// Score
		char scorebuffer[]{ 0x3F, 0x07, 0x3E, 0x77 };
		return (memcmp(m_bitDMD + 21, scorebuffer, 4) == 0);
	}

	bool is_hiscore_label() const
	{
		char buffer[]{ 0xE0, 0x73, 0x70, 0x00 };
		return (memcmp(m_bitDMD + 20, buffer, 4) == 0);
	}

private:
	uint8_t m_bitDMD[BIT_DMD_SIZE];
	uint8_t m_decodedDMD[FANTASIES_DMD_WIDTH * FANTASIES_DMD_HEIGHT];
};

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
	FantasiesWindow(QWidget* parent, DMDAnimationEngine* animation_engine);
	~FantasiesWindow();

	void key_left();
	void key_right();
	void key_up();
	void key_down();

private:
	void initUI();
	void update_image();
	void paint_spans(const QImage& img);
	void determine_spans();
	bool is_scrolling_rtl() const;
	bool is_current_frame_empty() const;
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

	DMDAnimationEngine* m_animation_engine = nullptr;
	FantasiesDMD m_fantasies_DMD;
	int32_t m_byte_index = 0;
	std::vector<Span> m_spans;
	std::vector<Span> m_prev_frame_spans;
	EAnimation m_current_animation = NONE;
};
