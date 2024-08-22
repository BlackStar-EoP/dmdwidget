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

#include "shaderanimation.h"

#include "dmdconfig.h"
#include "dmdframe.h"

#include <QDir>
#include <QImage>
#include <assert.h>

ShaderAnimation::ShaderAnimation()
{
	m_shader = new PlasmaShader();
}

ShaderAnimation::~ShaderAnimation()
{
}

DMDFrame* ShaderAnimation::current_frame()
{
	m_time += 0.016f;

	for (uint32_t y = 0; y < DMDConfig::DMDHEIGHT; ++y)
	{
		for (uint32_t x = 0; x < DMDConfig::DMDWIDTH; ++x)
		{
			m_shader_frame.set_pixel(x, y, m_shader->render(x, y, m_time));
		}
	}

	return &m_shader_frame;
}

void ShaderAnimation::button_pressed(DMDKeys::Button button)
{
		Q_UNUSED(button);
}

void ShaderAnimation::button_released(DMDKeys::Button button)
{
	Q_UNUSED(button);
}

