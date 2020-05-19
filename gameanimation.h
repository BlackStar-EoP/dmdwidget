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

#include "dmdanimation.h"

#include "dmdframe.h"

class QImage;
class DMDFrame;
class QStringList;

class Block;

class GameAnimation : public DMDAnimation
{
public:
	GameAnimation();
	~GameAnimation();

	DMDFrame* current_frame() override;
	void button_pressed(DMDKeys::Button button) override;
	void button_released(DMDKeys::Button button) override;

private:
	void copy_playfield();
	void copy_block(int32_t dest_x, int32_t dest_y);

private:
	Block* m_current_block = nullptr;
	int32_t m_block_x = 0;
	int32_t m_block_y = 0;
	DMDFrame m_game_frame;
};