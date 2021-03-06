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

#ifdef QT_VERSION
#include <QWidget>
#include <QSet>
#include <QMap>
#include <QVector>
#include <QFile>
#include <QTimer>
#include <QTextEdit>
#include <QLineEdit>
#endif

#include "span.h"

#include <set>
#include <vector>
#include <assert.h>


#include "dmdframe.h"

class DMDAnimationEngine;
class QLabel;

class FantasiesDMD
{
public:
	enum EAnimation
	{
		CLEAR_SCREEN,
		SCROLLING_RTL,
		LOOP,
		CYCLONE,
		MILLION,
		BONUS, // 27994
		SCORE,
		SCORE2,
		MEGALAUGH,
		REALSIMULATOR,
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
				default:
					m_decodedDMD[desty * FANTASIES_DMD_WIDTH + x] = 0;
					break;
				case 0xfff3d330:
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

		determine_spans();
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

			determine_spans();
			return true;
		}
		return false;
	}

	QImage image(uint32_t marked_byte_index, bool color_marked_byte = true) const
	{
		QImage img(FANTASIES_DMD_WIDTH, FANTASIES_DMD_HEIGHT, QImage::Format_RGBA8888);

		uint32_t x = 0;
		uint32_t y = 0;
		for (int32_t byte = 0; byte < DMD_SIZE; ++byte)
		{
			uint8_t value = m_decodedDMD[byte];
			if (value)
			{
				if (color_marked_byte && (byte / 8 == marked_byte_index))
					img.setPixel(x, y, qRgb(255, 0, 0));
				else
					img.setPixel(x, y, qRgb(value, value, value));
			}
			else
			{
				if (color_marked_byte && (byte / 8 == marked_byte_index))
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
		return image(0, false);
	}

	QImage dmd_image()
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


	inline bool is_column_candidate(uint32_t column) const
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

	inline bool is_fantasies_logo() const
	{
		char cmpbufferp[]{ 0x70, 0xF0, 0x77, 0xF7 };
		return  (memcmp(m_bitDMD + 261, cmpbufferp, 4) == 0);
	}

	inline bool is_the_real_simulator() const
	{
		char cmpbufferp[]{ 0x3F, 0x7F, 0x3E, 0x07 };
		return  (memcmp(m_bitDMD + 305, cmpbufferp, 4) == 0);
	}

	inline bool is_score() const
	{
		// Score
		char scorebuffer[]{ 0x3F, 0x07, 0x3E, 0x77 };
		return (memcmp(m_bitDMD + 21, scorebuffer, 4) == 0);
	}

	inline bool is_score2() const
	{
		// Score
		char scorebuffer[]{ 0x3F, 0x07, 0x3E, 0x77 };
		return (memcmp(m_bitDMD + 20, scorebuffer, 4) == 0);
	}

	inline bool is_hiscore_label() const
	{
		char buffer[]{ 0xE0, 0x73, 0x70, 0x00 };
		return (memcmp(m_bitDMD + 20, buffer, 4) == 0);
	}

	inline bool is_hiscore_scores() const
	{
		return m_bitDMD[6] == 0 && m_bitDMD[26] == 0 && m_bitDMD[46] == 0 &&
			m_bitDMD[146] == 0x7F && m_bitDMD[313] == 0xC0 && m_bitDMD[316] == 0xC0;
	}

	inline bool is_crazy_letter_spotted() const
	{
		char buffer[]{ 0x7F, 0x7F, 0x7F, 0x7F };
		return (memcmp(m_bitDMD + 60, buffer, 4) == 0);
	}

	inline bool is_tunnel_skill_shot() const
	{
		char buffer[]{ 0x7F, 0x77, 0x7F, 0x7F };
		return (memcmp(m_bitDMD + 41, buffer, 4) == 0);
	}

	inline bool is_cyclone_skill_shot() const
	{
		char buffer[]{ 0x7F, 0x77, 0x7F, 0x07 };
		return (memcmp(m_bitDMD + 41, buffer, 4) == 0);
	}

	inline bool is_shoot_again() const
	{
		char buffer[]{ 0x7E, 0x77, 0x3E, 0x3E };
		return (memcmp(m_bitDMD + 20, buffer, 4) == 0);
	}

	inline bool is_hit_some_ducks_first() const
	{
		char buffer[]{ 0x77, 0x7F, 0x7F, 0x00 };
		return (memcmp(m_bitDMD + 20, buffer, 4) == 0);
	}

	inline bool is_party_on() const
	{
		char buffer[]{ 0x3F, 0x3E, 0x3F, 0x7F };
		return (memcmp(m_bitDMD + 21, buffer, 4) == 0);
	}

	inline bool is_match() const
	{
		if (m_bitDMD[0] != 0 &&
			m_bitDMD[20] != 0 &&
			m_bitDMD[40] != 0 &&
			m_bitDMD[60] != 0 &&
			m_bitDMD[80] != 0)
		{
			char buffer[]{ 0x00, 0x00, 0x00, 0x00 };
			return (memcmp(m_bitDMD + 1, buffer, 4) == 0) &&
				(memcmp(m_bitDMD + 21, buffer, 4) == 0) &&
				(memcmp(m_bitDMD + 41, buffer, 4) == 0) &&
				(memcmp(m_bitDMD + 61, buffer, 4) == 0) &&
				(memcmp(m_bitDMD + 81, buffer, 4) == 0);
		}
		
		return false;
	}

	inline bool is_eob_cyclone() const
	{
		//27841
		char buffer[]{ 0x3E, 0x77, 0x3E, 0x07 };
		return (memcmp(m_bitDMD + 3, buffer, 4) == 0);
	}

	inline bool is_eob_counting() const
	{
		// 27991 & 68089
		return m_bitDMD[127] == 0x3E &&
			m_bitDMD[147] == 0x7F &&
			m_bitDMD[167] == 0x73 &&
			m_bitDMD[187] == 0x7B &&

			m_bitDMD[39] == 0x3E &&
			m_bitDMD[59] == 0x7F &&
			m_bitDMD[79] == 0x7F &&
			m_bitDMD[99] == 0x77;
	}

	inline bool is_cyclone() const
	{
		// 20787
		char buffer[]{ 0x00, 0x7F, 0xCB, 0xE5 };
		return (memcmp(m_bitDMD + 207, buffer, 4) == 0);
	}

	inline bool is_mega_laugh() const
	{
		char buffer[]{ 0x90, 0xE8, 0x4D, 0x1A };
		return (memcmp(m_bitDMD + 20, buffer, 4) == 0);
	}

	inline size_t num_spans() const
	{
		return m_spans.size();
	}

	inline const Span& span(size_t index) const
	{
		assert(index < m_spans.size());
		return m_spans[index];
	}

	inline const Span& first_span() const
	{
		assert(m_spans.size() != 0);
		return m_spans.front();
	}

	inline const Span& last_span() const
	{
		assert(m_spans.size() != 0);
		return m_spans.back();
	}

	inline bool is_scrolling_rtl() const
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

	inline bool is_loop() const
	{
		// 15586
		return m_bitDMD[262] == 0xE0 &&
			m_bitDMD[282] == 0xF8 &&
			m_bitDMD[302] == 0x1C;
	}

	inline bool is_current_frame_empty() const
	{
		if (m_spans.size() == 1)
			return m_spans[0].is_clear_screen();

		return false;
	}

	inline bool apply_span_fix()
	{
		std::set<uint32_t> remove_columns;

		uint32_t num_big_spans = 0;
		uint32_t total_span_width = 0;
		for (size_t i = 0; i < num_spans(); ++i)
		{
			uint32_t spanwidth = span(i).width();
			total_span_width += spanwidth;

			if (spanwidth >= Span::REMOVE_COLUMN_COUNT)
			{
				++num_big_spans;
			}
		}

		// Can't fix this by removing columns
		if (total_span_width < Span::REMOVE_COLUMN_COUNT)
			return false;

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

		// Not enough columns to remove for proper image
		if (remove_columns.size() < Span::REMOVE_COLUMN_COUNT)
			return false;

		int current_column = 0;
		for (uint32_t x = 0; x < FANTASIES_DMD_WIDTH; ++x)
		{
			if (remove_columns.find(x) != remove_columns.end())
				continue;

			for (uint32_t y = 0; y < FANTASIES_DMD_HEIGHT; ++y)
			{
				uint8_t val = pixel(x, y);
				m_dmd_frame.set_pixel(current_column, y + 8, val);
			}
			++current_column;
			if (current_column == DMDConfig::DMDWIDTH)
				break;
		}

		return true;
	}

	inline bool apply_span_fix2()
	{
		std::set<uint32_t> remove_columns;

		uint32_t total_span_width = 0;
		for (size_t i = 0; i < num_spans(); ++i)
		{
			total_span_width += span(i).width();
		}

		// Can't fix this by removing columns
		if (total_span_width < Span::REMOVE_COLUMN_COUNT)
			return false;

		if (num_spans() >= 2) // attempt left and right removal
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
				else // More than 3
				{
					uint32_t remaining_span_width = 0;
					for (size_t i = 1; i < num_spans() - 1; ++i)
					{
						const Span& sp = span(i);
						remaining_span_width += sp.width();
					}

					if (remaining_span_width > remaining_columns)
					{
						uint32_t iteration = 0;
						
						while (remaining_columns > 0)
						{
							uint32_t prev_remaining_columns = remaining_columns;
							for (size_t i = 1; i < num_spans() - 1; ++i)
							{
								const Span& sp = span(i);
								if (sp.width() >= (iteration + 1))
								{
									remove_columns.insert(sp.start_column() + iteration);
									remaining_columns--;
									if (remaining_columns == 0)
										break;
								}
							}
							++iteration;

							// if no columns were removed, we're stuck in an infinite loop
							if (prev_remaining_columns == remaining_columns)
								return false;
						}
					}
				}
			}
		}

		// Not enough columns to remove for proper image
		if (remove_columns.size() < Span::REMOVE_COLUMN_COUNT)
			return false;

		apply_removed_columns(remove_columns);

		return true;
	}

	inline void apply_removed_columns(const std::set<uint32_t> remove_columns)
	{
		int current_column = 0;
		for (uint32_t x = 0; x < FANTASIES_DMD_WIDTH; ++x)
		{
			if (remove_columns.find(x) != remove_columns.end())
				continue;

			for (uint32_t y = 0; y < FANTASIES_DMD_HEIGHT; ++y)
			{
				uint8_t val = pixel(x, y);
				m_dmd_frame.set_pixel(current_column, y + (FANTASIES_DMD_HEIGHT / 2), val);
			}
			++current_column;
			if (current_column == DMDConfig::DMDWIDTH)
				break;
		}
	}
	inline void apply_crop_fix()
	{
		copyblock(16, 0, 143, 15, 0, 8);
	}

	inline const DMDFrame& parse_DMD()
	{
		m_dmd_frame.clear();

		if (is_current_frame_empty())
		{
			m_current_animation = NONE;
		}
		else if (is_scrolling_rtl())
		{
			m_current_animation = SCROLLING_RTL;
		}
		else if (is_loop())
		{
			m_current_animation = LOOP;
		}
		else if (is_cyclone())
		{
			m_current_animation = CYCLONE;
		}
		else if (is_mega_laugh())
		{
			m_current_animation = MEGALAUGH;
		}
		else if (m_current_animation == NONE)
		{
			// Fantasies logo
			if (is_fantasies_logo())
			{
				/// 12 - 67 = PINBALL
				copyblock_centered(12, 0, 66, 15, 0);
				copyblock_centered(76, 0, 146, 15, 16);
			}
			else if (is_the_real_simulator())
			{
				m_current_animation = REALSIMULATOR;
			}
			else if (is_score())
			{
				m_current_animation = SCORE;
			}
			else if (is_score2())
			{
				m_current_animation = SCORE2;
			}
			else if (is_eob_cyclone())
			{
				copyblock_centered(0, 0, 87, 5, 0);
				copyblock_centered(96, 0, 159, 5, 7);
				copyblock(16, 6, 143, 15, 0, 17);
				uint8_t correction = 255;
				uint8_t correction2 = 0;
				m_dmd_frame.set_pixel(70, 13, correction);
				m_dmd_frame.set_pixel(71, 13, correction);

				m_dmd_frame.set_pixel(119, 17, correction2);
				m_dmd_frame.set_pixel(118, 17, correction2);

			}
			else if (is_eob_counting())
			{
				copyblock_centered(0, 0, 62, 15, 0);
				copyblock_centered(96, 0, 159, 15, 16);
			}
			else if (is_hiscore_label())
			{
				copyblock(4, 0, 66, 15, 33, 0);
				copyblock(76, 0, 154, 15, 25, 16);
			}
			else if (is_hiscore_scores())
			{
				if (m_spans.size() > 0)
				{
					copyblock_centered(0, 0, 38, 15, 0);
					uint32_t score_start = m_spans.back().end_column() + 1;
					copyblock_centered(score_start, 0, 159, 15, 15);
				}
			}
			else if (is_crazy_letter_spotted())
			{
				copyblock_centered(0, 0, 94, 15, 0);
				copyblock_centered(104, 0, 159, 15, 16);
			}
			else if (is_tunnel_skill_shot())
			{
				copyblock_centered(8, 0, 54, 15, 0);
				copyblock_centered(64, 0, 142, 15, 16);
			}
			else if (is_cyclone_skill_shot())
			{
				copyblock_centered(8, 0, 62, 15, 0);
				copyblock_centered(72, 0, 150, 15, 16);
			}
			else if (is_shoot_again())
			{
				copyblock_centered(0, 0, 86, 15, 0);
				copyblock_centered(96, 0, 159, 15, 16);
			}
			else if (is_hit_some_ducks_first())
			{
				copyblock_centered(0, 0, 62, 15, 0);
				copyblock_centered(72, 0, 159, 15, 16);
			}
			else if (is_party_on())
			{
				copyblock_centered(8, 0, 70, 15, 0);
				copyblock_centered(88, 0, 150, 15, 16);
			}
			else if (is_match())
			{
				//0 =            0 -   6
				//1 = 7,16      17 -  20
				//2 = 7,31      32 -  38
				//3 = 7,47      48 -  54
				//4 = 7,63      64 -  70
				//5 = 7,79      80 -  86
				//6 = 7,95      96 - 102
				//7 = 7,111    112 - 118
				//8 = 7,127    128 - 134
				//9 = 7,143    144 - 150
				std::set<uint32_t> remove_columns;
				for (uint32_t i = 151; i < 156; ++i)
				{
					remove_columns.insert(i);
				}
				remove_columns.insert(7);
				remove_columns.insert(31);
				remove_columns.insert(39);
				remove_columns.insert(55);
				remove_columns.insert(71);
				remove_columns.insert(87);
				remove_columns.insert(103);
				remove_columns.insert(119);
				remove_columns.insert(135);

				remove_columns.insert(8);
				remove_columns.insert(32);
				remove_columns.insert(40);
				remove_columns.insert(56);
				remove_columns.insert(72);
				remove_columns.insert(88);
				remove_columns.insert(104);
				remove_columns.insert(120);
				remove_columns.insert(136);

				remove_columns.insert(9);
				remove_columns.insert(33);
				remove_columns.insert(41);
				remove_columns.insert(57);
				remove_columns.insert(73);
				remove_columns.insert(89);
				remove_columns.insert(105);
				remove_columns.insert(121);
				remove_columns.insert(137);
				apply_removed_columns(remove_columns);
			}
			else
			{
				if (!apply_span_fix2())
					apply_crop_fix();
			}
		}

		if (m_current_animation == SCROLLING_RTL)
		{
			copyblock(32, 0, 159, 15, 0, 8);
		}
		else if (m_current_animation == LOOP)
		{
			copyblock_centered(19, 0, 132, 15, 8);
		}
		else if (m_current_animation == CYCLONE)
		{
			copyblock_centered(21, 0, 131, 15, 8);
		}
		else if (m_current_animation == SCORE)
		{
			copyblock(8, 0, 76, 15, 8, 0);
			copyblock(77, 0, 159, 15, 45, 16);
		}
		else if (m_current_animation == SCORE2)
		{
			copyblock(0, 0, 76, 15, 0, 0);
			copyblock(77, 0, 159, 15, 45, 16);
		}
		else if (m_current_animation == MEGALAUGH)
		{
			copyblock(0, 0, 30, 15, 0, 0);
			copyblock(129, 0, 159, 15, 97, 0);
			copyblock_centered(31, 0, 129, 15, 15);
		}
		else if (m_current_animation == REALSIMULATOR)
		{
			copyblock_centered(8, 0, 70, 15, 0);
			copyblock_centered(80, 0, 150, 15, 16);
		}
		// 17537 MEGALAUGH
		// 19678 MEGALAUGH TOTAL SCROLL

		// 22087 EAT SOME POPCORNS  not working
		// 24012 SCORE disappearing not working
		// 24697 4X bonus not working

		// 25660 EXTRA BALL
		// 27740 BONUS
		// 27989 BONUS COUNT
		// 28107 SHOOT AGAIN
		// 31748 TUNNEL SKILL SHOT
		// 35645 HIT SOME DUCKS FIRST
		// 37371 TILT TILT TILT
		// 37755 CYCLONE SKILL SHOT
		// 68158 MATCH
		// 68261 HISCORE
		m_prev_frame_spans = m_spans;
		return m_dmd_frame;
	}

	inline void copyblock(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t dest_x, int32_t dest_y)
	{
		assert(x1 <= x2);
		assert(y1 <= y2);
		int32_t width = x2 - x1 + 1;
		int32_t height = y2 - y1 + 1;
		assert(width <= FantasiesDMD::FANTASIES_DMD_WIDTH);
		assert(height <= FantasiesDMD::FANTASIES_DMD_HEIGHT);
		assert(dest_x + width <= DMDConfig::DMDWIDTH);
		assert(dest_y + height <= DMDConfig::DMDHEIGHT);
		for (int32_t y = 0; y < height; ++y)
		{
			for (int32_t x = 0; x < width; ++x)
			{
				uint8_t val = pixel(x + x1, y + y1);
				m_dmd_frame.set_pixel(dest_x + x, dest_y + y, val);
			}
		}
	}

	inline void copyblock_centered(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t dest_y)
	{
		int32_t width = x2 - x1 + 1;
		int32_t dest_x = (DMDConfig::DMDWIDTH - width) / 2;
		copyblock(x1, y1, x2, y2, dest_x, dest_y);
	}
private:
	inline void determine_spans()
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

private:
	uint8_t m_bitDMD[BIT_DMD_SIZE];
	uint8_t m_decodedDMD[FANTASIES_DMD_WIDTH * FANTASIES_DMD_HEIGHT];
	DMDFrame m_dmd_frame;

	std::vector<Span> m_spans;
	std::vector<Span> m_prev_frame_spans;

	EAnimation m_current_animation = NONE;
};
