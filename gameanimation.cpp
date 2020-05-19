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

#include "gameanimation.h"

#include "dmdconfig.h"
#include "dmdframe.h"

#include <QDir>
#include <QImage>
#include <assert.h>

const uint8_t BLOCKPIXELS = 3;
const uint8_t BLOCKSIZE = 3;
const uint8_t BLOCK[BLOCKPIXELS][BLOCKPIXELS] = { { 255, 255, 85 },
												  { 255, 170, 85 }, 
												  {  85,  85, 85 }
												};

const uint8_t PLAYFIELD_WIDTH = 10;
const uint8_t PLAYFIELD_HEIGHT = 20;


const uint8_t EMPTY_SPACE = 0;
const uint8_t FILLED_SPACE = 1;
uint8_t playfield[PLAYFIELD_WIDTH][PLAYFIELD_HEIGHT]{ 0 };
uint8_t game_playfield[PLAYFIELD_WIDTH][PLAYFIELD_HEIGHT]{ 0 };

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
		UNKNOWN
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
	EBlockType m_block_type = UNKNOWN;
};

class Block3x3 : public Block
{
public:
	static const uint8_t BLOCK_SIZE = 3;

	Block3x3(EBlockType block_type)
	: Block(block_type)
	{
		m_block_matrix_buffers[0] = m_block_matrix1;
		m_block_matrix_buffers[1] = m_block_matrix2;
	}

	void rotate(ERotationDirection direction) override
	{
		uint8_t* source = m_block_matrix_buffers[0];
		uint8_t* dest = m_block_matrix_buffers[0];
		memset(dest, 0, BLOCK_SIZE * BLOCK_SIZE);

		if (direction == Block::LEFT)
		{
			dest[0] = source[2];
			dest[1] = source[5];
			dest[2] = source[8];
			dest[3] = source[1];
			dest[4] = source[4];
			dest[5] = source[7];
			dest[6] = source[0];
			dest[7] = source[3];
			dest[8] = source[6];
		}
		else if (direction == Block::RIGHT)
		{
			dest[0] = source[6];
			dest[1] = source[3];
			dest[2] = source[0];
			dest[3] = source[7];
			dest[4] = source[4];
			dest[5] = source[1];
			dest[6] = source[8];
			dest[7] = source[5];
			dest[8] = source[2];
		}

		m_block_matrix_buffers[0] = dest;
		m_block_matrix_buffers[1] = source;
	}

	uint8_t block_size() const override
	{
		return BLOCK_SIZE;
	}

	uint8_t* block_matrix() override
	{
		return m_block_matrix_buffers[0];
	}

	uint8_t block_value(int32_t x, int32_t y) const override
	{
		assert(x >= 0 && x < BLOCKSIZE);
		assert(y >= 0 && y < BLOCKSIZE);
		uint8_t* block_matrix = m_block_matrix_buffers[0];
		return block_matrix[y * BLOCK_SIZE + x];
	}

protected:
	void set_block_layout(uint8_t* block_layout)
	{
		memcpy(m_block_layout, block_layout, BLOCK_SIZE * BLOCK_SIZE);
	}

private:
	uint8_t m_block_layout[BLOCK_SIZE * BLOCK_SIZE];
	uint8_t* m_block_matrix_buffers[2];
	uint8_t m_block_matrix1[BLOCK_SIZE * BLOCK_SIZE];
	uint8_t m_block_matrix2[BLOCK_SIZE * BLOCK_SIZE];
};

class J_Block : public Block3x3
{
public:
	J_Block()
	: Block3x3(J_BLOCK)
	{
		uint8_t block_layout[BLOCK_SIZE * BLOCK_SIZE] = 
		{
			0, 1, 0,
			0, 1, 0,
			1, 1, 0
		};

		set_block_layout(block_layout);
	}
};

GameAnimation::GameAnimation()
{
	m_current_block = new J_Block();
	memset(game_playfield, 0, sizeof(game_playfield));
}

GameAnimation::~GameAnimation()
{
}

DMDFrame* GameAnimation::current_frame()
{
	copy_playfield();
	return &m_game_frame;
}

void GameAnimation::button_pressed(DMDKeys::Button button)
{
	switch (button)
	{
	case DMDKeys::LEFT_FLIPPER:
		m_block_x--;
		break;
	case DMDKeys::RIGHT_FLIPPER:
		m_block_x++;
		break;
	}
}

void GameAnimation::button_released(DMDKeys::Button button)
{
	Q_UNUSED(button);
}


void GameAnimation::copy_playfield()
{
	m_game_frame.clear();
	memcpy(playfield, game_playfield, PLAYFIELD_WIDTH * PLAYFIELD_HEIGHT);

	static int framenumber = 0;

	framenumber++;

	if (framenumber % 60 == 0)
	{
		m_block_y++;
	}

	uint8_t* block_matrix = m_current_block->block_matrix();
	uint8_t block_size = m_current_block->block_size();
	for (uint8_t block_y = 0; block_y < block_size; ++block_y)
	{
		for (uint8_t block_x = 0; block_y < block_size; ++block_y)
		{
			uint8_t block_value = m_current_block->block_value(block_x, block_y);
			if (block_value == 0)
				continue;

			uint8_t dest_x = m_block_x + block_x;
			uint8_t dest_y = m_block_y + block_y;
			playfield[dest_x][dest_y] = block_value;
		}
	}

	for (uint32_t y = 0; y < PLAYFIELD_HEIGHT; ++y)
	{
		for (uint32_t x = 0; x < PLAYFIELD_WIDTH; ++x)
		{
			if (playfield[x][y] != 0)
				//copy_block(x * 3 + 68, y * 3 + 1);
				copy_block(y * 3 + 68,  30 - (x * 3));
		}
	}
}

void GameAnimation::copy_block(int32_t dest_x, int32_t dest_y)
{
	for (uint32_t y = 0; y < BLOCKPIXELS; ++y)
	{
		for (uint32_t x = 0; x < BLOCKPIXELS; ++x)
		{
			m_game_frame.set_pixel(dest_x + x, dest_y - y, BLOCK[x][y]);
		}
	}
	

}