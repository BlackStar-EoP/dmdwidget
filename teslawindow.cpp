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

#include "dmdanimationengine.h"
#include "imageanimation.h"
#include "dmdlabel.h"

#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QFile>
#include <QApplication>
#include <QDesktopWidget>
#include <QScrollArea>
#include <QSlider>
#include <QKeyEvent>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDirIterator>

#include <assert.h>

class TeslaEventFilter : public QObject
{
public:
	TeslaEventFilter(TeslaWindow& window)
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
				m_window.width_min_button_clicked();
				break;
			case Qt::Key_6:
			case Qt::Key_D: 
				m_window.width_plus_button_clicked();
				break;
			case Qt::Key_8:
			case Qt::Key_W:
				m_window.height_plus_button_clicked();
				break;
			case Qt::Key_2:
			case Qt::Key_S:
				m_window.height_min_button_clicked();
				break;
			}
		}
		return false;
	}

private:
	TeslaWindow& m_window;
};

TeslaWindow::TeslaWindow(QWidget* parent, DMDAnimationEngine* animation_engine)
	: QWidget(parent, Qt::Window)
	, m_animation_engine(animation_engine)
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
	installEventFilter(new TeslaEventFilter(*this));

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

	// ellende


	QScrollArea* scroll_area = new QScrollArea(this);
	scroll_area->setGeometry(600, 10, 1024, 1024);
	scroll_area->setWidgetResizable(true);

	m_image_label = new ImageLabel(scroll_area);
	m_image_label->setGeometry(600, 10, 1024, 1024);
	scroll_area->setWidget(m_image_label);
	connect(m_image_label, SIGNAL(clicked(QPoint)), this, SLOT(image_clicked(QPoint)));



	// qt en scrollbars...

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

	m_width_label = new QLabel("", this);
	m_width_label->setGeometry(10, 200, 100, 20);
	m_height_label = new QLabel("", this);
	m_height_label->setGeometry(10, 220, 100, 20);


	QPushButton* inc_color_mode_button = new QPushButton("M+", this);
	inc_color_mode_button->setGeometry(10, 240, 100, 20);
	connect(inc_color_mode_button, SIGNAL(clicked()), this, SLOT(inc_color_mode_button_clicked()));

	QPushButton* dec_color_mode_button = new QPushButton("M-", this);
	dec_color_mode_button->setGeometry(10, 260, 100, 20);
	connect(dec_color_mode_button, SIGNAL(clicked()), this, SLOT(dec_color_mode_button_clicked()));

	m_color_mode_label = new QLabel("", this);
	m_color_mode_label->setGeometry(10, 280, 100, 20);

	m_image_clicked_label = new QLabel(this);
	m_image_clicked_label->setGeometry(10, 300, 300, 20);


	QPushButton* rotate_plus_button = new QPushButton("R+", this);
	rotate_plus_button->setGeometry(10, 320, 100, 20);
	connect(rotate_plus_button, SIGNAL(clicked()), this, SLOT(rotate_plus_button_clicked()));

	QPushButton* rotate_min_button = new QPushButton("R-", this);
	rotate_min_button->setGeometry(10, 340, 100, 20);
	connect(rotate_min_button, SIGNAL(clicked()), this, SLOT(rotate_min_button_clicked()));

	m_rotate_label = new QLabel("Rotate value = ", this);
	m_rotate_label->setGeometry(10, 360, 100, 20);

	m_swap_nibbles_checkbox = new QCheckBox("Swap nibbles", this);
	m_swap_nibbles_checkbox->setGeometry(10, 380, 100, 20);
	connect(m_swap_nibbles_checkbox, SIGNAL(stateChanged(int)), this, SLOT(swap_nibbles_checkbox_clicked(int)));
	
	m_reverse_bits_checkbox = new QCheckBox("Reverse bits", this);
	m_reverse_bits_checkbox->setGeometry(10, 400, 100, 20);
	connect(m_reverse_bits_checkbox, SIGNAL(stateChanged(int)), this, SLOT(reverse_bits_checkbox_clicked(int)));

	QPushButton* save_button = new QPushButton("SAVE", this);
	save_button->setGeometry(10, 420, 100, 20);
	connect(save_button, SIGNAL(clicked()), this, SLOT(save_button_clicked()));

	QPushButton* permutate_save_button = new QPushButton("Permutate save", this);
	permutate_save_button->setGeometry(10, 440, 100, 20);
	connect(permutate_save_button, SIGNAL(clicked()), this, SLOT(permutate_save_button_clicked()));


	m_dmd_label = new DMDLabel(this);
	m_dmd_label->setPosition(120, 500);

	QPushButton* open_dmd_file_button= new QPushButton("open dmd", this);
	open_dmd_file_button->setGeometry(10, 500, 100, 25);
	connect(open_dmd_file_button, SIGNAL(clicked()), this, SLOT(zen_dmd_button_clicked()));

	QPushButton* prev_frame_button = new QPushButton("prev frame", this);
	prev_frame_button->setGeometry(10, 530, 100, 25);
	connect(prev_frame_button, SIGNAL(clicked()), this, SLOT(prev_frame_button_clicked()));

	QPushButton* next_frame_button = new QPushButton("next frame", this);
	next_frame_button->setGeometry(10, 560, 100, 25);
	connect(next_frame_button, SIGNAL(clicked()), this, SLOT(next_frame_button_clicked()));


	QPushButton* show_anim_button = new QPushButton("show anim", this);
	show_anim_button->setGeometry(10, 590, 100, 25);
	connect(show_anim_button, SIGNAL(clicked()), this, SLOT(show_anim_button_clicked()));

	QPushButton* save_anim_button = new QPushButton("save anim", this);
	save_anim_button->setGeometry(10, 620, 100, 25);
	connect(save_anim_button, SIGNAL(clicked()), this, SLOT(save_anim_button_clicked()));

	QPushButton* dump_anim_button = new QPushButton("dump anims", this);
	dump_anim_button->setGeometry(10, 620, 100, 25);
	connect(dump_anim_button, SIGNAL(clicked()), this, SLOT(dump_anim_button_clicked()));

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

		//QFileInfo fileInfo(file.fileName());
		//m_file_name_label->setText(fileInfo.fileName());
		m_file_name_label->setText(file.fileName());
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
	m_parse_width+=32;
	update_image();
}

void TeslaWindow::width_min_button_clicked()
{
	if (m_parse_width > 0)
		m_parse_width-=32;
	update_image();
}

void TeslaWindow::height_plus_button_clicked()
{
	m_parse_height+=32;
	update_image();
}

void TeslaWindow::height_min_button_clicked()
{
	if (m_parse_height > 0)
		m_parse_height-=32;
	update_image();

}

void TeslaWindow::zen_dmd_button_clicked()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open ZEN DMD animation"), "", tr("DMV (*.dmv)"));
	load_zen_animation(filename);
}

void TeslaWindow::next_frame_button_clicked()
{
	if (m_frame_index < m_zen_dmd_frames.size() - 1u)
	{
		m_frame_index++;
		show_frame();
	}
}

void TeslaWindow::prev_frame_button_clicked()
{
	if (m_frame_index > 0)
	{
		m_frame_index--;
		show_frame();
	}
}

void TeslaWindow::save_frame_button_clicked()
{

}


void TeslaWindow::show_anim_button_clicked()
{
	//m_animation_engine->show_animation();
}

void TeslaWindow::save_anim_button_clicked()
{
	if (m_dmd_filename == "")
		return;

	// Set up the save directory
	QString dir = QString("D:/TeslaDMD/") + m_dmd_filename + "/";
	auto test1 = dir.toStdString();
	QDir saveDir(dir);
	bool res = saveDir.mkpath(dir);
	
	for (int i = 0; i < m_zen_dmd_frames.size(); ++i)
	{
		uint framelength = 1;
		// Skip duplicate frames
		for (int j = i+1; j < m_zen_dmd_frames.size(); ++j)
		{
			if (m_zen_dmd_frames[j] == m_zen_dmd_frames[j - 1])
				++framelength;
			else
				break;
		}
		//if (i > 0 && m_zen_dmd_frames[i] == m_zen_dmd_frames[i - 1])
		//	continue;
		
		QString savefile = dir + QString("%1_%2.png").arg(i).arg(framelength);
		auto test = savefile.toStdString();
		m_zen_dmd_frames[i].img().save(savefile);
	}

}

void TeslaWindow::dump_anim_button_clicked()
{
	QString dir = QFileDialog::getExistingDirectory();
	auto td = dir.toStdString();

	QDir dmdDir(dir);
	QFileInfoList files = dmdDir.entryInfoList();
	for (QFileInfo& file : files)
	{
		if (file.absoluteFilePath().endsWith("dmv"))
		{
			auto test = file.absoluteFilePath().toStdString();
				load_zen_animation(file.absoluteFilePath());
				
				save_anim_button_clicked();
		}
	}
	//QDirIterator it(dir, { "*.dmv" }, QDir::Files, QDirIterator::NoIteratorFlags);
	//while (it.hasNext())
	//{
	//	auto test = it.fileName().toStdString();
	//	load_zen_animation(it.fileName());
	//	
	//	save_anim_button_clicked();
	//}
		

}


void TeslaWindow::inc_color_mode_button_clicked()
{
	if (m_color_mode < (NUM_COLOR_MODES - 1))
	{
		uint32_t new_mode = m_color_mode + 1;
		m_color_mode = (ColorMode)new_mode;
	}
		
	update_image();
}

void TeslaWindow::dec_color_mode_button_clicked()
{
	if (m_color_mode > 0)
	{
		uint32_t new_mode = m_color_mode - 1;
		m_color_mode = (ColorMode)new_mode;
	}

	update_image();
}

void TeslaWindow::image_clicked(QPoint pos)
{
	if (m_data == nullptr)
		return;

	uint32_t index = (m_parse_width * m_parse_height * 4) * m_image_nr;
	
	index += (pos.y() * m_parse_width + pos.x()) * 4;

	if (index >= m_size)
		return;

	uint32_t col = ((uint32_t)m_data[index] << 24) | 
					((uint32_t)m_data[index + 1] << 16) |
					((uint32_t)m_data[index + 2] << 8) |
					((uint32_t)m_data[index + 3]);


	m_image_clicked_label->setText(QString("Click (%1, %2) is 0x%3, address = 0x%4").arg(pos.x()).arg(pos.y()).arg(col, 0, 16).arg(index, 0, 16));
}

void TeslaWindow::save_button_clicked()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("SavePNG"), m_file_name_label->text() + ".png", tr("PNG (*.png)"));
	if (filename != "")
		m_image.save(filename);
}

void TeslaWindow::rotate_plus_button_clicked()
{
	if (m_rotate_value < 7)
		m_rotate_value++;

	m_rotate_label->setText(QString("Rotate value = %1").arg(m_rotate_value));
	update_image();
}

void TeslaWindow::rotate_min_button_clicked()
{
	if (m_rotate_value > 0)
		m_rotate_value--;
	
	m_rotate_label->setText(QString("Rotate value = %1").arg(m_rotate_value));
	update_image();
}

void TeslaWindow::swap_nibbles_checkbox_clicked(int)
{
	update_image();
}

void TeslaWindow::reverse_bits_checkbox_clicked(int)
{
	update_image();
}

void TeslaWindow::update_image()
{
	uint32_t x = 0;
	uint32_t y = 0;


	uint32_t index = (m_parse_width * m_parse_height * 4) * m_image_nr;
	uint32_t bytes_per_row = (m_parse_width * 4);

	if (bytes_per_row == 0u)
		return;

	uint32_t height = m_size / bytes_per_row;
	height += bytes_per_row;

	QImage img(m_parse_width, height, QImage::Format_RGBA8888);

	while (index < m_size)
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		parse_color(r, g, b, a, index);

		++x;
		if (x >= m_parse_width)
		{
			x = 0;
			++y;
		}

		if (y >= height)
			break;
		
		////img.setPixel(x, y, qRgb(rgb >> 11, rgb >> 5, rgb &  0x1F));
		//img.setPixel(x, y, qRgb(r, g, b)); // slow!!!

		// Get the line we want
		QRgb* line = (QRgb*)img.scanLine(y);

		// Go to the pixel we want
		line += x;

		// Actually set the pixel
		*line = qRgb(r, g, b);

	}
	m_image_label->setGeometry(0, 0, m_parse_width, height);
	m_image_label->setPixmap(QPixmap::fromImage(img));
	m_width_label->setText(QString("w = %1").arg(m_parse_width));
	m_height_label->setText(QString("h = %1").arg(m_parse_height));
	m_color_mode_label->setText(color_mode_string(m_color_mode));
	m_image = img;
}


void TeslaWindow::load_zen_animation(const QString& filename)
{
	if (filename != "")
	{
		uint8_t* dmd;
		QFile file(filename);
		m_dmd_filename = "";
		if (file.open(QIODevice::ReadOnly))
		{
			dmd = new uint8_t[file.size()];
			file.read(reinterpret_cast<char*>(dmd), file.size());
			file.close();
			QFileInfo fileInfo(file.fileName());
			m_dmd_filename = fileInfo.fileName();

			//m_dmd_filename = file.fileName();
		}

		uint16_t num_images = (uint16_t)dmd[1] << 8u | dmd[0];
		uint32_t offset = 2;
		
		m_zen_dmd_frames.clear();
		m_frame_index = 0u;

		for (uint16_t i = 0; i < num_images; ++i)
		{
			uint32_t header = dmd[offset] << 24 | dmd[offset + 1] | dmd[offset + 2] + dmd[offset + 3];
			offset += 4;
			DMDFrame frame;
			for (uint32_t y = 0; y < 32; ++y)
			{
				for (uint32_t x = 0; x < 128; x += 4)
				{
					uint8_t pixels = dmd[offset++];
					uint8_t pixel4 = pixels >> 6;
					uint8_t pixel3 = (pixels >> 4) & 3;
					uint8_t pixel2 = (pixels >> 2) & 3;
					uint8_t pixel1 = pixels & 3;
					const uint8_t MULTIPLIER = 127;
					if (pixel1 == 3)
						frame.set_pixel(x, y, 255);
					else
						frame.set_pixel(x, y, (uint8_t)(pixel1) * MULTIPLIER);

					if (pixel2 == 3)
						frame.set_pixel(x + 1, y, 255);
					else
						frame.set_pixel(x + 1, y, (uint8_t)(pixel2) * MULTIPLIER);
					
					if (pixel3 == 3)
						frame.set_pixel(x + 2, y, 255);
					else
						frame.set_pixel(x + 2, y, (uint8_t)(pixel3) * MULTIPLIER);
	
					if (pixel4 == 3)
						frame.set_pixel(x + 3, y, 255);
					else
						frame.set_pixel(x + 3, y, (uint8_t)(pixel4) * MULTIPLIER);

					
				}
			}
			m_zen_dmd_frames.push_back(frame);
		}
		//int i = m_frames.size();
		//show_anim_button_clicked();

		show_frame();
	}
}

void TeslaWindow::show_frame()
{
	m_dmd_label->show_frame(m_zen_dmd_frames[m_frame_index]);
}

void TeslaWindow::permutate_save_button_clicked()
{
	if (m_data == nullptr)
		return;

	m_swap_nibbles_checkbox->setChecked(false);
	m_reverse_bits_checkbox->setChecked(false);

	for (uint32_t rb = 0; rb < 2; ++rb)
	{
		m_swap_nibbles_checkbox->setChecked(false);
		for (uint32_t sn = 0; sn < 2; ++sn)
		{
			for (uint32_t rotate = 0; rotate < 8; ++rotate)
			{
				m_rotate_value = rotate;

				for (uint32_t colmode = 0; colmode < NUM_COLOR_MODES; ++colmode)
				{
					m_color_mode = (ColorMode) colmode;
					update_image();

					QString filename = QString("PERM_RB%1_SN%2_ROT%3_COLMODE_%4.PNG").arg(rb).arg(sn).arg(rotate).arg(color_mode_string(m_color_mode));
					QByteArray tmp1 = filename.toLatin1();
					const char* data = tmp1.data();
					m_image.save(filename);
				}
			}
			m_swap_nibbles_checkbox->setChecked(true);
		}
		m_reverse_bits_checkbox->setChecked(true);
	}
}

QString TeslaWindow::color_mode_string(ColorMode color_mode)
{
	switch (color_mode)
	{
		case RGBA: return "RGBA"; break;
		case RGAB: return "RGAB"; break;
		case RBGA: return "RBGA"; break;
		case RBAG: return "RBAG"; break;
		case RAGB: return "RAGB"; break;
		case RABG: return "RABG"; break;

		case GRBA: return "GRBA"; break;
		case GRAB: return "GRAB"; break;
		case GBAR: return "GBAR"; break;
		case GBRA: return "GBRA"; break;
		case GABR: return "GABR"; break;
		case GARB: return "GARB"; break;

		case BRGA: return "BRGA"; break;
		case BRAG: return "BRAG"; break;
		case BGRA: return "BGRA"; break;
		case BGAR: return "BGAR"; break;
		case BAGR: return "BAGR"; break;
		case BARG: return "BARG"; break;

		case ARGB: return "ARGB"; break;
		case ARBG: return "ARBG"; break;
		case ABGR: return "ABGR"; break;
		case ABRG: return "ABRG"; break;
		case AGBR: return "AGBR"; break;
		case AGRB: return "AGRB"; break;
		case NUM_COLOR_MODES: return "NUM_COLOR_MODES"; break;
	}

	return "UNKNOWN";
}

void TeslaWindow::parse_color(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a, uint32_t& index)
{
	switch (m_color_mode)
	{
	case RGBA:
		r = m_data[index++];
		g = m_data[index++];
		b = m_data[index++];
		a = m_data[index++];
		break;

	case RGAB:
		r = m_data[index++];
		g = m_data[index++];
		a = m_data[index++];
		b = m_data[index++];
		break;

	case RBGA:
		r = m_data[index++];
		b = m_data[index++];
		g = m_data[index++];
		a = m_data[index++];
		break;

	case RBAG:
		r = m_data[index++];
		b = m_data[index++];
		a = m_data[index++];
		g = m_data[index++];
		break;

	case RAGB:
		r = m_data[index++];
		a = m_data[index++];
		g = m_data[index++];
		b = m_data[index++];
		break;

	case RABG:
		r = m_data[index++];
		a = m_data[index++];
		b = m_data[index++];
		g = m_data[index++];
		break;


	case GRBA:
		g = m_data[index++];
		r = m_data[index++];
		b = m_data[index++];
		a = m_data[index++];
		break;

	case GRAB:
		g = m_data[index++];
		r = m_data[index++];
		a = m_data[index++];
		b = m_data[index++];
		break;

	case GBAR:
		g = m_data[index++];
		b = m_data[index++];
		a = m_data[index++];
		r = m_data[index++];
		break;

	case GBRA:
		g = m_data[index++];
		b = m_data[index++];
		r = m_data[index++];
		a = m_data[index++];
		break;

	case GABR:
		g = m_data[index++];
		a = m_data[index++];
		b = m_data[index++];
		r = m_data[index++];
		break;

	case GARB:
		g = m_data[index++];
		a = m_data[index++];
		r = m_data[index++];
		b = m_data[index++];
		break;


	case BRGA:
		b = m_data[index++];
		g = m_data[index++];
		r = m_data[index++];
		a = m_data[index++];
		break;

	case BRAG:
		b = m_data[index++];
		r = m_data[index++];
		a = m_data[index++];
		g = m_data[index++];
		break;

	case BGRA:
		b = m_data[index++];
		g = m_data[index++];
		r = m_data[index++];
		a = m_data[index++];
		break;

	case BGAR:
		b = m_data[index++];
		g = m_data[index++];
		a = m_data[index++];
		r = m_data[index++];
		break;

	case BAGR:
		b = m_data[index++];
		a = m_data[index++];
		g = m_data[index++];
		r = m_data[index++];
		break;

	case BARG:
		b = m_data[index++];
		a = m_data[index++];
		r = m_data[index++];
		g = m_data[index++];
		break;


	case ARGB:
		a = m_data[index++];
		r = m_data[index++];
		g = m_data[index++];
		b = m_data[index++];
		break;

	case ARBG:
		a = m_data[index++];
		r = m_data[index++];
		b = m_data[index++];
		g = m_data[index++];
		break;

	case ABGR:
		a = m_data[index++];
		b = m_data[index++];
		g = m_data[index++];
		r = m_data[index++];
		break;

	case ABRG:
		a = m_data[index++];
		b = m_data[index++];
		r = m_data[index++];
		g = m_data[index++];
		break;

	case AGBR:
		a = m_data[index++];
		g = m_data[index++];
		b = m_data[index++];
		r = m_data[index++];
		break;

	case AGRB:
		a = m_data[index++];
		g = m_data[index++];
		r = m_data[index++];
		b = m_data[index++];
		break;

		default:
			break;
	}

	rotate_bits(r, m_rotate_value);
	rotate_bits(g, m_rotate_value);
	rotate_bits(b, m_rotate_value);
	rotate_bits(a, m_rotate_value);

	if (m_swap_nibbles_checkbox->isChecked())
	{
		swap_nibbles(r);
		swap_nibbles(g);
		swap_nibbles(b);
		swap_nibbles(a);
	}

	if (m_reverse_bits_checkbox->isChecked())
	{
		reverse_bits(r);
		reverse_bits(g);
		reverse_bits(b);
		reverse_bits(a);
	}

	bool show_alpha = false;
	if (show_alpha)
	{
		r = a;
		b = a;
		g = a;
	}

	bool convertHSVtoRGB = false;
	if (convertHSVtoRGB)
	{
		HSVtoRGB(r, g, b, r, g, b);
	}

}

void TeslaWindow::rotate_bits(uint8_t& val, uint8_t rotate)
{
	if (rotate == 0u)
		return;

	uint8_t tmp = val >> rotate;
	tmp |= (val << (8u - rotate));

	val = tmp;
}

void TeslaWindow::swap_nibbles(uint8_t& val)
{
	uint8_t tmp = val >> 4u;
	tmp |= (val << 4u);

	val = tmp;
}

void TeslaWindow::reverse_bits(uint8_t& val)
{
	uint8_t tmp = 0u;
	for (uint8_t i = 0; i < 8u; ++i)
	{
		bool bit = (val & (1u << i)) != 0u;
		if (bit)
		{
			tmp |= (1u << (7u - i));
		}
		/*0 - 7 
		* 1 - 6
		* 2 - 5
		* 3 - 4
		* 4 - 3
		* 5 - 2
		* 6 - 1
		* 7 - 0
		*/
	}

	val = tmp;
}

void TeslaWindow::HSVtoRGB(float H, float S, float V, uint8_t& ro, uint8_t& go, uint8_t& bo)
{
	if (H > 360 || H < 0 || S>100 || S < 0 || V>100 || V < 0)
	{
		printf("");
		return;
	}
	float s = S / 100;
	float v = V / 100;
	float C = s * v;
	float X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	float m = v - C;
	float r, g, b;
	if (H >= 0 && H < 60) {
		r = C, g = X, b = 0;
	}
	else if (H >= 60 && H < 120) {
		r = X, g = C, b = 0;
	}
	else if (H >= 120 && H < 180) {
		r = 0, g = C, b = X;
	}
	else if (H >= 180 && H < 240) {
		r = 0, g = X, b = C;
	}
	else if (H >= 240 && H < 300) {
		r = X, g = 0, b = C;
	}
	else
	{
		r = C, g = 0, b = X;
	}
	int R = (r + m) * 255;
	int G = (g + m) * 255;
	int B = (b + m) * 255;

	if (R > 255)
		printf("");
	if (G > 255)
		printf("");
	if (B > 255)
		printf("");

	ro = R;
	go = G;
	bo = B;
	bo = B;
}