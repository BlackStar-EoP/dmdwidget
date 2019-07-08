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

#include "dmddata.h"

bool DMDData::isGarbage(const uint8_t* rawDMD) const
{
	const uint32_t pixelCount = DMDWIDTH * DMDHEIGHT;

	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		if (rawDMD[i] > 6)
			return true;
	}

	return false;
}

bool DMDData::isWilliamsDMD(const uint8_t* rawDMD) const
{
	const uint32_t pixelCount = DMDWIDTH * DMDHEIGHT;

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

void DMDData::correctWilliamsDMD(uint8_t* rawDMD)
{
	const uint32_t pixelCount = DMDWIDTH * DMDHEIGHT;
	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		rawDMD[i] -= 3;
	}
}

void DMDData::normalizeDMD(uint8_t* rawDMD)
{
	const uint32_t pixelCount = DMDWIDTH * DMDHEIGHT;
	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		switch (rawDMD[i])
		{
		case 1:
			rawDMD[i] = 128;
			break;
		case 2:
			rawDMD[i] = 255;
			break;
		}
	}
}

void DMDData::normalizeWilliamsDMD(uint8_t* rawDMD)
{
	const uint32_t pixelCount = DMDWIDTH * DMDHEIGHT;
	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		switch (rawDMD[i])
		{
		case 1:
			rawDMD[i] = 85;
			break;
		case 2:
			rawDMD[i] = 170;
			break;
		case 3:
			rawDMD[i] = 255;
			break;
		}
	}
}

bool DMDData::isEmpty(const uint8_t* rawDMD) const
{
	const uint32_t pixelCount = DMDWIDTH * DMDHEIGHT;
	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		if (rawDMD[i] != 0)
			return false;
	}
	return true;
}

bool DMDData::isEqual(const uint8_t* DMD1, const uint8_t* DMD2)
{
	const uint32_t pixelCount = DMDWIDTH * DMDHEIGHT;
	for (uint32_t i = 0; i < pixelCount; ++i)
	{
		if (DMD1[i] != DMD2[i])
			return false;
	}
	return true;
}