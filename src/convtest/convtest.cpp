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

#include <cstdio>
#include <cmath>
#include <algorithm>

#include "convtest/convtest.h"
#include "convtest/pdiff/metric.h"

using namespace lux;

//------------------------------------------------------------------------------
// ConvergenceTest class
//------------------------------------------------------------------------------

ConvergenceTest::ConvergenceTest(const u_int w, const u_int h) : width(w), height(h) {
}

ConvergenceTest::~ConvergenceTest() {
}

void ConvergenceTest::NeedTVI() {
	tvi.resize(width * height, 0.f);
}

void ConvergenceTest::Reset() {
	reference.resize(0);
}

void ConvergenceTest::Reset(const u_int w, const u_int h) {
	width = w;
	height = h;
	reference.resize(0);	
}

u_int ConvergenceTest::Test(const float *image) {
	const u_int pixelCount = width * height;

	if (reference.size() == 0) {
		reference.resize(pixelCount * 3);
		std::copy(image, image + pixelCount * 3, reference.begin());
		return pixelCount;
	} else {
		const u_int count = Yee_Compare(&reference[0], image, NULL, &tvi[0], width, height);
		std::copy(image, image + pixelCount * 3, reference.begin());
		return count;
	}
}
