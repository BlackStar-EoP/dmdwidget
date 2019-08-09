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

#include "dmdframe.h"

#include <memory>

DMDFrame::DMDFrame(uint8_t* grayscale_frame, uint32_t* color_frame)
{
	memcpy(m_grayscale_frame, grayscale_frame, DMDConfig::DMDWIDTH * DMDConfig::DMDHEIGHT);
	memcpy(m_color_frame, color_frame, DMDConfig::DMDWIDTH * DMDConfig::DMDHEIGHT * sizeof(uint32_t));
}

DMDFrame::DMDFrame()
{
	memset(m_grayscale_frame, 0, sizeof(m_grayscale_frame));
	memset(m_color_frame, 0, sizeof(m_color_frame));
}

const uint8_t* const DMDFrame::const_grayscale_frame() const
{
	return m_grayscale_frame;
}

const uint32_t* const DMDFrame::const_color_frame() const
{
	return m_color_frame;
}

uint8_t* const DMDFrame::grayscale_frame()
{
	return m_grayscale_frame;
}

uint32_t* const DMDFrame::color_frame()
{
	return m_color_frame;
}
