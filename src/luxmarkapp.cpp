/***************************************************************************
 *   Copyright (C) 1998-2013 by authors (see AUTHORS.txt)                  *
 *                                                                         *
 *   This file is part of LuxRays.                                         *
 *                                                                         *
 *   LuxRays is free software; you can redistribute it and/or modify       *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   LuxRays is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 *   LuxRays website: http://www.luxrender.net                             *
 ***************************************************************************/

#include <limits>
#include <boost/algorithm/string.hpp>

#include <QFile>
#include <QGraphicsSceneMouseEvent>

#include "slg/film/film.h"
#include "slg/renderengine.h"
#include "slg/engines/pathocl/pathocl.h"

#include "luxmarkcfg.h"
#include "luxmarkapp.h"
#include "luxvrdialog.h"
#include "resultdialog.h"
#include "luxmarkdefs.h"

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

using namespace luxrays;

static void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
	printf("\n*** ");
	if(fif != FIF_UNKNOWN)
		printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));

	printf("%s", message);
	printf(" ***\n");
}

static void LuxCoreErrorHandler(const char *msg) {
	if (strncmp(msg, "[LuxRays]", 9) == 0) {
		LM_LOG_LUXRAYS(&msg[9]);
	} else if (strncmp(msg, "[SDL]", 5) == 0) {
		LM_LOG_SDL(&msg[5]);
	} else if (strncmp(msg, "[LuxCore]", 9) == 0) {
		LM_LOG_LUXCORE(&msg[9]);
	} else {
		LM_LOG_LUXCORE(msg);
	}
}

//------------------------------------------------------------------------------
// LuxMark Qt application
//------------------------------------------------------------------------------

LuxMarkApp::LuxMarkApp(int &argc, char **argv) : QApplication(argc, argv) {
	// Initialize FreeImage Library, it must be done after QApplication() in
	// order to avoid a crash
	FreeImage_Initialise(TRUE);
	FreeImage_SetOutputMessage(FreeImageErrorHandler);

	// Initialize rand() number generator
	srand(time(NULL));

	// Initialize LuxRender API
	luxcore::Init(LuxCoreErrorHandler);

	singleRun = false;

	mainWin = NULL;
	engineInitThread = NULL;
	engineInitDone = false;
	renderingStartTime = 0.0;
	luxSession = NULL;
	renderRefreshTimer = NULL;
	hardwareTreeModel = NULL;
    
#ifdef __APPLE__ // reliable reference for cwd, mandatory for bundles
    boost::filesystem::path bundlePath;
    char result[1024];
    uint32_t size=1023;
    if (!_NSGetExecutablePath(result, &size)) {
    bundlePath=string(result);
    boost::filesystem::current_path(bundlePath.parent_path().parent_path()); // LuxMark.app/Contents, where we now have the scenes and slg4 dir
    }
#endif
    // Look for the directory where Lux executables are
	exePath = boost::filesystem::path(boost::filesystem::initial_path<boost::filesystem::path>());
}

LuxMarkApp::~LuxMarkApp() {
	if (engineInitThread) {
		// Wait for the init rendering thread
		engineInitThread->join();
		delete engineInitThread;
	}
	delete luxSession;
	delete mainWin;
	delete hardwareTreeModel;
}

void LuxMarkApp::Init(LuxMarkAppMode mode, const char *scnName, const bool single) {
	mainWin = new MainWindow();
	mainWin->setWindowTitle("LuxMark v" LUXMARK_VERSION_MAJOR "." LUXMARK_VERSION_MINOR);
	mainWin->show();
	mainWin->SetLuxApp(this);
	LogWindow = mainWin;
	singleRun = single;

	LM_LOG("<FONT COLOR=\"#0000ff\">LuxMark v" << LUXMARK_VERSION_MAJOR << "." << LUXMARK_VERSION_MINOR << "</FONT>");
	LM_LOG("Based on <FONT COLOR=\"#0000ff\">LuxCore v" << LUXCORE_VERSION_MAJOR << "." << LUXCORE_VERSION_MINOR << "</FONT>");

	// Initialize hardware information
	hardwareTreeModel = new HardwareTreeModel(mainWin);
	mainWin->SetHardwareTreeModel(hardwareTreeModel);

	InitRendering(mode, scnName);
}

void LuxMarkApp::SetMode(LuxMarkAppMode m) {
	InitRendering(m, sceneName);
}

void LuxMarkApp::SetScene(const char *name) {
	InitRendering(mode, name);
}

void LuxMarkApp::Stop() {
	delete renderRefreshTimer;
	renderRefreshTimer = NULL;

	if (engineInitThread) {
		// Wait for the init rendering thread
		engineInitThread->join();
		delete engineInitThread;
		engineInitThread = NULL;
	}
	engineInitDone = false;

	// Free the scene if required
	delete luxSession;
	luxSession = NULL;
}

void LuxMarkApp::InitRendering(LuxMarkAppMode m, const char *scnName) {
	mode = m;
	sceneName = scnName;

	Stop();

	if (!strcmp(scnName, SCENE_ROOM))
		mainWin->SetSceneCheck(0);
	else if (!strcmp(scnName, SCENE_SALA))
		mainWin->SetSceneCheck(1);
	else if (!strcmp(scnName, SCENE_LUXBALL_HDR))
		mainWin->SetSceneCheck(2);

	// Initialize the new mode
	mainWin->SetModeCheck(mode);
	if ((mode == PAUSE) || (mode == DEMO_LUXVR)) {
		// Nothing to do
	} else {
		// Update timer
		renderRefreshTimer = new QTimer();
		connect(renderRefreshTimer, SIGNAL(timeout()), SLOT(RenderRefreshTimeout()));

		// Refresh the screen every 4 secs in benchmark or stress mode
		renderRefreshTimer->start(4 * 1000);
	}

	mainWin->ShowLogo();

	if (mode == PAUSE) {
		// Nothing to do
	} else if (mode == DEMO_LUXVR) {
		// Show LuxVR dialog
		LuxVRDialog *dialog = new LuxVRDialog(sceneName, exePath);

		dialog->exec();
		delete dialog;
		mainWin->raise(); // make luxmark gui frontmost win 
		
		// Go in PAUSE mode
		InitRendering(PAUSE, sceneName);
	} else {
		// Start the engine init thread
		engineInitThread = new boost::thread(boost::bind(LuxMarkApp::EngineInitThreadImpl, this));
	}
}

void LuxMarkApp::EngineInitThreadImpl(LuxMarkApp *app) {
	try {
		// Initialize the new mode
		string sname(app->sceneName); 
		// At the first run, hardwareTreeModel is NULL
		const string deviceSelection = (app->hardwareTreeModel) ?
			(app->hardwareTreeModel->getDeviceSelectionString()) : "";
		app->luxSession = new LuxRenderSession(sname, app->mode, deviceSelection);

		// Start the rendering
		app->luxSession->Start();

		// Done
		app->renderingStartTime = luxrays::WallClockTime();
		app->engineInitDone = true;
	} catch (cl::Error err) {
		LM_ERROR("OpenCL ERROR: " << err.what() << "(" << err.err() << ")");
	} catch (runtime_error err) {
		LM_ERROR("RUNTIME ERROR: " << err.what());
	} catch (exception err) {
		LM_ERROR("ERROR: " << err.what());
	}
}

void LuxMarkApp::RenderRefreshTimeout() {
	if (!engineInitDone)
		return;

	const Properties &stats = luxSession->GetStats();

	// Update shown image
	const int width = luxSession->GetFrameBufferWidth();
	const int height = luxSession->GetFrameBufferHeight();
	const double sampleCount = stats.Get("stats.renderengine.total.samplecount").Get<double>();
	mainWin->ShowFrameBuffer(luxSession->GetFrameBuffer(), width, height);
	const unsigned char *pixels = mainWin->GetFrameBuffer();

	// To save reference image
	/*const u_int samlePerPixel = (u_int)(sampleCount / (width * height));
	LM_LOG("Sample per pixel: " << samlePerPixel);
	static bool saved = false;
	if (!saved && samlePerPixel > 5000) {
		LM_LOG("Saving reference...");
		const QByteArray data = QByteArray::fromRawData((char *)pixels, width * height * 3);
		QFile refImage("reference.raw");
		refImage.open(QIODevice::WriteOnly);
		refImage.write(data);
		refImage.close();
		saved = true;
	}*/

	// Update the statistics
	const double renderingTime = stats.Get("stats.renderengine.time").Get<double>();
	const double sampleSec = (renderingTime > 0.0) ? (sampleCount / renderingTime) : 0.0;

	vector<string> deviceNames;
	vector<double> deviceRaysSecs;
	vector<double> deviceMem, deviceMaxMem;
	double triangleCount = 0.0;
	double minPerf = 0.0;
	double totalPerf = 0.0;
	switch (mode) {
		case STRESSTEST_OCL_GPU:
		case STRESSTEST_OCL_CPUGPU:
		case STRESSTEST_OCL_CPU:
		case BENCHMARK_OCL_GPU:
		case BENCHMARK_OCL_CPUGPU:
		case BENCHMARK_OCL_CPU:
		case BENCHMARK_OCL_CUSTOM:
		case BENCHMARK_HYBRID_GPU:
		case BENCHMARK_HYBRID_CUSTOM: {
			triangleCount = stats.Get("stats.dataset.trianglecount").Get<double>();

			// Get each device statistics
			minPerf = numeric_limits<double>::infinity();
			totalPerf = 0.0;
			const Property &devNames = stats.Get("stats.renderengine.devices");
			for (u_int i = 0; i < devNames.GetSize(); ++i) {
				const string deviceName = devNames.Get<string>(i);
                deviceNames.push_back(deviceName);

				const double raySecs = stats.Get("stats.renderengine.devices." + deviceName + ".performance.total").Get<double>();
				deviceRaysSecs.push_back(raySecs);
				deviceMaxMem.push_back(stats.Get("stats.renderengine.devices." + deviceName + ".memory.total").Get<double>());
				deviceMem.push_back(stats.Get("stats.renderengine.devices." + deviceName + ".memory.used").Get<double>());

				minPerf = Min(raySecs, minPerf);
				totalPerf += raySecs;
			}
			break;
		}
		case BENCHMARK_NATIVE:
			break;
		default: {
			LM_LOG("<FONT COLOR=\"#ff0000\">Unknown render mode in LuxMarkApp::RenderRefreshTimeout(): " << mode << "</FONT>");
			break;
		}
	}

	// Get the list of device names
	// After 120secs of benchmark, show the result dialog
	const bool benchmarkDone = (renderingTime > 120.0) &&
		(mode != STRESSTEST_OCL_GPU) &&
		(mode != STRESSTEST_OCL_CPUGPU) &&
		(mode != STRESSTEST_OCL_GPU);

	char buf[512];
	stringstream ss("");

	char validBuf[128];
	if (benchmarkDone)
		strcpy(validBuf, " (OK)");
	else {
		if ((mode != STRESSTEST_OCL_GPU) &&
				(mode != STRESSTEST_OCL_CPUGPU) &&
				(mode != STRESSTEST_OCL_GPU))
			sprintf(validBuf, " (%dsecs remaining)", Max<int>(120 - renderingTime, 0));
		else
			strcpy(validBuf, "");
	}

	char triCountBuf[128];
	if (triangleCount > 0.0)
		sprintf(triCountBuf, "[Rays/sec % 6dK on %.1fK tris]",
			int(totalPerf / 1000.0), triangleCount / 1000.0);
	else
		strcpy(triCountBuf, "");

	sprintf(buf, "[Mode: %s][Time: %dsecs%s][Samples/sec % 6dK]%s",
			LuxMarkAppMode2String(mode).c_str(),
			int(renderingTime), validBuf, int(sampleSec / 1000.0),
			triCountBuf);
	ss << buf;

	if (deviceNames.size() > 0) {
		ss << "\n\nOpenCL rendering devices:";
		for (size_t i = 0; i < deviceNames.size(); ++i) {
			sprintf(buf, "\n    [%s][Rays/sec % 3dK][Prf Idx %.2f][Wrkld %.1f%%][Mem %.1fM/%dM]",
					deviceNames[i].c_str(),
					int(deviceRaysSecs[i] / 1000.0),
					(minPerf > 0.0) ? (deviceRaysSecs[i] / minPerf) : 0.f,
					(totalPerf > 0.0) ? (100.0 * deviceRaysSecs[i] / totalPerf) : 0.f,
					deviceMem[i] / (1024 * 1024), int(deviceMaxMem[i] / (1024 * 1024)));
			ss << buf;
		}
	}

	mainWin->UpdateScreenLabel(ss.str().c_str(), benchmarkDone);

	if (benchmarkDone) {
		// Check if I'm in single run mode
		if (singleRun) {
			cout << "Score: " << int(sampleSec / 1000.0) << endl;

			exit(EXIT_SUCCESS);
		} else {
			Stop();

            vector<BenchmarkDeviceDescription> descs = hardwareTreeModel->getSelectedDeviceDescs(mode);
			ResultDialog *dialog = new ResultDialog(mode, sceneName, sampleSec,
                    descs, pixels, width, height);
			dialog->exec();
			delete dialog;

			// Go in PAUSE mode
			InitRendering(PAUSE, sceneName);
		}
	}
}
