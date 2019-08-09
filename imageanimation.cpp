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

#include "imageanimation.h"

#include "dmdconfig.h"
#include "dmdframe.h"

#include <QDir>
#include <QImage>
#include <assert.h>

ImageAnimation::ImageAnimation(const QString& path, const QString& directory)
{
	load_animation(path, directory);
}

bool ImageAnimation::is_valid() const
{
	return m_valid;
}

ImageAnimation::EColorMode ImageAnimation::determine_animation_color_mode(const QString& path, const QStringList& filelist) const
{
	for (int32_t i = 0; i < filelist.count(); ++i)
	{
		QImage frame_img(path + QString("/") + filelist[i]);
		bool empty_frame = true;
		for (int32_t y = 0; y < frame_img.height(); ++y)
		{
			for (int32_t x = 0; x < frame_img.width(); ++x)
			{
				QRgb pixel = frame_img.pixel(x, y);
				if (qRed(pixel) != 0 || qGreen(pixel) != 0 || qBlue(pixel) != 0)
					return determine_color_mode(frame_img);
			}
		}
	}

	return GRAYSCALE;
}

ImageAnimation::EColorMode ImageAnimation::determine_color_mode(const QImage& image) const
{
	uint32_t num_red_frames = 0;
	uint32_t num_green_frames = 0;
	uint32_t num_blue_frames = 0;
	uint32_t num_grayscale_frames = 0;
	uint32_t num_color_frames = 0;

	for (int32_t y = 0; y < image.height(); ++y)
	{
		for (int32_t x = 0; x < image.width(); ++x)
		{
			QRgb pixel = image.pixel(x, y);
			int r = qRed(pixel);
			int g = qGreen(pixel);
			int b = qBlue(pixel);
			
			if (r != 0 && g == 0 && b == 0) // Red frame
				num_red_frames++;
			else if (r == 0 && g != 0 && b == 0) // Green frame
				num_green_frames++;
			else if (r == 0 && g == 0 && b != 0) // Blue frame
				num_blue_frames++;
			else if (r == g == b)
				num_grayscale_frames++;
			else
				num_color_frames++;
		}
	}

	if (is_largest(num_red_frames, num_green_frames, num_blue_frames, num_grayscale_frames, num_color_frames))
		return ImageAnimation::RED_CHANNEL_ONLY;
	
	if (is_largest(num_green_frames, num_red_frames, num_blue_frames, num_grayscale_frames, num_color_frames))
		return ImageAnimation::GREEN_CHANNEL_ONLY;
	
	if (is_largest(num_blue_frames, num_red_frames, num_green_frames, num_grayscale_frames, num_color_frames))
		return ImageAnimation::BLUE_CHANNEL_ONLY;
	
	if (is_largest(num_grayscale_frames, num_red_frames, num_green_frames, num_blue_frames, num_color_frames))
		return ImageAnimation::GRAYSCALE;
	
	if (is_largest(num_color_frames, num_red_frames, num_green_frames, num_blue_frames, num_grayscale_frames))
		return ImageAnimation::FULL_COLOR;

	assert(false);
	return ImageAnimation::GRAYSCALE;
}

bool ImageAnimation::is_largest(uint32_t l1, uint32_t r1, uint32_t r2, uint32_t r3, uint32_t r4) const
{
	return l1 > r1 && l1 > r2 && l1 > r3 && l1 > r4;
}

void ImageAnimation::load_animation(const QString& path, const QString& directory)
{
	QDir dir = path + directory;
	dir.setNameFilters(QStringList("*.png"));
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

	QStringList filelist = dir.entryList();
	filelist.sort();

	if (filelist.count() == 0)
	{
		//qWarning() << "Animation has 0 frames: " << dir.path();
		return;
	}

	for (int32_t i = 0; i < filelist.count(); ++i)
	{
		QString frame_string = QString::number(i).rightJustified(4, '0');
		const QString& filename = filelist[i];
		if (!filename.contains(frame_string))
		{
			//qWarning() << "Could not load animation: " << dir.path();
			return;
		}
	}
	
	// TODO add animation parameter support
	//dir.setNameFilters(QStringList("*.anm"));
	//dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

	EColorMode color_mode = determine_animation_color_mode(dir.absolutePath(), filelist);

	QVector<DMDFrame*> frames;
	for (int32_t i = 0; i < filelist.count(); ++i)
	{
		QImage frame_img(dir.absolutePath() + QString("/") + filelist[i]);
		if (frame_img.width() != DMDConfig::DMDWIDTH)
		{
			qDeleteAll(frames);
			//qWarning() << "Animation frame width problem: " << dir.path();
			return;
		}

		if (frame_img.height() != DMDConfig::DMDHEIGHT)
		{
			qDeleteAll(frames);
			//qWarning() << "Animation frame height problem: " << dir.path();
			return;
		}

		DMDFrame* frame = parse_image(frame_img, color_mode);
		frames.push_back(frame);
	}
}

DMDFrame* ImageAnimation::parse_image(const QImage& image, ImageAnimation::EColorMode color_mode)
{
	uint32_t index = 0;
	assert(image.width() == DMDConfig::DMDWIDTH);
	assert(image.height() == DMDConfig::DMDHEIGHT);

	DMDFrame* frame = new DMDFrame();
	uint8_t* grayscale_frame = frame->grayscale_frame();
	uint32_t* color_frame = frame->color_frame();

	for (int32_t y = 0; y < image.height(); ++y)
	{
		for (int32_t x = 0; x < image.width(); ++x)
		{
			const QRgb& pixel = image.pixel(x, y);
			switch (color_mode)
			{
				case FULL_COLOR:
					grayscale_frame[index] = (qRed(pixel) + qGreen(pixel) + qBlue(pixel)) / 3;
					break;

				case RED_CHANNEL_ONLY:
					grayscale_frame[index] = qRed(pixel);
					break;

				case GREEN_CHANNEL_ONLY:
					grayscale_frame[index] = qGreen(pixel);
					break;

				case BLUE_CHANNEL_ONLY:
					grayscale_frame[index] = qBlue(pixel);
					break;
			}
			color_frame[index] = pixel;
			++index;
		}
	}

	return frame;
}
