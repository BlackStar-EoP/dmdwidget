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

#include "animationwindow.h"

#include "dmdanimationengine.h"

#include <QListWidget>
#include <QPushButton>

AnimationWindow::AnimationWindow(QWidget* parent, DMDAnimationEngine* animation_engine)
: QWidget(parent, Qt::Window)
, m_animation_engine(animation_engine)
{
	setWindowTitle("Animations");
	initUI();
	showNormal();
}

void AnimationWindow::show_animation_button_clicked()
{
	QListWidgetItem* current_item = m_animation_list->currentItem();
	if (current_item != nullptr)
	{
		const QMap<QString, DMDAnimation*>& animations = m_animation_engine->animations();
		DMDAnimation* animation = animations[current_item->text()];
		m_animation_engine->show_animation(animation);
	}
}

void AnimationWindow::initUI()
{
	m_animation_list = new QListWidget(this);
	m_animation_list->setGeometry(10, 30, 200, 400);
	const QMap<QString, DMDAnimation*>& animations = m_animation_engine->animations();
	for (const QString& animation : animations.keys())
	{
		m_animation_list->addItem(new QListWidgetItem(animation));
	}

	QPushButton* show_animation_button = new QPushButton("Show animation", this);
	show_animation_button->setGeometry(10, 440, 200, 20);
	connect(show_animation_button, SIGNAL(clicked()), this, SLOT(show_animation_button_clicked()));
}