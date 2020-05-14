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

#include "imageanimation.h"

#include "dmdconfig.h"
#include "dmdframe.h"

#include <QDir>
#include <QImage>
#include <assert.h>

ImageAnimation::ImageAnimation(const QString& path, const QString& directory, uint8_t animation_speed)
: m_animation_speed(animation_speed)
{
	load_animation(path, directory);
}

ImageAnimation::ImageAnimation(const QVector<QImage>& images, uint8_t animation_speed)
: m_animation_speed(animation_speed)
{
	for (const QImage& img : images)
	{
		m_frames.push_back(parse_image(img));
	}

	m_valid = true;
}

ImageAnimation::~ImageAnimation()
{
	qDeleteAll(m_frames);
}

DMDFrame* ImageAnimation::current_frame()
{
	assert(m_frames.size() > 0);
	// TODO implement animationmode repeat etc
	if (m_current_frame_number >= m_frames.size())
		m_current_frame_number = 0;

	DMDFrame* frame = m_frames[m_current_frame_number];
	if (m_animation_speed > 1)
	{
		m_animation_speed_tick++;
		if (m_animation_speed_tick >= m_animation_speed)
		{
			m_animation_speed_tick = 0;
			m_current_frame_number++;
		}
	}
	else
	{
		m_current_frame_number++;
	}
	return frame;
}

bool ImageAnimation::is_valid() const
{
	return m_valid;
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

	for (int32_t i = 0; i < filelist.count(); ++i)
	{
		QImage frame_img(dir.absolutePath() + QString("/") + filelist[i]);
		if (frame_img.width() != DMDConfig::DMDWIDTH)
		{
			qDeleteAll(m_frames);
			//qWarning() << "Animation frame width problem: " << dir.path();
			return;
		}

		if (frame_img.height() != DMDConfig::DMDHEIGHT)
		{
			qDeleteAll(m_frames);
			//qWarning() << "Animation frame height problem: " << dir.path();
			return;
		}

		DMDFrame* frame = parse_image(frame_img);
		m_frames.push_back(frame);
	}
	m_valid = true;
}

DMDFrame* ImageAnimation::parse_image(const QImage& image)
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
			
			uint8_t avg = (qRed(pixel) + qGreen(pixel) + qBlue(pixel)) / 3;
			uint8_t col = avg;
			//if (avg > 212)
			//	col = 255;
			//else if (avg > 127)
			//	col = 170;
			//else if (avg > 42)
			//	col = 85;

			grayscale_frame[index] = col;
			color_frame[index] = pixel;
			++index;
		}
	}

	return frame;
}
