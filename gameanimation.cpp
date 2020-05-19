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

const uint8_t GameAnimation::BLOCK[BLOCKPIXELS][BLOCKPIXELS] =
{ 
	{ 255, 255, 85 },
	{ 255, 170, 85 },
	{ 85,  85, 85 }
};

const uint8_t GameAnimation::EMPTY[BLOCKPIXELS][BLOCKPIXELS] =
{
	{ 85, 85, 85 },
	{ 85,  0, 85 },
	{ 85, 85, 85 }
};

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
		uint8_t* dest = m_block_matrix_buffers[1];
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
		assert(x >= 0 && x < GameAnimation::BLOCKSIZE);
		assert(y >= 0 && y < GameAnimation::BLOCKSIZE);
		uint8_t* block_matrix = m_block_matrix_buffers[0];
		return block_matrix[y * BLOCK_SIZE + x];
	}

protected:
	void set_block_layout(uint8_t* block_layout)
	{
		memcpy(m_block_layout, block_layout, BLOCK_SIZE * BLOCK_SIZE);
		memcpy(m_block_matrix_buffers[0], block_layout, BLOCK_SIZE * BLOCK_SIZE);
		memset(m_block_matrix_buffers[1], 0, BLOCK_SIZE * BLOCK_SIZE);
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

class L_Block : public Block3x3
{
public:
	L_Block()
		: Block3x3(L_BLOCK)
	{
		uint8_t block_layout[BLOCK_SIZE * BLOCK_SIZE] =
		{
			0, 1, 0,
			0, 1, 0,
			0, 1, 1
		};

		set_block_layout(block_layout);
	}
};

class S_Block : public Block3x3
{
public:
	S_Block()
		: Block3x3(S_BLOCK)
	{
		uint8_t block_layout[BLOCK_SIZE * BLOCK_SIZE] =
		{
			0, 1, 1,
			1, 1, 0,
			0, 0, 0
		};

		set_block_layout(block_layout);
	}
};

class T_Block : public Block3x3
{
public:
	T_Block()
		: Block3x3(T_BLOCK)
	{
		uint8_t block_layout[BLOCK_SIZE * BLOCK_SIZE] =
		{
			0, 0, 0,
			1, 1, 1,
			0, 1, 0
		};

		set_block_layout(block_layout);
	}
};

class Z_Block : public Block3x3
{
public:
	Z_Block()
		: Block3x3(Z_BLOCK)
	{
		uint8_t block_layout[BLOCK_SIZE * BLOCK_SIZE] =
		{
			1, 1, 0,
			0, 1, 1,
			0, 0, 0
		};

		set_block_layout(block_layout);
	}
};
/*
	 // I_BLOCK, 4x4
		J_BLOCK,
		L_BLOCK,
		//O_BLOCK, 2x2
		S_BLOCK,
		T_BLOCK,
		Z_BLOCK,
		UNKNOWN
*/

GameAnimation::GameAnimation()
{
	m_current_block = new J_Block();
	memset(m_game_playfield, 0, sizeof(m_game_playfield));
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
	case DMDKeys::LEFT_MAGNA_SAVE:
		m_current_block->rotate(Block::LEFT);
		break;
	case DMDKeys::RIGHT_MAGNA_SAVE:
		m_current_block->rotate(Block::RIGHT);
		break;
	}
}

void GameAnimation::button_released(DMDKeys::Button button)
{
	Q_UNUSED(button);
}

bool GameAnimation::detect_collision(int32_t y) const
{
	uint8_t block_size = m_current_block->block_size();

	for (uint8_t block_y = 0; block_y < block_size; ++block_y)
	{
		for (uint8_t block_x = 0; block_x < block_size; ++block_x)
		{
			uint8_t block_value = m_current_block->block_value(block_x, block_y);
			if (block_value == 0)
				continue;

			if (y + block_y >= PLAYFIELD_HEIGHT)
				return true;

			int8_t dest_x = m_block_x + block_x;
			int8_t dest_y = m_block_y + block_y;
			assert(dest_x >= 0 && dest_x < PLAYFIELD_WIDTH);
			assert(dest_y >= 0 && dest_y < PLAYFIELD_HEIGHT);
			if (m_game_playfield[dest_x + (dest_y * PLAYFIELD_WIDTH)] != 0)
				return true;
		}
	}

	return false;
}

void GameAnimation::store_block(uint8_t* playfield)
{
	uint8_t block_size = m_current_block->block_size();
	for (uint8_t block_y = 0; block_y < block_size; ++block_y)
	{
		for (uint8_t block_x = 0; block_x < block_size; ++block_x)
		{
			uint8_t block_value = m_current_block->block_value(block_x, block_y);
			if (block_value == 0)
				continue;

			int8_t dest_x = m_block_x + block_x;
			int8_t dest_y = m_block_y + block_y;
			assert(dest_x >= 0 && dest_x < PLAYFIELD_WIDTH);
			assert(dest_y >= 0 && dest_y < PLAYFIELD_HEIGHT);
			int32_t index = dest_x + (dest_y * PLAYFIELD_WIDTH);
			playfield[index] = block_value;
		}
	}
}

void GameAnimation::copy_playfield()
{
	m_game_frame.clear();

	static int framenumber = 0;

	framenumber++;

	if (framenumber % 20 == 0)
	{
		if (detect_collision(m_block_y + 1))
		{
			store_block(m_game_playfield);

			m_block_x = 0;
			m_block_y = 0;

			delete m_current_block;
			int32_t r = rand() % Block::NUM_BLOCKS;
			switch (r)
			{
			case Block::I_BLOCK: m_current_block = new J_Block(); break;
			case Block::J_BLOCK: m_current_block = new J_Block(); break;
			case Block::L_BLOCK: m_current_block = new L_Block(); break;
			case Block::O_BLOCK: m_current_block = new J_Block(); break;
			case Block::S_BLOCK: m_current_block = new S_Block(); break;
			case Block::T_BLOCK: m_current_block = new T_Block(); break;
			case Block::Z_BLOCK: m_current_block = new Z_Block(); break;
			}
		}
		else
		{
			m_block_y++;
		}
	}

	memcpy(m_playfield, m_game_playfield, PLAYFIELD_WIDTH * PLAYFIELD_HEIGHT);

	store_block(m_playfield);

	for (uint32_t y = 0; y < PLAYFIELD_HEIGHT; ++y)
	{
		for (uint32_t x = 0; x < PLAYFIELD_WIDTH; ++x)
		{
			if (m_playfield[x + (y * PLAYFIELD_WIDTH)] != 0)
				copy_block(x * 3, y * 3);
			else
				copy_empty(x * 3, y * 3);
		}
	}
}

void GameAnimation::copy_block(int32_t dest_x, int32_t dest_y)
{
	uint32_t start_x = 31; // for y axis
	for (uint32_t y = 0; y < BLOCKPIXELS; ++y)
	{
		for (uint32_t x = 0; x < BLOCKPIXELS; ++x)
		{
			uint32_t pixel_x = dest_y + y + 68;
			uint32_t pixel_y = 31 - dest_x - x;
			m_game_frame.set_pixel(pixel_x, pixel_y, BLOCK[x][y]);
		}
	}
}

void GameAnimation::copy_empty(int32_t dest_x, int32_t dest_y)
{
	for (uint32_t y = 0; y < BLOCKPIXELS; ++y)
	{
		for (uint32_t x = 0; x < BLOCKPIXELS; ++x)
		{
			uint32_t pixel_x = dest_y + y + 68;
			uint32_t pixel_y = 31 - dest_x - x;
			m_game_frame.set_pixel(pixel_x, pixel_y, EMPTY[x][y]);
		}
	}
}