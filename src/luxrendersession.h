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

#ifndef LUXRENDERSESSION_H
#define	LUXRENDERSESSION_H

#include <string>

#include "api.h"
#include "error.h"
#include "server/renderserver.h"

#include "luxmarkdefs.h"

class LuxRenderSession {
public:
	LuxRenderSession(const string &sceneFileName, const LuxMarkAppMode mode,
			const string &slgDevSel, const string &luxDevSel);
	~LuxRenderSession();

	void Start();
	void Stop();

private:
	static void RenderthreadImpl(LuxRenderSession *session);

	boost::filesystem::path originalCurrentDirectory;
	boost::filesystem::path bundleDir;

	std::string sceneFileName;
	LuxMarkAppMode renderMode;
	string slgDeviceSelection, luxDeviceSelection;

	boost::thread *parseThread;

	bool started, parseError;
};

#endif	/* LUXRENDERSESSION_H */
