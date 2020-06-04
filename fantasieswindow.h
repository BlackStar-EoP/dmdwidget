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
#include <QSet>
#include <QMap>
#include <QVector>
#include <set>
#include <assert.h>
#include <QFile>
#include <QTimer>
#include <QTextEdit>
#include <QLineEdit>

#include "fantasiesdmd.h"

#include "dmdframe.h"

class DMDAnimationEngine;
class QLabel;

class FantasiesWindow : public QWidget
{
	Q_OBJECT
public:
	FantasiesWindow(QWidget* parent, DMDAnimationEngine* animation_engine);
	~FantasiesWindow();

	void key_left();
	void key_right();
	void key_up();
	void key_down();

private:
	void initUI();
	void update_image();
	void paint_spans(const QImage& img);

private slots:
	void inc_img_button_clicked();
	void dec_img_button_clicked();

	void inc_img10_button_clicked();
	void dec_img10_button_clicked();

	void inc_img100_button_clicked();
	void dec_img100_button_clicked();

	void inc_img1000_button_clicked();
	void dec_img1000_button_clicked();

	void split_range_button_clicked();
	
	void debug_button_clicked();

	void auto_button_clicked();
	void stop_auto_button_clicked();
	void save_png_button_clicked();
	void create_animation_button_clicked();

	void addtext_button_clicked();

	void index_button_clicked();

private:



	QLabel* m_file_name_label = nullptr;
	QLabel* m_image_label = nullptr;
	QLabel* m_byte_label = nullptr;
	QLabel* m_spans_image_label = nullptr;
	QLabel* m_spans_text_label = nullptr;
	QLabel* m_parsed_image_label = nullptr;
	QLabel* m_parsed_text_label = nullptr;
	QTextEdit* m_text_edit = nullptr;
	QLineEdit* m_index_edit = nullptr;
	QString m_text_string;

	uint32_t m_current_file_nr = 0;

	DMDAnimationEngine* m_animation_engine = nullptr;
	FantasiesDMD m_fantasies_DMD;
	int32_t m_byte_index = 0;

	QTimer m_timer;
};
