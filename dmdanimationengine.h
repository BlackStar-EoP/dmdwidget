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

#include "animationthread.h"

#include "dmdkeys.h"

#include <QString>
#include <QMap>

class DMDAnimation;
class DMDFrame;
class DMDOutputDevice;
class ImageAnimation;

class DMDAnimationEngine
{
public:
	DMDAnimationEngine(QVector<DMDOutputDevice*> output_devices);
	~DMDAnimationEngine();
	void show_animation(DMDAnimation* animation);
	void show_frame(const DMDFrame& frame);
	const QMap<QString, DMDAnimation*>& animations() const;
	void set_DMD_invalid();
	void start_recording();
	void stop_recording();
	void clear_recordings();
	const QVector<QImage>& recordings();

	void button_pressed(DMDKeys::Button button);
	void button_released(DMDKeys::Button button);

private:
	void create_internal_animations();

	QString m_animation_path;
	QMap<QString, DMDAnimation*> m_animations;
	AnimationThread m_animation_thread;
	ImageAnimation* m_loading_animation = nullptr;
	ImageAnimation* m_waiting_for_fx3_animation = nullptr;
	ImageAnimation* m_select_table_animation = nullptr;
};