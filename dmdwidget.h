#include <QWidget>

#include <string>
#include <Windows.h>

#include <vector>

class QLabel;
class QPushButton;
class QTimer;

class DMDWidget : public QWidget
{
	Q_OBJECT
public:
	static const uint32_t DMDWIDTH = 128;
	static const uint32_t DMDHEIGHT = 32;


public:
	DMDWidget(QWidget* parent);

private slots:
	void findDMDButton_clicked();
	void captureDMDButton_clicked();
	void captureTimeout();

private:
	bool findFX3();
	bool findDMD();
	void getDMDColor();
	void captureDMD();
	bool isGarbage(const uint8_t* rawDMD) const;
	bool isWilliamsDMD(const uint8_t* rawDMD) const;
	bool isEmpty(const uint8_t* rawDMD) const;
	bool isEqual(const uint8_t* DMD1, const uint8_t* DMD2);
	void correctWilliamsDMD(uint8_t* rawDMD);
	void normalizeDMD(uint8_t* rawDMD);
	void normalizeWilliamsDMD(uint8_t* rawDMD);

	uint32_t findDMDMemoryOffset(uint8_t* buffer, SIZE_T buffer_size);

private:
	std::wstring m_executable_name = L"Pinball FX3.exe";
	DWORD m_FX3_process_id = 0;
	HANDLE m_FX3_process_handle = nullptr;
	uint32_t m_FX3_base_offset = 0;
	uint32_t m_DMD_memory_offset = 0;
	bool m_DMD_found = false;
	bool m_DMD_color_found = false;
	float m_DMD_r = 1.0f;
	float m_DMD_g = 1.0f;
	float m_DMD_b = 1.0f;

	QLabel* m_DMD_label = nullptr;
	QTimer* captureTimer = nullptr;
};