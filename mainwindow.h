#pragma once

#include <QMainWindow>
#include <stdint.h>

class DMDWidget;

class MainWindow : public QMainWindow
{
public:
	MainWindow();

private:
	DMDWidget* m_DMD_widget = nullptr;
};