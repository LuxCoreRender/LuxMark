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

#include "luxrendersession.h"

LuxRenderSession::LuxRenderSession(const std::string &fileName, const LuxMarkAppMode mode,
		const string devSelection) {
	// Save the current directory
	originalCurrentDirectory = boost::filesystem::current_path();

	sceneFileName = boost::filesystem::system_complete(fileName).string();
	renderMode = mode;
	deviceSelection = devSelection;

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

	switch (session->renderMode) {
		case BENCHMARK_NOSPECTRAL_OCL_GPU: {
			luxRenderer("slg", "string config", "[\"screen.refresh.interval = 2000\" \"opencl.gpu.use = 1\" \"opencl.cpu.use = 0\"]", LUX_NULL);
			luxSurfaceIntegrator("path", "integer maxdepth", &maxpath, LUX_NULL);
			break;
		}
		case BENCHMARK_NOSPECTRAL_OCL_CPUGPU: {
			luxRenderer("slg", "string config", "[\"screen.refresh.interval = 2000\" \"opencl.gpu.use = 1\" \"opencl.cpu.use = 1\"]", LUX_NULL);
			luxSurfaceIntegrator("path", "integer maxdepth", &maxpath, LUX_NULL);
			break;
		}
		case BENCHMARK_NOSPECTRAL_OCL_CPU: {
			luxRenderer("slg", "string config", "[\"screen.refresh.interval = 2000\" \"opencl.gpu.use = 0\" \"opencl.cpu.use = 1\"]", LUX_NULL);
			luxSurfaceIntegrator("path", "integer maxdepth", &maxpath, LUX_NULL);
			break;
		}
		case BENCHMARK_NOSPECTRAL_OCL_CUSTOM: {
			// At the first run, hardwareTreeModel is NULL
			if (session->deviceSelection == "")
				luxRenderer("slg", "string config", "[\"screen.refresh.interval = 2000\" \"opencl.gpu.use = 1\" \"opencl.cpu.use = 0\"]", LUX_NULL);
			else {
				const string opts = "[\"screen.refresh.interval = 2000\" \"opencl.devices.select = " + session->deviceSelection + "\"]";
				luxRenderer("slg", "string config", opts.c_str(), LUX_NULL);
			}

			luxSurfaceIntegrator("path", "integer maxdepth", &maxpath, LUX_NULL);
			break;
		}
		case BENCHMARK_SPECTRAL_NATIVE_PATH: {
			luxRenderer("sampler", LUX_NULL);
			luxSurfaceIntegrator("path", "integer maxdepth", &maxpath, LUX_NULL);
			break;
		}
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
