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

#include <QApplication>
#include <QTimer>

#include "fx3animation.h"

class DMDAnimationEngine;
class FX3Process;

class DMDApplication : public QApplication
{
	Q_OBJECT
public:
	DMDApplication(int argc, char *argv[], FX3Process* fx3_process);

	void set_animation_engine(DMDAnimationEngine* animation_engine);
	void start_polling();

private slots:
	void find_FX3_executable_timeout();
	void find_DMD_timeout();

private:
	QTimer m_find_fx3_timer;
	QTimer m_find_DMD_timer;
	DMDAnimationEngine* m_animation_engine = nullptr;
	FX3Process* m_fx3_process = nullptr;
	FX3Animation m_fx3_animation;

	bool m_valid_DMD = false;
};