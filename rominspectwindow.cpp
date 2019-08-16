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

#include "rominspectwindow.h"

#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QFile>
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>

ROMInspectWindow::ROMInspectWindow(QWidget* parent, DMDAnimationEngine* animation_engine)
: QWidget(parent, Qt::Window)
, m_animation_engine(animation_engine)
{
	setWindowTitle("ROM Inspector");
	initUI();
	this->setGeometry(0, 0, 800, 600);

	QRect screenGeometry = QApplication::desktop()->screenGeometry();
	int x = (screenGeometry.width() - width()) / 2;
	int y = (screenGeometry.height() -height()) / 2;
	move(x, y);
	show();
}

ROMInspectWindow::~ROMInspectWindow()
{
	delete[] m_rom_content;
}

void ROMInspectWindow::initUI()
{
	QLabel* file_label = new QLabel("File:", this);
	file_label->setGeometry(10, 10, 100, 20);
	m_file_name_label = new QLabel("filename", this);
	m_file_name_label->setGeometry(115, 10, 200, 20);
	QPushButton* file_open_button = new QPushButton("Open ROM", this);
	file_open_button->setGeometry(220, 10, 120, 20);
	connect(file_open_button, SIGNAL(clicked()), this, SLOT(file_open_button_clicked()));
	m_image_label = new QLabel(this);
	m_image_label->setGeometry(10, 30, 128 * 4, 32 * 4);

	QPushButton* reset_rom_index_button = new QPushButton("Reset", this);
	reset_rom_index_button->setGeometry(10, 160, 40, 20);
	connect(reset_rom_index_button, SIGNAL(clicked()), this, SLOT(reset_rom_index_button_clicked()));

	QPushButton* dec_rom_index_button = new QPushButton("<", this);
	dec_rom_index_button->setGeometry(50, 160, 40, 20);
	connect(dec_rom_index_button, SIGNAL(clicked()), this, SLOT(dec_rom_index_button_clicked()));

	QPushButton* dec_line_rom_index_button = new QPushButton("<-", this);
	dec_line_rom_index_button->setGeometry(90, 160, 40, 20);
	connect(dec_line_rom_index_button, SIGNAL(clicked()), this, SLOT(dec_line_rom_index_button_clicked()));
	
	QPushButton* pgdn_rom_index_button = new QPushButton("<<", this);
	pgdn_rom_index_button->setGeometry(130, 160, 40, 20);
	connect(pgdn_rom_index_button, SIGNAL(clicked()), this, SLOT(pgdn_rom_index_button_clicked()));

	QPushButton* pgup_rom_index_button = new QPushButton(">>", this);
	pgup_rom_index_button->setGeometry(170, 160, 40, 20);
	connect(pgup_rom_index_button, SIGNAL(clicked()), this, SLOT(pgup_rom_index_button_clicked()));

	QPushButton* inc_line_rom_index_button = new QPushButton("->", this);
	inc_line_rom_index_button->setGeometry(210, 160, 40, 20);
	connect(inc_line_rom_index_button, SIGNAL(clicked()), this, SLOT(inc_line_rom_index_button_clicked()));

	QPushButton* inc_rom_index_button = new QPushButton(">", this);
	inc_rom_index_button->setGeometry(250, 160, 40, 20);
	connect(inc_rom_index_button, SIGNAL(clicked()), this, SLOT(inc_rom_index_button_clicked()));

	QLabel* rom_index = new QLabel("ROM index:", this);
	rom_index->setGeometry(10, 185, 120, 20);

	m_rom_index_label = new QLabel(this);
	m_rom_index_label->setGeometry(130, 185, 120, 20);
}

void ROMInspectWindow::parse_image()
{
	if (m_rom_content == nullptr)
		return;

	QImage image(128, 32, QImage::Format_RGBA8888);
	image.fill(Qt::black);

	uint32_t current_x = 0;
	uint32_t current_y = 0;
	for (uint32_t byte_pos = 0; byte_pos < FRAME_SIZE; ++byte_pos)
	{
		uint32_t index = byte_pos + m_rom_index;
		uint8_t current_byte = m_rom_content[index];
#ifdef SEQUENTIAL
		uint8_t mask = 0x80;
		for (uint32_t bit = 0; bit < 8; ++bit)
		{
			if (current_byte & mask)
				image.setPixelColor(current_x, current_y, Qt::white);
			mask >>= 1;
			++current_x;
		}
#else
		uint8_t mask = 0x01;
		for (uint32_t bit = 0; bit < 8; ++bit)
		{
			if (current_byte & mask)
				image.setPixelColor(current_x, current_y, Qt::white);
			mask <<= 1;
			++current_x;
		}

#endif
		if (current_x >= 128)
		{
			current_x = 0;
			current_y++;
		}
	}

	m_image_label->setPixmap(QPixmap::fromImage(image).scaled(128 * 4, 32 * 4));
}

void ROMInspectWindow::update_index()
{
	QString index_str = QString::number(m_rom_index);
	index_str += " (0x";
	index_str += QString::number(m_rom_index, 16).toUpper();
	index_str += ")";
	m_rom_index_label->setText(index_str);
}

void ROMInspectWindow::file_open_button_clicked()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open ROM"), "", tr("ROM (*.rom)"));
	if (filename != "")
	{
		delete[] m_rom_content;
		m_rom_content = nullptr;
		QFile file(filename);
		if (file.open(QIODevice::ReadOnly))
		{
			m_rom_content = new uint8_t[file.size()];
			file.read(reinterpret_cast<char*>(m_rom_content), file.size());
			m_file_name_label->setText(filename);
			m_rom_index = 0;
			update_index();
		}
	}
}

void ROMInspectWindow::reset_rom_index_button_clicked()
{
	m_rom_index = 0;
	update_index();
	parse_image();
}

void ROMInspectWindow::dec_rom_index_button_clicked()
{
	if (m_rom_index != 0)
		m_rom_index--;
	update_index();
	parse_image();
}

void ROMInspectWindow::dec_line_rom_index_button_clicked()
{
	const uint32_t LINE_WIDTH = 128 / 8;
	if (m_rom_index > LINE_WIDTH)
		m_rom_index -= LINE_WIDTH;
	update_index();
	parse_image();
}

void ROMInspectWindow::pgdn_rom_index_button_clicked()
{
	if (m_rom_index >= FRAME_SIZE)
		m_rom_index -= FRAME_SIZE;
	update_index();
	parse_image();
}

void ROMInspectWindow::pgup_rom_index_button_clicked()
{
	m_rom_index += FRAME_SIZE;
	update_index();
	parse_image();
}

void ROMInspectWindow::inc_line_rom_index_button_clicked()
{
	const uint32_t LINE_WIDTH = 128 / 8;
	m_rom_index += LINE_WIDTH;
	update_index();
	parse_image();
}

void ROMInspectWindow::inc_rom_index_button_clicked()
{
	m_rom_index++;
	update_index();
	parse_image();
}
