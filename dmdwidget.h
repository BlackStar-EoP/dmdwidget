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
	DMDWidget(QWidget* parent);

private slots:
	void findDMDButton_clicked();
	void captureDMDButton_clicked();
	void captureTimeout();

private:
	bool findFX3();
	bool findDMD();
	void captureDMD();

	uint32_t findDMDMemoryOffset(uint8_t* buffer, SIZE_T buffer_size);

private:
	std::wstring m_executable_name = L"Pinball FX3.exe";
	DWORD m_FX3_process_id = 0;
	HANDLE m_FX3_process_handle = nullptr;
	uint32_t m_FX3_base_offset = 0;
	uint32_t m_DMD_memory_offset = 0;
	bool m_DMD_found = false;
	QLabel* m_DMD_label = nullptr;
	QTimer* captureTimer = nullptr;
};