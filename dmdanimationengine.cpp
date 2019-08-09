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

#include "imageanimation.h"
#include "dmdconfig.h"

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

	for (const QString& animation_dir : animation_dir_list)
	{
		ImageAnimation* animation = new ImageAnimation(m_animation_path, animation_dir);
		if (!animation->is_valid())
			delete animation;
		else
			m_animations[animation_dir] = animation;
	}
}

DMDAnimationEngine::~DMDAnimationEngine()
{
	qDeleteAll(m_animations);
}
