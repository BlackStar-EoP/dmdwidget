#include "dmdwidget.h"

#include <processthreadsapi.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <psapi.h>

#include <QPushButton>
#include <QLabel>
#include <QImage>
#include <QPainter>
#include <QRect>
#include <QPixmap>
#include <QMessageBox>

int DMDWIDTH = 128;
int DMDHEIGHT = 32;

DMDWidget::DMDWidget(QWidget* parent)
: QWidget(parent)
{
	QPushButton* findFX3Button = new QPushButton("Find FX3", this);
	findFX3Button->setGeometry(10, 10, 120, 20);
	connect(findFX3Button, SIGNAL(clicked()), this, SLOT(findFX3Button_clicked()));

	m_FX3_proc_id_label = new QLabel(this);
	m_FX3_proc_id_label->setGeometry(10, 35, 120, 20);

	QPushButton* openFX3Button = new QPushButton("Open FX3", this);
	openFX3Button->setGeometry(10, 60, 120, 20);
	connect(openFX3Button, SIGNAL(clicked()), this, SLOT(openFX3Button_clicked()));

	m_FX3_proc_open_label = new QLabel(this);
	m_FX3_proc_open_label->setGeometry(10, 85, 120, 20);

	QPushButton* findDMDMemoryOffsetButton = new QPushButton("Open FX3", this);
	findDMDMemoryOffsetButton->setGeometry(10, 110, 120, 20);
	connect(findDMDMemoryOffsetButton, SIGNAL(clicked()), this, SLOT(findDMDMemoryOffsetButton_clicked()));

	m_FX3_find_memory_offset_label = new QLabel(this);
	m_FX3_find_memory_offset_label->setGeometry(10, 135, 120, 20);

	m_DMD_label = new QLabel(this);
	m_DMD_label->setGeometry(10, 160, DMDWIDTH, DMDHEIGHT);

	foundBlockCountLabel = new QLabel("Found 0 blocks", this);
	foundBlockCountLabel->setGeometry(10, 300, 300, 20);
	



	increaseBlockNumberButton = new QPushButton("Inc. block number", this);
	increaseBlockNumberButton->setGeometry(10, 325, 100, 20);
	connect(increaseBlockNumberButton, SIGNAL(clicked()), this, SLOT(increaseBlockNumberButton_clicked()));
	
	decreaseBlockNumberButton = new QPushButton("Dec. block number", this);
	decreaseBlockNumberButton->setGeometry(115, 325, 100, 20);
	connect(decreaseBlockNumberButton, SIGNAL(clicked()), this, SLOT(decreaseBlockNumberButton_clicked()));

	byteOffsetLabel = new QLabel("Byte offset = 0", this);
	byteOffsetLabel->setGeometry(10, 350, 150, 20);
	increaseByteOffsetButton = new QPushButton("Inc. byte offset", this);
	increaseByteOffsetButton->setGeometry(10, 375, 100, 20);
	connect(increaseByteOffsetButton, SIGNAL(clicked()), this, SLOT(increaseByteOffsetButton_clicked()));
	decreaseByteOffsetButton = new QPushButton("Dec. byte offset", this);
	decreaseByteOffsetButton->setGeometry(115, 375, 100, 20);
	connect(decreaseByteOffsetButton, SIGNAL(clicked()), this, SLOT(decreaseByteOffsetButton_clicked()));

	increaseByteOffsetIntButton = new QPushButton("Inc. 4 byte offset", this);
	increaseByteOffsetIntButton->setGeometry(220, 375, 100, 20);
	connect(increaseByteOffsetIntButton, SIGNAL(clicked()), this, SLOT(increaseByteOffsetIntButton_clicked()));
	decreaseByteOffsetIntButton = new QPushButton("Dec. 4 byte offset", this);
	decreaseByteOffsetIntButton->setGeometry(325, 375, 100, 20);
	connect(decreaseByteOffsetIntButton, SIGNAL(clicked()), this, SLOT(decreaseByteOffsetIntButton_clicked()));

	increaseByteOffsetPageButton = new QPushButton("Inc. page byte offset", this);
	increaseByteOffsetPageButton->setGeometry(430, 375, 100, 20);
	connect(increaseByteOffsetPageButton, SIGNAL(clicked()), this, SLOT(increaseByteOffsetPageButton_clicked()));
	decreaseByteOffsetPageButton = new QPushButton("Dec. page byte offset", this);
	decreaseByteOffsetPageButton->setGeometry(535, 375, 100, 20);
	connect(decreaseByteOffsetPageButton, SIGNAL(clicked()), this, SLOT(decreaseByteOffsetPageButton_clicked()));

	widthLabel = new QLabel("Width : 128", this);
	widthLabel->setGeometry(10, 400, 100, 20);
	increaseWidthButton = new QPushButton("Inc. width", this);
	increaseWidthButton->setGeometry(10, 425, 100, 20);
	connect(increaseWidthButton, SIGNAL(clicked()), this, SLOT(increaseWidthButton_clicked()));
	decreaseWidthButton = new QPushButton("Dec. width", this);
	decreaseWidthButton->setGeometry(115, 425, 100, 20);
	connect(decreaseWidthButton, SIGNAL(clicked()), this, SLOT(decreaseWidthButton_clicked()));

	increaseWidth10Button = new QPushButton("Inc. 10 width", this);
	increaseWidth10Button->setGeometry(220, 425, 100, 20);
	connect(increaseWidth10Button, SIGNAL(clicked()), this, SLOT(increaseWidth10Button_clicked()));
	decreaseWidth10Button = new QPushButton("Dec. 10 width", this);
	decreaseWidth10Button->setGeometry(325, 425, 100, 20);
	connect(decreaseWidth10Button, SIGNAL(clicked()), this, SLOT(decreaseWidth10Button_clicked()));

	heightLabel = new QLabel("Height : 32", this);
	heightLabel->setGeometry(10, 450, 100, 20);
	increaseHeightButton = new QPushButton("Inc. height", this);
	increaseHeightButton->setGeometry(10, 475, 100, 20);
	connect(increaseHeightButton, SIGNAL(clicked()), this, SLOT(increaseHeightButton_clicked()));
	decreaseHeightButton = new QPushButton("Dec. height", this);
	decreaseHeightButton->setGeometry(115, 475, 100, 20);
	connect(decreaseHeightButton, SIGNAL(clicked()), this, SLOT(decreaseHeightButton_clicked()));

	increaseHeight10Button = new QPushButton("Inc. 10 height", this);
	increaseHeight10Button->setGeometry(220, 475, 100, 20);
	connect(increaseHeight10Button, SIGNAL(clicked()), this, SLOT(increaseHeight10Button_clicked()));
	decreaseHeight10Button = new QPushButton("Dec. height", this);
	decreaseHeight10Button->setGeometry(325, 475, 100, 20);
	connect(decreaseHeight10Button, SIGNAL(clicked()), this, SLOT(decreaseHeight10Button_clicked()));


	m_width = DMDWIDTH;
	m_height = DMDHEIGHT;
}

void DMDWidget::findFX3Button_clicked()
{
	if (findFX3())
	{
		m_FX3_proc_id_label->setText(QString("Found, ID = ") + QString::number(m_FX3_process_id));
	}
	else
	{
		m_FX3_proc_id_label->setText(QString("Not found"));
	}
	
}

void DMDWidget::openFX3Button_clicked()
{
	openFX3();
}

void DMDWidget::increaseBlockNumberButton_clicked()
{
	if (m_DMD_blocks.size() == 0)
	{
		foundBlockCountLabel->setText("Found 0 blocks");
		return;
	}

	if (m_current_block == m_DMD_blocks.size() - 1)
	{
		return;
	}

	m_current_block++;
	m_byte_offset = m_DMD_block_offsets[m_current_block];
	QString t = QString("Block ") + QString::number(m_current_block);
	t += QString(" of ") + QString::number(m_DMD_blocks.size() - 1);
	t += QString(" size = ") + QString::number(m_DMD_block_sizes[m_current_block]);
	foundBlockCountLabel->setText(t);
	byteOffsetLabel->setText(QString("Byte offset = %1 (%2)").arg(m_byte_offset).arg(m_byte_offset - m_DMD_block_offsets[m_current_block]));
	updateImage();
}

void DMDWidget::decreaseBlockNumberButton_clicked()
{
	if (m_DMD_blocks.size() == 0)
	{
		foundBlockCountLabel->setText("Found 0 blocks");
		return;
	}

	if (m_current_block == 0)
	{
		return;
	}

	m_current_block--;
	m_byte_offset = m_DMD_block_offsets[m_current_block];
	QString t = QString("Block ") + QString::number(m_current_block);
	t += QString(" of ") + QString::number(m_DMD_blocks.size() - 1);
	t += QString(" size = ") + QString::number(m_DMD_block_sizes[m_current_block]);
	foundBlockCountLabel->setText(t);
	byteOffsetLabel->setText(QString("Byte offset = %1 (%2)").arg(m_byte_offset).arg(m_byte_offset - m_DMD_block_offsets[m_current_block]));
	updateImage();
}

void DMDWidget::increaseByteOffsetButton_clicked()
{
	if (m_DMD_blocks.size() == 0)
		return;

	m_byte_offset++;
	byteOffsetLabel->setText(QString("Byte offset = %1 (%2)").arg(m_byte_offset).arg(m_byte_offset - m_DMD_block_offsets[m_current_block]));
	updateImage();
}

void DMDWidget::decreaseByteOffsetButton_clicked()
{
	if (m_DMD_blocks.size() == 0)
		return;
	
	if (m_byte_offset == 0)
		return;

	m_byte_offset--;
	byteOffsetLabel->setText(QString("Byte offset = %1 (%2)").arg(m_byte_offset).arg(m_byte_offset - m_DMD_block_offsets[m_current_block]));
	updateImage();
}

void DMDWidget::increaseByteOffsetIntButton_clicked()
{
	if (m_DMD_blocks.size() == 0)
		return;

	m_byte_offset += 4;
	byteOffsetLabel->setText(QString("Byte offset = %1 (%2)").arg(m_byte_offset).arg(m_byte_offset - m_DMD_block_offsets[m_current_block]));
	updateImage();
}

void DMDWidget::decreaseByteOffsetIntButton_clicked()
{
	if (m_DMD_blocks.size() == 0)
		return;

	if (m_byte_offset < 4)
		return;

	m_byte_offset -= 4;
	byteOffsetLabel->setText(QString("Byte offset = %1 (%2)").arg(m_byte_offset).arg(m_byte_offset - m_DMD_block_offsets[m_current_block]));
	updateImage();
}

void DMDWidget::increaseByteOffsetPageButton_clicked()
{
	if (m_DMD_blocks.size() == 0)
		return;

	m_byte_offset += (m_width * m_height * 4);
	byteOffsetLabel->setText(QString("Byte offset = %1 (%2)").arg(m_byte_offset).arg(m_byte_offset - m_DMD_block_offsets[m_current_block]));
	updateImage();
}

void DMDWidget::decreaseByteOffsetPageButton_clicked()
{
	if (m_DMD_blocks.size() == 0)
		return;

	if (m_byte_offset < (m_width * m_height * 4))
	{
		m_byte_offset = 0;
	}
	else
	{
		m_byte_offset -= 4;
		byteOffsetLabel->setText(QString("Byte offset = %1 (%2)").arg(m_byte_offset).arg(m_byte_offset - m_DMD_block_offsets[m_current_block]));
	}

	updateImage();
}

void DMDWidget::increaseWidthButton_clicked()
{
	m_width++;
	widthLabel->setText(QString("Width : %1").arg(m_width));
	updateImage();
}

void DMDWidget::decreaseWidthButton_clicked()
{
	if (m_width == 1)
		return;
	m_width--;

	widthLabel->setText(QString("Width : %1").arg(m_width));
	updateImage();
}

void DMDWidget::increaseHeightButton_clicked()
{
	m_height++;
	heightLabel->setText(QString("Height : %1").arg(m_height));
	updateImage();
}

void DMDWidget::decreaseHeightButton_clicked()
{
	if (m_height == 1)
		return;
	m_height--;

	heightLabel->setText(QString("Height : %1").arg(m_height));
	updateImage();
}




void DMDWidget::increaseWidth10Button_clicked()
{
	m_width += 10;
	widthLabel->setText(QString("Width : %1").arg(m_width));
	updateImage();
}

void DMDWidget::decreaseWidth10Button_clicked()
{
	if (m_width < 11)
		return;
	m_width -= 10;

	widthLabel->setText(QString("Width : %1").arg(m_width));
	updateImage();
}

void DMDWidget::increaseHeight10Button_clicked()
{
	m_height += 10;
	heightLabel->setText(QString("Height : %1").arg(m_height));
	updateImage();
}

void DMDWidget::decreaseHeight10Button_clicked()
{
	if (m_height < 11)
		return;
	m_height -= 10;

	heightLabel->setText(QString("Height : %1").arg(m_height));
	updateImage();
}










bool DMDWidget::findFX3()
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	m_FX3_process_id = 0;
	if (Process32First(snapshot, &entry) == TRUE)
	{
		do
		{
			if (m_executable_name == entry.szExeFile)
			{
				m_FX3_process_id = entry.th32ProcessID;
				CloseHandle(snapshot);
				return true;
			}
		} while (Process32Next(snapshot, &entry) == TRUE);
	}

	CloseHandle(snapshot);
	return false;
}

bool DMDWidget::openFX3()
{
	m_FX3_process_handle = nullptr;

	if (m_FX3_process_id == 0)
	{
		m_FX3_proc_open_label->setText("Cannot open, proc id 0");
		return false;
	}
		
	m_FX3_process_handle = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, m_FX3_process_id);
	if (m_FX3_process_handle == nullptr)
	{
		m_FX3_proc_open_label->setText("Cannot open, OpenProcess failed!");
		return false;
	}

	m_FX3_proc_open_label->setText("Process opened!");
	return true;
}

bool DMDWidget::findDMDMemoryOffsetButton_clicked()
{
#if 0
	std::vector<uint8_t> buffer;

	SIZE_T bytes_read;
	uint32_t READSIZE = 20000000;
	buffer.resize(READSIZE);
	ReadProcessMemory(m_FX3_process_handle, (void*) 0x00100000, &buffer[0], READSIZE, &bytes_read);

	buffer.resize(bytes_read);

	uint64_t addressDMD = 0;
	bool found = findDMDMemoryOffset(&buffer[0], bytes_read, &addressDMD);
	if (found)
	{
	}

#endif

	//HEAPENTRY32 heapEntry;
	//heapEntry.dwSize = sizeof(HEAPENTRY32);
	//if (Heap32First()



	//	SIZE_T VirtualQueryEx(
	//		HANDLE                    hProcess,
	//		LPCVOID                   lpAddress,
	//		PMEMORY_BASIC_INFORMATION lpBuffer,
	//		SIZE_T                    dwLength
	//	);

	// brute force all
#if 0
	unsigned char *p = NULL;
	MEMORY_BASIC_INFORMATION info;

	int count = 0;
	int totalmem = 0;
	for (p = NULL;
		VirtualQueryEx(m_FX3_process_handle, p, &info, sizeof(info)) == sizeof(info);
		p += info.RegionSize)
	{
		std::vector<uint8_t> buffer;

		if (info.State == MEM_COMMIT &&
			(info.Type == MEM_MAPPED || info.Type == MEM_PRIVATE))
		{
			SIZE_T bytes_read;
			buffer.resize(info.RegionSize);
			ReadProcessMemory(m_FX3_process_handle, p, &buffer[0], info.RegionSize, &bytes_read);
			if (bytes_read == 0)
				continue;

			++count;
			totalmem += bytes_read;
			buffer.resize(bytes_read);

			uint64_t addressDMD = 0;
			bool found = findDMDMemoryOffset(&buffer[0], bytes_read, &addressDMD);
			if (found)
			{
			}
		}
	}
#endif
// using module
#if 1
	if (m_FX3_process_id == 0 || m_FX3_process_handle == nullptr)
	{
		m_FX3_find_memory_offset_label->setText("Cannot access Pinball FX3 memory");
		return false;
	}

	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	//  Take a snapshot of all modules in the specified process. 
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_FX3_process_id);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	//  Set the size of the structure before using it. 
	me32.dwSize = sizeof(MODULEENTRY32);

	//  Retrieve information about the first module, 
	//  and exit if unsuccessful 
	if (!Module32First(hModuleSnap, &me32))
	{
		CloseHandle(hModuleSnap);     // Must clean up the snapshot object! 
		return false;
	}

	//  Now walk the module list of the process, 
	//  and display information about each module 
	do
	{
		if (m_executable_name == me32.szModule)
		{
			std::vector<uint8_t> buffer;

			SIZE_T bytes_read;
			uint32_t READSIZE = 20000000;
			buffer.resize(READSIZE);
			ReadProcessMemory(m_FX3_process_handle, (void*)me32.modBaseAddr, &buffer[0], READSIZE, &bytes_read);

			buffer.resize(bytes_read);

			bool found = findDMDMemoryOffset(&buffer[0], bytes_read);
			if (found)
			{
			}
		}
	} while (Module32Next(hModuleSnap, &me32));
#endif
	return true;
}

bool DMDWidget::findDMDMemoryOffset(uint8_t* buffer, SIZE_T buffer_size, uint64_t* dmdAddress)
{
	uint8_t DMDSIGNATURE[] = { 0x8B, 0x81, 0xFF, 0xFF, 0xFF, 0xFF, 0x89, 0x44, 0x24, 0xFF, 0x8B, 0x81, 0xFF, 0xFF, 0xFF, 0xFF, 0x89, 0x44, 0x24, 0xFF, 0xA1 };
	uint32_t DMDSIGNATURELENGTH = sizeof(DMDSIGNATURE);

	//uint8_t* dmdBlock = new uint8_t[buffer_size];
	//memcpy(dmdBlock, buffer, buffer_size);
	//m_DMD_blocks.push_back(dmdBlock);
	//m_DMD_block_offsets.push_back(0);
	//m_DMD_block_sizes.push_back(buffer_size);
	//return true;


	for (SIZE_T i = 0; i < buffer_size - DMDSIGNATURELENGTH; ++i)
	{
		if (buffer[i] != DMDSIGNATURE[0])
			continue;

		bool match = true;
		for (uint32_t test = 0; test < DMDSIGNATURELENGTH; ++test)
		{
			if (DMDSIGNATURE[test] == 0xFF)
				continue;

			if (buffer[i + test] != DMDSIGNATURE[test])
			{
				match = false;
				break;
			}
		}

		if (match)
		{
			uint8_t* dmdBlock = new uint8_t[buffer_size];
			memcpy(dmdBlock, buffer, buffer_size);
			m_DMD_blocks.push_back(dmdBlock);
			m_DMD_block_offsets.push_back(i);
			m_DMD_block_sizes.push_back(buffer_size);
			return true;
		}
	}

	return false;
}

void DMDWidget::updateImage()
{
	const int DMDSIZE = 2;
	m_DMD_label->setGeometry(700, 160, m_width * DMDSIZE, m_height * DMDSIZE);
	QImage image(m_width, m_height, QImage::Format_RGBA8888);
	QPainter p(&image);
	if (m_DMD_blocks.size() == 0)
	{
#if 0
		const int BUFFERSIZE = 1000000;
		uint8_t* noise = new uint8_t[BUFFERSIZE];
		for (int i = 0; i < BUFFERSIZE; ++i)
		{
			noise[i] = rand() % 256;
		}

		int bytepos = 0;
		for (int y = 0; y < m_height; ++y)
		{
			for (int x = 0; x < m_width; ++x)
			{
				uint8_t r = noise[bytepos];
				uint8_t g = noise[bytepos + 1];
				uint8_t b = noise[bytepos + 2];
				uint8_t a = noise[bytepos + 3];
				p.setPen(QColor(r, g, b, a));
				p.drawPoint(x, y);
				bytepos += 4;
			}
		}

		delete[] noise;
#else
		p.fillRect(QRect(0, 0, m_width, m_height), Qt::black);
#endif
	}
	else
	{
		int bytepos = m_byte_offset;
		uint8_t* buffer = m_DMD_blocks[m_current_block];
		uint32_t buffersize = m_DMD_block_sizes[m_current_block];

		for (int y = 0; y < m_height; ++y)
		{
			for (int x = 0; x < m_width; ++x)
			{
				if (bytepos >= (buffersize - 4))
				{
					p.setPen(Qt::red);
					p.drawPoint(x, y);
				}
				else
				{
					uint8_t r = buffer[bytepos];
					uint8_t g = buffer[bytepos + 1];
					uint8_t b = buffer[bytepos + 2];
					uint8_t a = buffer[bytepos + 3];
					p.setPen(QColor(r, g, b, a));
					p.drawPoint(x, y);
				}
				bytepos += 4;
			}
		}
	}

	m_DMD_label->setPixmap(QPixmap::fromImage(image).scaled(QSize(m_width * DMDSIZE, m_height * DMDSIZE)));
}