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

#include "mainwindow.h"

#include "dmdanimationengine.h"
#include "dmdapplication.h"
#include "widgetoutputdevice.h"
#include "fx3process.h"
#include "animationwindow.h"
#include "deviceconfigwindow.h"

#include <QtWidgets>
#include <QImage>
#include <QFile>

uint8_t find_closest_palette_color(uint8_t val)
{
	if (val < 128)
		return 0;

	return 255;
}

void floyd_steinberg_dither(const QString& filename)
{
	QImage source;
	if (!source.load(filename))
		return;

	for (int y = 0; y < source.height(); ++y)
	{
		for (int x = 0; x < source.width(); ++x)
		{
			source.setPixel(x, y, qGray(source.pixel(x, y)));
		}
	}


	QImage dest(source.width(), source.height(), QImage::Format_RGBX8888);
	for (int y = 0; y < source.height() - 1; ++y)
	{
		for (int x = 0; x < source.width() - 1; ++x)
		{
			uint8_t oldVal = qRed(source.pixel(x, y));
			uint8_t newVal = find_closest_palette_color(oldVal);

		}
	}

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
	FX3Process fx3_process;
	DMDApplication app(argc, argv, &fx3_process);

	DMDOutputDevice* outputDevice = new WidgetOutputDevice(nullptr, 4);
	DMDAnimationEngine animation_engine(outputDevice);
	app.set_animation_engine(&animation_engine);

	AnimationWindow animation_window(nullptr, &animation_engine);
	DeviceConfigWindow config_window(nullptr);

	app.start_polling();
	
    return app.exec();
}