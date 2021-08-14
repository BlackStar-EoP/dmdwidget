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

#include "mainwindow.h"

#include "dmdanimationengine.h"
#include "dmdapplication.h"
#include "widgetoutputdevice.h"
#include "pindmd2outputdevice.h"
#include "fx3process.h"
#include "animationwindow.h"
#include "deviceconfigwindow.h"
#include "rominspectwindow2.h"
#include "fantasieswindow.h"

#include <QtWidgets>
#include <QImage>
#include <QFile>
#include <QPainter>

uint8_t find_closest_palette_color(uint8_t val)
{
#define ONE_BIT
#ifdef ONE_BIT
	if (val < 128)
		return 0;

	return 255;
#else
	if (val < (85 - 42))
		return 0;
	if (val < (170 - 42))
		return 85;
	if (val < (255 - 42))
		return 170;
	return 255;
#endif
}

void dump_vertical_scroll(const QString& filename)
{
	QImage source;
	if (!source.load(filename))
		return;

	if (source.height() < 32 || source.width() != 128)
		return;

	uint32_t frame_nr = 0;
	for (int32_t y = 0; y < source.height() - 32; ++y)
	{
		QImage frame(128, 32, QImage::Format_RGBA8888);
		QPainter p(&frame);
		p.drawImage(QPoint(0,0), source.copy(QRect(0, frame_nr, 128, 32)));
		QString fileName = QString("scroll/") + QString::number(frame_nr).rightJustified(4, '0') + ".png";
		frame.save(fileName);
		++frame_nr;
	}
}

void floyd_steinberg_dither(const QString& filename)
{
	QImage source;
	if (!source.load(filename))
		return;

	source = source.convertToFormat(QImage::Format_RGBA8888);
	for (int y = 0; y < source.height(); ++y)
	{
		for (int x = 0; x < source.width(); ++x)
		{
			int gray = qGray(source.pixel(x, y));
			source.setPixelColor(x, y, QColor(gray, gray, gray));
		}
	}

	
	for (int y = 0; y < source.height() - 1; ++y)
	{
		for (int x = 1; x < source.width() - 1; ++x)
		{
			uint8_t oldVal = qRed(source.pixel(x, y));
			uint8_t newVal = find_closest_palette_color(oldVal);
			source.setPixelColor(x, y, QColor(newVal, newVal, newVal));
			int32_t quant_error = oldVal - newVal;
			uint32_t px1 = qRed(source.pixel(x + 1, y    )) + (quant_error * 7 / 16);
			uint32_t px2 = qRed(source.pixel(x - 1, y + 1)) + (quant_error * 3 / 16);
			uint32_t px3 = qRed(source.pixel(x    , y + 1)) + (quant_error * 5 / 16);
			uint32_t px4 = qRed(source.pixel(x + 1, y + 1)) + (quant_error * 1 / 16);
			if (px1 > 255) px1 = 255;
			if (px2 > 255) px2 = 255;
			if (px3 > 255) px3 = 255;
			if (px4 > 255) px4 = 255;
			source.setPixelColor(x + 1, y    , QColor(px1, px1, px1));
			source.setPixelColor(x - 1, y + 1, QColor(px2, px2, px2));
			source.setPixelColor(x    , y + 1, QColor(px3, px3, px3));
			source.setPixelColor(x + 1, y + 1, QColor(px4, px4, px4));
		}
	}
	source.save(filename + "_dithered.png");
	/*
	for each y from top to bottom
	for each x from left to right
	oldpixel  := pixel[x][y]
	newpixel  := find_closest_palette_color(oldpixel)
	pixel[x][y]  := newpixel
	quant_error  := oldpixel - newpixel
	pixel[x + 1][y    ] := pixel[x + 1][y    ] + quant_error * 7 / 16
	pixel[x - 1][y + 1] := pixel[x - 1][y + 1] + quant_error * 3 / 16
	pixel[x    ][y + 1] := pixel[x    ][y + 1] + quant_error * 5 / 16
	pixel[x + 1][y + 1] := pixel[x + 1][y + 1] + quant_error * 1 / 16
	*/
}
int main(int argc, char *argv[])
{
	//dump_vertical_scroll("scroll.png");

	/*
	*/
	//QImage img(QDir::currentPath() + "/fx3.png");
	//QImage mask(img.width(), img.height(), img.format());
	//for (int y = 0; y < img.height(); ++y)
	//	for (int x = 0; x < img.width(); ++x)
	//	{
	//		QRgb px = img.pixel(x, y);
	//		int a = qAlpha(px);
	//		mask.setPixel(x, y, qRgb(a, a, a));
	//	}

	//mask.save("fx3mask.png");
	/*
	*/

	FX3Process fx3_process;
	DMDApplication app(argc, argv, &fx3_process);
	//floyd_steinberg_dither("01.JPG");
	//floyd_steinberg_dither("02.JPG");
	//floyd_steinberg_dither("03.JPG");
	//floyd_steinberg_dither("04.JPG");
	//floyd_steinberg_dither("05.JPG");
	
	QVector<DMDOutputDevice*> output_devices;
	output_devices.push_back(new PinDMD2OutputDevice());
	output_devices.push_back(new WidgetOutputDevice(nullptr, 4));
	
	DMDAnimationEngine animation_engine(output_devices);
	app.set_animation_engine(&animation_engine);
	AnimationWindow animation_window(nullptr, &animation_engine);
	//DeviceConfigWindow config_window(nullptr);
	ROMInspectWindow2 rom_window(nullptr, nullptr);
	//FantasiesWindow fantasies_window(nullptr, &animation_engine);

	app.start_polling();
	
    int32_t ret = app.exec();
	return ret;
}