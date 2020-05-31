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

#include "dmdframe.h"

class DMDAnimationEngine;
class QLabel;

class Span
{
public:
	static const uint32_t REMOVE_COLUMN_COUNT = 32;
	static const uint32_t FANTASIES_DMD_WIDTH = 160;

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

class FantasiesDMD
{
public:
	enum EAnimation
	{
		CLEAR_SCREEN,
		SCROLLING_RTL,
		LOOP,
		NONE
	};


	static const uint32_t FANTASIES_DMD_WIDTH = 160;
	static const uint32_t FANTASIES_DMD_HEIGHT = 16;
	static const uint32_t DMD_SIZE = FANTASIES_DMD_WIDTH * FANTASIES_DMD_HEIGHT;
	static const uint32_t BIT_DMD_SIZE = DMD_SIZE / 8;

	FantasiesDMD()
	{
		clear();
	}

	FantasiesDMD(uint32_t* dmddata)
	{
		int desty = 0;
		for (uint32_t y = 0; y < FANTASIES_DMD_HEIGHT + 0; ++y)
		{
			for (uint32_t x = 0; x < FANTASIES_DMD_WIDTH; ++x)
			{
				uint32_t pixel = dmddata[(y * 320 * 2) + (x * 2)];
				switch (pixel)
				{
				case 0xff000000:
				case 0xff515151:
				case 0xff555555:
				default:
					m_decodedDMD[desty * FANTASIES_DMD_WIDTH + x] = 0;
					break;
				case 0xfff3b245:
					m_decodedDMD[desty * FANTASIES_DMD_WIDTH + x] = 255;
					break;
				}
			}
			++desty;
		}

		uint32_t blockindex = 0;
		for (uint32_t block = 0; block < DMD_SIZE; block += 8)
		{
			uint8_t bitblock = 0;
			for (uint32_t pixel = 0; pixel < 8; ++pixel)
			{
				if (m_decodedDMD[block + pixel] == 255)
				{
					uint8_t bit = 1;
					bit <<= pixel;
					bitblock |= bit;
				}
			}
			m_bitDMD[blockindex++] = bitblock;
		}
	}

	inline void clear()
	{
		memset(m_bitDMD, 0, sizeof(m_bitDMD));
	}

#ifdef QT_VERSION
	inline bool read_file(const QString& filename)
	{
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
		return false;
	}

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

	QImage image()
	{
		QImage dmd(DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT, QImage::Format_RGBA8888);
		dmd.fill(Qt::black);

		for (int32_t y = 0; y < DMDConfig::DMDHEIGHT; ++y)
		{
			for (int32_t x = 0; x < DMDConfig::DMDWIDTH; ++x)
			{
				uint8_t pixel = m_dmd_frame.grayscale_pixel(x, y);
				dmd.setPixel(x, y, qRgb(pixel, pixel, pixel));
			}
		}

		return dmd;
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

	void determine_spans()
	{
		m_spans.clear();

		bool in_span = false;
		uint32_t span_start = 0;

		for (uint32_t x = 0; x < FantasiesDMD::FANTASIES_DMD_WIDTH + 1; ++x)
		{
			if (is_column_candidate(x))
			{
				if (!in_span)
				{
					in_span = true;
					span_start = x;
				}
			}
			else
			{
				if (in_span)
				{
					in_span = false;
					if (x - span_start > 3)
						m_spans.push_back(Span(span_start, x - 1));
				}
			}
		}
	}

	size_t num_spans() const
	{
		return m_spans.size();
	}

	const Span& span(size_t index) const
	{
		assert(index < m_spans.size());
		return m_spans[index];
	}

	const Span& first_span() const
	{
		assert(m_spans.size() != 0);
		return m_spans.front();
	}

	const Span& last_span() const
	{
		assert(m_spans.size() != 0);
		return m_spans.back();
	}


	bool is_scrolling_rtl() const
	{
		if (m_prev_frame_spans.size() != 1 || m_spans.size() != 1)
			return false;

		if (m_prev_frame_spans[0].is_clear_screen())
		{
			const Span& current_span = m_spans[0];
			if (current_span.start_column() == 0 &&
				current_span.end_column() == 158)
				return true;
		}

		return false;
	}

	bool is_current_frame_empty() const
	{
		if (m_spans.size() == 1)
			return m_spans[0].is_clear_screen();

		return false;
	}

	const DMDFrame& span_fix()
	{
		std::set<uint32_t> remove_columns;

		uint32_t num_big_spans = 0;
		for (size_t i = 0; i < num_spans(); ++i)
		{
			uint32_t spanwidth = span(i).width();
			if (spanwidth >= Span::REMOVE_COLUMN_COUNT)
			{
				++num_big_spans;
			}
		}

		if (num_big_spans > 0)
		{
			if (num_big_spans == 1)
			{
				for (size_t i = 0; i < num_spans(); ++i)
				{
					const Span& sp = span(i);
					if (sp.is_big_span())
					{
						if (!sp.is_left_span())
						{
							for (uint32_t i = 0; i < Span::REMOVE_COLUMN_COUNT; ++i)
							{
								remove_columns.insert(sp.start_column() + i);
							}
						}
						else
						{
							const Span& lspan = first_span();
							const Span& rspan = last_span();
							if (lspan.width() > 16 && rspan.width() > 16)
							{
								for (uint32_t i = 0; i < 16; ++i)
								{
									remove_columns.insert(lspan.start_column() + i);
									remove_columns.insert(rspan.start_column() + i);
								}
							}
						}
					}
				}
			}
			else
			{
				const Span& lspan = first_span();
				const Span& rspan = last_span();
				if (lspan.width() > 16 && rspan.width() > 16)
				{
					for (uint32_t i = 0; i < 16; ++i)
					{
						remove_columns.insert(lspan.start_column() + i);
						remove_columns.insert(rspan.start_column() + i);
					}
				}
			}
		}
		else if (num_spans() >= 2 || num_big_spans > 1) // attempt left and right removal
		{
			const Span& lspan = first_span();
			const Span& rspan = last_span();

			if (lspan.width() >= 16 && rspan.width() >= 16)
			{
				for (uint32_t i = 0; i < 16; ++i)
				{
					remove_columns.insert(lspan.start_column() + i);
					remove_columns.insert(rspan.start_column() + i);
				}
			}
			else
			{
				for (uint32_t i = 0; i < lspan.width(); ++i)
				{
					remove_columns.insert(lspan.start_column() + i);
				}
				for (uint32_t i = 0; i < rspan.width(); ++i)
				{
					remove_columns.insert(rspan.start_column() + i);
				}
				uint32_t remaining_columns = Span::REMOVE_COLUMN_COUNT - lspan.width() - rspan.width();
				if (num_spans() == 3)
				{
					const Span& mspan = span(1);
					if (mspan.width() > remaining_columns)
					{
						for (uint32_t i = 0; i < remaining_columns; ++i)
						{
							remove_columns.insert(mspan.start_column() + i);
						}
					}
				}
			}
		}


		if (remove_columns.size() == 0)
		{
			m_dmd_frame.clear();
			return m_dmd_frame;
		}

		int current_column = 0;
		for (uint32_t x = 0; x < FantasiesDMD::FANTASIES_DMD_WIDTH; ++x)
		{
			if (remove_columns.find(x) != remove_columns.end())
				continue;

			for (uint32_t y = 0; y < 16; ++y)
			{
				uint8_t val = pixel(x, y);
				m_dmd_frame.set_pixel(current_column, y + 8, qRgb(val, val, val));
			}
			++current_column;
			if (current_column == DMDConfig::DMDWIDTH)
				break;
		}

		return m_dmd_frame;
	}

	const DMDFrame& parsed_fix()
	{
		if (is_current_frame_empty())
		{
			m_current_animation = NONE;
		}
		else if (is_scrolling_rtl())
		{
			m_current_animation = SCROLLING_RTL;
		}

		if (m_current_animation == NONE)
		{
			// Fantasies logo
			if (is_fantasies_logo())
			{
				/// 12 - 67 = PINBALL
				copyblock(12, 0, 66, 15, 27, 0);
				copyblock(76, 0, 146, 15, 35, 16);
			}
			else if (is_score())
			{
				copyblock(8, 0, 76, 15, 8, 0);
				copyblock(77, 0, 159, 15, 45, 16);
			}
			else if (is_hiscore_label())
			{
				//4 - 66 -> 33
				//76 - 154 -> 25
				copyblock(4, 0, 66, 15, 33, 0);
				copyblock(76, 0, 154, 15, 25, 16);
			}
		}
		else if (m_current_animation == SCROLLING_RTL)
		{
			copyblock(32, 0, 159, 15, 0, 8);
		}

		m_prev_frame_spans = m_spans;
		return m_dmd_frame;
	}

	void copyblock(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t dest_x, int32_t dest_y)
	{
		assert(x1 <= x2);
		assert(y1 <= y2);
		int32_t width = x2 - x1 + 1;
		int32_t height = y2 - y1 + 1;
		assert(width <= FantasiesDMD::FANTASIES_DMD_WIDTH);
		assert(height <= FantasiesDMD::FANTASIES_DMD_HEIGHT);

		for (int32_t y = 0; y < height; ++y)
		{
			for (int32_t x = 0; x < width; ++x)
			{
				uint8_t val = pixel(x + x1, y + y1);
				m_dmd_frame.set_pixel(dest_x + x, dest_y + y, val);
			}
		}
	}

private:
	uint8_t m_bitDMD[BIT_DMD_SIZE];
	uint8_t m_decodedDMD[FANTASIES_DMD_WIDTH * FANTASIES_DMD_HEIGHT];
	DMDFrame m_dmd_frame;

	std::vector<Span> m_spans;
	std::vector<Span> m_prev_frame_spans;

	EAnimation m_current_animation = NONE;
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
};
