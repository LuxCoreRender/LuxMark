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

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
#include <Windows.h>
#endif

using namespace luxrays;
using namespace luxcore;

LuxCoreRenderSession::LuxCoreRenderSession(const std::string &fileName, const LuxMarkAppMode mode,
		const string &devSel, const string &optixSel, const bool cpuSel) {
    renderMode = mode;
    
    sceneFileName = fileName;
    deviceSelection = devSel;
	optixSelection = optixSel;
	cpuSelection = cpuSel;

	config = NULL;
	session = NULL;

	started = false;
}

LuxCoreRenderSession::~LuxCoreRenderSession() {
	if (started)
		Stop();
}

void LuxCoreRenderSession::SetUp() {
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

	const unsigned int threadsCount =
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
		(unsigned int)GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
#else
		boost::thread::hardware_concurrency();
#endif
	
	switch (renderMode) {
		case BENCHMARK_CUDA_GPU:
		case STRESSTEST_CUDA_GPU: {
			props <<
					Property("opencl.gpu.use")(true) <<
					Property("opencl.cpu.use")(false) <<
					Property("opencl.native.threads.count")(0) <<
					Property("native.threads.count")(0) <<
					Property("renderengine.type")("PATHOCL");
			break;
		}
		case BENCHMARK_OCL_GPU:
		case STRESSTEST_OCL_GPU: {
			props <<
					Property("opencl.gpu.use")(true) <<
					Property("opencl.cpu.use")(false) <<
					Property("opencl.native.threads.count")(0) <<
					Property("native.threads.count")(0) <<
					Property("renderengine.type")("PATHOCL");
			break;
		}
		case BENCHMARK_NATIVE:
		case STRESSTEST_NATIVE: {
			props <<
					Property("native.threads.count")(threadsCount) <<
					Property("renderengine.type")((sceneFileName == SCENE_WALLPAPER) ? "BIDIRCPU" : "PATHCPU");
			break;
		}
		case BENCHMARK_CUSTOM:
		case STRESSTEST_CUSTOM: {
			// At the first run, hardwareTreeModel is NULL
			if (deviceSelection == "") {
				props <<
						Property("opencl.gpu.use")(true) <<
						Property("opencl.cpu.use")(false);
			} else {
				props <<
						Property("opencl.devices.select")(deviceSelection);
			}
			
			if (optixSelection != "") {
				props <<
						Property("cuda.optix.devices.select")(optixSelection);
			}

			if (cpuSelection) {
				props <<
					Property("opencl.native.threads.count")(threadsCount) <<
					Property("native.threads.count")(threadsCount);
			} else {
				props <<
					Property("opencl.native.threads.count")(0) <<
					Property("native.threads.count")(0);
			
			}
			
			props <<
					Property("renderengine.type")("PATHOCL");
			break;
		}
		default: {
			LM_LOG("<FONT COLOR=\"#ff0000\">Unknown render mode in LuxRenderSession::RenderthreadImpl(): " << renderMode << "</FONT>");
			break;
		}
	}

	config = RenderConfig::Create(props);
}

void LuxCoreRenderSession::Start() {
	assert (!started);
	started = true;

	session = RenderSession::Create(config);

	session->Start();
}

void LuxCoreRenderSession::Stop() {
	assert (started);

	delete session;
	delete config;
}

const float *LuxCoreRenderSession::UpdateFrameBuffer(const u_int imagePipelineIndex) {
	if (frameBufferPtrs.size() <= imagePipelineIndex)
		frameBufferPtrs.resize(imagePipelineIndex + 1, NULL);

	frameBufferPtrs[imagePipelineIndex] = session->GetFilm().GetChannel<float>(Film::CHANNEL_IMAGEPIPELINE, imagePipelineIndex);

	return frameBufferPtrs[imagePipelineIndex];
}

const float *LuxCoreRenderSession::GetFrameBufferPtr(const u_int imagePipelineIndex) {
	if (frameBufferPtrs.size() <= imagePipelineIndex)
		frameBufferPtrs.resize(imagePipelineIndex + 1, NULL);

	return frameBufferPtrs[imagePipelineIndex];
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
