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

#include <QtWidgets>
#include <QImage>
#include <QFile>

int main(int argc, char *argv[])
{
	FX3Process fx3_process;
	DMDApplication app(argc, argv, &fx3_process);

	DMDOutputDevice* outputDevice = new WidgetOutputDevice(nullptr, 4);
	DMDAnimationEngine animation_engine(outputDevice);
	app.set_animation_engine(&animation_engine);

	AnimationWindow animation_window(nullptr, &animation_engine);
	app.start_polling();
	
    return app.exec();
}