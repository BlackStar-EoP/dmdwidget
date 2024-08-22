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

#include "dmdanimation.h"

#include "dmdframe.h"

class QImage;
class DMDFrame;

class Shader
{
public:
	Shader() = default;
	~Shader() = default;
	virtual uint8_t render(int32_t x, int32_t y, float time) = 0;
};

class PlasmaShader : public Shader
{
public:
	/*
void mainImage(out vec4 col, in vec2 pc)
{
	float time = iTime;
	vec2 a = vec2(iResolution.x /iResolution.y, 1);
	vec2 c = pc.xy / iResolution.xy * a * 4. + time * .3;

	float k = .1 + cos(c.y + sin(.148 - time)) + 2.4 * time;
	float w = .9 + sin(c.x + cos(.628 + time)) - 0.7 * time;
	float d = length(c);
	float s = 7. * cos(d+w) * sin(k+w);

	col = vec4(.5 + .5 * cos(s + vec3(.2, .5, .9)), 1);

	#ifdef SPECULAR
	col *= vec4(1, .7, .4, 1)
		*  pow(max(normalize(vec3(length(dFdx(col)), length(dFdy(col)), .5/iResolution.y)).z, 0.), 2.)
		+ .75;
	#endif
}	*/

	uint8_t render(int32_t x, int32_t y, float time) override
	{
		/*
void mainImage(out vec4 col, in vec2 pc)
{
	float time = iTime;
	vec2 a = vec2(iResolution.x /iResolution.y, 1);
	vec2 c = pc.xy / iResolution.xy * a * 4. + time * .3;

	float k = .1 + cos(c.y + sin(.148 - time)) + 2.4 * time;
	float w = .9 + sin(c.x + cos(.628 + time)) - 0.7 * time;
	float d = length(c);
	float s = 7. * cos(d+w) * sin(k+w);

	col = vec4(.5 + .5 * cos(s + vec3(.2, .5, .9)), 1);
}	*/
		float val = (sin(sin(x + time) * sin(y+time) * time / 10) + 1.0f) / 2.0f;
		uint8_t rval = val * 255;
		rval >>= 4;
		return rval << 4;
	}
};

class ShaderAnimation : public DMDAnimation
{
public:

	ShaderAnimation();
	~ShaderAnimation();

	DMDFrame* current_frame() override;
	void button_pressed(DMDKeys::Button button) override;
	void button_released(DMDKeys::Button button) override;

	DMDFrame m_shader_frame;
	float m_time = 0.0f;
	Shader* m_shader = nullptr;
};