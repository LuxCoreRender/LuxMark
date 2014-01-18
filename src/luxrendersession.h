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

#include "luxcore/luxcore.h"
#include "luxmarkdefs.h"

class LuxRenderSession {
public:
	LuxRenderSession(const string &sceneFileName, const LuxMarkAppMode mode,
			const string &devSel);
	~LuxRenderSession();

	void Start();
	void Stop();

	const float *GetFrameBuffer() const;
	u_int GetFrameBufferWidth() const;
	u_int GetFrameBufferHeight() const;

	const luxrays::Properties &GetStats() const;

private:
	static void RenderThreadImpl(LuxRenderSession *session);

	std::string sceneFileName;
	LuxMarkAppMode renderMode;
	string deviceSelection;

	luxcore::RenderConfig *config;
	luxcore::RenderSession *session;

	bool started;
};

#endif	/* LUXRENDERSESSION_H */
