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

namespace DMDKeys
{
#ifdef WIN32
	const uint32_t SHIFT = 16;
	const uint32_t CTRL = 17;
	const uint32_t LSHIFT_MODIFIER = 1;
	const uint32_t RSHIFT_MODIFIER = 16;
	const uint32_t LCTRL_MODIFIER = 2;
	const uint32_t RCTRL_MODIFIER = 17;
#else
#error Needs implementation of native keys
#endif

	enum Button : uint32_t
	{
		LEFT_FLIPPER, // Left shift
		RIGHT_FLIPPER, // Right shift
		LEFT_MAGNA_SAVE, // Left control
		RIGHT_MAGNA_SAVE, // Right control
		LAUNCH_BALL, // Enter
		SERVICE_MENU_CANCEL, // 7
		SERVICE_MENU_DOWN,  // 8
		SERVICE_MENU_UP, // 9
		SERVICE_MENU_ENTER, // 0
		COIN, // 5
		COINDOOR, // END
		START, // 1
		BUYIN, // 2
		EXIT, // Q
		UNKNOWN
	};
}