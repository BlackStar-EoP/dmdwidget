#include "mainwindow.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QMimeData>
#include <QDebug>
#include <QFileInfo>
#include <QFile>

#include "dmdwidget.h"

MainWindow::MainWindow()
{
	m_DMD_widget = new DMDWidget(this);
	setCentralWidget(m_DMD_widget);
	m_DMD_widget->show();
}