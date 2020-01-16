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

#include <dmdoutputdevice.h>
#include <QWidget>
#include <stdint.h>
class QLabel;
class DMDFrame;

class WidgetOutputDevice : public DMDOutputDevice, public QWidget
{
public:
	WidgetOutputDevice(QWidget* parent, uint32_t size);
	~WidgetOutputDevice();

	void set_use_RGB(bool use_RGB);

	bool isDeviceAvailable() override;
	void clearDMD() override;
	void sendFrame(const DMDFrame& frame) override;

	bool supportsColor() const override;
	void setDotColor(const QColor& color);

private:
	QLabel* m_DMD_label = nullptr;

	uint32_t m_DMD_width = 0;
	uint32_t m_DMD_height = 0;

	bool m_use_RGB = false;
	QColor m_dot_color = QColor(255, 102, 0);
};