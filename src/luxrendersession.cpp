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

LuxRenderSession::LuxRenderSession(const std::string &fileName) {
	// Save the current directory
	originalCurrentDirectory = boost::filesystem::current_path();

	sceneFileName = boost::filesystem::system_complete(fileName).string();

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
	luxParse(session->sceneFileName.c_str());
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
}

void LuxRenderSession::Stop() {
	assert (started);
	
	// Make sure lux core shuts down
	int haltspp = luxGetIntAttribute("film", "haltSamplesPerPixel");
	luxSetHaltSamplesPerPixel(haltspp, true, false);

	parseThread->join();
	delete parseThread;
	parseThread = NULL;
}
