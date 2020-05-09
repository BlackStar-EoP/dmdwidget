#pragma once

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

#include <QWidget>

class DMDAnimationEngine;
class QLabel;

class ROMInspectWindow : public QWidget
{
	Q_OBJECT
public:
	ROMInspectWindow(QWidget* parent, DMDAnimationEngine* animation_engine);
	~ROMInspectWindow();
private:
	void initUI();
	void parse_image();
	void update_index();
	void inc_rom_index(uint32_t amount);
	void dec_rom_index(uint32_t amount);

private slots:
	void file_open_button_clicked();

	void reset_rom_index_button_clicked();
	void dec_rom_index_button_clicked();
	void dec_line_rom_index_button_clicked();
	void pgdn_rom_index_button_clicked();
	void pgup_rom_index_button_clicked();
	void inc_rom_index_button_clicked();
	void inc_line_rom_index_button_clicked();
	void inc_bank_rom_index_button_clicked();
	void dec_bank_rom_index_button_clicked();
	void scan_rle_index_button_clicked();
	void scan_sparse_index_button_clicked();
	void scan_normal_index_button_clicked();
	void bruteforce_table_button_clicked();

private:
	DMDAnimationEngine* m_animation_engine = nullptr;
	uint32_t m_rom_index = 0;

	static const uint32_t FRAME_IMAGE_HEIGHT = 128;
	static const uint32_t FRAME_SIZE = 128 * FRAME_IMAGE_HEIGHT / 8;
	QLabel* m_rom_index_label = nullptr;
	QLabel* m_file_name_label = nullptr;
	QLabel* m_image_label = nullptr;
	uint8_t* m_rom_content = nullptr;
	uint32_t m_rom_size = 0;
};