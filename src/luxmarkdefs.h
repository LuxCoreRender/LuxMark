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

#include <luxrays/luxrays.h>
			
using namespace std;

enum LuxMarkAppMode {
	BENCHMARK_OCL_GPU,
	BENCHMARK_OCL_CPUGPU,
	BENCHMARK_OCL_CPU,
	BENCHMARK_OCL_CUSTOM,
	BENCHMARK_HYBRID_GPU,
	BENCHMARK_HYBRID_CUSTOM,
	BENCHMARK_NATIVE_PATH,
	STRESSTEST_OCL_GPU,
	STRESSTEST_OCL_CPUGPU,
	STRESSTEST_OCL_CPU,
	DEMO_LUXVR,
	PAUSE
};

inline string LuxMarkAppMode2String(const LuxMarkAppMode mode) {
	switch (mode) {
		//----------------------------------------------------------------------
		// Benchmark
		//----------------------------------------------------------------------
		case BENCHMARK_OCL_GPU:
			return "OpenCL GPUs";
		case BENCHMARK_OCL_CPUGPU:
			return "OpenCL CPUs+GPUs";
		case BENCHMARK_OCL_CPU:
			return "OpenCL CPUs";
		case BENCHMARK_OCL_CUSTOM:
			return "OpenCL Custom";
		case BENCHMARK_HYBRID_GPU:
			return "Hybrid C++/OpenCL GPUs";
		case BENCHMARK_HYBRID_CUSTOM:
			return "Hybrid C++/OpenCL Custom";
		case BENCHMARK_NATIVE_PATH:
			return "C++";
		//----------------------------------------------------------------------
		// Stress test
		//----------------------------------------------------------------------
		case STRESSTEST_OCL_GPU:
			return "OpenCL GPUs (stress test)";
		case STRESSTEST_OCL_CPUGPU:
			return "OpenCL CPUs+GPUs (stress test)";
		case STRESSTEST_OCL_CPU:
			return "OpenCL CPUs (stress test)";
		case PAUSE:
			return "Pause";
		default:
			return "Unknown";
	}
}

#endif	/* _LUXMARKDEFS_H */
