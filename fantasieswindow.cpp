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
#include <QKeyEvent>

#include <assert.h>


const int DMD_SIZE = 4;

class FantasiesEventFilter : public QObject
{
public:
	FantasiesEventFilter(FantasiesWindow& window)
		: m_window(window)
	{

	}

	bool eventFilter(QObject* watched, QEvent* event) override
	{
		Q_UNUSED(watched);
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent* key_event = (QKeyEvent*)event;
			int key = key_event->key();
			switch (key)
			{
			case Qt::Key_4:
			case Qt::Key_A:
				m_window.key_left();
				break;
			case Qt::Key_6:
			case Qt::Key_D:
				m_window.key_right();
				break;
			case Qt::Key_8:
			case Qt::Key_W:
				m_window.key_up();
				break;
			case Qt::Key_2:
			case Qt::Key_S:
				m_window.key_down();
				break;
			}
		}
		return false;
	}

	private:
		FantasiesWindow& m_window;
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
	installEventFilter(new FantasiesEventFilter(*this));
	move(x, y);
	show();

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}

FantasiesWindow::~FantasiesWindow()
{
}

void FantasiesWindow::key_left()
{
	if (m_byte_index > 0)
		m_byte_index--;
	update_image();
}
void FantasiesWindow::key_right()
{
	if (m_byte_index < 319)
		m_byte_index++;
	update_image();
}
void FantasiesWindow::key_up()
{
	if (m_byte_index > 19)
		m_byte_index -= 20;
	update_image();
}
void FantasiesWindow::key_down()
{
	if (m_byte_index < 300)
		m_byte_index += 20;
	update_image();
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

	QPushButton* inc_img1000_button = new QPushButton("INC1000", this);
	inc_img1000_button->setGeometry(10, 180, 100, 20);
	connect(inc_img1000_button, SIGNAL(clicked()), this, SLOT(inc_img1000_button_clicked()));
	QPushButton* dec_img1000_button = new QPushButton("DEC1000", this);
	dec_img1000_button->setGeometry(10, 205, 100, 20);
	connect(dec_img1000_button, SIGNAL(clicked()), this, SLOT(dec_img1000_button_clicked()));


	QPushButton* debug_button = new QPushButton("TETRIS", this);
	debug_button->setGeometry(10, 230, 100, 20);
	connect(debug_button, SIGNAL(clicked()), this, SLOT(debug_button_clicked()));
	
	m_image_label = new QLabel(this);
	m_image_label->setGeometry(150, 30, FANTASIES_DMD_WIDTH * DMD_SIZE, FANTASIES_DMD_HEIGHT * DMD_SIZE);

	m_dmd_span_label = new QLabel(this);
	m_dmd_span_label->setGeometry(150, 300, DMDConfig::DMDWIDTH * DMD_SIZE, DMDConfig::DMDHEIGHT * DMD_SIZE);

	m_byte_label = new QLabel("byte label", this);
	m_byte_label->setGeometry(150, 100, 200, 20);

	m_spans_image_label = new QLabel(this);
	m_spans_image_label->setGeometry(150, 130, FANTASIES_DMD_WIDTH * DMD_SIZE, FANTASIES_DMD_HEIGHT * DMD_SIZE);

	m_spans_text_label = new QLabel("spans label", this);
	m_spans_text_label->setGeometry(150, 200, 400, 20);


	m_parsed_image_label = new QLabel(this);
	m_parsed_image_label->setGeometry(150, 450, DMDConfig::DMDWIDTH * DMD_SIZE, DMDConfig::DMDHEIGHT * DMD_SIZE);

	m_parsed_text_label = new QLabel("parsed label", this);
	m_parsed_text_label->setGeometry(150, 590, 400, 20);
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

#include <Windows.h>

void FantasiesWindow::update_image()
{
	if (_CrtCheckMemory() == FALSE)
	{
		printf("");
	}
	memset(rawDMD, 0, sizeof(rawDMD));
	QString filenamepng = QString("./dmd/shot") + QString::number(m_current_file_nr) + ".dmd";
	m_file_name_label->setText(filenamepng);

	QImage img(FANTASIES_DMD_WIDTH, FANTASIES_DMD_HEIGHT, QImage::Format_RGBA8888);
	QFile file(filenamepng);
	if (file.open(QIODevice::ReadOnly))
	{

		QByteArray data = file.readAll();
		const char* pixeldata = data.constData();
		memcpy(rawDMD, pixeldata, sizeof(rawDMD));
		file.close();
		QString byteval = QString(" Value = 0x") + QString::number(pixeldata[m_byte_index], 16).right(2).toUpper();
		m_byte_label->setText(QString("Byte index = ") + QString::number(m_byte_index) + byteval);

		uint32_t x = 0;
		uint32_t y = 0;
		for (int32_t byte = 0; byte < data.size(); ++byte)
		{
			uint8_t value = pixeldata[byte];
			for (uint32_t bitindex = 0; bitindex < 8; ++bitindex)
			{
				uint8_t bit = value & (1 << bitindex);
				if (bit)
				{
					if (byte == m_byte_index)
						img.setPixel(x, y, qRgb(255, 0, 0));
					else
						img.setPixel(x, y, qRgb(255, 255, 255));
					
					decodedDMD[FANTASIES_DMD_WIDTH * y + x] = 255;
				}
				else
				{
					if (byte == m_byte_index)
						img.setPixel(x, y, qRgb(0, 255, 0));
					else
						img.setPixel(x, y, qRgb(0, 0, 0));
					
					decodedDMD[FANTASIES_DMD_WIDTH * y + x] = 0;
				}
				
				++x;
			}
			if (x >= 160)
			{
				x = 0;
				++y;
			}
		}

	}

	determine_spans();
	paint_spans(img);

	QImage scaled = img.scaled(FANTASIES_DMD_WIDTH * DMD_SIZE, FANTASIES_DMD_HEIGHT * DMD_SIZE, Qt::KeepAspectRatio, Qt::FastTransformation);
	m_image_label->setPixmap(QPixmap::fromImage(scaled));
	QImage fixed = span_fix().scaled(DMDConfig::DMDWIDTH * DMD_SIZE, DMDConfig::DMDHEIGHT * DMD_SIZE, Qt::KeepAspectRatio, Qt::FastTransformation);
	m_dmd_span_label->setPixmap(QPixmap::fromImage(fixed));

	QImage parsed = parsed_fix().scaled(DMDConfig::DMDWIDTH * DMD_SIZE, DMDConfig::DMDHEIGHT * DMD_SIZE, Qt::KeepAspectRatio, Qt::FastTransformation);
	m_parsed_image_label->setPixmap(QPixmap::fromImage(parsed));
	//QVector<QImage> frames;
	//frames.push_back(fixed);
	//ImageAnimation* anim = new ImageAnimation(frames, 1);
	//m_animation_engine->show_animation(anim);
}

void FantasiesWindow::paint_spans(const QImage& img)
{
	QImage span_img = img;
	QPainter p(&span_img);
	const uint32_t NUM_COLORS = 8;
	QColor colors[NUM_COLORS] = { QColor(255,0,0),
						QColor(0,255,0),                                         
						QColor(0,0,255),
						QColor(255,255,0),
						QColor(255,0,255),
						QColor(0,255,255),
						QColor(128,255,0),
						QColor(255,0,128)
					  };
	QString spans_text = "Spans: " + QString::number(m_spans.size());

	uint32_t spans_total_width = 0;
	for (size_t i = 0; i < m_spans.size(); ++i)
	{
		const Span& span = m_spans[i];
		spans_total_width += span.width();
		spans_text += QString(" (w=") + QString::number(span.width()) + QString("(") + QString::number(span.start_column()) + QString(",") + QString::number(span.end_column()) + "))";
		p.fillRect(span.start_column(), 0, span.width(), 16, colors[i % NUM_COLORS]);
	}
	spans_text += QString(" Total width=") + QString::number(spans_total_width);
	QImage scaled = span_img.scaled(FANTASIES_DMD_WIDTH * DMD_SIZE, FANTASIES_DMD_HEIGHT * DMD_SIZE, Qt::KeepAspectRatio, Qt::FastTransformation);
	m_spans_image_label->setPixmap(QPixmap::fromImage(scaled));
	m_spans_text_label->setText(spans_text);
}

void FantasiesWindow::determine_spans()
{
	m_spans.clear();

	bool in_span = false;
	uint32_t span_start = 0;

	for (uint32_t x = 0; x < FANTASIES_DMD_WIDTH + 1; ++x)
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
					m_spans.push_back(Span(span_start, x-1));
			}
		}
	}
}

bool FantasiesWindow::is_scrolling_rtl() const
{
	if (m_prev_frame_spans.size() != 1 ||
		m_spans.size() != 1)
		return false;

	if (m_prev_frame_spans[0].is_clear_screen())
	{
		const Span& current_span = m_spans[0];
		if (current_span.start_column() == 0 &&
			current_span.end_column() == 158)
			return true;
	}

	return false;
}

bool FantasiesWindow::is_current_frame_empty() const
{
	if (m_spans.size() == 1)
	{
		return m_spans[0].is_clear_screen();
	}

	return false;
}

QImage FantasiesWindow::span_fix()
{
	std::set<uint32_t> remove_columns;

	uint32_t num_big_spans = 0;

	for (auto it = m_spans.begin(); it != m_spans.end(); ++it)
	{
		uint32_t spanwidth = it->width();
		if (spanwidth >= Span::REMOVE_COLUMN_COUNT)
		{
			++num_big_spans;
		}
	}

	if (m_spans.size() == 1)
	{
		auto first = m_spans.begin();
		if (first->width() == FANTASIES_DMD_WIDTH)
		{
			// TODO, DMD cleared, reset any animation flags
		}
	}

	if (num_big_spans > 0)
	{
		if (num_big_spans == 1)
		{
			for (auto span = m_spans.begin(); span != m_spans.end(); ++span)
			{
				if (span->is_big_span())
				{
					if (!span->is_left_span())
					{
						for (uint32_t i = 0; i < Span::REMOVE_COLUMN_COUNT; ++i)
						{
							remove_columns.insert(span->start_column() + i);
						}
					}
					else
					{
						auto lspan = m_spans.begin();
						auto rspan = m_spans.rbegin();
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
			auto lspan = m_spans.begin();
			auto rspan = m_spans.rbegin();
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
	else if (m_spans.size() >= 2 || num_big_spans > 1) // attempt left and right removal
	{
		auto lspan = m_spans.begin();
		auto rspan = m_spans.rbegin();

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
			uint32_t remaining_columns = Span::REMOVE_COLUMN_COUNT - lspan->width() - rspan->width();
			if (m_spans.size() == 3)
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


QImage FantasiesWindow::parsed_fix()
{
	DMDFrame frame;
	if (is_current_frame_empty())
	{
		m_current_animation = NONE;
	}
	else if (is_scrolling_rtl())
	{
		m_current_animation = SCROLLING_RTL;
	}

	if (m_current_animation == NONE)
	{
		// Fantasies logo
		char cmpbufferp[]{ 0xF0, 0xF3, 0xF7, 0xF3 };
		if (memcmp(rawDMD + 21, cmpbufferp, 4) == 0)
		{
			/// 12 - 67 = PINBALL
			copyblock(12, 0, 66, 15, 27, 0, frame);
			copyblock(76, 0, 146, 15, 35, 16, frame);
			printf("");
		}
		// Score
		char scorebuffer[]{ 0x3F, 0x07, 0x3E, 0x77 };
		{
			if (memcmp(rawDMD + 21, scorebuffer, 4) == 0)
			{
				copyblock(8, 0, 76, 15, 8, 0, frame);
				copyblock(77, 0, 159, 15, 45, 16, frame);
			}
		}
	}
	else if (m_current_animation == SCROLLING_RTL)
	{
		copyblock(32, 0, 159, 15, 0, 8, frame);
	}

		//21 0xF0
		//0xF3
		//0xF7
		//0xF3
	m_prev_frame_spans = m_spans;
	return image_from_DMD_frame(frame);
}

void FantasiesWindow::copyblock(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t dest_x, int32_t dest_y, DMDFrame& outputDMD)
{
	assert(x1 <= x2);
	assert(y1 <= y2);
	int32_t width = x2 - x1 + 1;
	int32_t height = y2 - y1 + 1;
	assert(width <= FANTASIES_DMD_WIDTH);
	assert(height <= FANTASIES_DMD_HEIGHT);

	for (int32_t y = 0; y < height; ++y)
	{
		for (int32_t x = 0; x < width; ++x)
		{
			uint8_t pixel = decodedDMD[(FANTASIES_DMD_WIDTH * (y1 + y)) + (x + x1)];
			outputDMD.set_pixel(dest_x + x, dest_y + y, pixel);
		}
	}
}

QImage FantasiesWindow::image_from_DMD_frame(const DMDFrame& frame)
{
	QImage dmd(DMDConfig::DMDWIDTH, DMDConfig::DMDHEIGHT, QImage::Format_RGBA8888);
	dmd.fill(Qt::black);

	for (int32_t y = 0; y < DMDConfig::DMDHEIGHT; ++y)
	{
		for (int32_t x = 0; x < DMDConfig::DMDWIDTH; ++x)
		{
			uint8_t pixel = frame.grayscale_pixel(x, y);
			dmd.setPixel(x, y, qRgb(pixel, pixel, pixel));
		}
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

void FantasiesWindow::inc_img1000_button_clicked()
{
	m_current_file_nr += 1000;
	update_image();
}

void FantasiesWindow::dec_img1000_button_clicked()
{
	if (m_current_file_nr > 1000)
		m_current_file_nr -= 1000;
	update_image();
}

void FantasiesWindow::split_range_button_clicked()
{
}

#include "gameanimation.h"

void FantasiesWindow::debug_button_clicked()
{
	m_animation_engine->show_animation(new GameAnimation());
}
