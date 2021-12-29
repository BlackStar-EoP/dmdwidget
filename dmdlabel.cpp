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

#include <QPainter>
#include <QImage>
#include "dmdlabel.h"


DMDLabel::DMDLabel(QWidget* parent, Qt::WindowFlags f)
 : QLabel(parent, f)
{
}

void DMDLabel::show_frame(const DMDFrame& frame)
{
	m_frame = frame;
	update_frame();
}

void DMDLabel::setPosition(int x, int y)
{
	setGeometry(x, y, DMDConfig::DMDWIDTH * m_dmd_scale_factor, DMDConfig::DMDHEIGHT * m_dmd_scale_factor);
}

void DMDLabel::update_frame()
{

	QImage image(DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT, QImage::Format_RGBA8888);
	QPainter p(&image);
	p.fillRect(0, 0, DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT, Qt::black);

	// TODO sometime (probably never) implement proper support for color DMD images...
	//if (m_use_RGB)
	//{
	//	const uint32_t* const frameData = frame.const_color_frame();

	//	uint32_t pos = 0;
	//	for (int y = 0; y < DMDConfig::DMDHEIGHT; ++y)
	//	{
	//		for (int x = 0; x < DMDConfig::DMDWIDTH; ++x)
	//		{
	//			uint32_t col = frameData[pos];
	//			p.setPen(QColor(qRed(col), qGreen(col), qBlue(col)));
	//			p.drawPoint(x * 2, y * 2);
	//			pos++;
	//		}
	//	}
	//}
	//else
	{
		const uint8_t* const frameData = m_frame.const_grayscale_frame();

		uint32_t bytepos = 0;
		for (int y = 0; y < DMDConfig::DMDHEIGHT; ++y)
		{
			for (int x = 0; x < DMDConfig::DMDWIDTH; ++x)
			{
				uint8_t c = frameData[bytepos];
				uint8_t r = c * m_dot_color.redF();
				uint8_t g = c * m_dot_color.greenF();
				uint8_t b = c * m_dot_color.blueF();
				p.setPen(QColor(r, g, b));
				p.drawPoint(x, y);
				bytepos++;
			}
		}
	}

	setPixmap(QPixmap::fromImage(image).scaled(QSize(DMDConfig::DMDWIDTH * m_dmd_scale_factor, DMDConfig::DMDHEIGHT * m_dmd_scale_factor)));
}