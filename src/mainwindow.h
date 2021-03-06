/***************************************************************************
 *   Copyright (C) 1998-2019 by authors (see AUTHORS.txt)                  *
 *                                                                         *
 *   This file is part of LuxMark.                                         *
 *                                                                         *
 *   LuxMark is free software; you can redistribute it and/or modify       *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   LuxMark is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 *   LuxMark website: https://www.luxcorerender.org                        *
 ***************************************************************************/

#ifndef _MAINWINDOW_H
#define	_MAINWINDOW_H

#ifndef Q_MOC_RUN
#include <cstddef>
#include <string>

#include <boost/thread/mutex.hpp>

#include "ui_mainwindow.h"
#include "kernelcompilationdialog.h"
#include "hardwaretree.h"
#include "luxmarkdefs.h"

#include <QGraphicsPixmapItem>
#endif

class LuxMarkApp;

class LuxFrameBuffer : public QGraphicsPixmapItem {
public:
	LuxFrameBuffer(const QPixmap &pixmap);
};

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget *parent = NULL, Qt::WindowFlags flags = 0);
	~MainWindow();

	void ShowLogo();
	bool IsShowingLogo() const;
	void ShowFrameBuffer(const float *frameBuffer,
		const float *frameBufferDenoisedSrc,
		const unsigned int width, const unsigned int height);
	const unsigned char *GetFrameBuffer() const { return frameBuffer; }

	void SetModeCheck(const LuxMarkAppMode mode);
	void SetSceneCheck(const int index);
	void UpdateSceneLabel(const char *name);
	void UpdateScreenLabel(const char *msg);
	void UpdateScreenLabelPosition();
	void SetHardwareTreeModel(HardwareTreeModel *treeModel);

	void Pause();

private:
	bool event(QEvent *event);

	Ui::MainWindow *ui;
	QGraphicsPixmapItem *luxLogo;
	QGraphicsPixmapItem *luxFrameBuffer;
	unsigned char *frameBuffer;
	unsigned int fbWidth, fbHeight;
	QGraphicsSimpleTextItem *authorLabel;
	QGraphicsSimpleTextItem *authorLabelBack;
	QGraphicsSimpleTextItem *raw2denoisedLabel;
	QGraphicsSimpleTextItem *screenLabel;
	QGraphicsRectItem *screenLabelBack;
	QLabel *statusBarLabel;

	QGraphicsScene *renderScene;

	KernelCompilationDialog *kernelCompilationDialog;

private slots:
	void exitApp();
	void showAbout();

	void setCannelleEtFromageScene();
	void setHallBenchScene();
	void setWallPaperScene();

	void setMode_BENCHMARK_CUDA_GPU();
	void setMode_BENCHMARK_OCL_GPU();
	void setMode_BENCHMARK_NATIVE();
	void setMode_BENCHMARK_CUSTOM();
	void setMode_STRESSTEST_CUDA_GPU();
	void setMode_STRESSTEST_OCL_GPU();
	void setMode_STRESSTEST_NATIVE();
	void setMode_STRESSTEST_CUSTOM();
	void setMode_DEMO_LUXCOREUI();
	void setMode_PAUSE();
};

//------------------------------------------------------------------------------
// Log related definitions
//------------------------------------------------------------------------------

extern MainWindow *LogWindow;

class LuxLogEvent: public QEvent {
public:
	LuxLogEvent(QString mesg);
 
	QString getMessage() { return message; }

private:
	QString message;
};

class LuxErrorEvent: public QEvent {
public:
	LuxErrorEvent(QString mesg);

	QString getMessage() { return message; }

private:
	QString message;
};

class ShowKernelCompilationDialogEvent: public QEvent {
public:
	ShowKernelCompilationDialogEvent();
};

class HideKernelCompilationDialogEvent: public QEvent {
public:
	HideKernelCompilationDialogEvent();
};

#define LM_SHOW_KERNEL_COMPILATION_DIALOG { if (LogWindow) { qApp->postEvent(LogWindow, new ShowKernelCompilationDialogEvent()); }}
#define LM_HIDE_KERNEL_COMPILATION_DIALOG { if (LogWindow) { qApp->postEvent(LogWindow, new HideKernelCompilationDialogEvent()); }}

#define LM_LOG(a) { if (LogWindow) { std::stringstream _LM_LOG_LOCAL_SS; _LM_LOG_LOCAL_SS << a; qApp->postEvent(LogWindow, new LuxLogEvent(QString(_LM_LOG_LOCAL_SS.str().c_str()))); }}
#define LM_LOG_LUXRAYS(a) { LM_LOG("<FONT COLOR=\"#002200\"><B>[LuxRays]</B></FONT> " << a); }
#define LM_LOG_SDL(a) { LM_LOG("<FONT COLOR=\"#005500\"><B>[SDL]</B></FONT> " << a); }
#define LM_LOG_LUXCORE(a) { LM_LOG("<FONT COLOR=\"#009900\"><B>[LuxCore]</B></FONT> " << a); }

#define LM_ERROR(a) { if (LogWindow) { std::stringstream _LM_ERR_LOCAL_SS; _LM_ERR_LOCAL_SS << a; qApp->postEvent(LogWindow, new LuxErrorEvent(QString(_LM_ERR_LOCAL_SS.str().c_str()))); }}

#endif	/* _MAINWINDOW_H */
