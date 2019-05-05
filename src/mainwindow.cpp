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

// List of new v4.0 features:
//  - New Brute force path tracing Food benchmark scene
//  - New Path tracing + Global Illumination cache Wall Bench benchmark scene
//  - New Brute force bidirectional path tracing Wall Paper benchmark scene
//  - New BENCHMARK_HYBRID (OpenCL GPUs + Native C++) benchmark mode
//  - New BENCHMARK_HYBRID_CUSTOM (OpenCL Custom + Native C++) benchmark mode

#include <QTextEdit>
#include <QDialogButtonBox>
#include <QDateTime>
#include <QTextStream>
#include <QGraphicsItem>

#include "mainwindow.h"
#include "aboutdialog.h"
#include "luxmarkdefs.h"
#include "luxmarkapp.h"
#include "luxmarkdefs.h"

using namespace luxrays;

MainWindow *LogWindow = NULL;

int EVT_LUX_LOG_MESSAGE = QEvent::registerEventType();
int EVT_LUX_ERR_MESSAGE = QEvent::registerEventType();

LuxLogEvent::LuxLogEvent(QString msg) : QEvent((QEvent::Type)EVT_LUX_LOG_MESSAGE), message(msg) {
	setAccepted(false);
}

LuxErrorEvent::LuxErrorEvent(QString msg) : QEvent((QEvent::Type)EVT_LUX_ERR_MESSAGE), message(msg) {
	setAccepted(false);
}

//------------------------------------------------------------------------------

LuxFrameBuffer::LuxFrameBuffer(const QPixmap &pixmap) : QGraphicsPixmapItem(pixmap) {
	setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
}

//------------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags),
		ui(new Ui::MainWindow) {
	ui->setupUi(this);

	// Setup rendering area
	renderScene = new QGraphicsScene();
	renderScene->setBackgroundBrush(QColor(127,127,127));
	luxLogo = renderScene->addPixmap(QPixmap(":/images/resources/luxlogo_bg.png"));
	luxFrameBuffer = new LuxFrameBuffer(QPixmap(":/images/resources/luxlogo_bg.png"));
	renderScene->addItem(luxFrameBuffer);

	authorLabelBack = new QGraphicsSimpleTextItem(QString("Scene designed by LuxCoreRender project"));
	renderScene->addItem(authorLabelBack);
	authorLabelBack->setBrush(Qt::black);
	authorLabel = new QGraphicsSimpleTextItem(authorLabelBack->text());
	authorLabel->setBrush(Qt::blue);
	renderScene->addItem(authorLabel);
	
	raw2denoisedLabel = new QGraphicsSimpleTextItem(QString("(Raw Vs. Denoised rendering)"));
	raw2denoisedLabel->setBrush(Qt::blue);
	raw2denoisedLabel->setVisible(false);
	renderScene->addItem(raw2denoisedLabel);
	

	screenLabelBack = renderScene->addRect(0.f, 0.f, 1.f, 1.f, QPen(), Qt::lightGray);
	screenLabel = new QGraphicsSimpleTextItem(QString("[Time: 0secs (Wait)]"));
	renderScene->addItem(screenLabel);

	ui->RenderView->setScene(renderScene);

	frameBuffer = NULL;
	fbWidth = 0;
	fbHeight = 0;

	// Setup status bar
	statusBarLabel = new QLabel(ui->statusbar);
	statusBarLabel->setText("");
	ui->statusbar->addWidget(statusBarLabel);

#if defined(__APPLE__)
	setFont(QFont  ("Lucida Grande", 11)); // todo: find best size			
#endif

	ShowLogo();
	
	this->setWindowState(Qt::WindowMaximized);
	ui->MainVerticalSplitter->setSizes(QList<int>() << 1024 << 128);
	ui->MainHorizontalSplitter->setSizes(QList<int>() << 1024 << 256);
}

MainWindow::~MainWindow() {
	delete ui;
	delete statusBarLabel;
	delete authorLabel;
	delete authorLabelBack;
	delete raw2denoisedLabel;
	delete screenLabel;
	delete screenLabelBack;
	delete luxFrameBuffer;
	delete luxLogo;
	delete renderScene;
	delete frameBuffer;

}

//------------------------------------------------------------------------------
// Qt Slots
//------------------------------------------------------------------------------

void MainWindow::exitApp() {
	// Win32 refuses to exit without the following line
	((LuxMarkApp *)qApp)->Stop();

	exit(0);
}

void MainWindow::showAbout() {
	AboutDialog *dialog = new AboutDialog();

	dialog->exec();
}

void MainWindow::UpdateSceneLabel(const char *name) {
	if (!strcmp(SCENE_WALLPAPER, name)) {
		authorLabelBack->setText(QString("Scene designed by Simon \"B.Y.O.B.\" Wendsche (based on Psor's Cube scene)"));
		authorLabelBack->setBrush(Qt::black);
		authorLabel->setText(authorLabelBack->text());
		authorLabel->setBrush(Qt::blue);
	} else if (!strcmp(SCENE_HALLBENCH, name)) {
		authorLabelBack->setText(QString("Scene designed by Charles \"Sharlybg\" Nandeya Ehouman"));
		authorLabelBack->setBrush(Qt::black);
		authorLabel->setText(authorLabelBack->text());
		authorLabel->setBrush(Qt::blue);
	} else if (!strcmp(SCENE_FOOD, name)) {
		authorLabelBack->setText(QString("Scene designed by Charles \"Sharlybg\" Nandeya Ehouman"));
		authorLabelBack->setBrush(Qt::black);
		authorLabel->setText(authorLabelBack->text());
		authorLabel->setBrush(Qt::blue);
	}
}

void MainWindow::setFoodScene() {
	LM_LOG("Set Food scene");
	UpdateSceneLabel(SCENE_FOOD);
	((LuxMarkApp *)qApp)->SetScene(SCENE_FOOD);
}

void MainWindow::setHallBenchScene() {
	LM_LOG("Set HallBench scene");
	UpdateSceneLabel(SCENE_HALLBENCH);
	((LuxMarkApp *)qApp)->SetScene(SCENE_HALLBENCH);
}

void MainWindow::setWallPaperScene() {
	LM_LOG("Set WallPaper scene");
	UpdateSceneLabel(SCENE_WALLPAPER);
	((LuxMarkApp *)qApp)->SetScene(SCENE_WALLPAPER);
}

void MainWindow::setMode_BENCHMARK_OCL_GPU() {
	LM_LOG("Set mode: BENCHMARK_OCL_GPU");
	((LuxMarkApp *)qApp)->SetMode(BENCHMARK_OCL_GPU);
}

void MainWindow::setMode_BENCHMARK_OCL_CPUGPU() {
	LM_LOG("Set mode: BENCHMARK_OCL_CPUGPU");
	((LuxMarkApp *)qApp)->SetMode(BENCHMARK_OCL_CPUGPU);
}

void MainWindow::setMode_BENCHMARK_OCL_CPU() {
	LM_LOG("Set mode: BENCHMARK_OCL_CPU");
	((LuxMarkApp *)qApp)->SetMode(BENCHMARK_OCL_CPU);
}

void MainWindow::setMode_BENCHMARK_OCL_CUSTOM() {
	LM_LOG("Set mode: BENCHMARK_OCL_CUSTOM");
	((LuxMarkApp *)qApp)->SetMode(BENCHMARK_OCL_CUSTOM);
}

void MainWindow::setMode_BENCHMARK_HYBRID() {
	LM_LOG("Set mode: BENCHMARK_HYBRID");
	((LuxMarkApp *)qApp)->SetMode(BENCHMARK_HYBRID);
}

void MainWindow::setMode_BENCHMARK_HYBRID_CUSTOM() {
	LM_LOG("Set mode: BENCHMARK_HYBRID_CUSTOM");
	((LuxMarkApp *)qApp)->SetMode(BENCHMARK_HYBRID_CUSTOM);
}

void MainWindow::setMode_BENCHMARK_NATIVE() {
	LM_LOG("Set mode: BENCHMARK_NATIVE");
	((LuxMarkApp *)qApp)->SetMode(BENCHMARK_NATIVE);
}

void MainWindow::setMode_STRESSTEST_OCL_GPU() {
	LM_LOG("Set mode: STRESSTEST_OCL_GPU");
	((LuxMarkApp *)qApp)->SetMode(STRESSTEST_OCL_GPU);
}

void MainWindow::setMode_STRESSTEST_OCL_CPUGPU() {
	LM_LOG("Set mode: STRESSTEST_OCL_CPUGPU");
	((LuxMarkApp *)qApp)->SetMode(STRESSTEST_OCL_CPUGPU);
}

void MainWindow::setMode_STRESSTEST_OCL_CPU() {
	LM_LOG("Set mode: STRESSTEST_OCL_CPU");
	((LuxMarkApp *)qApp)->SetMode(STRESSTEST_OCL_CPU);
}

void MainWindow::setMode_DEMO_LUXVR() {
	LM_LOG("Set mode: DEMO_LUXVR");
	((LuxMarkApp *)qApp)->SetMode(DEMO_LUXVR);
}

void MainWindow::setMode_PAUSE() {
	LM_LOG("Set Pause mode");
	((LuxMarkApp *)qApp)->SetMode(PAUSE);
}

void MainWindow::setOCLOpts_cl_fast_relaxed_math(bool enable) {
	LM_LOG("Toggle OpenCL compiler option: -cl-fast-relaxed-math");
	((LuxMarkApp *)qApp)->SetMode(PAUSE);
	((LuxMarkApp *)qApp)->SetOpenCLCompilerOpts(FAST_RELAXED_MATH, enable);
}

void MainWindow::setOCLOpts_cl_mad_enable(bool enable) {
	LM_LOG("Toggle OpenCL compiler option: -cl-mad-enabled");
	((LuxMarkApp *)qApp)->SetMode(PAUSE);
	((LuxMarkApp *)qApp)->SetOpenCLCompilerOpts(MAD_ENABLED, enable);
}

void MainWindow::setOCLOpts_cl_strict_aliasing(bool enable) {
	LM_LOG("Toggle OpenCL compiler option: -cl-strict-aliasing");
	((LuxMarkApp *)qApp)->SetMode(PAUSE);
	((LuxMarkApp *)qApp)->SetOpenCLCompilerOpts(STRICT_ALIASING, enable);
}

void MainWindow::setOCLOpts_cl_no_signed_zeros(bool enable) {
	LM_LOG("Toggle OpenCL compiler option: -cl-no-signed-zeros");
	((LuxMarkApp *)qApp)->SetMode(PAUSE);
	((LuxMarkApp *)qApp)->SetOpenCLCompilerOpts(NO_SIGNED_ZEROS, enable);	
}

//------------------------------------------------------------------------------

void MainWindow::Pause() {
	setMode_PAUSE();
	SetModeCheck(PAUSE);
}

void MainWindow::ShowLogo() {
	if (luxFrameBuffer->isVisible()) {
		luxFrameBuffer->hide();
		authorLabel->hide();
		authorLabelBack->hide();
		raw2denoisedLabel->hide();
		screenLabelBack->hide();
		screenLabel->hide();
	}

	if (!luxLogo->isVisible()) {
		luxLogo->show();
		renderScene->setSceneRect(0.f, 0.f, 416.f, 389.f);
		ui->RenderView->centerOn(luxLogo);
	}

	statusBarLabel->setText("");
	ui->RenderView->setInteractive(false);
}

void MainWindow::SetModeCheck(const LuxMarkAppMode mode) {
	ui->action_OpenCL_GPUs->setChecked(mode == BENCHMARK_OCL_GPU);
	ui->action_OpenCL_CPUs_GPUs->setChecked(mode == BENCHMARK_OCL_CPUGPU);
	ui->action_OpenCL_CPUs->setChecked(mode == BENCHMARK_OCL_CPU);
	ui->action_OpenCL_Custom->setChecked(mode == BENCHMARK_OCL_CUSTOM);
	ui->action_Hybrid->setChecked(mode == BENCHMARK_HYBRID);
	ui->action_Hybrid_Custom->setChecked(mode == BENCHMARK_HYBRID_CUSTOM);
	ui->action_NativeCPP->setChecked(mode == BENCHMARK_NATIVE);

	ui->action_StressTest_OpenCL_GPUs->setChecked(mode == STRESSTEST_OCL_GPU);
	ui->action_StressTest_OpenCL_CPUs_GPUs->setChecked(mode == STRESSTEST_OCL_CPUGPU);
	ui->action_StressTest_OpenCL_CPUs->setChecked(mode == STRESSTEST_OCL_CPU);

	ui->action_LuxVR->setChecked(mode == DEMO_LUXVR);

	ui->action_Pause->setChecked(mode == PAUSE);
}

void MainWindow::SetSceneCheck(const int index) {
	if (index == 0) {
		ui->action_WallPaper->setChecked(true);
		ui->action_HallBench->setChecked(false);
		ui->action_Food->setChecked(false);
		
		ui->action_OpenCL_GPUs->setEnabled(false);
		ui->action_OpenCL_CPUs->setEnabled(false);
		ui->action_OpenCL_CPUs_GPUs->setEnabled(false);
		ui->action_OpenCL_Custom->setEnabled(false);
		ui->action_Hybrid->setEnabled(false);
		ui->action_Hybrid_Custom->setEnabled(false);
		ui->action_NativeCPP->setChecked(true);
	} else if (index == 1) {
		ui->action_WallPaper->setChecked(false);
		ui->action_HallBench->setChecked(true);
		ui->action_Food->setChecked(false);

		ui->action_OpenCL_GPUs->setEnabled(true);
		ui->action_OpenCL_CPUs->setEnabled(true);
		ui->action_OpenCL_CPUs_GPUs->setEnabled(true);
		ui->action_OpenCL_Custom->setEnabled(true);
		ui->action_Hybrid->setEnabled(true);
		ui->action_Hybrid_Custom->setEnabled(true);
		ui->action_NativeCPP->setEnabled(true);
	} else if (index == 2) {
		ui->action_WallPaper->setChecked(false);
		ui->action_HallBench->setChecked(false);
		ui->action_Food->setChecked(true);

		ui->action_OpenCL_GPUs->setEnabled(true);
		ui->action_OpenCL_CPUs->setEnabled(true);
		ui->action_OpenCL_CPUs_GPUs->setEnabled(true);
		ui->action_OpenCL_Custom->setEnabled(true);
		ui->action_Hybrid->setEnabled(true);
		ui->action_Hybrid_Custom->setEnabled(true);
		ui->action_NativeCPP->setEnabled(true);
	} else
		assert(false);
}

bool MainWindow::IsShowingLogo() const {
	return luxLogo->isVisible();
}

void MainWindow::ShowFrameBuffer(const float *frameBufferSrc,
		const float *frameBufferDenoisedSrc,
		const unsigned  int width, const unsigned  int height) {
	if (luxLogo->isVisible())
		luxLogo->hide();

	// Check if I have to allocate a new frame buffer
	if (!frameBuffer || (width != fbWidth) || (height != fbHeight)) {
		delete frameBuffer;
		fbWidth = width;
		fbHeight = height;
		frameBuffer = new unsigned char[fbWidth * fbHeight * 3];
	}

	for (u_int y = 0; y < fbHeight; ++y) {
		for (u_int x = 0; x < fbWidth; ++x) {
			const u_int srcIndex = (x + y * fbWidth) * 3;
			const u_int dstIndex = (x + (fbHeight - y - 1) * fbWidth) * 3;
			frameBuffer[dstIndex] = (unsigned char)(Clamp(frameBufferSrc[srcIndex] * 255.f + .5f, 0.f, 255.f));
			frameBuffer[dstIndex + 1] = (unsigned char)(Clamp(frameBufferSrc[srcIndex + 1] * 255.f + .5f, 0.f, 255.f));
			frameBuffer[dstIndex + 2] = (unsigned char)(Clamp(frameBufferSrc[srcIndex + 2] * 255.f + .5f, 0.f, 255.f));
		}
	}

	if (!frameBufferDenoisedSrc) {
		luxFrameBuffer->setPixmap(QPixmap::fromImage(QImage(
			frameBuffer, fbWidth, fbHeight, fbWidth * 3, QImage::Format_RGB888)));
	} else {
		vector<unsigned char> frameBufferTmp(2 * fbWidth * fbHeight * 3);
		
		for (u_int y = 0; y < fbHeight; ++y) {
			for (u_int x = 0; x < fbWidth; ++x) {
				const u_int srcIndex = (x + y * fbWidth) * 3;

				const u_int dstIndexRaw = (x + y * fbWidth * 2) * 3;
				frameBufferTmp[dstIndexRaw] = frameBuffer[srcIndex];
				frameBufferTmp[dstIndexRaw + 1] = frameBuffer[srcIndex + 1];
				frameBufferTmp[dstIndexRaw + 2] = frameBuffer[srcIndex + 2];

				const u_int dstIndexDenoised = (fbWidth + x + (fbHeight - y - 1) * fbWidth * 2) * 3;
				frameBufferTmp[dstIndexDenoised] = (unsigned char)(Clamp(frameBufferDenoisedSrc[srcIndex] * 255.f + .5f, 0.f, 255.f));
				frameBufferTmp[dstIndexDenoised + 1] = (unsigned char)(Clamp(frameBufferDenoisedSrc[srcIndex + 1] * 255.f + .5f, 0.f, 255.f));
				frameBufferTmp[dstIndexDenoised + 2] = (unsigned char)(Clamp(frameBufferDenoisedSrc[srcIndex + 2] * 255.f + .5f, 0.f, 255.f));
			}
		}	

		luxFrameBuffer->setPixmap(QPixmap::fromImage(QImage(
			&frameBufferTmp[0], 2 * fbWidth, fbHeight, 2 * fbWidth * 3, QImage::Format_RGB888)));
	}

	if (!luxFrameBuffer->isVisible()) {
		luxFrameBuffer->show();
		authorLabelBack->show();
		authorLabel->show();
		raw2denoisedLabel->show();
		screenLabelBack->show();
		screenLabel->show();
	}

	if (frameBufferDenoisedSrc && !raw2denoisedLabel->isVisible())
		raw2denoisedLabel->setVisible(true);
	else
		raw2denoisedLabel->setVisible(false);

	UpdateScreenLabelPosition();

	ui->RenderView->setDragMode(QGraphicsView::ScrollHandDrag);
	ui->RenderView->setInteractive(true);

	//LM_LOG("Screen updated");
}

void MainWindow::SetHardwareTreeModel(HardwareTreeModel *treeModel) {
	if (!ui->HardwareView->model()) {
		ui->HardwareView->setModel(treeModel);
		ui->HardwareView->expandAll();
	}
}

bool MainWindow::event(QEvent *event) {
	bool retval = false;
	int eventtype = event->type();

	// Check if it's one of "our" events
	if (eventtype == EVT_LUX_LOG_MESSAGE) {
		QString buf;
		QTextStream ss(&buf);
		ss << QDateTime::currentDateTime().toString(tr("yyyy-MM-dd hh:mm:ss")) << " - " <<
				((LuxLogEvent *)event)->getMessage();

		ui->LogView->append(ss.readAll());
	} else if (eventtype == EVT_LUX_ERR_MESSAGE) {
		QString buf;
		QTextStream ss(&buf);
		ss << "<FONT COLOR=\"#ff0000\">" << QDateTime::currentDateTime().toString(tr("yyyy-MM-dd hh:mm:ss")) << " - " <<
				((LuxLogEvent *)event)->getMessage() << "</FONT>";

		ui->LogView->append(ss.readAll());

		((LuxMarkApp *)qApp)->SetMode(PAUSE);

		// If I'm in single-run mode, I have to exit
		if ((((LuxMarkApp *)qApp))->IsSingleRun()) {
			cout << "Error: " << ((LuxLogEvent *)event)->getMessage().toStdString() << endl;
			exit(EXIT_SUCCESS);
		}
	}

	if (retval) {
		// Was our event, stop the event propagation
		event->accept();
	}

	return QMainWindow::event(event);
}

void MainWindow::UpdateScreenLabel(const char *msg) {
	screenLabel->setText(QString(msg));

	// Update status bar with the first line of the message
	QString qMsg(msg);
	statusBarLabel->setText(qMsg.split(QString("\n"))[0]);
	
	UpdateScreenLabelPosition();
}

void MainWindow::UpdateScreenLabelPosition() {
	const int pixmapWidth = luxFrameBuffer->pixmap().width();
	const int pixmapHeight = luxFrameBuffer->pixmap().height();

	screenLabel->setBrush(Qt::blue);
	screenLabel->setPos(0.f, pixmapHeight);

	// Update scene size
	qreal w = Max<qreal>(pixmapWidth, screenLabel->boundingRect().width());
	qreal h = pixmapHeight + screenLabel->boundingRect().height();
	renderScene->setSceneRect(0.f, 0.f, w, h);
	luxFrameBuffer->setPos(Max<qreal>(0.f, (w - pixmapWidth) / 2), 0.f);
	screenLabelBack->setRect(0.f, pixmapHeight, w, screenLabel->boundingRect().height());

	// Update author label position
	const qreal alw = Max<qreal>(0.f, (w - pixmapWidth) / 2);
	authorLabelBack->setPos(alw + 1.f, 1.f);
	authorLabel->setPos(alw, 0.f);

	// Raw Vs. Denoised label position
	raw2denoisedLabel->setPos(alw, authorLabel->boundingRect().height() + 1.f);
}
