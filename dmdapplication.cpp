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

#include "dmdapplication.h"

#include "fx3process.h"
#include "dmdanimationengine.h"
#include "dmdeventfilter.h"

#include <assert.h>

DMDApplication::DMDApplication(int argc, char *argv[], FX3Process* fx3_process)
: QApplication(argc, argv)
, m_fx3_process(fx3_process)
, m_fx3_animation(fx3_process)
{
	connect(&m_find_fx3_timer, SIGNAL(timeout()), this, SLOT(find_FX3_executable_timeout()));
	connect(&m_find_DMD_timer, SIGNAL(timeout()), this, SLOT(find_DMD_timeout()));
}

void DMDApplication::set_animation_engine(DMDAnimationEngine* animation_engine)
{
	m_animation_engine = animation_engine;
	installEventFilter(new DMDEventFilter(m_animation_engine));
}

void DMDApplication::start_polling()
{
	assert(m_animation_engine != nullptr);
	assert(m_fx3_process != nullptr);

	m_find_fx3_timer.start(500);
}

void DMDApplication::find_FX3_executable_timeout()
{
	if (m_fx3_process->findFX3())
	{
		m_find_fx3_timer.stop();
		m_find_DMD_timer.start(100);
	}
}

void DMDApplication::find_DMD_timeout()
{
	if (m_fx3_process->findDMD())
	{
		if (m_fx3_process->is_valid_DMD())
		{
			if (!m_valid_DMD)
			{
				m_animation_engine->show_animation(&m_fx3_animation);
				m_valid_DMD = true;
			}
		}
		else
		{
			m_valid_DMD = false;
			m_animation_engine->set_DMD_invalid();
		}
	}
}