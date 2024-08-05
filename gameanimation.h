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

class Block
{
public:
	enum EBlockType
	{
		I_BLOCK,
		J_BLOCK,
		L_BLOCK,
		O_BLOCK,
		S_BLOCK,
		T_BLOCK,
		Z_BLOCK,
		NUM_BLOCKS
	};

	enum ERotationDirection
	{
		LEFT,
		RIGHT
	};

public:

	Block(EBlockType block_type)
		: m_block_type(block_type)
	{
	}

	virtual void rotate(ERotationDirection direction) = 0;
	virtual uint8_t block_size() const = 0;
	virtual uint8_t* block_matrix() = 0;
	virtual uint8_t block_value(int32_t x, int32_t y) const = 0;
	EBlockType block_type() const
	{
		return m_block_type;
	}

protected:
	EBlockType m_block_type = NUM_BLOCKS;
};

class GameAnimation : public DMDAnimation
{
public:
	enum ERotationDirection
	{
		LEFT,
		RIGHT
	};


	static const uint8_t BLOCKPIXELS = 3;
	static const uint8_t BLOCKSIZE = 3;
	static const uint8_t BLOCK[BLOCKPIXELS][BLOCKPIXELS];
	static const uint8_t EMPTY[BLOCKPIXELS][BLOCKPIXELS];

	static const uint8_t PLAYFIELD_WIDTH = 10;
	static const uint8_t PLAYFIELD_HEIGHT = 20;


	static const uint8_t EMPTY_SPACE = 0;
	static const uint8_t FILLED_SPACE = 1;
	uint8_t m_playfield[PLAYFIELD_WIDTH * PLAYFIELD_HEIGHT] = { 0 };
	uint8_t m_game_playfield[PLAYFIELD_WIDTH * PLAYFIELD_HEIGHT] = { 0 };

	GameAnimation();
	~GameAnimation();

	DMDFrame* current_frame() override;
	void button_pressed(DMDKeys::Button button) override;
	void button_released(DMDKeys::Button button) override;

private:
	bool detect_collision(int32_t x, int32_t y) const;
	bool is_movement_allowed(int32_t x) const;
	void rotate_if_allowed(Block::ERotationDirection direction);
	void correct_out_of_bounds();
	bool full_line(uint32_t line_nr) const;
	void check_lines();
	void store_block(uint8_t* playfield);
	void copy_playfield();
	void copy_block(int32_t dest_x, int32_t dest_y);
	void copy_empty(int32_t dest_x, int32_t dest_y);
private:
	Block* m_current_block = nullptr;
	int32_t m_block_x = 0;
	int32_t m_block_y = 0;
	DMDFrame m_game_frame;
};