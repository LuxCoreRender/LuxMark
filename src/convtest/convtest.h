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

#ifndef LUX_CONVTEST_H
#define LUX_CONVTEST_H

#include <vector>

#include "luxrays/luxrays.h"
#include "convtest/pdiff/metric.h"

namespace lux {

class ConvergenceTest {
public:
	ConvergenceTest(const u_int w, const u_int h);
	virtual ~ConvergenceTest();

	void NeedTVI();
	const float *GetTVI() const { return &tvi[0]; }
	
	void Reset();
	void Reset(const u_int w, const u_int h);
	u_int Test(const float *image);

private:
	u_int width, height;
	
	std::vector<float> reference;
	std::vector<float> tvi;
};

}

#endif
