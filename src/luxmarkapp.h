/***************************************************************************
 *   Copyright (C) 1998-2013 by authors (see AUTHORS.txt)                  *
 *                                                                         *
 *   This file is part of LuxRays.                                         *
 *                                                                         *
 *   LuxRays is free software; you can redistribute it and/or modify       *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   LuxRays is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 *   LuxRays website: http://www.luxrender.net                             *
 ***************************************************************************/

#ifndef _LUXMARKAPP_H
#define _LUXMARKAPP_H

#include <QtGui/QApplication>
#include <QTimer>

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include "luxmarkdefs.h"
#include "mainwindow.h"
#include "hardwaretree.h"
#include "luxrendersession.h"

//------------------------------------------------------------------------------
// LuxMark Qt application
//------------------------------------------------------------------------------

// List of supported scenes
#define SCENE_ROOM "scenes/room/render.cfg"
#define SCENE_MICROPHONE "scenes/mic/render.cfg"
#define SCENE_LUXBALL_HDR "scenes/luxball/render.cfg"

class LuxMarkApp : public QApplication {
	Q_OBJECT

public:
	MainWindow *mainWin;

	LuxMarkApp(int &argc, char **argv);
	~LuxMarkApp();
	
	void Init(LuxMarkAppMode mode, const char *scnName, const bool singleRun);
	void Stop();

	void SetMode(LuxMarkAppMode m);
	void SetScene(const char *name);

	bool IsSingleRun() const { return singleRun; }

	const boost::filesystem::path &GetExePath() const { return exePath; }

private:
	static void EngineInitThreadImpl(LuxMarkApp *app);

	void InitRendering(LuxMarkAppMode mode, const char *scnName);

	boost::filesystem::path exePath;

	LuxMarkAppMode mode;
	const char *sceneName;
	bool singleRun;

	HardwareTreeModel *hardwareTreeModel;

	boost::thread *engineInitThread;
	double renderingStartTime;
	bool engineInitDone;
	LuxRenderSession *luxSession;

	QTimer *renderRefreshTimer;

	bool mouseButton0;
	bool mouseButton2;
	qreal mouseGrabLastX;
	qreal mouseGrabLastY;
	double lastMouseUpdate;

private slots:
	void RenderRefreshTimeout();
};

#endif // _LUXMARKAPP_H
