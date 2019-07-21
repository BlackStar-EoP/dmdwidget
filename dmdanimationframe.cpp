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

#include "dmdanimationframe.h"

#include <QImage>
#include <assert.h>

DMDAnimationFrame::DMDAnimationFrame(const QImage& image)
{
	parse_image(image);

}

const uint8_t* const DMDAnimationFrame::grayscale_frame() const
{
	return m_grayscale_frame;
}

const uint32_t* const DMDAnimationFrame::color_frame() const
{
	return m_color_frame;
}

void DMDAnimationFrame::parse_image(const QImage& image)
{
	uint32_t index = 0;
	assert(image.width() == DMDData::DMDWIDTH);
	assert(image.height() == DMDData::DMDHEIGHT);

	for (int32_t y = 0; y < image.height(); ++y)
	{
		for (int32_t x = 0; x < image.width(); ++x)
		{
			const QRgb& pixel = image.pixel(x, y);
			switch (m_grayscale_mode)
			{
			case AVERAGE:
				m_grayscale_frame[index] = (qRed(pixel) + qGreen(pixel) + qBlue(pixel)) / 3;
				break;

			case RED_CHANNEL_ONLY:
				m_grayscale_frame[index] = qRed(pixel);
				break;

			case GREEN_CHANNEL_ONLY:
				m_grayscale_frame[index] = qGreen(pixel);
				break;

			case BLUE_CHANNEL_ONLY:
				m_grayscale_frame[index] = qBlue(pixel);
				break;
			}
			m_color_frame[index] = pixel;
			++index;
		}
	}
}