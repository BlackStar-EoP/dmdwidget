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
#include <QPainter>

DMDAnimationEngine::DMDAnimationEngine(DMDOutputDevice* output_device)
: m_animation_thread(output_device)
{
	create_internal_animations();
	m_animation_thread.start();
	m_animation_thread.set_animation(m_loading_animation);

	m_animation_path = QCoreApplication::applicationDirPath() + "/animations/";

	QDir animation_directory = m_animation_path;
	animation_directory.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
	QStringList animation_dir_list = animation_directory.entryList();

	for (const QString& animation_dir : animation_dir_list)
	{
		ImageAnimation* animation = new ImageAnimation(m_animation_path, animation_dir, 5);
		if (!animation->is_valid())
			delete animation;
		else
			m_animations[animation_dir] = animation;
	}
	m_animation_thread.set_animation(m_waiting_for_fx3_animation);
}

DMDAnimationEngine::~DMDAnimationEngine()
{
	m_animation_thread.terminate();
	qDeleteAll(m_animations);

	delete m_loading_animation;
	delete m_waiting_for_fx3_animation;
	delete m_select_table_animation;

}

void DMDAnimationEngine::show_animation(DMDAnimation* animation)
{
	m_animation_thread.set_animation(animation);
}

const QMap<QString, DMDAnimation*>& DMDAnimationEngine::animations() const
{
	return m_animations;
}

void DMDAnimationEngine::set_DMD_invalid()
{
	m_animation_thread.set_animation(m_select_table_animation);
}

void DMDAnimationEngine::create_internal_animations()
{
	QImage empty_image(DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT, QImage::Format_RGBA8888);
	QPainter empty_painter(&empty_image);
	empty_painter.fillRect(QRect(0, 0, DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT), Qt::black);

	/* this is crap TODO refactor this, either fixed PNG, or function */
	QImage loading_image(DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT, QImage::Format_RGBA8888);
	QPainter loading_painter(&loading_image);
	loading_painter.fillRect(QRect(0, 0, DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT), Qt::black);
	loading_painter.setPen(Qt::white);
	QFont font("Courier New");
	font.setPixelSize(10);
	loading_painter.setFont(font);
	loading_painter.drawText(8, 21, "LOADING ANIMATIONS");

	QImage waiting_for_fx3_image(DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT, QImage::Format_RGBA8888);
	QPainter waiting_for_fx3_painter(&waiting_for_fx3_image);
	waiting_for_fx3_painter.fillRect(QRect(0, 0, DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT), Qt::black);
	waiting_for_fx3_painter.setPen(Qt::white);
	waiting_for_fx3_painter.setFont(font);
	waiting_for_fx3_painter.drawText(20, 21, "WAITING FOR FX3");

	QImage select_table_image(DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT, QImage::Format_RGBA8888);
	QPainter select_table_painter(&select_table_image);
	select_table_painter.fillRect(QRect(0, 0, DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT), Qt::black);
	select_table_painter.setPen(Qt::white);
	select_table_painter.setFont(font);
	select_table_painter.drawText(30, 21, "SELECT TABLE");


	QVector<QImage> loading_frames;
	loading_frames.push_back(loading_image);
	loading_frames.push_back(empty_image);

	QVector<QImage> waiting_frames;
	waiting_frames.push_back(waiting_for_fx3_image);
	waiting_frames.push_back(empty_image);

	QVector<QImage> select_frames;
	select_frames.push_back(select_table_image);
	select_frames.push_back(empty_image);


	m_loading_animation = new ImageAnimation(loading_frames, ImageAnimation::GRAYSCALE, 5);
	m_waiting_for_fx3_animation = new ImageAnimation(waiting_frames, ImageAnimation::GRAYSCALE, 5);
	m_select_table_animation = new ImageAnimation(select_frames, ImageAnimation::GRAYSCALE, 5);
}