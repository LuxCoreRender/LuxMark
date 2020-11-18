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

#include <limits>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <QFile>
#include <QGraphicsSceneMouseEvent>

#include "luxmarkcfg.h"
#include "luxmarkapp.h"
#include "luxcoreuidialog.h"
#include "resultdialog.h"
#include "luxmarkdefs.h"

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

using namespace luxrays;

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
	// Initialize rand() number generator
	srand(time(NULL));

	// Initialize LuxRender API
	luxcore::Init(LuxCoreErrorHandler);

	singleRun = false;
	singleRunExtInfo = false;

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
	exePath = boost::filesystem::path(boost::filesystem::initial_path<boost::filesystem::path>());
#else

    // Look for the directory where Lux executables are
	const boost::filesystem::path luxMarkExe(argv[0]);
	exePath = boost::filesystem::absolute(luxMarkExe).parent_path();
#endif
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

void LuxMarkApp::Init(LuxMarkAppMode mode, const bool useCPU,
		const string &enabledDevices, const string &enabledOptixDevices,
		const char *scnName, const bool single, const bool extInfo) {
	mainWin = new MainWindow();
	mainWin->setWindowTitle("LuxMark v" LUXMARK_VERSION_MAJOR "." LUXMARK_VERSION_MINOR);
	mainWin->show();
	LogWindow = mainWin;
	singleRun = single;
	singleRunExtInfo = extInfo;

	LM_LOG("<FONT COLOR=\"#0000ff\">LuxMark v" << LUXMARK_VERSION_MAJOR << "." << LUXMARK_VERSION_MINOR << "</FONT>");
	LM_LOG("Based on <FONT COLOR=\"#0000ff\">LuxCore v" << LUXCORE_VERSION_MAJOR << "." << LUXCORE_VERSION_MINOR << "</FONT>");

	// Initialize hardware information
	hardwareTreeModel = new HardwareTreeModel(mainWin, useCPU, enabledDevices, enabledOptixDevices);
	mainWin->SetHardwareTreeModel(hardwareTreeModel);

	InitRendering(mode, scnName);
}

void LuxMarkApp::SetMode(LuxMarkAppMode m) {
	InitRendering(m, sceneName);
}

void LuxMarkApp::SetScene(const char *scnName) {
	InitRendering(mode, scnName);
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

	if (!strcmp(scnName, SCENE_WALLPAPER)) {
		mainWin->SetSceneCheck(0);

		// Wall Paper scene can be rendered only with BiDir
		switch (mode) {
			case BENCHMARK_CUDA_GPU:
			case BENCHMARK_OCL_GPU:
			case BENCHMARK_CUSTOM:
				mode = BENCHMARK_NATIVE;
				break;
			case STRESSTEST_CUDA_GPU:
			case STRESSTEST_OCL_GPU:
			case STRESSTEST_CUSTOM:
				mode = STRESSTEST_NATIVE;
				break;
			case DEMO_LUXCOREUI:
				// Nothing to do
				break;
			case PAUSE:
			default:
				break;
		}
	} else if (!strcmp(scnName, SCENE_HALLBENCH))
		mainWin->SetSceneCheck(1);
	else if (!strcmp(scnName, SCENE_CANNELLE_ET_FROMAGE))
		mainWin->SetSceneCheck(2);

	// Initialize the new mode
	mainWin->SetModeCheck(mode);
	if ((mode == PAUSE) || (mode == DEMO_LUXCOREUI)) {
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
	} else if (mode == DEMO_LUXCOREUI) {
		// Show LuxCoreUI dialog
		LuxCoreUIDialog *dialog = new LuxCoreUIDialog(sceneName, exePath);

		dialog->exec();
		delete dialog;
		mainWin->raise(); // Make LuxMark GUI front most win 
		
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
		const string optixSelection = (app->hardwareTreeModel) ?
			(app->hardwareTreeModel->getOptixSelectionString()) : "";
		const bool cpuSelection = (app->hardwareTreeModel) ?
			(app->hardwareTreeModel->getUseNativeCPU()) : false;

		app->luxSession = new LuxCoreRenderSession(sname, app->mode,
				deviceSelection, optixSelection, cpuSelection);

		// SetUp the rendering
		app->luxSession->SetUp();

		// Check if I need to show the kernel compilation dialog
		if (!app->luxSession->HasCachedKernels()) {
			LM_SHOW_KERNEL_COMPILATION_DIALOG;

			// Start the rendering
			app->luxSession->Start();
			
			LM_HIDE_KERNEL_COMPILATION_DIALOG;
		} else {
			// Start the rendering
			app->luxSession->Start();			
		}

		// Done
		app->renderingStartTime = luxrays::WallClockTime();
		app->lastFrameBufferDenoisedUpdate = app->renderingStartTime;
		app->engineInitDone = true;
	} catch (runtime_error &err) {
		LM_ERROR("RUNTIME ERROR: " << err.what());
	} catch (exception &err) {
		LM_ERROR("ERROR: " << err.what());
	}
}

void LuxMarkApp::RenderRefreshTimeout() {
	if (!engineInitDone)
		return;

	const bool isStressTest = (mode == STRESSTEST_CUDA_GPU) ||
		(mode == STRESSTEST_OCL_GPU) ||
		(mode == STRESSTEST_NATIVE) ||
		(mode == STRESSTEST_CUSTOM);

	const Properties &stats = luxSession->GetStats();
	const double renderingTime = stats.Get("stats.renderengine.time").Get<double>();

	// Update shown image
	const int width = luxSession->GetFrameBufferWidth();
	const int height = luxSession->GetFrameBufferHeight();
	
	const float *frameBuffer = luxSession->UpdateFrameBuffer(0);
	const float *frameBufferDenoised = NULL;
	if (isStressTest) {
		// Check it is time to refresh the denoised frame buffer
		const double deltaTime = luxrays::WallClockTime() - lastFrameBufferDenoisedUpdate;
		if (((renderingTime < 30.0) && (deltaTime > 10.0)) ||
				((renderingTime < 30.0 + 60.0) && (deltaTime > 20.0)) ||
				((renderingTime < 30.0 + 60.0 + 90.0) && (deltaTime > 30.0)) ||
				(deltaTime > 300.0)) {
			frameBufferDenoised = luxSession->UpdateFrameBuffer(1);
			lastFrameBufferDenoisedUpdate = luxrays::WallClockTime();
		} else
			frameBufferDenoised = luxSession->GetFrameBufferPtr(1);
	}

	mainWin->ShowFrameBuffer(frameBuffer, frameBufferDenoised, width, height);

	// To save reference image
	/*
	{
		const double sampleCount = stats.Get("stats.renderengine.total.samplecount").Get<double>();
		const unsigned char *pixels = mainWin->GetFrameBuffer();

		const u_int samlePerPixel = (u_int)(sampleCount / (width * height));
		LM_LOG("Samples per pixel: " << samlePerPixel);
		static u_int savedSpp = 0;
		if (samlePerPixel / 100 > savedSpp) {
			LM_LOG("Saving reference...");
			const QByteArray data = QByteArray::fromRawData((char *)pixels, width * height * 3);
			QFile refImage("reference.raw");
			refImage.open(QIODevice::WriteOnly);
			refImage.write(data);
			refImage.close();
			++savedSpp;
		}
	}
	*/

	//--------------------------------------------------------------------------
	// Update the statistics
	//--------------------------------------------------------------------------

	const double sampleCount = stats.Get("stats.renderengine.total.samplecount").Get<double>();
	const double sampleSec = (renderingTime > 0.0) ? (sampleCount / renderingTime) : 0.0;

	vector<string> deviceNames;
	vector<double> deviceRaysSecs;
	vector<double> deviceMem, deviceMaxMem;
	vector<bool> deviceIsOpenCL;
	vector<bool> deviceIsCUDA;
	bool hasOpenCLDevices = false;
	bool hasCUDADevices = false;
	bool hasNativeDevices = false;
	double triangleCount = 0.0;
	double minPerf = 0.0;
	double totalPerf = 0.0;

	triangleCount = stats.Get("stats.dataset.trianglecount").Get<double>();

	// Get each device statistics
	minPerf = numeric_limits<double>::infinity();
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

		const string deviceType = stats.Get("stats.renderengine.devices." + deviceName + ".type").Get<string>();

		const bool isOpenCLDevice = boost::starts_with(deviceType, "OPENCL_") ? true : false;
		deviceIsOpenCL.push_back(isOpenCLDevice);

		const bool isCUDADevice = boost::starts_with(deviceType, "CUDA_") ? true : false;
		deviceIsCUDA.push_back(isCUDADevice);

		const bool isNativeDevice = boost::starts_with(deviceType, "NATIVE_") ? true : false;

		hasOpenCLDevices = (hasOpenCLDevices || isOpenCLDevice);
		hasCUDADevices = (hasCUDADevices || isCUDADevice);
		hasNativeDevices = (hasNativeDevices || isNativeDevice);
	}

	// Get the list of device names
	// After 120secs of benchmark, show the result dialog
	const bool benchmarkDone = (renderingTime > 120.0) && (!isStressTest);

	char buf[512];
	stringstream ss("");

	char validBuf[128];
	if (benchmarkDone)
		strcpy(validBuf, " (OK)");
	else {
		if (!isStressTest)
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

	sprintf(buf, "[Mode: %s][Time: %dsecs%s][Samples/sec % 6dK][Samples/pixel %.1f]%s",
			LuxMarkAppMode2String(mode).c_str(),
			int(renderingTime), validBuf, int(sampleSec / 1000.0),
			sampleCount / (width * height),
			triCountBuf);
	ss << buf;

	if (hasOpenCLDevices) {
		ss << "\n\nOpenCL rendering devices:";
		for (size_t i = 0; i < deviceNames.size(); ++i) {
			if (deviceIsOpenCL[i]) {
				sprintf(buf, "\n    [%s][Rays/sec % 3dK][Prf Idx %.2f][Wrkld %.1f%%][Mem %.1fM/%dM]",
						deviceNames[i].c_str(),
						int(deviceRaysSecs[i] / 1000.0),
						(minPerf > 0.0) ? (deviceRaysSecs[i] / minPerf) : 0.f,
						(totalPerf > 0.0) ? (100.0 * deviceRaysSecs[i] / totalPerf) : 0.f,
						deviceMem[i] / (1024 * 1024), int(deviceMaxMem[i] / (1024 * 1024)));
				ss << buf;
			}
		}
	}

	if (hasCUDADevices) {
		ss << "\n\nCUDA rendering devices:";
		for (size_t i = 0; i < deviceNames.size(); ++i) {
			if (deviceIsCUDA[i]) {
				sprintf(buf, "\n    [%s][Rays/sec % 3dK][Prf Idx %.2f][Wrkld %.1f%%][Mem %.1fM/%dM]",
						deviceNames[i].c_str(),
						int(deviceRaysSecs[i] / 1000.0),
						(minPerf > 0.0) ? (deviceRaysSecs[i] / minPerf) : 0.f,
						(totalPerf > 0.0) ? (100.0 * deviceRaysSecs[i] / totalPerf) : 0.f,
						deviceMem[i] / (1024 * 1024), int(deviceMaxMem[i] / (1024 * 1024)));
				ss << buf;
			}
		}
	}

	if (hasNativeDevices) {
		ss << "\n\nNative C++ rendering devices:";
		for (size_t i = 0; i < deviceNames.size(); ++i) {
			if (!deviceIsOpenCL[i] && !deviceIsCUDA[i]) {
				sprintf(buf, "\n    [%s][Rays/sec % 3dK][Prf Idx %.2f][Wrkld %.1f%%][Mem %.1fM/%dM]",
						deviceNames[i].c_str(),
						int(deviceRaysSecs[i] / 1000.0),
						(minPerf > 0.0) ? (deviceRaysSecs[i] / minPerf) : 0.f,
						(totalPerf > 0.0) ? (100.0 * deviceRaysSecs[i] / totalPerf) : 0.f,
						deviceMem[i] / (1024 * 1024), int(deviceMaxMem[i] / (1024 * 1024)));
				ss << buf;
			}
		}
	}

	mainWin->UpdateScreenLabel(ss.str().c_str());

	//--------------------------------------------------------------------------
	// End the benchmark if it is time
	//--------------------------------------------------------------------------

	if (benchmarkDone) {
		// Check if I'm in single run mode
		if (singleRun && !singleRunExtInfo) {
			// The case (singleRun && singleRunExtInfo) is handled inside ResultDialog()
			cout << "Score: " << int(sampleSec / 1000.0) << endl;

			exit(EXIT_SUCCESS);
		} else {
			mainWin->ShowFrameBuffer(luxSession->UpdateFrameBuffer(0), luxSession->UpdateFrameBuffer(1), width, height);

			Stop();

            vector<BenchmarkDeviceDescription> descs = hardwareTreeModel->getSelectedDeviceDescs(mode);
			const unsigned char *pixels = mainWin->GetFrameBuffer();
			ResultDialog *dialog = new ResultDialog(mode, sceneName, sampleSec,
                    descs, pixels, width, height,
					singleRun && singleRunExtInfo);
			dialog->exec();
			delete dialog;

			// Go in PAUSE mode
			InitRendering(PAUSE, sceneName);
		}
	}
}
