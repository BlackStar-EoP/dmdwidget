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

ImageAnimation::ImageAnimation(const QString& path, const QString& directory)
{
	load_animation(path, directory);
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

		//		DMDAnimationFrame* frame = new DMDAnimationFrame(frame_img);
		//frames.push_back(frame);
	}

	//m_animations[animation_dir] = new DMDAnimation(frames);

	// TODO add animation parameter support
	//dir.setNameFilters(QStringList("*.anm"));
	//dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
}

/*
FROM DMDAnimationFrame

#include "dmdconfig.h"

#include <stdint.h>

class QImage;


#include "dmdanimationframe.h"

#include <QImage>
#include <assert.h>

DMDAnimationFrame::DMDAnimationFrame(const QImage& image)
{
parse_image(image);

}

const uint8_t* const DMDAnimationFrame::grayscale_frame() const
{
return m_grayscale_frame;
}

const uint32_t* const DMDAnimationFrame::color_frame() const
{
return m_color_frame;
}

void DMDAnimationFrame::parse_image(const QImage& image)
{
uint32_t index = 0;
assert(image.width() == DMDConfig::DMDWIDTH);
assert(image.height() == DMDConfig::DMDHEIGHT);

for (int32_t y = 0; y < image.height(); ++y)
{
for (int32_t x = 0; x < image.width(); ++x)
{
const QRgb& pixel = image.pixel(x, y);
switch (m_grayscale_mode)
{
case AVERAGE:
m_grayscale_frame[index] = (qRed(pixel) + qGreen(pixel) + qBlue(pixel)) / 3;
break;

case RED_CHANNEL_ONLY:
m_grayscale_frame[index] = qRed(pixel);
break;

case GREEN_CHANNEL_ONLY:
m_grayscale_frame[index] = qGreen(pixel);
break;

case BLUE_CHANNEL_ONLY:
m_grayscale_frame[index] = qBlue(pixel);
break;
}
m_color_frame[index] = pixel;
++index;
}
}
}

*/