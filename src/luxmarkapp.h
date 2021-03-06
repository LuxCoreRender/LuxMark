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

#ifndef _LUXMARKAPP_H
#define _LUXMARKAPP_H

#ifndef Q_MOC_RUN
#include <QtWidgets/QApplication>
#include <QTimer>

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include "luxmarkdefs.h"
#include "mainwindow.h"
#include "hardwaretree.h"
#include "luxcorerendersession.h"
#endif

//------------------------------------------------------------------------------
// LuxMark Qt application
//------------------------------------------------------------------------------

// List of supported scenes
#define SCENE_WALLPAPER "scenes/wallpaper/render.cfg"
#define SCENE_HALLBENCH "scenes/hallbench/render.cfg"
#define SCENE_CANNELLE_ET_FROMAGE "scenes/cannelleetfromage/render.cfg"

class LuxMarkApp : public QApplication {
	Q_OBJECT

public:
	MainWindow *mainWin;

	LuxMarkApp(int &argc, char **argv);
	~LuxMarkApp();
	
	void Init(LuxMarkAppMode mode,
			const bool useCPU, const string &enabledDevices, const string &enabledOptixDevices,
			const char *scnName, const bool singleRun, const bool singleRunExtInfo);
	void Stop();

	void SetMode(LuxMarkAppMode m);
	void SetScene(const char *scnName);

	bool IsSingleRun() const { return singleRun; }

	const boost::filesystem::path &GetExePath() const { return exePath; }

private:
	static void EngineInitThreadImpl(LuxMarkApp *app);

	void InitRendering(LuxMarkAppMode mode, const char *scnName);

	boost::filesystem::path exePath;

	LuxMarkAppMode mode;
	const char *sceneName;
	bool singleRun, singleRunExtInfo;

	HardwareTreeModel *hardwareTreeModel;

	boost::thread *engineInitThread;
	double renderingStartTime, lastFrameBufferDenoisedUpdate;
	bool engineInitDone;
	LuxCoreRenderSession *luxSession;

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
