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

#include "dmdeventfilter.h"

#include <QEvent>
#include <QKeyEvent>
#include <QSound>

#include <assert.h>

bool DMDEventFilter::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		quint32 native_key = keyEvent->nativeVirtualKey();
		uint32_t native_modifiers = keyEvent->nativeModifiers();

		DMDKeys::Button button = DMDKeys::UNKNOWN;
		if (is_shift(native_key))
			button = flipper(native_key, native_modifiers);
		else if (is_ctrl(native_key))
			button = magna_save(native_key, native_modifiers);
		else
			button = button_for_key(keyEvent->key());

		if (event->type() == QEvent::KeyPress)
		{
			fire_button_pressed(button);
		//	QSound::play(":/sfx/wms_save.wav");
		}
		else if (event->type() == QEvent::KeyRelease)
			fire_button_released(button);
	}
	return false;
}

bool DMDEventFilter::is_shift(uint32_t native_key) const
{
	return native_key == DMDKeys::SHIFT;
}

bool DMDEventFilter::is_ctrl(uint32_t native_key) const
{
	return native_key == DMDKeys::CTRL;
}

DMDKeys::Button DMDEventFilter::flipper(uint32_t native_key, uint32_t native_modifier) const
{
	assert(native_key == DMDKeys::SHIFT);
	if (native_modifier == DMDKeys::LSHIFT_MODIFIER)
		return DMDKeys::LEFT_FLIPPER;
	
	if (native_modifier == DMDKeys::RSHIFT_MODIFIER)
		return DMDKeys::RIGHT_FLIPPER;

	return DMDKeys::UNKNOWN;
}

DMDKeys::Button DMDEventFilter::magna_save(uint32_t native_key, uint32_t native_modifier) const
{
	assert(native_key == DMDKeys::CTRL);
	if (native_modifier == DMDKeys::LCTRL_MODIFIER)
		return DMDKeys::LEFT_FLIPPER;

	if (native_modifier == DMDKeys::RCTRL_MODIFIER)
		return DMDKeys::RIGHT_FLIPPER;

	return DMDKeys::UNKNOWN;
}

DMDKeys::Button DMDEventFilter::button_for_key(int keycode) const
{
	DMDKeys::Button button = DMDKeys::UNKNOWN;
	switch (keycode)
	{
		case Qt::Key_Enter: button = DMDKeys::LAUNCH_BALL; break;
		case Qt::Key_7:     button = DMDKeys::SERVICE_MENU_CANCEL; break;
		case Qt::Key_8:     button = DMDKeys::SERVICE_MENU_DOWN; break;
		case Qt::Key_9:     button = DMDKeys::SERVICE_MENU_UP; break;
		case Qt::Key_0:     button = DMDKeys::SERVICE_MENU_ENTER; break;
		case Qt::Key_5:     button = DMDKeys::COIN; break;
		case Qt::Key_End:   button = DMDKeys::COINDOOR; break;
		case Qt::Key_1:     button = DMDKeys::START; break;
		case Qt::Key_2:     button = DMDKeys::BUYIN; break;
		case Qt::Key_Q:     button = DMDKeys::EXIT; break;
	}

	return button;
}

void DMDEventFilter::fire_button_pressed(DMDKeys::Button button)
{

}

void DMDEventFilter::fire_button_released(DMDKeys::Button button)
{

}
