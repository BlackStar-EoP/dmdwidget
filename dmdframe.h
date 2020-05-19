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
#include <assert.h>
#include <memory>
#include "dmdconfig.h"

class DMDFrame
{
public:
	DMDFrame(uint8_t* grayscale_frame, uint32_t* color_frame);
	DMDFrame();
	~DMDFrame() = default;

	const uint8_t* const const_grayscale_frame() const;
	const uint32_t* const const_color_frame() const;

	uint8_t* const grayscale_frame();
	uint32_t* const color_frame();

	inline void set_pixel(int32_t x, int32_t y, uint8_t grayscale)
	{
		assert(x >= 0 && x < DMDConfig::DMDWIDTH);
		assert(y >= 0 && y < DMDConfig::DMDHEIGHT);
		m_grayscale_frame[y * DMDConfig::DMDWIDTH + x] = grayscale;
	}

	inline void set_pixel(int32_t x, int32_t y, uint32_t color)
	{
		assert(x >= 0 && x < DMDConfig::DMDWIDTH);
		assert(y >= 0 && y < DMDConfig::DMDHEIGHT);
		m_color_frame[y * DMDConfig::DMDWIDTH + x] = color;
	}

	inline void set_pixel(int32_t x, int32_t y, uint8_t grayscale, uint32_t color)
	{
		set_pixel(x, y, grayscale);
		set_pixel(x, y, color);
	}

	inline void clear()
	{
		memset(m_grayscale_frame, 0, sizeof(m_grayscale_frame));
		memset(m_color_frame, 0, sizeof(m_color_frame));
	}

private:
	uint8_t m_grayscale_frame[DMDConfig::DMDWIDTH * DMDConfig::DMDHEIGHT];
	uint32_t m_color_frame[DMDConfig::DMDWIDTH * DMDConfig::DMDHEIGHT];
};
