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
class QTextEdit;
class DMD;
class DMDFrame;

class ROMInspectWindow2 : public QWidget
{
	Q_OBJECT
public:
	ROMInspectWindow2(QWidget* parent, DMDAnimationEngine* animation_engine);
	~ROMInspectWindow2();
private:
	void initUI();
	void updateImages();

	QImage dmd_image(const DMDFrame& dmd_frame);

private slots:
	void file_open_button_clicked();

	void dec2_button_clicked();
	void dec_button_clicked();
	void inc_button_clicked();
	void inc2_button_clicked();

private:
	DMDAnimationEngine* m_animation_engine = nullptr;

	static const uint32_t FRAME_IMAGE_HEIGHT = 128;
	static const uint32_t FRAME_SIZE = 128 * FRAME_IMAGE_HEIGHT / 8;
	QLabel* m_file_name_label = nullptr;
	
	QTextEdit* m_debug_text_edit = nullptr;

	QLabel* m_framecurrent_label = nullptr;
	QLabel* m_framenext_label = nullptr;
	QLabel* m_framemerged_label = nullptr;

	uint8_t* m_rom_content = nullptr;
	uint32_t m_rom_size = 0;

	DMD* m_wpcedit_dmd = nullptr;
	const int DMD_SIZE = 2; // Should be pixel size
};

//////////////////////
// TODO redefined original dialog types
#define DMD_DIALOG_TYPE_GRAPHICS 0
#define DMD_DIALOG_TYPE_FONTDATA 1
#define DMD_DIALOG_TYPE_ANIDATA 2

#define DMD_ROWS         32
#define DMD_COLUMNS     128
#define DMD_PAGE_BYTES  ((DMD_ROWS * DMD_COLUMNS)/8)  // 512 bytes

#define PAGE_LENGTH                 0x4000
#define BASE_CODE_ADDR_PAGED_ROM    0x4000
#define BASE_CODE_ADDR_NONPAGED_ROM 0x8000
#define NONPAGED_LENGTH             0x8000
#define NONPAGED_BANK_INDICATOR     0xFF

#define WRITE_TYPE_COLUMNS       0
#define WRITE_TYPE_ROWS          1

#define HITTYPE_NONE                    0
#define HITTYPE_TBL_ADDR_ADDR_ADDR      1
#define HITTYPE_TBL_ADDR_ADDR           2
#define HITTYPE_TBL_ADDR                3

#define IMAGE_CODE_MONOCHROME            0x00   // Single color pixel data
#define IMAGE_CODE_FD                    0xFD   // Unsure (maybe inversed paint?) IJ uses.  Use normal header 0x00 processing
#define IMAGE_CODE_BICOLOR_INDIRECT      0xFE   // Bi-color pixel data contains one plane and pointer to the other
#define IMAGE_CODE_BICOLOR_DIRECT        0xFF   // Bi-color pixel data contains both planes

#define PIXEL_HEIGHT  5
#define PIXEL_WIDTH   5

#define MAX_GRAPHIC_INDEX  2048 // at this time we will restrict value

#define IMAGE_SHIFT_X_PIXEL_COUNT     8
#define IMAGE_SHIFT_Y_PIXEL_COUNT     8

#define PLANE0_ON      0x01
#define PLANE0_XORED   0x02
#define PLANE0_SKIPPED 0x04
#define PLANE1_ON      0x10
#define PLANE1_XORED   0x20
#define PLANE1_SKIPPED 0x40

#define DMD_FULLFRAME_PAINT_DIM     0x01
#define DMD_FULLFRAME_PAINT_MEDIUM  0x02
#define DMD_FULLFRAME_PAINT_BLENDED 0x04

#define PLANE_STATUS_VALID                 0
#define PLANE_STATUS_INVALID               1
#define PLANE_STATUS_UNKNOWN_TYPE          2
#define PLANE_STATUS_UNIMPLEMENTED_TYPE    3
#define PLANE_STATUS_TABLEENTRYOUTOFRANGE  4
#define PLANE_STATUS_BADDIMENSION          5
#define PLANE_STATUS_IMAGEOUTOFRANGE       6

#define DEFAULT_DEBOUNCE_TIME  500  // 1/2 second before repeating graphics quickly
#define DEBOUNCE_STATE_IDLE        0
#define DEBOUNCE_STATE_DEBOUNCING  1
#define DEBOUNCE_STATE_DONE        2

#define DEBUG_KEY_BIT_SHIFTKEYS           0x01
#define DEBUG_KEY_BIT_CONTROLKEYS         0x02

const static char StatusText [(PLANE_STATUS_IMAGEOUTOFRANGE + 1)][32] = 
	{
		"Valid Decoding",
		"Invalid",
		"Unknown Type",
		"Unimplemented Type",
		"Table out of Range",
		"Bad Dimension",
		"Image out of Range",
	};



/////////////////////////////////////////////////////////////////////////////
// DMD dialog
struct COMMONData
{
	unsigned long ROMSize;
	unsigned char TotalPages;    // includes non-banked, conceptual pages (2 pages nonbanked)
	unsigned char BasePageIndex;
	
	// todo refactor to index
	unsigned char* StartPtr; // 8 bit TODO
	unsigned char* EndPtr;
};

typedef unsigned char Plane[DMD_PAGE_BYTES];
struct DMDPlane
{
	unsigned char Plane_Status;
	unsigned int Plane_Size;
	Plane Plane_Data;
	Plane Plane_Skipped;
	Plane Plane_XorFlags;
	Plane Plane_XorBits;
};

struct DMDPlanes
{
	DMDPlane Plane0;
	DMDPlane Plane1;
};

struct FULLFRAMEImageData
{
	unsigned long TableAddress;
	int CurrentImageIndex;
	//
	DMDPlanes Planes;
};

struct VARIABLESIZEDImageData
{
	unsigned long TableAddress;
	//
	int CurrentTableIndex;
	int CurrentImageIndex;
	int CurrentImageXSize;
	int CurrentImageYSize;
	//
	int CurrentImageXShift;
	int CurrentImageYShift;
	//
	int minTableIndex;
	int minImageIndex;
	int maxTableIndex;
	int maxImageIndex;
	//
	DMDPlanes Planes;
};

struct ImageHeader
{
	unsigned char SpecialFlagByte;
	unsigned char RepeatBytes[8];
	unsigned char ReadMask;
};

typedef enum
{
   ThisPixel_Off = 0,
   ThisPixel_On,
   ThisPixel_Xored,
   ThisPixel_Skipped
} ThisPixel;

/*
# wpcedit

WPCEdit
Copyright (c) 2011
Garrett Lee, mrglee@yahoo.com
*/

class DMD
{
	// Construction
public:
	DMD(QTextEdit* debug_text_edit);
	int Init(unsigned char* data, uint32_t Length);

	void OnButtonNextGraphic();
	void OnButtonNextGraphicx2();
	void OnButtonPreviousGraphic();
	void OnButtonPreviousGraphicx2();

private:
	COMMONData CommonData;
	FULLFRAMEImageData FullFrameImageData;
	VARIABLESIZEDImageData VariableSizedImageData;;
	int dialogType;
	int debugKeyBitmask;
	int selectedTitleBox;

	Plane PreviousPlaneDataPane0;
	Plane PreviousPlaneDataPane1;

	void CheckKeyStateForDebugFlags();
	//	void PaintDMDPanelImage(CPaintDC* pDc, DMDPlanes* pPlanes, unsigned char PaneMask);
	void DecodePlaneInit(DMDPlane* pPlane);
	void DecodeCurrentIndex(void);
	void DecodePreviousIndex(int count);
	void DecodeNextIndex(int count);
	void ButtonHandlerNext(int count);
	void ButtonHandlerPrevious(int count);

	void DecodeVariableSizedImage(unsigned char** Source, DMDPlanes* pPlanes, int TableIndex);
	unsigned char DecodeVariableSizedImage_Centered(unsigned char** SourcePtr, unsigned char** DestPtr, int ImageHeight, int ImageWidth);
	void DecodeVariableSizedImageIndex_NoHeader(unsigned char** SourcePtr, DMDPlanes* pPlanes, int TableHeight);
	void DecodeVariableSizedImageIndex_Header(unsigned char** SourcePtr, DMDPlanes* pPlanes, int TableHeight, int TableIndex);
	void DecodeFullFrameGraphic(unsigned long GraphicIndex);
	void DecodeImageToPlane(int Index, DMDPlane* pPlane);
	unsigned char DecodeFullFrameGraphicImage(unsigned char** Source, DMDPlane* pPlane);
	void Decode_00(unsigned char** Source, unsigned char* Dest);
	void Decode_01(unsigned char** Source, unsigned char* Dest);
	void Decode_02(unsigned char** Source, unsigned char* Dest);
	void Decode_03(unsigned char** Source, unsigned char* Dest);
	void Decode_04(unsigned char** Source, unsigned char* Dest);
	void Decode_05(unsigned char** Source, unsigned char* Dest);
	void Decode_06(unsigned char** Source, unsigned char* Dest, unsigned char* XorFlags, unsigned char* XorBits);
	void Decode_07(unsigned char** Source, unsigned char* Dest, unsigned char* XorFlags, unsigned char* XorBits);
	void Decode_08(unsigned char** Source, unsigned char* Dest, unsigned char* Skipped);
	void Decode_09(unsigned char** Source, unsigned char* Dest, unsigned char* Skipped);
	void Decode_0A(unsigned char** Source, unsigned char* Dest, unsigned char* Skipped);
	void Decode_0B(unsigned char** Source, unsigned char* Dest, unsigned char* Skipped);
	unsigned char ReadNext8BitValue(struct ImageHeader* Header, unsigned char** SourcePtr);
	unsigned char ReadNextBit(struct ImageHeader* Header, unsigned char** SourcePtr);
	void WriteNext8BitValue(unsigned char** DestPtr, unsigned int* WriteCounterPtr, unsigned char ch, unsigned char Type);
	void Decode_01or02(unsigned char** SourcePtr, unsigned char** DestPtr, unsigned char Type);
	void Decode_04or05(unsigned char** SourcePtr, unsigned char** DestPtr, unsigned char Type);
	void Decode_06or07(unsigned char** SourcePtr, unsigned char** DestPtr, unsigned char** XorFlagsPtr, unsigned char** XorBitsPtr, unsigned char Type);
	void Decode_08or09(unsigned char** SourcePtr, unsigned char** DestPtr, unsigned char** Skipped, unsigned char Type);
	void Decode_0Aor0B(unsigned char** SourcePtr, unsigned char** DestPtr, unsigned char** Skipped, unsigned char Type);

	void InvalidateDMDPages();
	void UpdateStaticTextBoxes();
	void UpdateStaticTextBoxesFullFrameGraphics();
	void UpdateStaticTextBoxesVariableSizedImage();
	void UpdateControls();
	void SaveDMDRegistrySettings();
	void UpdateStaticTextBoxesVariableSizedImagePane(int Pane);
	//void UpdateStaticTextBoxesVariableSizedImagePrint(int TableIndex, int ImageIndex, CStatic* pWnd, CStatic* pWndTitle, unsigned char Status);
	void UpdateCheckboxText();

	void DebugKeyMsgStrPrint(const QString& str, int KeyMask);
	void DebugShiftKeyMsgStrPrint(const QString& str);
	void DebugControlKeyMsgStrPrint(const QString& str);

	int GetROMAddressFromWPCAddrAndPage(unsigned long* pRomAddr, unsigned long Addr, unsigned char Page);
	int ExtractWPCAddrAndPageFromBuffer(unsigned char* pSrc, unsigned long* pDstAddr, unsigned char* pDstPage);
	int GetROMAddressFromAddrOf3ByteWPCAddrPage(unsigned char* pSrc, unsigned long* pDst);
	int ProcessHitType(int HitType, unsigned char* HitTablePtr, unsigned char* HitPagePtr, unsigned char* Ptr, unsigned long* pTbl);

	int InitCommon(unsigned char* data, uint32_t Length); // TODO refactor to support better structure instead of start and end pointer hell
	int InitTableAddrs();

	int PreAnalyzeVariableSizedImageTable();
	int ExtractWPCAddrAndPageOfImageTable(int* pAddr, int* pPage, int TableIndex);
	int GetROMAddressOfVariableSizedImageIndex(unsigned long* pRomAddr, int TableIndex, int ImageIndex);
	int GetROMAddressOfVariableSizedImageTable(unsigned long* pRomAddr, int TableIndex);
	int GetAddrToWPCAddressOfVariableSizedImageTable(unsigned long* pAddr, int TableIndex);
	int GetVariableSizedImageTableMetadata(int TableIndex, int* pTableHeight, int* pTableSpacing);
	int GetPrevImageIndex(int* pImageIndex, int TableIndex);
	int GetNextImageIndex(int* pImageIndex, int TableIndex);
	int GetFirstImageIndex(int* pImageIndex, int TableIndex);
	int GetLastImageIndex(int* pImageIndex, int TableIndex);
	int IncrementVariableSizedImageIndex(int* pTableIndex, int* pImageIndex);
	int DecrementVariableSizedImageIndex(int* pTableIndex, int* pImageIndex);

	void DecodeVariableSizedImageData();
	void DecodeVariableSizedImageIndexToPlane(int TableIndex, int ImageIndex, DMDPlanes* pPlanes);
	void exportCurrent();

	// TODO

	bool bWiped = false;

	// BlackStar add stuff
	QTextEdit* m_debug_text_edit = nullptr;
};