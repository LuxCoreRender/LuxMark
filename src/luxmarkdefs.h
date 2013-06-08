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

#ifndef _LUXMARKDEFS_H
#define	_LUXMARKDEFS_H

#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstddef>

#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)
#include <stddef.h>
#include <sys/time.h>
#elif defined (WIN32)
#include <windows.h>
#else
	Unsupported Platform !!!
#endif

#include "mainwindow.h"

using namespace std;
using namespace luxrays;

extern MainWindow *LogWindow;

class LuxLogEvent: public QEvent {
public:
	LuxLogEvent(QString mesg);
 
	QString getMessage() { return message; }

private:
	QString message;
};

class LuxErrorEvent: public QEvent {
public:
	LuxErrorEvent(QString mesg);

	QString getMessage() { return message; }

private:
	QString message;
};

#define LM_LOG(a) { if (LogWindow) { std::stringstream _LM_LOG_LOCAL_SS; _LM_LOG_LOCAL_SS << a; qApp->postEvent(LogWindow, new LuxLogEvent(QString(_LM_LOG_LOCAL_SS.str().c_str()))); }}
#define LM_LOG_LUXRAYS(a) { LM_LOG("<FONT COLOR=\"#002200\"><B>[LuxRays]</B></FONT> " << a); }
#define LM_LOG_SLG(a) { LM_LOG("<FONT COLOR=\"#009900\"><B>[SLG]</B></FONT> " << a); }
#define LM_LOG_SDL(a) { LM_LOG("<FONT COLOR=\"#004400\"><B>[SDL]</B></FONT> " << a); }
#define LM_LOG_LUX_ERROR(a) { LM_LOG("<FONT COLOR=\"#990000\"><B>[LuxRender]</B></FONT> " << a); }
#define LM_LOG_LUX_WARNING(a) { LM_LOG("<FONT COLOR=\"#999900\"><B>[LuxRender]</B></FONT> " << a); }
#define LM_LOG_LUX(a) { LM_LOG("<FONT COLOR=\"#449944\"><B>[LuxRender]</B></FONT> " << a); }

#define LM_ERROR(a) { if (LogWindow) { std::stringstream _LM_ERR_LOCAL_SS; _LM_ERR_LOCAL_SS << a; qApp->postEvent(LogWindow, new LuxErrorEvent(QString(_LM_ERR_LOCAL_SS.str().c_str()))); }}

enum LuxMarkAppMode {
	BENCHMARK_NOSPECTRAL_OCL_GPU,
	BENCHMARK_NOSPECTRAL_OCL_CPUGPU,
	BENCHMARK_NOSPECTRAL_OCL_CPU,
	BENCHMARK_NOSPECTRAL_OCL_CUSTOM,
	BENCHMARK_SPECTRAL_HYBRID_GPU,
	BENCHMARK_SPECTRAL_HYBRID_CUSTOM,
	BENCHMARK_SPECTRAL_NATIVE_PATH,
	BENCHMARK_SPECTRAL_NATIVE_BIDIR,
	INTERACTIVE, PAUSE
};

inline string LuxMarkAppMode2String(const LuxMarkAppMode mode) {
	switch (mode) {
		case BENCHMARK_NOSPECTRAL_OCL_GPU:
			return "No Spectral OpenCL GPUs";
		case BENCHMARK_NOSPECTRAL_OCL_CPUGPU:
			return "No Spectral OpenCL CPUs+GPUs";
		case BENCHMARK_NOSPECTRAL_OCL_CPU:
			return "No Spectral OpenCL CPUs";
		case BENCHMARK_NOSPECTRAL_OCL_CUSTOM:
			return "No Spectral OpenCL Custom";
		case BENCHMARK_SPECTRAL_HYBRID_GPU:
			return "Spectral Hybrid C++/OpenCL GPUs";
		case BENCHMARK_SPECTRAL_HYBRID_CUSTOM:
			return "Spectral Hybrid C++/OpenCL Custom";
		case BENCHMARK_SPECTRAL_NATIVE_PATH:
			return "Spectral C++";
		case BENCHMARK_SPECTRAL_NATIVE_BIDIR:
			return "Advanced Spectral C++";
		case INTERACTIVE:
			return "Interactive";
		case PAUSE:
			return "Pause";
		default:
			return "Unknown";
	}
}

#endif	/* _LUXMARKDEFS_H */
