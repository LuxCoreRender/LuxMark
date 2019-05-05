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

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include "luxcorerendersession.h"
#include "luxmarkapp.h"
#include "mainwindow.h"

using namespace luxrays;
using namespace luxcore;

LuxCoreRenderSession::LuxCoreRenderSession(const std::string &fileName, const LuxMarkAppMode mode,
		const string &devSel, const string &oclCompOpts) {
    renderMode = mode;
    
    sceneFileName = fileName;
    deviceSelection = devSel;
	oclCompilerOpts = oclCompOpts;

	config = NULL;
	session = NULL;

	started = false;
}

LuxCoreRenderSession::~LuxCoreRenderSession() {
	if (started)
		Stop();
}

void LuxCoreRenderSession::Start() {
	assert (!started);
	started = true;

	// Clear the file name resolver list
	luxcore::ClearFileNameResolverPaths();
	// Add the current directory to the list of place where to look for files
	luxcore::AddFileNameResolverPath(".");
	// Add the .cfg directory to the list of place where to look for files
	boost::filesystem::path path(sceneFileName);
	luxcore::AddFileNameResolverPath(path.parent_path().generic_string());

	// Load the configuration from file
	Properties props(sceneFileName.c_str());
	props << Property("screen.refresh.interval")(2000);

	//--------------------------------------------------------------------------
	// BENCHMARK and STRESSTEST render modes
	//--------------------------------------------------------------------------

	switch (renderMode) {
		case STRESSTEST_OCL_GPU:
		case BENCHMARK_OCL_GPU: {
			props <<
					Property("opencl.gpu.use")(true) <<
					Property("opencl.cpu.use")(false) <<
					Property("opencl.native.threads.count")(0) <<
					Property("native.threads.count")(0) <<
					Property("opencl.kernel.options")(oclCompilerOpts) <<
					Property("renderengine.type")("PATHOCL");
			break;
		}
		case STRESSTEST_OCL_CPUGPU:
		case BENCHMARK_OCL_CPUGPU: {
			props <<
					Property("opencl.gpu.use")(true) <<
					Property("opencl.cpu.use")(true) <<
					Property("opencl.native.threads.count")(0) <<
					Property("native.threads.count")(0) <<
					Property("opencl.kernel.options")(oclCompilerOpts) <<
					Property("renderengine.type")("PATHOCL");
			break;
		}
		case STRESSTEST_OCL_CPU:
		case BENCHMARK_OCL_CPU: {
			props <<
					Property("opencl.gpu.use")(false) <<
					Property("opencl.cpu.use")(true) <<
					Property("opencl.native.threads.count")(0) <<
					Property("native.threads.count")(0) <<
					Property("opencl.kernel.options")(oclCompilerOpts) <<
					Property("renderengine.type")("PATHOCL");
			break;
		}
		case BENCHMARK_OCL_CUSTOM: {
			// At the first run, hardwareTreeModel is NULL
			if (deviceSelection == "") {
				props <<
						Property("opencl.gpu.use")(true) <<
						Property("opencl.cpu.use")(false);
			} else {
				props <<
						Property("opencl.devices.select")(deviceSelection);
			}

			props <<
					Property("opencl.native.threads.count")(0) <<
					Property("native.threads.count")(0) <<
					Property("opencl.kernel.options")(oclCompilerOpts) <<
					Property("renderengine.type")("PATHOCL");
			break;
		}
		case BENCHMARK_HYBRID: {
			props <<
					Property("opencl.gpu.use")(true) <<
					Property("opencl.cpu.use")(false) <<
					Property("opencl.native.threads.count")(boost::thread::hardware_concurrency()) <<
					Property("native.threads.count")(0) <<
					Property("opencl.kernel.options")(oclCompilerOpts) <<
					Property("renderengine.type")("PATHOCL");
			break;
		}
		case BENCHMARK_HYBRID_CUSTOM: {
			// At the first run, hardwareTreeModel is NULL
			if (deviceSelection == "") {
				props <<
						Property("opencl.gpu.use")(true) <<
						Property("opencl.cpu.use")(false);
			} else {
				props <<
						Property("opencl.devices.select")(deviceSelection);
			}

			props <<
					Property("opencl.native.threads.count")(boost::thread::hardware_concurrency()) <<
					Property("native.threads.count")(0) <<
					Property("opencl.kernel.options")(oclCompilerOpts) <<
					Property("renderengine.type")("PATHOCL");
			break;
		}
		case BENCHMARK_NATIVE: {
			props <<
					Property("native.threads.count")(boost::thread::hardware_concurrency()) <<
					Property("renderengine.type")((sceneFileName == SCENE_WALLPAPER) ? "BIDIRCPU" : "PATHCPU");
			break;
		}
		default: {
			LM_LOG("<FONT COLOR=\"#ff0000\">Unknown render mode in LuxRenderSession::RenderthreadImpl(): " << renderMode << "</FONT>");
			break;
		}
	}

	config = RenderConfig::Create(props);
	session = RenderSession::Create(config);

	session->Start();
}

void LuxCoreRenderSession::Stop() {
	assert (started);

	delete session;
	delete config;
}

const float *LuxCoreRenderSession::GetFrameBuffer(const u_int imagePipelineIndex) const {
	return session->GetFilm().GetChannel<float>(Film::CHANNEL_IMAGEPIPELINE, imagePipelineIndex);
}

u_int LuxCoreRenderSession::GetFrameBufferWidth() const {
	return session->GetFilm().GetWidth();
}

u_int LuxCoreRenderSession::GetFrameBufferHeight() const {
	return session->GetFilm().GetHeight();
}

const Properties &LuxCoreRenderSession::GetStats() const {
	session->UpdateStats();
	return session->GetStats();
}
