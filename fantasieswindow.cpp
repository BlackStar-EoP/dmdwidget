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

class Span
{
public:

	Span(uint32_t start_column, uint32_t end_column)
	{
		m_start_column = start_column;
		m_end_column = end_column;
	}

	uint32_t width() const
	{
		return m_end_column - m_start_column + 1;
	}

	bool is_big_span() const
	{
		return width() >= FantasiesWindow::REMOVE_COLUMN_COUNT;
	}

	uint32_t start_column() const
	{
		return m_start_column;
	}

	uint32_t end_column() const
	{
		return m_end_column;
	}

	bool is_left_span() const
	{
		return m_start_column == 0;
	}

	bool is_right_span() const
	{
		return m_end_column == FantasiesWindow::FANTASIES_DMD_WIDTH - 1;
	}

	uint32_t m_start_column = 0;
	uint32_t m_end_column = 0;
};

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

	QPushButton* debug_button = new QPushButton("TETRIS", this);
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
#undef USE_PNGS
#ifdef USE_PNGS
	QString filenamepng = QString("D:/pf/shots/shot") + QString::number(m_current_file_nr) + ".png";
#else
	QString filenamepng = QString("D:/pf/dmd/shot") + QString::number(m_current_file_nr) + ".dmd";
#endif
	m_file_name_label->setText(filenamepng);

#ifdef USE_PNGS
	QImage img(filenamepng);
#else
	QImage img(FANTASIES_WIDTH, FANTASIES_HEIGHT, QImage::Format_RGBA8888);
	QFile file(filenamepng);
	if (file.open(QIODevice::ReadOnly))
	{
		QByteArray data = file.readAll();
		const char* pixeldata = data.constData();
		memcpy(rawDMD, pixeldata, sizeof(rawDMD));
		file.close();

		//for (uint32_t y = 0; y < FANTASIES_DMD_HEIGHT; ++y)
		//{
		//	for (uint32_t x = 0; x < FANTASIES_DMD_WIDTH; ++x)
		//	{
		//		uint8_t pixel = pixeldata[FANTASIES_DMD_WIDTH * y + x];
		//		if (pixel == 255)
		//			img.setPixel(x, y, qRgb(255, 255, 255));
		//		else
		//			img.setPixel(x, y, qRgb(0, 0, 0));
		//	}
		//}

		uint32_t x = 0;
		uint32_t y = 0;
		for (int32_t byte = 0; byte < data.size(); ++byte)
		{
			uint8_t value = pixeldata[byte];
			//for (int32_t bitindex = 7; bitindex >= 0; --bitindex)
			for (uint32_t bitindex = 0; bitindex < 8; ++bitindex)
			{
				uint8_t bit = value & (1 << bitindex);
				if (bit)
					img.setPixel(x, y, qRgb(255, 255, 255));
				else
					img.setPixel(x, y, qRgb(0, 0, 0));
				++x;
			}
			if (x >= 160)
			{
				x = 0;
				++y;
			}
		}

	}
#endif
	
	m_image_label->setPixmap(QPixmap::fromImage(img));
	QImage dmd = dedot_dmd(img);
	m_dmd_label->setPixmap(QPixmap::fromImage(dmd));
		QImage fixed = span_fix();
	m_dmd_span_label->setPixmap(QPixmap::fromImage(fixed));
	QVector<QImage> frames;
	frames.push_back(fixed);
	ImageAnimation* anim = new ImageAnimation(frames, 1);
	m_animation_engine->show_animation(anim);
}

QImage FantasiesWindow::span_fix()
{
	// Span fix
	bool in_span = false;
	uint32_t span_start = 0;
	std::vector<Span> spans;
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
					spans.push_back(Span(span_start, x));
			}
		}
	}
	if (in_span)
	{
		in_span = false;
		if ((FANTASIES_DMD_WIDTH - 1) - span_start > 3)
			spans.push_back(Span(span_start, FANTASIES_DMD_WIDTH - 1));
	}

	std::set<uint32_t> remove_columns;

	uint32_t num_big_spans = 0;

	for (auto it = spans.begin(); it != spans.end(); ++it)
	{
		uint32_t spanwidth = it->width();
		if (spanwidth >= REMOVE_COLUMN_COUNT)
		{
			++num_big_spans;
		}
	}

	if (spans.size() == 1)
	{
		auto first = spans.begin();
		if (first->width() == FANTASIES_DMD_WIDTH)
		{
			// TODO, DMD cleared, reset any animation flags
		}
	}

	if (num_big_spans > 0)
	{
		if (num_big_spans == 1)
		{
			for (auto span = spans.begin(); span != spans.end(); ++span)
			{
				if (span->is_big_span())
				{
					if (!span->is_left_span())
					{
						for (uint32_t i = 0; i < REMOVE_COLUMN_COUNT; ++i)
						{
							remove_columns.insert(span->start_column() + i);
						}
					}
					else
					{
						auto lspan = spans.begin();
						auto rspan = spans.rbegin();
						if (lspan->width() > 16 && rspan->width() > 16)
						{
							for (uint32_t i = 0; i < 16; ++i)
							{
								remove_columns.insert(lspan->start_column() + i);
								remove_columns.insert(rspan->start_column() + i);
							}
						}
					}
				}
			}
		}
		else
		{
			auto lspan = spans.begin();
			auto rspan = spans.rbegin();
			if (lspan->width() > 16 && rspan->width() > 16)
			{
				for (uint32_t i = 0; i < 16; ++i)
				{
					remove_columns.insert(lspan->start_column() + i);
					remove_columns.insert(rspan->start_column() + i);
				}
			}
		}
	}
	else if (spans.size() >= 2 || num_big_spans > 1) // attempt left and right removal
	{
		auto lspan = spans.begin();
		auto rspan = spans.rbegin();

		if (lspan->width() >= 16 && rspan->width() >= 16)
		{
			for (uint32_t i = 0; i < 16; ++i)
			{
				remove_columns.insert(lspan->start_column() + i);
				remove_columns.insert(rspan->start_column() + i);
			}
		}
		else
		{
			for (uint32_t i = 0; i < lspan->width(); ++i)
			{
				remove_columns.insert(lspan->start_column() + i);
			}
			for (uint32_t i = 0; i < rspan->width(); ++i)
			{
				remove_columns.insert(rspan->start_column() + i);
			}
			uint32_t remaining_columns = REMOVE_COLUMN_COUNT - lspan->width() - rspan->width();
			if (spans.size() == 3)
			{
				auto& mspan = ++lspan;
				if (mspan->width() > remaining_columns)
				{
					for (uint32_t i = 0; i < remaining_columns; ++i)
					{
						remove_columns.insert(mspan->start_column() + i);
					}
				}
			}
		}
	}


	// finally set span:
	QImage spanDMD(DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT, QImage::Format_RGBA8888);
	spanDMD.fill(Qt::black);
	if (remove_columns.size() == 0)
		return spanDMD;

	int dest_y = 8;
	int current_column = 0;
	for (uint32_t x = 0; x < FANTASIES_DMD_WIDTH; ++x)
	{
		if (remove_columns.find(x) != remove_columns.end())
			continue;

		for (uint32_t y = 0; y < 16; ++y)
		{
			uint8_t val = RAWPIXEL(x, y);
			spanDMD.setPixel(current_column, y + 8, qRgb(val, val, val));
		}
		++current_column;
		if (current_column == DMDConfig::DMDWIDTH)
			break;
	}

	return spanDMD;
}

QImage FantasiesWindow::dedot_dmd(const QImage& img)
{
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

	return dmd;
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
	bool save_frames = true;
	QVector<QImage> frames;
	for (uint32_t i = 35; i < 15851; ++i)
	//for (uint32_t i = 35; i < 136; ++i)
	{
		memset(rawDMD, 0, sizeof(rawDMD));
		QString filenamepng = QString("D:/pf/shots/shot") + QString::number(i) + ".png";
		QImage img(filenamepng);

		QImage dmd = dedot_dmd(img);
		QImage fixed = span_fix();
		
		if (save_frames)
		{
			QString savepng = QString("D:/pf/convert/cnv") + QString::number(i) + ".png";
			QImage sub = fixed.copy(QRect(0, 8, 128, 16));
			sub.save(savepng);
		}
		else
		{
			frames.push_back(fixed);
		}
	}
	ImageAnimation* anim = new ImageAnimation(frames, 1);
	m_animation_engine->show_animation(anim);
}

#include "gameanimation.h"

void FantasiesWindow::debug_button_clicked()
{
	m_animation_engine->show_animation(new GameAnimation());
}
