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

class DMDAnimationEngine;
class QLabel;

class FantasiesWindow : public QWidget
{
	Q_OBJECT
public:
	static const uint32_t FANTASIES_WIDTH = 320;
	static const uint32_t FANTASIES_HEIGHT = 34;

	static const uint32_t FANTASIES_DMD_WIDTH = 160;
	static const uint32_t FANTASIES_DMD_HEIGHT = 16;

public:
	FantasiesWindow(QWidget* parent, DMDAnimationEngine* animation_engine);
	~FantasiesWindow();
private:
	void initUI();
	bool is_column_candidate(uint32_t column);
	void update_image();

	void span_fix(const QImage& dmd);


private slots:
	void inc_img_button_clicked();
	void dec_img_button_clicked();

	void inc_img10_button_clicked();
	void dec_img10_button_clicked();

	void inc_img100_button_clicked();
	void dec_img100_button_clicked();
	void auto_button_clicked();
	void debug_button_clicked();

private:
	QLabel* m_file_name_label = nullptr;
	QLabel* m_image_label = nullptr;
	QLabel* m_dmd_label = nullptr;
	QLabel* m_dmd_span_label = nullptr;
	uint32_t m_current_file_nr = 0;

	uint8_t rawDMD[FANTASIES_DMD_WIDTH * FANTASIES_DMD_HEIGHT];
	DMDAnimationEngine* m_animation_engine = nullptr;
};
