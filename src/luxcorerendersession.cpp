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
 *   LuxMark website: https://www.luxcorerender.org                        *
 ***************************************************************************/

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include "luxcorerendersession.h"
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
					Property("opencl.kernel.options")(oclCompilerOpts) <<
					Property("renderengine.type")("PATHOCL");
			break;
		}
		case STRESSTEST_OCL_CPUGPU:
		case BENCHMARK_OCL_CPUGPU: {
			props <<
					Property("opencl.gpu.use")(true) <<
					Property("opencl.cpu.use")(true) <<
					Property("opencl.kernel.options")(oclCompilerOpts) <<
					Property("renderengine.type")("PATHOCL");
			break;
		}
		case STRESSTEST_OCL_CPU:
		case BENCHMARK_OCL_CPU: {
			props <<
					Property("opencl.gpu.use")(false) <<
					Property("opencl.cpu.use")(true) <<
					Property("opencl.kernel.options")(oclCompilerOpts) <<
					Property("renderengine.type")("PATHOCL");
			break;
		}
		case BENCHMARK_OCL_CUSTOM: {
			// At the first run, hardwareTreeModel is NULL
			if (deviceSelection == "") {
				props <<
						Property("opencl.gpu.use")(true) <<
						Property("opencl.cpu.use")(false) <<
						Property("opencl.kernel.options")(oclCompilerOpts) <<
						Property("renderengine.type")("PATHOCL");
			} else {
				props <<
						Property("opencl.devices.select")(deviceSelection) <<
						Property("opencl.kernel.options")(oclCompilerOpts) <<
						Property("renderengine.type")("PATHOCL");
			}
			break;
		}
		case BENCHMARK_NATIVE: {
			props <<
					Property("renderengine.type")("PATHCPU");
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

const float *LuxCoreRenderSession::GetFrameBuffer() const {
	return session->GetFilm().GetChannel<float>(Film::CHANNEL_IMAGEPIPELINE);
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
