#pragma once

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

#include <stdint.h>

/* TODO rename this to FX3DMDFrame */
class DMDData
{
public:
	static const uint32_t DMDWIDTH = 128;
	static const uint32_t DMDHEIGHT = 32;

public:
	const uint8_t* const frameData() const;

private:
	bool isGarbage(const uint8_t* rawDMD) const;
	bool isWilliamsDMD(const uint8_t* rawDMD) const;
	bool isEmpty(const uint8_t* rawDMD) const;
	bool isEqual(const uint8_t* DMD1, const uint8_t* DMD2);
	void correctWilliamsDMD(uint8_t* rawDMD);
	void normalizeZenDMD(uint8_t* rawDMD);
	void normalizeWilliamsDMD(uint8_t* rawDMD);

private:
	uint8_t m_current_frame[DMDWIDTH * DMDHEIGHT];
	uint8_t m_previous_frame[DMDWIDTH * DMDHEIGHT];
};