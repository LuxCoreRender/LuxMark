/***************************************************************************
 *   Copyright (C) 1998-2013 by authors (see AUTHORS.txt)                  *
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
 *   LuxMark website: http://www.luxrender.net                             *
 ***************************************************************************/

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include "luxmarkapp.h"
#include "luxrendersession.h"
#include "mainwindow.h"

LuxRenderSession::LuxRenderSession(const std::string &fileName, const LuxMarkAppMode mode,
		const string &slgDevSel, const string &luxDevSel) {
	// Save the current directory
	originalCurrentDirectory = boost::filesystem::current_path();
	
#ifdef __APPLE__ // workaround for reliable find the bundlepath/scenefiles especially in 10.9	
	LuxMarkApp* app = dynamic_cast<LuxMarkApp*>(QCoreApplication::instance());
	if(app)
	{
		bundleDir = app->GetExePath().parent_path().parent_path(); // LuxMark.app/Contents
	}
	cout << bundleDir << endl;
	sceneFileName = bundleDir.string() + (string)"/" + boost::filesystem::path(fileName).string();
#else
	sceneFileName = boost::filesystem::system_complete(fileName).string();
#endif
	renderMode = mode;
	slgDeviceSelection = slgDevSel;
	luxDeviceSelection = luxDevSel;

	parseThread = NULL;
	started = false;
	parseError = false;
}

LuxRenderSession::~LuxRenderSession() {
	if (started)
		Stop();

	// Restore the current directory
	boost::filesystem::current_path(originalCurrentDirectory);
}

void LuxRenderSession::RenderthreadImpl(LuxRenderSession *session) {
	// Disable the start of the rendering at WorldEnd so I can
	// overwrite Renderer settings
	luxStartRenderingAfterParse(false);

	// Parse the scene
	luxParse(session->sceneFileName.c_str());

	// Overwrite the Renderer settings
	const int maxpath = 12;
	const int raybuffersize = 16383;

	switch (session->renderMode) {
		//----------------------------------------------------------------------
		// No Spectral
		//----------------------------------------------------------------------
		case STRESSTEST_NOSPECTRAL_OCL_GPU:
		case BENCHMARK_NOSPECTRAL_OCL_GPU: {
			luxRenderer("slg", "string config", "["
					"\"screen.refresh.interval = 2000\" "
					"\"opencl.gpu.use = 1\" "
					"\"opencl.cpu.use = 0\" "
					"\"renderengine.type = PATHOCL\"]", LUX_NULL);
			luxSurfaceIntegrator("path", "integer maxdepth", &maxpath, LUX_NULL);
			break;
		}
		case STRESSTEST_NOSPECTRAL_OCL_CPUGPU:
		case BENCHMARK_NOSPECTRAL_OCL_CPUGPU: {
			luxRenderer("slg", "string config", "["
					"\"screen.refresh.interval = 2000\" "
					"\"opencl.gpu.use = 1\" "
					"\"opencl.cpu.use = 1\" "
					"\"renderengine.type = PATHOCL\"]", LUX_NULL);
			luxSurfaceIntegrator("path", "integer maxdepth", &maxpath, LUX_NULL);
			break;
		}
		case STRESSTEST_NOSPECTRAL_OCL_CPU:
		case BENCHMARK_NOSPECTRAL_OCL_CPU: {
			luxRenderer("slg", "string config", "["
					"\"screen.refresh.interval = 2000\" "
					"\"opencl.gpu.use = 0\" "
					"\"opencl.cpu.use = 1\" "
					"\"renderengine.type = PATHOCL\"]", LUX_NULL);
			luxSurfaceIntegrator("path", "integer maxdepth", &maxpath, LUX_NULL);
			break;
		}
		case BENCHMARK_NOSPECTRAL_OCL_CUSTOM: {
			// At the first run, hardwareTreeModel is NULL
			if (session->slgDeviceSelection == "")
				luxRenderer("slg", "string config", "["
					"\"screen.refresh.interval = 2000\" "
					"\"opencl.gpu.use = 1\" "
					"\"opencl.cpu.use = 0\" "
					"\"renderengine.type = PATHOCL\"]", LUX_NULL);
			else {
				const string opts = "["
					"\"screen.refresh.interval = 2000\" "
					"\"opencl.devices.select = " + session->slgDeviceSelection + "\" "
					"\"renderengine.type = PATHOCL\"]";
				luxRenderer("slg", "string config", opts.c_str(), LUX_NULL);
			}

			luxSurfaceIntegrator("path", "integer maxdepth", &maxpath, LUX_NULL);
			break;
		}
		case BENCHMARK_NOSPECTRAL_HYBRID_GPU: {
			luxRenderer("slg", "string config", "["
					"\"screen.refresh.interval = 2000\" "
					"\"opencl.gpu.use = 1\" "
					"\"opencl.cpu.use = 0\" "
					"\"renderengine.type = PATHHYBRID\"]", LUX_NULL);
			luxSurfaceIntegrator("path", "integer maxdepth", &maxpath, LUX_NULL);
			break;
		}
		case BENCHMARK_NOSPECTRAL_HYBRID_CUSTOM: {
			// At the first run, hardwareTreeModel is NULL
			if (session->slgDeviceSelection == "")
				luxRenderer("slg", "string config", "["
					"\"screen.refresh.interval = 2000\" "
					"\"opencl.gpu.use = 1\" "
					"\"opencl.cpu.use = 0\" "
					"\"renderengine.type = PATHHYBRID\"]", LUX_NULL);
			else {
				const string opts = "["
					"\"screen.refresh.interval = 2000\" "
					"\"opencl.devices.select = " + session->slgDeviceSelection + "\" "
					"\"renderengine.type = PATHHYBRID\"]";
				luxRenderer("slg", "string config", opts.c_str(), LUX_NULL);
			}

			luxSurfaceIntegrator("path", "integer maxdepth", &maxpath, LUX_NULL);
			break;
		}
		case BENCHMARK_NOSPECTRAL_NATIVE_PATH: {
			luxRenderer("slg", "string config", "["
					"\"screen.refresh.interval = 2000\" "
					"\"renderengine.type = PATHCPU\"]", LUX_NULL);
			luxSurfaceIntegrator("path", "integer maxdepth", &maxpath, LUX_NULL);
			break;
		}
		//----------------------------------------------------------------------
		// Spectral
		//----------------------------------------------------------------------
		case BENCHMARK_SPECTRAL_HYBRID_GPU: {
			luxRenderer("hybrid",
					"integer raybuffersize", &raybuffersize,
					LUX_NULL);
			luxSurfaceIntegrator("path", "integer maxdepth", &maxpath, LUX_NULL);
			break;
		}
		case BENCHMARK_SPECTRAL_HYBRID_CUSTOM: {
			// At the first run, hardwareTreeModel is NULL
			if (session->luxDeviceSelection == "")
				luxRenderer("hybrid",
						"integer raybuffersize", &raybuffersize,
						LUX_NULL);
			else {
				const string opts = session->luxDeviceSelection;
				luxRenderer("hybrid",
						"integer raybuffersize", &raybuffersize,
						"string opencl.devices.select", opts.c_str(),
						LUX_NULL);
			}

			luxSurfaceIntegrator("path", "integer maxdepth", &maxpath, LUX_NULL);
			break;
		}
		case BENCHMARK_SPECTRAL_NATIVE_PATH: {
			luxRenderer("sampler", LUX_NULL);
			luxSurfaceIntegrator("path", "integer maxdepth", &maxpath, LUX_NULL);
			break;
		}
		//----------------------------------------------------------------------
		// Advanced Spectral
		//----------------------------------------------------------------------
		case STRESSTEST_SPECTRAL_NATIVE_BIDIR:
		case BENCHMARK_SPECTRAL_NATIVE_BIDIR: {
			luxRenderer("sampler", LUX_NULL);
			luxSurfaceIntegrator("bidirectional", "integer eyedepth", &maxpath, "integer lightdepth", &maxpath, LUX_NULL);
			break;
		}
		default: {
			LM_LOG("<FONT COLOR=\"#ff0000\">Unknown render mode in LuxRenderSession::RenderthreadImpl(): " << session->renderMode << "</FONT>");
			break;
		}
	}

	// Close the parsing phase. This function returns only at the end of the rendering.
	luxParseEnd();

	if (luxStatistics("sceneIsReady") == 0.)
        session->parseError = true;
}

void LuxRenderSession::Start() {
	assert (!started);
	started = true;

	luxDisableRandomMode();

	// Change the current working directory to the one storing the scene
	boost::filesystem::path sceneFileComplete(boost::filesystem::system_complete(sceneFileName));
	boost::filesystem::path workingDirectory = sceneFileComplete.parent_path();
	boost::filesystem::current_path(workingDirectory);

	// Create the thread for the scene parsing
	parseThread = new boost::thread(&RenderthreadImpl, this);
	
	// Wait the scene parsing to finish
	while (!luxStatistics("sceneIsReady") && !parseError)
		boost::this_thread::sleep(boost::posix_time::millisec(250));

	// Add rendering threads
	int threadsToAdd = boost::thread::hardware_concurrency();
	while (--threadsToAdd)
		luxAddThread();
}

void LuxRenderSession::Stop() {
	assert (started);

	if (!parseError) {
		// Make sure lux core shuts down
		int haltspp = luxGetIntAttribute("film", "haltSamplesPerPixel");
		luxSetHaltSamplesPerPixel(haltspp, true, false);
	}

	parseThread->join();
	delete parseThread;
	parseThread = NULL;

	if (!parseError) {
		luxWait();
		luxCleanup();
	}
}
