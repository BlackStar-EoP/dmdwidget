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

#include "animationthread.h"

#include "dmdanimation.h"
#include "dmdoutputdevice.h"
#include "dmdframe.h"

#include <QPainter>

AnimationThread::AnimationThread(QVector<DMDOutputDevice*> output_devices)
: m_output_devices(output_devices)
{
}

AnimationThread::~AnimationThread()
{
	qDeleteAll(m_output_devices);
}

void AnimationThread::run()
{
	const unsigned long ANIMATION_THREAD_SLEEP_US = 16667;
	bool run = true;

	while (run)
	{
		DMDFrame* frame;
		{
			QMutexLocker lock(&m_animation_mutex);
			frame = m_animation->current_frame();
		}
		
		if (frame != nullptr)
		{
			for (DMDOutputDevice* device : m_output_devices)
			{
				if (device->isDeviceAvailable())
					device->sendFrame(*frame);
			}

			if (m_recording)
			{
				uint8_t* grayscale_frame = frame->grayscale_frame();
				QImage recframe(DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT, QImage::Format_RGBA8888);
				QPainter rec(&recframe);
				rec.fillRect(QRect(0, 0, DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT), Qt::black);
				uint32_t bytepos = 0;
				for (int y = 0; y < DMDConfig::DMDHEIGHT; ++y)
				{
					for (int x = 0; x < DMDConfig::DMDWIDTH; ++x)
					{
						uint8_t c = grayscale_frame[bytepos];
						rec.setPen(QColor(c,c,c));
						rec.drawPoint(x, y);
						bytepos++;
					}
				}

				m_recordings.push_back(recframe);

			}
		}

		usleep(ANIMATION_THREAD_SLEEP_US);
	}
}

void AnimationThread::set_animation(DMDAnimation* animation)
{
	QMutexLocker lock(&m_animation_mutex);
	m_animation = animation;
}

void AnimationThread::start_recording()
{
	m_recording = true;
}

void AnimationThread::stop_recording()
{
	m_recording = false;
}

void AnimationThread::clear_recordings()
{
	m_recordings.clear();
}

const QVector<QImage>& AnimationThread::recordings()
{
	return m_recordings;
}
