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
	m_image_label->setGeometry(150, 30, FantasiesDMD::FANTASIES_DMD_WIDTH * DMD_SIZE, FantasiesDMD::FANTASIES_DMD_HEIGHT * DMD_SIZE);

	m_byte_label = new QLabel("byte label", this);
	m_byte_label->setGeometry(150, 100, 600, 20);

	m_spans_image_label = new QLabel(this);
	m_spans_image_label->setGeometry(150, 130, FantasiesDMD::FANTASIES_DMD_WIDTH * DMD_SIZE, FantasiesDMD::FANTASIES_DMD_HEIGHT * DMD_SIZE);

	m_spans_text_label = new QLabel("spans label", this);
	m_spans_text_label->setGeometry(150, 200, 600, 20);


	m_parsed_image_label = new QLabel(this);
	m_parsed_image_label->setGeometry(150, 450, DMDConfig::DMDWIDTH * DMD_SIZE, DMDConfig::DMDHEIGHT * DMD_SIZE);

	m_parsed_text_label = new QLabel("parsed label", this);
	m_parsed_text_label->setGeometry(150, 590, 400, 20);
}

void FantasiesWindow::update_image()
{
	m_fantasies_DMD.clear();
	QString filenamepng = QString("./dmd/shot") + QString::number(m_current_file_nr) + ".dmd";
	m_file_name_label->setText(filenamepng);

	if (m_fantasies_DMD.read_file(filenamepng))
	{
		QString byteval = QString(" Value = 0x") + QString::number(m_fantasies_DMD.bytevalue(m_byte_index), 16).right(2).toUpper();
		m_byte_label->setText(QString("Byte index = ") + QString::number(m_byte_index) + byteval);
	}

	m_fantasies_DMD.determine_spans();
	QImage img = m_fantasies_DMD.image(m_byte_index);
	paint_spans(img);

	QImage scaled = img.scaled(FantasiesDMD::FANTASIES_DMD_WIDTH * DMD_SIZE, FantasiesDMD::FANTASIES_DMD_HEIGHT * DMD_SIZE, Qt::KeepAspectRatio, Qt::FastTransformation);
	m_image_label->setPixmap(QPixmap::fromImage(scaled));
	
	const DMDFrame& parsed_dmd = m_fantasies_DMD.parse_DMD();
	
	QImage parsed = m_fantasies_DMD.image().scaled(DMDConfig::DMDWIDTH * DMD_SIZE, DMDConfig::DMDHEIGHT * DMD_SIZE, Qt::KeepAspectRatio, Qt::FastTransformation);
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
	QString spans_text = "Spans: " + QString::number(m_fantasies_DMD.num_spans());

	uint32_t spans_total_width = 0;
	for (size_t i = 0; i < m_fantasies_DMD.num_spans(); ++i)
	{
		const Span& span = m_fantasies_DMD.span(i);
		spans_total_width += span.width();
		spans_text += QString(" (w=") + QString::number(span.width()) + QString("(") + QString::number(span.start_column()) + QString(",") + QString::number(span.end_column()) + "))";
		p.fillRect(span.start_column(), 0, span.width(), 16, colors[i % NUM_COLORS]);
	}
	spans_text += QString(" Total width=") + QString::number(spans_total_width);
	QImage scaled = span_img.scaled(FantasiesDMD::FANTASIES_DMD_WIDTH * DMD_SIZE, FantasiesDMD::FANTASIES_DMD_HEIGHT * DMD_SIZE, Qt::KeepAspectRatio, Qt::FastTransformation);
	m_spans_image_label->setPixmap(QPixmap::fromImage(scaled));
	m_spans_text_label->setText(spans_text);
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

	// // Rename stuff
	//int32_t out_nr = 42000;
	//for (int32_t i = 0; i < 27000; ++i)
	//{
	//	QString filenamepng = QString("d:/pf/dmd/shot") + QString::number(i) + ".dmd";
	//	QString newname = QString("d:/pf/dmd/shot") + QString::number(out_nr) + ".dmd";
	//	QFile file(filenamepng);
	//	file.rename(newname);
	//	out_nr++;
	//}
}
