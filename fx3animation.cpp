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

#include "fx3animation.h"
#include "fx3process.h"

FX3Animation::FX3Animation(FX3Process* fx3_process)
: m_fx3_process(fx3_process)
{

}

DMDFrame* FX3Animation::current_frame()
{
	bool valid_DMD = m_fx3_process->captureDMD(m_RAW_DMD);
	if (!valid_DMD)
	{
		m_DMD_color_found = false;
		// TODO notify animation engine here that it needs to clear or do something
		return nullptr;
	}

	if (isGarbage(m_RAW_DMD))
	{
		memset(m_RAW_DMD, 0, sizeof(m_RAW_DMD));
		memset(m_previous_RAW_DMD, 0, sizeof(m_previous_RAW_DMD));
	}
	else
	{
		if (!m_DMD_color_found)
		{
			m_DMD_color = m_fx3_process->getDMDColor();
			m_DMD_color_found = true;
		}
	}

	if (isEqual(m_RAW_DMD, m_previous_RAW_DMD))
		return nullptr;
	else
		memcpy(m_previous_RAW_DMD, m_RAW_DMD, sizeof(m_RAW_DMD));

	if (isWilliamsDMD(m_RAW_DMD))
	{
		normalizeWilliamsDMD(m_RAW_DMD);
	}
	else
	{
		normalizeZenDMD(m_RAW_DMD);
	}

	uint8_t* const grayscale_frame = m_DMD_frame.grayscale_frame();
	uint32_t* const color_frame = m_DMD_frame.color_frame();

	const uint32_t NUM_PIXELS = DMDConfig::DMDWIDTH * DMDConfig::DMDHEIGHT;
	for (uint32_t i = 0; i < NUM_PIXELS; ++i)
	{
		uint8_t c = m_RAW_DMD[i];
		float col = c / 255.0f;

		int32_t r = col * m_DMD_color.red();
		int32_t g = col * m_DMD_color.green();
		int32_t b = col * m_DMD_color.blue();

		QRgb color = qRgb(r, g, b);
		
		grayscale_frame[i] = c;
		color_frame[i] = color;

	}
	return &m_DMD_frame;
}

bool FX3Animation::isGarbage(const uint8_t* rawDMD) const
{
	const uint32_t pixelCount = DMDConfig::DMDWIDTH * DMDConfig::DMDHEIGHT;

	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		if (rawDMD[i] > 6)
			return true;
	}

	return false;
}

bool FX3Animation::isWilliamsDMD(const uint8_t* rawDMD) const
{
	const uint32_t pixelCount = DMDConfig::DMDWIDTH * DMDConfig::DMDHEIGHT;

#ifdef FAST_WILLIAMS_CHECK
	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		if (rawDMD[i] > 2)
			return true;
	}
	return false;
#else
	uint32_t count3 = 0;
	uint32_t count4 = 0;
	uint32_t count5 = 0;
	uint32_t count6 = 0;

	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		switch (rawDMD[i])
		{
		case 3:
			count3++;
			break;
		case 4:
			count4++;
			break;
		case 5:
			count5++;
			break;
		case 6:
			count6++;
			break;
		}
	}

	return count3 + count4 + count5 + count6 == pixelCount;
#endif
}

void FX3Animation::normalizeZenDMD(uint8_t* rawDMD)
{
	const uint32_t pixelCount = DMDConfig::DMDWIDTH * DMDConfig::DMDHEIGHT;
	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		switch (rawDMD[i])
		{
		case 0:
			rawDMD[i] = 0;
			break;
		case 1:
			rawDMD[i] = 128;
			break;
		case 2:
			rawDMD[i] = 255;
			break;
		}
	}
}

void FX3Animation::normalizeWilliamsDMD(uint8_t* rawDMD)
{
	const uint32_t pixelCount = DMDConfig::DMDWIDTH * DMDConfig::DMDHEIGHT;
	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		switch (rawDMD[i])
		{
		case 3:
			rawDMD[i] = 0;
			break;
		case 4:
			rawDMD[i] = 85;
			break;
		case 5:
			rawDMD[i] = 170;
			break;
		case 6:
			rawDMD[i] = 255;
			break;
		}
	}
}

bool FX3Animation::isEmpty(const uint8_t* rawDMD) const
{
	const uint32_t pixelCount = DMDConfig::DMDWIDTH * DMDConfig::DMDHEIGHT;
	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		if (rawDMD[i] != 0)
			return false;
	}
	return true;
}

bool FX3Animation::isEqual(const uint8_t* DMD1, const uint8_t* DMD2)
{
	const uint32_t pixelCount = DMDConfig::DMDWIDTH * DMDConfig::DMDHEIGHT;
	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		if (DMD1[i] != DMD2[i])
			return false;
	}
	return true;
}
