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
using namespace luxrays;

enum LuxMarkAppMode {
	BENCHMARK_NOSPECTRAL_OCL_GPU,
	BENCHMARK_NOSPECTRAL_OCL_CPUGPU,
	BENCHMARK_NOSPECTRAL_OCL_CPU,
	BENCHMARK_NOSPECTRAL_OCL_CUSTOM,
	BENCHMARK_NOSPECTRAL_HYBRID_GPU,
	BENCHMARK_NOSPECTRAL_HYBRID_CUSTOM,
	BENCHMARK_NOSPECTRAL_NATIVE_PATH,
	BENCHMARK_SPECTRAL_HYBRID_GPU,
	BENCHMARK_SPECTRAL_HYBRID_CUSTOM,
	BENCHMARK_SPECTRAL_NATIVE_PATH,
	BENCHMARK_SPECTRAL_NATIVE_BIDIR,
	STRESSTEST_NOSPECTRAL_OCL_GPU,
	STRESSTEST_NOSPECTRAL_OCL_CPUGPU,
	STRESSTEST_NOSPECTRAL_OCL_CPU,
	STRESSTEST_SPECTRAL_NATIVE_BIDIR,
	PAUSE
};

inline string LuxMarkAppMode2String(const LuxMarkAppMode mode) {
	switch (mode) {
		//----------------------------------------------------------------------
		// No Spectral
		//----------------------------------------------------------------------
		case BENCHMARK_NOSPECTRAL_OCL_GPU:
			return "No Spectral OpenCL GPUs";
		case BENCHMARK_NOSPECTRAL_OCL_CPUGPU:
			return "No Spectral OpenCL CPUs+GPUs";
		case BENCHMARK_NOSPECTRAL_OCL_CPU:
			return "No Spectral OpenCL CPUs";
		case BENCHMARK_NOSPECTRAL_OCL_CUSTOM:
			return "No Spectral OpenCL Custom";
		case BENCHMARK_NOSPECTRAL_HYBRID_GPU:
			return "No Spectral Hybrid C++/OpenCL GPUs";
		case BENCHMARK_NOSPECTRAL_HYBRID_CUSTOM:
			return "No Spectral Hybrid C++/OpenCL Custom";
		case BENCHMARK_NOSPECTRAL_NATIVE_PATH:
			return "No Spectral C++";
		//----------------------------------------------------------------------
		// Spectral
		//----------------------------------------------------------------------
		case BENCHMARK_SPECTRAL_HYBRID_GPU:
			return "Spectral Hybrid C++/OpenCL GPUs";
		case BENCHMARK_SPECTRAL_HYBRID_CUSTOM:
			return "Spectral Hybrid C++/OpenCL Custom";
		case BENCHMARK_SPECTRAL_NATIVE_PATH:
			return "Spectral C++";
		//----------------------------------------------------------------------
		// Advanced Spectral
		//----------------------------------------------------------------------
		case BENCHMARK_SPECTRAL_NATIVE_BIDIR:
			return "Advanced Spectral C++";
		//----------------------------------------------------------------------
		// Others
		//----------------------------------------------------------------------
		case STRESSTEST_NOSPECTRAL_OCL_GPU:
			return "No Spectral OpenCL GPUs (stress test)";
		case STRESSTEST_NOSPECTRAL_OCL_CPUGPU:
			return "No Spectral OpenCL CPUs+GPUs (stress test)";
		case STRESSTEST_NOSPECTRAL_OCL_CPU:
			return "No Spectral OpenCL CPUs (stress test)";
		case STRESSTEST_SPECTRAL_NATIVE_BIDIR:
			return "Advanced Spectral C++ (stress test)";
		case PAUSE:
			return "Pause";
		default:
			return "Unknown";
	}
}

#endif	/* _LUXMARKDEFS_H */
