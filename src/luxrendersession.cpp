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
#include "mainwindow.h"
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

using namespace luxrays;
using namespace luxcore;

LuxRenderSession::LuxRenderSession(const std::string &fileName, const LuxMarkAppMode mode,
		const string &devSel) {
	// Save the current directory
	originalCurrentDirectory = boost::filesystem::current_path();
	
#ifdef __APPLE__ // workaround for reliable find the bundlepath/scenefiles especially in 10.9	
	boost::filesystem::path exePath;
	char result[1024]; // too short perhaps ?
	uint32_t size=1023;
	if (!_NSGetExecutablePath(result, &size)) {
		exePath=string(result);
		boost::filesystem::current_path(exePath.parent_path().parent_path()); // LuxMark.app/Contents, where we now have the scenes dir
	}
	sceneFileName = exePath.parent_path().parent_path().string() + (string)"/" + boost::filesystem::path(fileName).string();
#else
	sceneFileName = boost::filesystem::system_complete(fileName).string();
#endif
	renderMode = mode;

	config = NULL;
	session = NULL;

	started = false;
}

LuxRenderSession::~LuxRenderSession() {
	if (started)
		Stop();

	// Restore the current directory
	boost::filesystem::current_path(originalCurrentDirectory);
}

void LuxRenderSession::Start() {
	assert (!started);
	started = true;

	// Change the current working directory to the one storing the scene
	boost::filesystem::path sceneFileComplete(boost::filesystem::system_complete(sceneFileName));
	boost::filesystem::path workingDirectory = sceneFileComplete.parent_path();
	boost::filesystem::current_path(workingDirectory);

	// Load the configuration from file
	Properties props(sceneFileName.c_str());
	props << Property("screen.refresh.interval")(2000);

	switch (renderMode) {
		//----------------------------------------------------------------------
		// BENCHMARK and STRESSTEST
		//----------------------------------------------------------------------
		case STRESSTEST_OCL_GPU:
		case BENCHMARK_OCL_GPU: {
			props <<
					Property("opencl.gpu.use")(true) <<
					Property("opencl.cpu.use")(false) <<
					Property("renderengine.type")("PATHOCL");
			break;
		}
		case STRESSTEST_OCL_CPUGPU:
		case BENCHMARK_OCL_CPUGPU: {
			props <<
					Property("opencl.gpu.use")(true) <<
					Property("opencl.cpu.use")(true) <<
					Property("renderengine.type")("PATHOCL");
			break;
		}
		case STRESSTEST_OCL_CPU:
		case BENCHMARK_OCL_CPU: {
			props <<
					Property("opencl.gpu.use")(false) <<
					Property("opencl.cpu.use")(true) <<
					Property("renderengine.type")("PATHOCL");
			break;
		}
		case BENCHMARK_OCL_CUSTOM: {
			// At the first run, hardwareTreeModel is NULL
			if (deviceSelection == "") {
				props <<
						Property("opencl.gpu.use")(true) <<
						Property("opencl.cpu.use")(false) <<
						Property("renderengine.type")("PATHOCL");
			} else {
				props <<
						Property("opencl.devices.select")(deviceSelection) <<
						Property("renderengine.type")("PATHOCL");
			}
			break;
		}
		case BENCHMARK_HYBRID_GPU: {
			props <<
					Property("opencl.gpu.use")(true) <<
					Property("opencl.cpu.use")(false) <<
					Property("renderengine.type")("PATHHYBRID");
			break;
		}
		case BENCHMARK_HYBRID_CUSTOM: {
			// At the first run, hardwareTreeModel is NULL
			if (deviceSelection == "") {
				props <<
						Property("opencl.gpu.use")(true) <<
						Property("opencl.cpu.use")(false) <<
						Property("renderengine.type")("PATHHYBRID");
			} else {
				props <<
						Property("opencl.devices.select")(deviceSelection) <<
						Property("renderengine.type")("PATHHYBRID");
			}
			break;
		}
		case BENCHMARK_NATIVE_PATH: {
			props <<
					Property("renderengine.type")("PATHCPU");
			break;
		}
		default: {
			LM_LOG("<FONT COLOR=\"#ff0000\">Unknown render mode in LuxRenderSession::RenderthreadImpl(): " << renderMode << "</FONT>");
			break;
		}
	}

	config = new RenderConfig(props);
	session = new RenderSession(config);

	session->Start();
}

void LuxRenderSession::Stop() {
	assert (started);

	delete session;
	delete config;
}

const float *LuxRenderSession::GetFrameBuffer() const {
	return session->GetFilm().GetChannel<float>(Film::CHANNEL_RGB_TONEMAPPED);
}

u_int LuxRenderSession::GetFrameBufferWidth() const {
	return session->GetFilm().GetWidth();
}

u_int LuxRenderSession::GetFrameBufferHeight() const {
	return session->GetFilm().GetHeight();
}

const Properties &LuxRenderSession::GetStats() const {
	session->UpdateStats();
	return session->GetStats();
}
