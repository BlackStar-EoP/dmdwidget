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

#include "dmdanimationengine.h"

#include "dmdanimation.h"
#include "dmdanimationframe.h"
#include "dmddata.h"

#include <QCoreApplication>
#include <QVector>
#include <QDir>
#include <QtGlobal>
#include <QImage>

DMDAnimationEngine::DMDAnimationEngine()
{
	m_animation_path = QCoreApplication::applicationDirPath() + "/animations/";

	QDir animation_directory = m_animation_path;
	animation_directory.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
	QStringList animation_dir_list = animation_directory.entryList();

	for (QString animation_dir : animation_dir_list)
	{
		load_animation(animation_dir);
	}

}

void DMDAnimationEngine::load_animation(const QString& animation_dir)
{
	QDir dir = m_animation_path + animation_dir;
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
	
	QVector<DMDAnimationFrame*> frames;
	for (int32_t i = 0; i < filelist.count(); ++i)
	{
		QImage frame_img(dir.absolutePath() + QString("/") + filelist[i]);
		if (frame_img.width() != DMDData::DMDWIDTH)
		{
			qDeleteAll(frames);
			//qWarning() << "Animation frame width problem: " << dir.path();
			return;
		}
		
		if (frame_img.height() != DMDData::DMDHEIGHT)
		{
			qDeleteAll(frames);
			//qWarning() << "Animation frame height problem: " << dir.path();
			return;
		}

		DMDAnimationFrame* frame = new DMDAnimationFrame(frame_img);
		frames.push_back(frame);
	}
	
	m_animations[animation_dir] = new DMDAnimation(frames);

	// TODO add animation parameter support
	//dir.setNameFilters(QStringList("*.anm"));
	//dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
}
