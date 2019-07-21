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

#include "dmddata.h"

#include <stdint.h>

class QImage;

class DMDAnimationFrame
{
public:
	enum EGrayScaleMode
	{
		AVERAGE,
		RED_CHANNEL_ONLY,
		GREEN_CHANNEL_ONLY,
		BLUE_CHANNEL_ONLY,
	};

public:
	DMDAnimationFrame(const QImage& image);
	~DMDAnimationFrame() = default;

	const uint8_t* const grayscale_frame() const;
	const uint32_t* const color_frame() const;

private:
	void parse_image(const QImage& image);

private:
	uint8_t m_grayscale_frame[DMDData::DMDWIDTH * DMDData::DMDHEIGHT];
	uint32_t m_color_frame[DMDData::DMDWIDTH * DMDData::DMDHEIGHT];
	EGrayScaleMode m_grayscale_mode = RED_CHANNEL_ONLY;
};