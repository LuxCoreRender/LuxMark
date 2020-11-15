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

#include <cstdio>
#include <cstdlib>

#include "luxmarkdefs.h"
#include "mainwindow.h"
#include "luxmarkapp.h"

static void PrintCmdLineHelp(const QString &cmd) {
	cerr << "Usage: " << cmd.toLatin1().data() << " [options]" << endl <<
			" --help (display this help and exit)" << endl <<
			" --scene=FOOD|HALLBENCH|WALLPAPER (select the scene to use)" << endl <<
			" --mode="
                "BENCHMARK_CUDA_GPU|BENCHMARK_OCL_GPU|BENCHMARK_NATIVE|BENCHMARK_CUSTOM|"
				"STRESSTEST_CUDA_GPU|STRESSTEST_OCL_GPU|STRESSTEST_CUSTOM|STRESSTEST_NATIVE|"
				"DEMO_LUXCOREUI|PAUSE"
				" (select the mode to use)" << endl <<
			" --devices=<a string of 1 or 0 to enable/disable each OpenCL/CUDA device in CUSTOM modes>" << endl <<
			" --optix-devices=<a string of 1 or 0 to enable/disable Optix on each CUDA device in CUSTOM modes>" << endl <<
			" --use-cpu (enable the CPU use in CUSTOM modes, disabled by default)" << endl <<
			" --single-run (run the benchmark, print the result to the stdout and exit)" << endl <<
			" --ext-info (print scene and image verification too with --single-run)" <<endl;
}

int main(int argc, char **argv) {
	
#if defined(__APPLE__) // for since OSX 10.9 changed default font
	QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande");
	QFont::insertSubstitution(".Helvetica Neue DeskInterface", "Lucida Grande");
#endif
	
	LuxMarkApp app(argc, argv);

	// Get the arguments into a list
	QStringList argsList = app.arguments();
	QRegExp argHelp("--help");
	QRegExp argScene("--scene=(FOOD|HALLBENCH|WALLPAPER)");
	QRegExp argMode("--mode=("
		"BENCHMARK_CUDA_GPU|BENCHMARK_OCL_GPU|BENCHMARK_NATIVE|BENCHMARK_CUSTOM|"
		"STRESSTEST_CUDA_GPU|STRESSTEST_OCL_GPU|STRESSTEST_CUSTOM|STRESSTEST_NATIVE|"
		"DEMO_LUXCOREUI|PAUSE"
		")");
	QRegExp argDevices("--devices=([01]+)");
	QRegExp argOptixDevices("--optix-devices=([01]+)");
	QRegExp argUseCPU("--use-cpu");
	QRegExp argSingleRun("--single-run");
	QRegExp argSingleRunExtInfo("--ext-info");

	LuxMarkAppMode mode = BENCHMARK_OCL_GPU;
	bool exit = false;
	string devices = "";
	string optixDevices = "";
	bool useCPU = false;
	bool singleRun = false;
	bool singleRunExtInfo = false;

	// Remember to change the default label in mainwindow.cpp too
	const char *scnName = SCENE_FOOD;
    for (int i = 1; i < argsList.size(); ++i) {
        if (argHelp.indexIn(argsList.at(i)) != -1 ) {   
			PrintCmdLineHelp(argsList.at(0));
            exit = true;
			break;
		} else if (argScene.indexIn(argsList.at(i)) != -1) {   
            QString scene = argScene.cap(1);
			if (scene.compare("FOOD", Qt::CaseInsensitive) == 0)
				scnName = SCENE_FOOD;
			else if (scene.compare("HALLBENCH", Qt::CaseInsensitive) == 0)
				scnName = SCENE_HALLBENCH;
			else if (scene.compare("WALLPAPER", Qt::CaseInsensitive) == 0)
				scnName = SCENE_WALLPAPER;
			else {
				cerr << "Unknown scene name: " << argScene.cap(1).toLatin1().data() << endl;
				PrintCmdLineHelp(argsList.at(0));
				exit = true;
				break;
			}
		} else if (argMode.indexIn(argsList.at(i)) != -1 ) {   
            QString scene = argMode.cap(1);
			if (scene.compare("BENCHMARK_CUDA_GPU", Qt::CaseInsensitive) == 0)
				mode = BENCHMARK_CUDA_GPU;
			else if (scene.compare("BENCHMARK_OCL_GPU", Qt::CaseInsensitive) == 0)
				mode = BENCHMARK_OCL_GPU;
			else if (scene.compare("BENCHMARK_NATIVE", Qt::CaseInsensitive) == 0)
				mode = BENCHMARK_NATIVE;
			else if (scene.compare("BENCHMARK_CUSTOM", Qt::CaseInsensitive) == 0)
				mode = BENCHMARK_CUSTOM;
			else if (scene.compare("STRESSTEST_CUDA_GPU", Qt::CaseInsensitive) == 0)
				mode = STRESSTEST_CUDA_GPU;
			else if (scene.compare("STRESSTEST_OCL_GPU", Qt::CaseInsensitive) == 0)
				mode = STRESSTEST_OCL_GPU;
			else if (scene.compare("STRESSTEST_NATIVE", Qt::CaseInsensitive) == 0)
				mode = STRESSTEST_NATIVE;
			else if (scene.compare("STRESSTEST_CUSTOM", Qt::CaseInsensitive) == 0)
				mode = STRESSTEST_CUSTOM;
			else if (scene.compare("DEMO_LUXCOREUI", Qt::CaseInsensitive) == 0)
				mode = DEMO_LUXCOREUI;
			else if (scene.compare("PAUSE", Qt::CaseInsensitive) == 0)
				mode = PAUSE;
			else {
				cerr << "Unknown mode name: " << argMode.cap(1).toLatin1().data() << endl;
				PrintCmdLineHelp(argsList.at(0));
				exit = true;
				break;
			}
		} else if (argDevices.indexIn(argsList.at(i)) != -1) {   
			if (mode != BENCHMARK_CUSTOM) {
				cerr << "--devices can be used only after a --mode=BENCHMARK_CUSTOM" << endl;
				PrintCmdLineHelp(argsList.at(0));
				exit = true;
				break;
			}

            devices = argDevices.cap(1).toLatin1().data();
		} else if (argOptixDevices.indexIn(argsList.at(i)) != -1) {   
			if (mode != BENCHMARK_CUSTOM) {
				cerr << "--optix-devices can be used only after a --mode=BENCHMARK_CUSTOM" << endl;
				PrintCmdLineHelp(argsList.at(0));
				exit = true;
				break;
			}

            optixDevices = argOptixDevices.cap(1).toLatin1().data();
		} else if (argUseCPU.indexIn(argsList.at(i)) != -1 ) {   
			useCPU = true;
		} else if (argSingleRun.indexIn(argsList.at(i)) != -1 ) {   
			singleRun = true;
		} else if (argSingleRunExtInfo.indexIn(argsList.at(i)) != -1 ) {   
			singleRunExtInfo = true;
        } else {
            cerr << "Unknown argument: " << argsList.at(i).toLatin1().data() << endl;
			PrintCmdLineHelp(argsList.at(0));
			exit = true;
			break;
        }
    }

	if (!singleRun && singleRunExtInfo) {
		cerr << "Option --ext-info must be used with --single-run" << endl;
		exit = true;
	}

	if (exit)
		return EXIT_SUCCESS;
	else {
		app.Init(mode, useCPU, devices, optixDevices, scnName, singleRun, singleRunExtInfo);

		// If current directory doesn't have the "scenes" directory, move
		// to where the executable is
		if (!boost::filesystem::exists("./scenes")) {
			// Look for the directory where LuxMark executable is installed
			boost::filesystem::path exePath(boost::filesystem::initial_path<boost::filesystem::path>());
			exePath = boost::filesystem::system_complete(boost::filesystem::path(argv[0])).parent_path();
			// Set the current directory to where the binary is
			boost::filesystem::current_path(exePath);
		}
	
		// Force C locale
		setlocale(LC_NUMERIC, "C");

		if (app.mainWin != NULL) {
			app.mainWin->UpdateSceneLabel(scnName);
			return app.exec();
		} else
			return EXIT_FAILURE;
	}
}
