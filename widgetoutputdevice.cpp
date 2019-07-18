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

#pragma once

#include <widgetoutputdevice.h>

#include "dmddata.h"

#include <QLabel>
#include <QImage>
#include <QPainter>
#include <QPoint>


WidgetOutputDevice::WidgetOutputDevice(QWidget* parent, uint32_t size)
: QWidget(parent, Qt::Window)
{
	m_DMD_label = new QLabel(this);

	m_DMD_width = DMDData::DMDWIDTH * size;
	m_DMD_height = DMDData::DMDHEIGHT * size;

	m_DMD_label->setGeometry(0, 0, m_DMD_width, m_DMD_height);
	clearDMD();
	showNormal();
}

WidgetOutputDevice::~WidgetOutputDevice()
{
}

bool WidgetOutputDevice::isDeviceAvailable()
{
	return true;
}

void WidgetOutputDevice::clearDMD()
{
	QImage image(DMDData::DMDWIDTH * 2, DMDData::DMDHEIGHT * 2, QImage::Format_RGBA8888);
	QPainter p(&image);
	p.fillRect(QRect(0, 0, DMDData::DMDWIDTH * 2, DMDData::DMDHEIGHT * 2), Qt::black);
	m_DMD_label->setPixmap(QPixmap::fromImage(image).scaled(QSize(m_DMD_width, m_DMD_height)));
}

void WidgetOutputDevice::sendFrame(const DMDData& frame)
{
	const uint8_t* const frameData = frame.frameData();

	QImage image(DMDData::DMDWIDTH * 2, DMDData::DMDHEIGHT * 2, QImage::Format_RGBA8888);
	QPainter p(&image);

	// Fill DMD with colors
	uint32_t bytepos = 0;
	for (int y = 0; y < DMDData::DMDHEIGHT; ++y)
	{
		for (int x = 0; x < DMDData::DMDWIDTH; ++x)
		{
			uint8_t c = frameData[bytepos];
			float col = c / 255.0f;
			p.setPen(QColor(c * m_r, c * m_g, c * m_b));
			p.drawPoint(x * 2, y * 2);
			bytepos++;
		}
	}

	m_DMD_label->setPixmap(QPixmap::fromImage(image).scaled(QSize(m_DMD_width, m_DMD_height)));
}

bool WidgetOutputDevice::supportsColor() const
{
	return true;
}

void WidgetOutputDevice::setColor(float r, float g, float b)
{
	m_r = r;
	m_g = g;
	m_b = b;
}