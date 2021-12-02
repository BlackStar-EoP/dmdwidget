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

#include "teslawindow.h"

#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QFile>
#include <QApplication>
#include <QDesktopWidget>
#include <QScrollArea>
#include <QSlider>
#include <assert.h>

TeslaWindow::TeslaWindow(QWidget* parent)
	: QWidget(parent, Qt::Window)
{
	setWindowTitle("Tesla Ripper");
	initUI();
	uint32_t WIDTH = 1800;
	uint32_t HEIGHT = 1080;
	this->setGeometry(0, 0, WIDTH, HEIGHT);

	QRect screenGeometry = QApplication::desktop()->screenGeometry();
	int x = (screenGeometry.width() - width()) / 2;
	int y = (screenGeometry.height() - height()) / 2;
	move(x, y);


	//QScrollArea* scrollarea = new QScrollArea(this);

	//scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	//scrollarea->setWidgetResizable(false);

	//scrollarea->setGeometry(0, 0, WIDTH, HEIGHT);
	//this->setCentralWidget(scrollarea);

	//ui->setupUi(this);

	//setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);
	//setFixedWidth(700);
	//setFixedHeight(600);

	show();
}

TeslaWindow ::~TeslaWindow()
{
}

void TeslaWindow::initUI()
{
	QLabel* file_label = new QLabel("File:", this);
	file_label->setGeometry(10, 10, 100, 20);
	m_file_name_label = new QLabel("filename", this);
	m_file_name_label->setGeometry(115, 10, 200, 20);

	QPushButton* open_file_button = new QPushButton("Open file", this);
	open_file_button->setGeometry(10, 30, 100, 20);
	connect(open_file_button, SIGNAL(clicked()), this, SLOT(open_file_button_clicked()));

	QPushButton* next_button = new QPushButton("Next", this);
	next_button->setGeometry(10, 60, 100, 20);
	connect(next_button, SIGNAL(clicked()), this, SLOT(next_button_clicked()));

	QPushButton* prev_button = new QPushButton("Prev", this);
	prev_button->setGeometry(10, 90, 100, 20);
	connect(prev_button, SIGNAL(clicked()), this, SLOT(prev_button_clicked()));


	m_image_label = new QLabel("", this);
	m_image_label->setGeometry(100, 60, 1024, 1024);


	m_width_slider = new QSlider(this);
	m_width_slider->setGeometry(10, 120, 100, 20);

	m_height_slider = new QSlider(this);
	m_height_slider->setGeometry(10, 150, 100, 20);

	QPushButton* width_plus = new QPushButton("W+", this);
	width_plus->setGeometry(10, 180, 20, 20);
	connect(width_plus, SIGNAL(clicked()), this, SLOT(width_plus_button_clicked()));

	QPushButton* width_min = new QPushButton("W+", this);
	width_min->setGeometry(30, 180, 20, 20);
	connect(width_min, SIGNAL(clicked()), this, SLOT(width_min_button_clicked()));

	QPushButton* height_plus = new QPushButton("H+", this);
	height_plus->setGeometry(50, 180, 20, 20);
	connect(height_plus, SIGNAL(clicked()), this, SLOT(height_plus_button_clicked()));

	QPushButton* height_min = new QPushButton("H-", this);
	height_min->setGeometry(70, 180, 20, 20);
	connect(height_min, SIGNAL(clicked()), this, SLOT(height_min_button_clicked()));
}

void TeslaWindow::open_file_button_clicked()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open ZEN Pinball FX Texture"), "", tr("PXT (*.pxt)"));
	if (filename != "")
	{
		QFile file(filename);
		if (file.open(QIODevice::ReadOnly))
		{
			delete[] m_data;
			m_size = file.size();
			m_data = new uint8_t[file.size()];
			file.read(reinterpret_cast<char*>(m_data), file.size());
			file.close();
			update_image();
		}
	}
}

void TeslaWindow::next_button_clicked()
{
	++m_image_nr;
	update_image();
}

void TeslaWindow::prev_button_clicked()
{
	--m_image_nr;
	update_image();
}


void TeslaWindow::width_plus_button_clicked()
{
	m_parse_width++;
	update_image();
}

void TeslaWindow::width_min_button_clicked()
{
	if (m_parse_width > 0)
		m_parse_width--;
	update_image();
}

void TeslaWindow::height_plus_button_clicked()
{
	m_parse_height++;
	update_image();
}

void TeslaWindow::height_min_button_clicked()
{
	if (m_parse_height > 0)
		m_parse_height--;
	update_image();

}


void TeslaWindow::update_image()
{
	QImage img(m_parse_width, m_parse_height, QImage::Format_RGBA8888);
	
	uint32_t x = 0;
	uint32_t y = 0;


	uint32_t index = (m_parse_width * m_parse_height * 4) * m_image_nr;

	while (index < m_size)
	{
		uint8_t r = m_data[index++];
		uint8_t g = m_data[index++];
		uint8_t b = m_data[index++];
		uint8_t a = m_data[index++];

		//uint16_t rgb = m_data[index] | ((uint16_t)m_data[index + 1] << 8);
		//index += 2;

		++x;
		if (x >= m_parse_width)
		{
			x = 0;
			++y;
		}

		if (x == 1023)
			printf("");
		if (y >= m_parse_height)
			break;

		////img.setPixel(x, y, qRgb(rgb >> 11, rgb >> 5, rgb &  0x1F));
		img.setPixel(x, y, qRgb(r, g, b));

	}

	m_image_label->setPixmap(QPixmap::fromImage(img));

}