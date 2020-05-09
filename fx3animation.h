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

#include <QColor>

#include "dmdanimation.h"

#include "dmdframe.h"

class FX3Process;

class FX3Animation : public DMDAnimation
{
public:
	FX3Animation(FX3Process* fx3_process);
	DMDFrame* current_frame() override;

private:
	bool isGarbage(const uint8_t* rawDMD) const;
	bool isWilliamsDMD(const uint8_t* rawDMD) const;
	void normalizeZenDMD(uint8_t* rawDMD);
	void normalizeWilliamsDMD(uint8_t* rawDMD);
	bool isEmpty(const uint8_t* rawDMD) const;
	bool isEqual(const uint8_t* DMD1, const uint8_t* DMD2);

private:
	FX3Process* m_fx3_process = nullptr;
	DMDFrame m_DMD_frame;
	uint8_t m_RAW_DMD[DMDConfig::DMDWIDTH * DMDConfig::DMDHEIGHT];
	uint8_t m_previous_RAW_DMD[DMDConfig::DMDWIDTH * DMDConfig::DMDHEIGHT];
	
	bool m_DMD_color_found = false;
	QColor m_DMD_color;
};