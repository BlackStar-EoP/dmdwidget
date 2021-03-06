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

#include <QObject>

#include "dmdkeys.h"

class DMDAnimationEngine;

class DMDEventFilter : public QObject
{
public:
	DMDEventFilter(DMDAnimationEngine* animation_engine);

private:
	bool eventFilter(QObject *watched, QEvent *event) override;
	bool is_shift(uint32_t native_key) const;
	bool is_ctrl(uint32_t native_key) const;
	DMDKeys::Button flipper(uint32_t native_key, uint32_t native_modifier) const;
	DMDKeys::Button magna_save(uint32_t native_key, uint32_t native_modifier) const;
	DMDKeys::Button DMDEventFilter::button_for_key(int keycode) const;
	void fire_button_pressed(DMDKeys::Button button);
	void fire_button_released(DMDKeys::Button button);

private:
	DMDAnimationEngine* m_animation_engine = nullptr;
	bool m_button_state[DMDKeys::NUM_BUTTONS];
};