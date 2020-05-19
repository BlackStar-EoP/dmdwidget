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

#include "dmdkeys.h"

#include <QThread>
#include <QMutex>
#include <QVector>
#include <QImage>

class DMDOutputDevice;
class DMDAnimation;

class AnimationThread : public QThread
{
public:
	AnimationThread(QVector<DMDOutputDevice*> output_devices);
	~AnimationThread();

	void run() override;

	void set_animation(DMDAnimation* animation);

	void start_recording();
	void stop_recording();
	void clear_recordings();
	const QVector<QImage>& recordings();

	void button_pressed(DMDKeys::Button button);
	void button_released(DMDKeys::Button button);

private:
	QVector<DMDOutputDevice*> m_output_devices;
	DMDAnimation* m_animation = nullptr;
	QMutex m_animation_mutex;
	bool m_recording = false;
	QVector<QImage> m_recordings;
};

