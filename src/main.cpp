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

#include <cstdio>
#include <cstdlib>

#include "luxmarkdefs.h"
#include "mainwindow.h"
#include "luxmarkapp.h"

static void PrintCmdLineHelp(const QString &cmd) {
	cerr << "Usage: " << cmd.toAscii().data() << " [options]" << endl <<
			" --help (display this help and exit)" << endl <<
			" --scene=LUXBALL_HDR|MICROPHONE|ROOM (select the scene to use)" << endl <<
			" --mode="
                "BENCHMARK_OCL_GPU|BENCHMARK_OCL_CPUGPU|BENCHMARK_OCL_CPU|"
                "BENCHMARK_HYBRID_GPU|BENCHMARK_NATIVE|"
                "STRESSTEST_OCL_GPU|STRESSTEST_OCL_CPUGPU|STRESSTEST_OCL_CPU|"
                "DEMO_LUXVR|PAUSE"
				" (select the mode to use)" << endl <<
			" --single-run (run the benchmark, print the result to the stdout and exit)" << endl;
}

int main(int argc, char **argv) {
	LuxMarkApp app(argc, argv);

	// Get the arguments into a list
	bool exit = false;
	bool singleRun = false;

	QStringList argsList = app.arguments();
	QRegExp argHelp("--help");
	QRegExp argScene("--scene=(LUXBALL_HDR|MICROPHONE|ROOM)");
	QRegExp argMode("--mode=("
		"BENCHMARK_OCL_GPU|BENCHMARK_OCL_CPUGPU|BENCHMARK_OCL_CPU|"
		"BENCHMARK_NATIVE|"
		"STRESSTEST_OCL_GPU|STRESSTEST_OCL_CPUGPU|STRESSTEST_OCL_CPU|"
		"DEMO_LUXVR|PAUSE"
		")");
	QRegExp argSingleRun("--single-run");

	LuxMarkAppMode mode = BENCHMARK_OCL_GPU;
	// Remember to change the default label in mainwindow.cpp too
	const char *scnName = SCENE_LUXBALL_HDR;
    for (int i = 1; i < argsList.size(); ++i) {
        if (argHelp.indexIn(argsList.at(i)) != -1 ) {   
			PrintCmdLineHelp(argsList.at(0));
            exit = true;
			break;
		} else if (argScene.indexIn(argsList.at(i)) != -1) {   
            QString scene = argScene.cap(1);
			if (scene.compare("LUXBALL_HDR", Qt::CaseInsensitive) == 0)
				scnName = SCENE_LUXBALL_HDR;
			else if (scene.compare("MICROPHONE", Qt::CaseInsensitive) == 0)
				scnName = SCENE_MICROPHONE;
			else if (scene.compare("ROOM", Qt::CaseInsensitive) == 0)
				scnName = SCENE_ROOM;
			else {
				cerr << "Unknown scene name: " << argScene.cap(1).toAscii().data() << endl;
				PrintCmdLineHelp(argsList.at(0));
				exit = true;
				break;
			}
		} else if (argMode.indexIn(argsList.at(i)) != -1 ) {   
            QString scene = argMode.cap(1);
			if (scene.compare("BENCHMARK_OCL_GPU", Qt::CaseInsensitive) == 0)
				mode = BENCHMARK_OCL_GPU;
			else if (scene.compare("BENCHMARK_OCL_CPUGPU", Qt::CaseInsensitive) == 0)
				mode = BENCHMARK_OCL_CPUGPU;
			else if (scene.compare("BENCHMARK_OCL_CPU", Qt::CaseInsensitive) == 0)
				mode = BENCHMARK_OCL_CPU;
			else if (scene.compare("BENCHMARK_NATIVE", Qt::CaseInsensitive) == 0)
				mode = BENCHMARK_NATIVE;
			else if (scene.compare("STRESSTEST_OCL_GPU", Qt::CaseInsensitive) == 0)
				mode = STRESSTEST_OCL_GPU;
			else if (scene.compare("STRESSTEST_OCL_CPUGPU", Qt::CaseInsensitive) == 0)
				mode = STRESSTEST_OCL_CPUGPU;
			else if (scene.compare("STRESSTEST_OCL_CPU", Qt::CaseInsensitive) == 0)
				mode = STRESSTEST_OCL_CPU;
			else if (scene.compare("DEMO_LUXVR", Qt::CaseInsensitive) == 0)
				mode = DEMO_LUXVR;
			else if (scene.compare("PAUSE", Qt::CaseInsensitive) == 0)
				mode = PAUSE;
			else {
				cerr << "Unknown mode name: " << argMode.cap(1).toAscii().data() << endl;
				PrintCmdLineHelp(argsList.at(0));
				exit = true;
				break;
			}
		} else if (argSingleRun.indexIn(argsList.at(i)) != -1 ) {   
			singleRun = true;
        } else {
            cerr << "Unknown argument: " << argsList.at(i).toAscii().data() << endl;
			PrintCmdLineHelp(argsList.at(0));
			exit = true;
			break;
        }
    }

	if (exit)
		return EXIT_SUCCESS;
	else {
		app.Init(mode, scnName, singleRun);

		// Force C locale
		setlocale(LC_NUMERIC, "C");

		if (app.mainWin != NULL) {
			app.mainWin->UpdateSceneLabel(scnName);
			return app.exec();
		} else
			return EXIT_FAILURE;
	}
}
