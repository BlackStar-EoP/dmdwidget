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

#include "fantasieswindow.h"

#include "dmdanimationengine.h"
#include "dmdconfig.h"
#include "dmdframe.h"
#include "imageanimation.h"

#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QFile>
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QSet>

#include <assert.h>

#define RAWPIXEL(x,y) rawDMD[y * FANTASIES_DMD_WIDTH + x]

FantasiesWindow::FantasiesWindow(QWidget* parent, DMDAnimationEngine* animation_engine)
: QWidget(parent, Qt::Window)
, m_animation_engine(animation_engine)
{
	setWindowTitle("Pinball Fantasies DMD analyzer");
	initUI();
	this->setGeometry(0, 0, 1024, 768);

	QRect screenGeometry = QApplication::desktop()->screenGeometry();
	int x = (screenGeometry.width() - width()) / 2;
	int y = (screenGeometry.height() -height()) / 2;
	move(x, y);
	show();
}

FantasiesWindow::~FantasiesWindow()
{
}

void FantasiesWindow::initUI()
{
	QLabel* file_label = new QLabel("File:", this);
	file_label->setGeometry(10, 10, 100, 20);
	m_file_name_label = new QLabel("filename", this);
	m_file_name_label->setGeometry(115, 10, 200, 20);

	QPushButton* inc_img_button = new QPushButton("INC", this);
	inc_img_button->setGeometry(10, 30, 100, 20);
	connect(inc_img_button, SIGNAL(clicked()), this, SLOT(inc_img_button_clicked()));
	QPushButton* dec_img_button = new QPushButton("DEC", this);
	dec_img_button->setGeometry(10, 55, 100, 20);
	connect(dec_img_button, SIGNAL(clicked()), this, SLOT(dec_img_button_clicked()));
	QPushButton* inc_img10_button = new QPushButton("INC10", this);
	inc_img10_button->setGeometry(10, 80, 100, 20);
	connect(inc_img10_button, SIGNAL(clicked()), this, SLOT(inc_img10_button_clicked()));
	QPushButton* dec_img10_button = new QPushButton("DEC10", this);
	dec_img10_button->setGeometry(10, 105, 100, 20);
	connect(dec_img10_button, SIGNAL(clicked()), this, SLOT(dec_img10_button_clicked()));
	QPushButton* inc_img100_button = new QPushButton("INC100", this);
	inc_img100_button->setGeometry(10, 130, 100, 20);
	connect(inc_img100_button, SIGNAL(clicked()), this, SLOT(inc_img100_button_clicked()));
	QPushButton* dec_img100_button = new QPushButton("DEC100", this);
	dec_img100_button->setGeometry(10, 155, 100, 20);
	connect(dec_img100_button, SIGNAL(clicked()), this, SLOT(dec_img100_button_clicked()));

	QPushButton* auto_button = new QPushButton("AUTO", this);
	auto_button->setGeometry(10, 180, 100, 20);
	connect(auto_button, SIGNAL(clicked()), this, SLOT(auto_button_clicked()));

	QPushButton* debug_button = new QPushButton("DEBUG", this);
	debug_button->setGeometry(10, 205, 100, 20);
	connect(debug_button, SIGNAL(clicked()), this, SLOT(debug_button_clicked()));


	m_image_label = new QLabel(this);
	m_image_label->setGeometry(100, 30, FANTASIES_WIDTH, FANTASIES_HEIGHT);

	m_dmd_label = new QLabel(this);
	m_dmd_label->setGeometry(440, 30, FANTASIES_DMD_WIDTH, FANTASIES_DMD_HEIGHT);

	m_dmd_span_label = new QLabel(this);
	m_dmd_span_label->setGeometry(440, 50, DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT);
}

bool FantasiesWindow::is_column_candidate(uint32_t column)
{
	if (column >= FANTASIES_DMD_WIDTH)
		return false;

	for (uint32_t y = 0; y < FANTASIES_DMD_HEIGHT; ++y)
	{
		if (RAWPIXEL(column, y) != 0)
		{
			return false;
		}
	}
	return true;
}

void FantasiesWindow::update_image()
{
	memset(rawDMD, 0, sizeof(rawDMD));

	QString filenamepng = QString("D:/pf/shots/shot") + QString::number(m_current_file_nr) + ".png";
	m_file_name_label->setText(filenamepng);

	QImage img(filenamepng);

	m_image_label->setPixmap(QPixmap::fromImage(img));

	int pixelnr = 0;


	// de-dot DMD
	QImage dmd(FANTASIES_DMD_WIDTH, FANTASIES_DMD_HEIGHT, QImage::Format_RGBA8888);
	int desty = 0;
	for (uint32_t y = 1; y < FANTASIES_DMD_HEIGHT + 1; ++y)
	{
		for (uint32_t x = 0; x < FANTASIES_DMD_WIDTH; ++x)
		{
			assert(x * 2 < img.width());
			assert(y * 2 < img.height());
			uint32_t pixel = img.pixel(x * 2, y * 2);
			switch (pixel)
			{
			case 0xff000000:
				dmd.setPixel(x, desty, qRgba(0, 0, 0, 255));
				break;
			case 0xff515151:
				dmd.setPixel(x, desty, qRgba(0, 0, 0, 255));
				break;
			case 0xff555555:
				dmd.setPixel(x, desty, qRgba(0, 0, 0, 255));
				break;
			case 0xfff3b245:
				dmd.setPixel(x, desty, qRgba(255, 255, 255, 255));
				rawDMD[desty * FANTASIES_DMD_WIDTH + x] = 255;
				break;
			}
		}
		++desty;
	}
	m_dmd_label->setPixmap(QPixmap::fromImage(dmd));
	span_fix(dmd);
}

void FantasiesWindow::span_fix(const QImage& dmd)
{
	return;
	// Span fix
	bool in_span = false;
	uint32_t span_start = 0;
	std::set<std::pair<uint32_t, uint32_t>> spans;
	for (uint32_t x = 0; x < FANTASIES_DMD_WIDTH; ++x)
	{
		if (is_column_candidate(x))
		{
			if (!in_span)
			{
				in_span = true;
				span_start = x;
			}
		}
		else
		{
			if (in_span)
			{
				in_span = false;
				if (x - span_start > 3)
					spans.insert(std::pair<uint32_t, uint32_t>(span_start, x));
			}
		}
	}
	if (in_span)
	{
		in_span = false;
		if ((FANTASIES_DMD_WIDTH - 1) - span_start > 3)
			spans.insert(std::pair<uint32_t, uint32_t>(span_start, FANTASIES_DMD_WIDTH - 1));
	}

	if (spans.size() > 3)
		printf("");

	std::set<uint32_t> remove_columns;

	bool has_big_span = false;

	for (auto it = spans.begin(); it != spans.end(); ++it)
	{
		uint32_t spanwidth = it->second - it->first;
		if (spanwidth >= 32)
		{
			has_big_span = true;
			for (uint32_t i = 0; i < 32; ++i)
			{
				remove_columns.insert(it->first + i);
			}
			break;
		}
	}

	if (has_big_span)
	{

	}
	else if (spans.size() >= 2) // attempt left and right removal
	{
		auto lspan = spans.begin();
		auto rspan = spans.rbegin();
		uint32_t lwidth = lspan->second - lspan->first;
		uint32_t rwidth = rspan->second - rspan->first;

		if (lwidth >= 16 && rwidth >= 16)
		{
			for (uint32_t i = 0; i < 16; ++i)
			{
				remove_columns.insert(lspan->first + i);
				remove_columns.insert(rspan->first + i);
			}
		}
	}

	if (remove_columns.size() == 0)
		return;

	// finally set span:
	int dest_y = 8;
	QImage spanDMD(DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT, QImage::Format_RGBA8888);
	spanDMD.fill(Qt::black);

	int current_column = 0;
	for (uint32_t x = 0; x < FANTASIES_DMD_WIDTH; ++x)
	{
		if (remove_columns.find(x) != remove_columns.end())
			continue;

		for (uint32_t y = 0; y < 16; ++y)
		{
			spanDMD.setPixel(current_column, y + 8, dmd.pixel(x, y));
		}
		++current_column;
		if (current_column == DMDConfig::DMDWIDTH)
			break;
	}

	m_dmd_span_label->setPixmap(QPixmap::fromImage(spanDMD));
	QVector<QImage> frames;
	frames.push_back(spanDMD);
	ImageAnimation* anim = new ImageAnimation(frames, 1);
	m_animation_engine->show_animation(anim);
}



void FantasiesWindow::inc_img_button_clicked()
{
	m_current_file_nr++;
	update_image();
}

void FantasiesWindow::dec_img_button_clicked()
{
	if (m_current_file_nr > 1)
		m_current_file_nr--;
	update_image();
}


void FantasiesWindow::inc_img10_button_clicked()
{
	m_current_file_nr += 10;
	update_image();
}

void FantasiesWindow::dec_img10_button_clicked()
{
	if (m_current_file_nr > 10)
		m_current_file_nr -= 10;
	update_image();
}


void FantasiesWindow::inc_img100_button_clicked()
{
	m_current_file_nr += 100;
	update_image();
}

void FantasiesWindow::dec_img100_button_clicked()
{
	if (m_current_file_nr > 100)
		m_current_file_nr -= 100;
	update_image();
}

void FantasiesWindow::auto_button_clicked()
{
	for (uint32_t i = 0; i < 20000; ++i)
	{
		QString filename = QString("D:/pf/shots/shot") + QString::number(i) + ".bsr";
		QString filenamepng = QString("D:/pf/shots/shot") + QString::number(i) + ".png";

		QImage img(FANTASIES_WIDTH, FANTASIES_HEIGHT, QImage::Format_RGBA8888);
		QFile file(filename);
		if (file.open(QIODevice::ReadOnly))
		{
			QByteArray data = file.readAll();
			const char* pixeldata = data.constData();
			file.close();
			int pos = 0;
			int pixelnr = 0;

			// Grab DMD
			const int startpos = 2560;
			uint32_t* dmddata = (uint32_t*)(pixeldata + startpos);

			for (uint32_t y = 0; y < FANTASIES_HEIGHT; ++y)
			{
				for (uint32_t x = 0; x < FANTASIES_WIDTH; ++x)
				{
					int b = pixeldata[pos++];
					int g = pixeldata[pos++];
					int r = pixeldata[pos++];
					int a = pixeldata[pos++];

					img.setPixel(x, y, qRgba(r, g, b, a));
				}
			}
			img.save(filenamepng);
		}
		else break;
	}
}

void FantasiesWindow::debug_button_clicked()
{
}
