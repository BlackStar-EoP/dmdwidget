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

AnimationThread::AnimationThread(DMDOutputDevice* output_device)
: m_output_device(output_device)
{

}

void AnimationThread::run()
{
	const unsigned long ANIMATION_THREAD_SLEEP_MS = 16;
	bool run = true;

	while (run)
	{
		DMDFrame* frame;
		{
			QMutexLocker lock(&m_animation_mutex);
			frame = m_animation->current_frame();
		}
		m_output_device->sendFrame(*frame);
		msleep(ANIMATION_THREAD_SLEEP_MS);
	}
}

void AnimationThread::set_animation(DMDAnimation* animation)
{
	QMutexLocker lock(&m_animation_mutex);
	m_animation = animation;
}