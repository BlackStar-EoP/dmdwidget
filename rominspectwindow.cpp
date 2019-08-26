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

#include <assert.h>

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
	int32_t IMAGE_LABEL_HEIGHT = FRAME_IMAGE_HEIGHT * 4;
	m_image_label->setGeometry(10, 30, 128 * 4, IMAGE_LABEL_HEIGHT);
	int32_t BUTTONS_Y = IMAGE_LABEL_HEIGHT + 35;

	QPushButton* reset_rom_index_button = new QPushButton("Reset", this);
	reset_rom_index_button->setGeometry(10, BUTTONS_Y, 40, 20);
	connect(reset_rom_index_button, SIGNAL(clicked()), this, SLOT(reset_rom_index_button_clicked()));

	QPushButton* dec_rom_index_button = new QPushButton("<", this);
	dec_rom_index_button->setGeometry(50, BUTTONS_Y, 40, 20);
	connect(dec_rom_index_button, SIGNAL(clicked()), this, SLOT(dec_rom_index_button_clicked()));

	QPushButton* dec_line_rom_index_button = new QPushButton("<-", this);
	dec_line_rom_index_button->setGeometry(90, BUTTONS_Y, 40, 20);
	connect(dec_line_rom_index_button, SIGNAL(clicked()), this, SLOT(dec_line_rom_index_button_clicked()));
	
	QPushButton* pgdn_rom_index_button = new QPushButton("<<", this);
	pgdn_rom_index_button->setGeometry(130, BUTTONS_Y, 40, 20);
	connect(pgdn_rom_index_button, SIGNAL(clicked()), this, SLOT(pgdn_rom_index_button_clicked()));

	QPushButton* pgup_rom_index_button = new QPushButton(">>", this);
	pgup_rom_index_button->setGeometry(170, BUTTONS_Y, 40, 20);
	connect(pgup_rom_index_button, SIGNAL(clicked()), this, SLOT(pgup_rom_index_button_clicked()));

	QPushButton* inc_line_rom_index_button = new QPushButton("->", this);
	inc_line_rom_index_button->setGeometry(210, BUTTONS_Y, 40, 20);
	connect(inc_line_rom_index_button, SIGNAL(clicked()), this, SLOT(inc_line_rom_index_button_clicked()));

	QPushButton* inc_rom_index_button = new QPushButton(">", this);
	inc_rom_index_button->setGeometry(250, BUTTONS_Y, 40, 20);
	connect(inc_rom_index_button, SIGNAL(clicked()), this, SLOT(inc_rom_index_button_clicked()));

	QPushButton* inc_bank_index_button = new QPushButton("BNK+", this);
	inc_bank_index_button->setGeometry(290, BUTTONS_Y, 40, 20);
	connect(inc_bank_index_button, SIGNAL(clicked()), this, SLOT(inc_bank_rom_index_button_clicked()));

	QPushButton* dec_bank_index_button = new QPushButton("BNK-", this);
	dec_bank_index_button->setGeometry(330, BUTTONS_Y, 40, 20);
	connect(dec_bank_index_button, SIGNAL(clicked()), this, SLOT(dec_bank_rom_index_button_clicked()));

	QPushButton* scan_index_button = new QPushButton("SCN02", this);
	scan_index_button->setGeometry(370, BUTTONS_Y, 40, 20);
	connect(scan_index_button, SIGNAL(clicked()), this, SLOT(scan_rle_index_button_clicked()));

	QPushButton* scan_sparse_index_button = new QPushButton("SCN04", this);
	scan_sparse_index_button->setGeometry(410, BUTTONS_Y, 40, 20);
	connect(scan_sparse_index_button, SIGNAL(clicked()), this, SLOT(scan_sparse_index_button_clicked()));

	QPushButton* scan_normal_index_button = new QPushButton("SCN00", this);
	scan_normal_index_button->setGeometry(450, BUTTONS_Y, 40, 20);
	connect(scan_normal_index_button, SIGNAL(clicked()), this, SLOT(scan_normal_index_button_clicked()));

	QPushButton* bruteforce_table_button = new QPushButton("Brute force find table", this);
	bruteforce_table_button->setGeometry(490, BUTTONS_Y, 140, 20);
	connect(bruteforce_table_button, SIGNAL(clicked()), this, SLOT(bruteforce_table_button_clicked()));

	QLabel* rom_index = new QLabel("ROM index:", this);
	rom_index->setGeometry(10, BUTTONS_Y + 25, 120, 20);

	m_rom_index_label = new QLabel(this);
	m_rom_index_label->setGeometry(130, BUTTONS_Y + 25, 120, 20);
}

uint16_t U16LE_to_U16BE(uint16_t LE)
{
	return LE << 8 | LE >> 8;
}

void decode_sparse_frame(uint8_t* data, uint8_t* dst)
{
	memset(dst, 0x00, 512);
	bool decoding = true;
	int32_t cursor_position = 0;
	uint32_t bytes_copied = 0;
	while (decoding)
	{
		uint8_t data_length = *data; data++;
		if (data_length == 0)
		{
			// done
			break;
		}
		int8_t move = *(data + data_length);
		printf("");
		if (move < 0)
		{
			// move cannot be higher than 127
			printf("");
		}
		cursor_position += (move * 2) + 1;
		for (uint8_t i = 0; i < data_length + 17; ++i)
		{
			dst[cursor_position] = *data++;
			cursor_position++;
			bytes_copied++;
		}
		
		// hack
		data += 2;
		data_length = 144;
		cursor_position += 32;
		for (uint8_t i = 0; i < data_length; ++i)
		{
			dst[cursor_position] = data[i];
			cursor_position++;
			bytes_copied++;
		}

		return;
	}

	//; --------------------------------------------------------
	//	;
	//; void frame_decode_sparse_asm(void *src);
	//;
	//; X = pointer to source image data
	//	;
	//; --------------------------------------------------------
	//	; A sparse image is used when the image data consists of many
	//	; strides of zero bytes.The image is encoded as a set of
	//	; <len, data[], move> triples.len says how long the data
	//	; array is.move says how to adjust the output pointer
	//	; to set up for the next triple.The zero bytes are implied,
	//	; and are written by a bulk zero of the entire page prior to
	//	; copying.
	//	.globl _frame_decode_sparse_asm
	//	_frame_decode_sparse_asm :
	//pshs	u

	//	; First, clear the output page.
	//	tfr	x, u
	//	ldx	#DMD_LOW_BASE
	//	jsr	_dmd_clean_page
	//	tfr	u, x

	//	ldu	#DMD_LOW_BASE

	//	sparse_loop :
	//; First byte is literal data count, in 16 - bit words.
	//	; If zero, this means no more blocks
	//	lda, x +
	//	beq	sparse_done
	//	sta	*m0

	//	; Second byte of block is skip count.
	//	; The cursor is moved forward this many BYTES
	//	; before copying the literals to memory.Note that this
	//	; count should not exceed 127, because leau treats the
	//	; offset as SIGNED.
	//	ldb, x +
	//	leau	b, u

	//	; Remaining bytes of block are the data
	//	sparse_block_loop :
	//ldd, x++
	//	std, u++
	//	dec	*m0
	//	bne	sparse_block_loop

	//	; On to the next block
	//	bra	sparse_loop

	//	sparse_done :
	//puls	u, pc

}

void decode_rle_frame(uint8_t* data, uint8_t* dst)
{
	//bool decoding = true;
	//uint32_t bytes_written = 0;
	//while (decoding)
	//{
	//	uint8_t 
	//}
	
	
/*
#define SAFE_DECODE
	uint16_t *src = (uint16_t*)data;
	uint32_t bytes_written = 0;
	while (bytes_written < 512)
	{
		uint16_t val = U16LE_to_U16BE(*src++);
		if ((val & 0xFF00) == 0xA800) // BE ??
		{
			uint8_t words = val & 0xFF;
			uint8_t repeater = *((uint8_t*) src);
			uint16_t repeated_word = repeater | ((uint16_t)repeater << 8);
			src = (uint16_t*)((uint8_t*)src + 1);
			while (words > 0)
			{
				*dst++ = repeated_word;
				words--;
				bytes_written += 2;
#ifdef SAFE_DECODE
			//	assert(words == 0);
				if (bytes_written == 512)
					return;
#endif
			}
		}
		else
		{
			*dst++ = val;
			bytes_written += 2;
		}
	}


	// Backup of the old version, I'm not sure if this is LE / BE
/*

while (bytes_written < 512)
{
uint16_t val = *src++;
if ((val & 0xFF00) == 0xA800)
{
uint8_t words = val & 0xFF;
uint8_t repeater = *((uint8_t*) src);
uint16_t repeated_word = repeater | ((uint16_t)repeater << 8);
src = (uint16_t*)((uint8_t*)src + 1);
while (words > 0)
{
*dst++ = repeated_word;
words--;
bytes_written += 2;
#ifdef SAFE_DECODE
//	assert(words == 0);
if (bytes_written == 512)
return;
#endif
}
}
else
{
*dst++ = val;
bytes_written += 2;
}
}

*/
}

void ROMInspectWindow::parse_image()
{
	if (m_rom_content == nullptr)
		return;

	uint8_t dest_frame[FRAME_SIZE];
	uint8_t type = m_rom_content[m_rom_index] & ~0x01;
	uint8_t* frame_start = m_rom_content + m_rom_index + 1;
	if (false)
	{
		if (type == 2) // RLE encoded
		{
			decode_rle_frame(frame_start, dest_frame);
		}
		else if (type == 4) // Sparse encoded
		{
			decode_sparse_frame(frame_start, dest_frame);
		}
		else /*if (type == 0) // Normal, block copy data */
		{
			memcpy(dest_frame, frame_start, FRAME_SIZE);
		}
	}
	else
	{
		memcpy(dest_frame, frame_start, FRAME_SIZE);
	}
	QImage image(128, FRAME_IMAGE_HEIGHT, QImage::Format_RGBA8888);
	image.fill(Qt::black);

	uint32_t current_x = 0;
	uint32_t current_y = 0;

	// Convert it to QImage
	for (uint32_t byte_pos = 0; byte_pos < FRAME_SIZE; ++byte_pos)
	{
		uint8_t current_byte = dest_frame[byte_pos];
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

	m_image_label->setPixmap(QPixmap::fromImage(image).scaled(128 * 4, FRAME_IMAGE_HEIGHT * 4));
}

void ROMInspectWindow::update_index()
{
	QString index_str = QString::number(m_rom_index);
	index_str += " (0x";
	index_str += QString::number(m_rom_index, 16).toUpper();
	index_str += ")";
	m_rom_index_label->setText(index_str);
}

void ROMInspectWindow::inc_rom_index(uint32_t amount)
{
	if (m_rom_index + amount + FRAME_SIZE < m_rom_size)
		m_rom_index += amount;
	else
		m_rom_index = m_rom_size - FRAME_SIZE;

	update_index();
	parse_image();
}

void ROMInspectWindow::dec_rom_index(uint32_t amount)
{
	if (m_rom_index >= amount)
		m_rom_index -= amount;
	else
		m_rom_index = 0;

	update_index();
	parse_image();
}

void ROMInspectWindow::scan_rle_index_button_clicked()
{
//	QVector<uint32_t> indices;
//	for (uint32_t i = 0; i < m_rom_size; ++i)
//	{
///*
//type = data[0];
//frame_decode (data + 1, type & ~0x1);
//*/
//		uint8_t type = m_rom_content[i];
//		if ((type & ~0x1) == 2)
//		{
//			indices.push_back(i);
//		}
//	}
//
//	printf("");

	for (uint32_t i = m_rom_index+1; i < m_rom_size; ++i)
	{
		uint8_t type = m_rom_content[i];
		if ((type & ~0x1) == 2)
		{
			// Brute force this stuff to see if we found a type 2 RLE image
			for (uint rle = i; rle < (i+512); ++rle)
			{
				if (m_rom_content[rle] == 0xA8)
				{
					m_rom_index = i;

					update_index();
					parse_image();
					return;
				}
			}
		}
	}
}

void ROMInspectWindow::scan_sparse_index_button_clicked()
{
#define GRAIL_HACK
#ifdef GRAIL_HACK
	m_rom_index = 0x52c8e; // Hardcoded grail sparse image
	update_index();
	parse_image();
#else
	for (uint32_t i = m_rom_index + 1; i < m_rom_size; ++i)
	{
		uint8_t type = m_rom_content[i];
		if ((type & ~0x1) == 4)
		{
			m_rom_index = i;

			update_index();
			parse_image();
			return;
		}
	}
#endif
}

void ROMInspectWindow::scan_normal_index_button_clicked()
{
	for (uint32_t i = m_rom_index + 1; i < m_rom_size; ++i)
	{
		uint8_t type = m_rom_content[i];
		if ((type & ~0x1) == 0)
		{
			m_rom_index = i;

			update_index();
			parse_image();
			return;
		}
	}
}

void ROMInspectWindow::file_open_button_clicked()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open ROM"), "", tr("ROM (*.rom)"));
	if (filename != "")
	{
		delete[] m_rom_content;
		m_rom_content = nullptr;
		m_rom_size = 0;
		QFile file(filename);
		if (file.open(QIODevice::ReadOnly))
		{
			m_rom_content = new uint8_t[file.size()];
			file.read(reinterpret_cast<char*>(m_rom_content), file.size());
			m_file_name_label->setText(filename);

			m_rom_index = 0;
			m_rom_size = file.size();
			update_index();
			parse_image();
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
	dec_rom_index(1);
}

void ROMInspectWindow::dec_line_rom_index_button_clicked()
{
	const uint32_t LINE_WIDTH = 128 / 8;
	dec_rom_index(LINE_WIDTH);
}

void ROMInspectWindow::pgdn_rom_index_button_clicked()
{
	dec_rom_index(FRAME_SIZE);
}

void ROMInspectWindow::pgup_rom_index_button_clicked()
{
	inc_rom_index(FRAME_SIZE);
}

void ROMInspectWindow::inc_line_rom_index_button_clicked()
{
	const uint32_t LINE_WIDTH = 128 / 8;
	inc_rom_index(LINE_WIDTH);
}

void ROMInspectWindow::inc_rom_index_button_clicked()
{
	inc_rom_index(1);
}

void ROMInspectWindow::inc_bank_rom_index_button_clicked()
{
	const uint32_t BANK_SIZE = 65536;
	inc_rom_index(BANK_SIZE);
}

void ROMInspectWindow::dec_bank_rom_index_button_clicked()
{
	const uint32_t BANK_SIZE = 65536;
	dec_rom_index(BANK_SIZE);
}

void ROMInspectWindow::bruteforce_table_button_clicked()
{
	QVector<uint32_t> indices;
	for (uint32_t i = 0; i < m_rom_size; ++i)
	{
		uint8_t type = m_rom_content[i];
		if ((type & ~0x1) == 2)
		{
			// Brute force this stuff to see if we found a type 2 RLE image
			for (uint rle = i; rle < (i + 512); ++rle)
			{
				if (m_rom_content[rle] == 0xA8)
				{
					indices.push_back(i);
					break;
				}
			}
		}
	}

	QMap<uint32_t, QVector<uint32_t>> locations;
	for (uint32_t index : indices)
	{
		uint16_t index16 = index % 65536;
		uint8_t* index_data = (uint8_t*)&index16;

		for (uint32_t i = 0; i < m_rom_size - 1; ++i)
		{
			if (m_rom_content[i + 1] == index_data[0] && m_rom_content[i] == index_data[1])
			{
				locations[index].push_back(i);
			}
		}

		//uint16_t index16_BE = U16LE_to_U16BE(index16);
		printf("");
	}

	printf("");
}