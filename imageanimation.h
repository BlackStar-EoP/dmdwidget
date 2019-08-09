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

#include "dmdanimation.h"

class QImage;
class DMDFrame;
class QStringList;

class ImageAnimation : public DMDAnimation
{
public:
	enum EColorMode
	{
		FULL_COLOR,
		RED_CHANNEL_ONLY,
		GREEN_CHANNEL_ONLY,
		BLUE_CHANNEL_ONLY,
		GRAYSCALE,
	};

public:
	ImageAnimation(const QString& path, const QString& directory);
	ImageAnimation(const QVector<QImage>& images, EColorMode color_mode);
	~ImageAnimation();

	DMDFrame* current_frame() override;

	bool is_valid() const;

private:
	EColorMode determine_animation_color_mode(const QString& path, const QStringList& filelist) const;
	EColorMode determine_color_mode(const QImage& image) const;
	bool is_largest(uint32_t l1, uint32_t r1, uint32_t r2, uint32_t r3, uint32_t r4) const;
	void load_animation(const QString& path, const QString& directory);
	DMDFrame* parse_image(const QImage& image, EColorMode color_mode);

	bool m_valid = false;
	QVector<DMDFrame*> m_frames;
	int32_t m_current_frame_number = 0;
};