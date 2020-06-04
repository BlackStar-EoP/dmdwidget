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

#include <stdint.h>

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

	inline uint32_t width() const
	{
		return m_end_column - m_start_column + 1;
	}

	inline bool is_big_span() const
	{
		return width() >= REMOVE_COLUMN_COUNT;
	}

	inline bool is_clear_screen() const
	{
		return width() == FANTASIES_DMD_WIDTH;
	}

	inline uint32_t start_column() const
	{
		return m_start_column;
	}

	inline uint32_t end_column() const
	{
		return m_end_column;
	}

	inline bool is_left_span() const
	{
		return m_start_column == 0;
	}

	inline bool is_right_span() const
	{
		return m_end_column == (FANTASIES_DMD_WIDTH - 1);
	}

private:

	uint32_t m_start_column = 0;
	uint32_t m_end_column = 0;
};
