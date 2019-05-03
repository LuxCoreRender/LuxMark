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

#ifndef LUXCORERENDERSESSION_H
#define	LUXCORERENDERSESSION_H

#ifndef Q_MOC_RUN
#include <string>

#include "luxcore/luxcore.h"
#include "luxmarkdefs.h"
#endif

class LuxCoreRenderSession {
public:
	LuxCoreRenderSession(const string &sceneFileName, const LuxMarkAppMode mode,
			const string &devSel, const string &oclCompOpts);
	~LuxCoreRenderSession();

	void Start();
	void Stop();

	const float *GetFrameBuffer(const u_int imagePipelineIndex) const;
	u_int GetFrameBufferWidth() const;
	u_int GetFrameBufferHeight() const;

	const luxrays::Properties &GetStats() const;

private:
	static void RenderThreadImpl(LuxCoreRenderSession *session);

	std::string sceneFileName;
	LuxMarkAppMode renderMode;
	string deviceSelection;
	string oclCompilerOpts;

	luxcore::RenderConfig *config;
	luxcore::RenderSession *session;

	bool started;
};

#endif	/* LUXCORERENDERSESSION_H */
