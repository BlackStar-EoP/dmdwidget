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

#include "fantasieswindow.h"

#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QFile>
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QSet>

#include <assert.h>

FantasiesWindow::FantasiesWindow(QWidget* parent, DMDAnimationEngine* animation_engine)
: QWidget(parent, Qt::Window)
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
}

void FantasiesWindow::update_image()
{
	QString filename = QString("D:/pf/shots/shot") + QString::number(m_current_file_nr) + ".bsr";
	m_file_name_label->setText(filename);

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

				if (m_debug_colors)
				{
					if (y < 34)
					{
						uint32_t px = dmddata[pixelnr++];
						switch (px)
						{
						case 0xff000000:
							img.setPixel(x, y, qRgba(0, 0, 0, 255));
							break;
						case 0xff515151:
							img.setPixel(x, y, qRgba(255, 0, 0, 255));
							break;
						case 0xff555555:
							img.setPixel(x, y, qRgba(0, 255, 0, 255));
							break;
						case 0xfff3b245:
							img.setPixel(x, y, qRgba(0, 0, 255, 255));
							break;
						}
					}
					else
					{
						img.setPixel(x, y, qRgba(r, g, b, a));
					}
				}
				else
				{
					img.setPixel(x, y, qRgba(r, g, b, a));
				}



			}
		}

		m_image_label->setPixmap(QPixmap::fromImage(img));

		pixelnr = 0;
		// Debug colors to set
		for (uint32_t line = 0; line < FANTASIES_DMD_HEIGHT; ++line)
		{
			for (uint32_t px = 0; px < FANTASIES_DMD_WIDTH; ++px)
			{
				uint32_t pixel = dmddata[pixelnr++];
				pixels[pixel].insert(m_current_file_nr);
				//pixels.insert(pixel);
				//(0xFFF3B245)
			}
		}


		// de-dot DMD
		pixelnr = 0;
		QImage dmd(FANTASIES_DMD_WIDTH, FANTASIES_DMD_HEIGHT, QImage::Format_RGBA8888);
		for (uint32_t y = 0; y < FANTASIES_DMD_HEIGHT; ++y)
		{
			for (uint32_t x = 0; x < FANTASIES_DMD_WIDTH; ++x)
			{
				uint32_t pixel = dmddata[pixelnr++];
				switch (pixel)
				{
				case 0xff000000:
					dmd.setPixel(x, y, qRgba(0, 0, 0, 255));
					break;
				case 0xff515151:
					dmd.setPixel(x, y, qRgba(0, 0, 0, 255));
					break;
				case 0xff555555:
					dmd.setPixel(x, y, qRgba(0, 0, 0, 255));
					break;
				case 0xfff3b245:
					dmd.setPixel(x, y, qRgba(255, 255, 255, 255));
					break;
				}

				pixelnr++; // Extra stride
			}

			pixelnr += 320; // Skip a complete line
		}
		m_dmd_label->setPixmap(QPixmap::fromImage(dmd));

	}
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
	m_current_file_nr = 1;
	while (m_current_file_nr < 1000)
	{
		update_image();
		++m_current_file_nr;
	}
	printf("");
}

void FantasiesWindow::debug_button_clicked()
{
	m_debug_colors = !m_debug_colors;
	update_image();
}
