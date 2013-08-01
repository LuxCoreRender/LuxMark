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

static void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
	printf("\n*** ");
	if(fif != FIF_UNKNOWN)
		printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));

	printf("%s", message);
	printf(" ***\n");
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
	luxInit();

	// Set LuxRender log level
	lux::luxLogFilter = LUX_DEBUG;
	//lux::luxLogFilter = LUX_INFO;

	singleRun = false;

	mainWin = NULL;
	engineInitThread = NULL;
	engineInitDone = false;
	renderingStartTime = 0.0;
	luxSession = NULL;
	renderRefreshTimer = NULL;
	hardwareTreeModel = NULL;
	
	// Look for the directory where Lux executable are
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
	LM_LOG("Based on <FONT COLOR=\"#0000ff\">LuxRender v" << luxVersion() << "</FONT>");

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
	else if (!strcmp(scnName, SCENE_LUXBALL))
		mainWin->SetSceneCheck(3);
	else if (!strcmp(scnName, SCENE_LUXBALL_SKY))
		mainWin->SetSceneCheck(4);

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
		const string slgDeviceSelection = (app->hardwareTreeModel) ?
			(app->hardwareTreeModel->getSLGDeviceSelectionString()) : "";
		const string luxDeviceSelection = (app->hardwareTreeModel) ?
			(app->hardwareTreeModel->getLuxDeviceSelectionString()) : "";
		app->luxSession = new LuxRenderSession(sname, app->mode, slgDeviceSelection, luxDeviceSelection);

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
	if (!engineInitDone || !luxStatistics("sceneIsReady"))
		return;

	// Update shown image

	// Get the rendered image
	luxUpdateFramebuffer();

	// Update the window
	const int width = luxGetIntAttribute("film", "xResolution");
	const int height = luxGetIntAttribute("film", "yResolution");
	const float sampleCount = luxGetDoubleAttribute("film", "numberOfLocalSamples");
	mainWin->ShowFrameBuffer(luxFramebuffer(), width, height);
	const unsigned char *pixels = mainWin->GetFrameBuffer();

	// To save reference image
	/*const u_int samlePerPixel = (u_int)(sampleCount / (width * height));
	LM_LOG("Sample per pixel: " << samlePerPixel);
	static bool saved = false;
	if (!saved && samlePerPixel > 400) {
		LM_LOG("Saving reference...");
		const QByteArray data = QByteArray::fromRawData((char *)pixels, width * height * 3);
		QFile refImage("reference.raw");
		refImage.open(QIODevice::WriteOnly);
		refImage.write(data);
		refImage.close();
		saved = true;
	}*/

	// Update the statistics
	double triangleCount = 0.0;
	const int renderingTime = luxGetDoubleAttribute("renderer_statistics", "elapsedTime");
	const double sampleSec = (renderingTime > 0.0) ? (sampleCount / renderingTime) : 0.0;

	vector<string> deviceNames;
	vector<double> deviceRaysSecs;
	vector<double> deviceMem, deviceMaxMem;
	double minPerf = 0.0;
	double totalPerf = 0.0;
	switch (mode) {
		case STRESSTEST_NOSPECTRAL_OCL_GPU:
		case STRESSTEST_NOSPECTRAL_OCL_CPUGPU:
		case STRESSTEST_NOSPECTRAL_OCL_CPU:
		case BENCHMARK_NOSPECTRAL_OCL_GPU:
		case BENCHMARK_NOSPECTRAL_OCL_CPUGPU:
		case BENCHMARK_NOSPECTRAL_OCL_CPU:
		case BENCHMARK_NOSPECTRAL_OCL_CUSTOM:
		case BENCHMARK_NOSPECTRAL_HYBRID_GPU:
		case BENCHMARK_NOSPECTRAL_HYBRID_CUSTOM:
		case BENCHMARK_SPECTRAL_HYBRID_GPU:
		case BENCHMARK_SPECTRAL_HYBRID_CUSTOM: {
			triangleCount = luxGetDoubleAttribute("renderer_statistics", "triangleCount");

			// Get the list of device names
			char buf[4096];
			luxGetStringAttribute("renderer_statistics", "deviceNames", buf, 4096);

			string names(buf);
			if (names.length() > 0) {
				boost::split(deviceNames, names, boost::is_any_of(","));

				// Get each device statistics
				minPerf = numeric_limits<double>::infinity();
				totalPerf = 0.0;
				for (u_int i = 0; i < deviceNames.size(); ++i) {
					const string attrName = "device." + boost::lexical_cast<string>(i);

					const double raySecs = luxGetDoubleAttribute("renderer_statistics", (attrName + ".raysecs").c_str());
					deviceRaysSecs.push_back(raySecs);
					deviceMaxMem.push_back(luxGetDoubleAttribute("renderer_statistics", (attrName + ".maxmem").c_str()));
					deviceMem.push_back(luxGetDoubleAttribute("renderer_statistics", (attrName + ".memusage").c_str()));

					minPerf = Min(raySecs, minPerf);
					totalPerf += raySecs;
				}
			}
			break;
		}
		case STRESSTEST_SPECTRAL_NATIVE_BIDIR:
		case BENCHMARK_NOSPECTRAL_NATIVE_PATH:
		case BENCHMARK_SPECTRAL_NATIVE_PATH:
		case BENCHMARK_SPECTRAL_NATIVE_BIDIR:
			break;
		default: {
			LM_LOG("<FONT COLOR=\"#ff0000\">Unknown render mode in LuxMarkApp::RenderRefreshTimeout(): " << mode << "</FONT>");
			break;
		}
	}

	// Get the list of device names
	// After 120secs of benchmark, show the result dialog
	const bool benchmarkDone = (renderingTime > 120) &&
		(mode != STRESSTEST_NOSPECTRAL_OCL_GPU) &&
		(mode != STRESSTEST_NOSPECTRAL_OCL_CPUGPU) &&
		(mode != STRESSTEST_NOSPECTRAL_OCL_GPU) &&
		(mode != STRESSTEST_SPECTRAL_NATIVE_BIDIR);

	char buf[512];
	stringstream ss("");

	char validBuf[128];
	if (benchmarkDone)
		strcpy(validBuf, " (OK)");
	else {
		if ((mode != STRESSTEST_NOSPECTRAL_OCL_GPU) &&
				(mode != STRESSTEST_NOSPECTRAL_OCL_CPUGPU) &&
				(mode != STRESSTEST_NOSPECTRAL_OCL_GPU) &&
				(mode != STRESSTEST_SPECTRAL_NATIVE_BIDIR))
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
			renderingTime, validBuf, int(sampleSec / 1000.0),
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
			const vector<BenchmarkDeviceDescription> descs;

			Stop();

			ResultDialog *dialog = new ResultDialog(mode, sceneName, sampleSec, descs,
					pixels, width, height);
			dialog->exec();
			delete dialog;

			// Go in PAUSE mode
			InitRendering(PAUSE, sceneName);
		}
	}
}
