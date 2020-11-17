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

#ifndef _KERNELCOMPILATIONDIALOG_H
#define	_KERNELCOMPILATIONDIALOG_H

#ifndef Q_MOC_RUN
#include <cstddef>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include <QtGui/qevent.h>

#include "luxmarkdefs.h"
#endif

#include "ui_kernelcompilationdialog.h"

class LuxCoreRenderSession;

class KernelCompilationDialog : public QDialog {
	Q_OBJECT

public:
	KernelCompilationDialog(QWidget *parent = NULL);
	~KernelCompilationDialog();

protected:
	void closeEvent(QCloseEvent *event) {
		event->ignore(); // Keep window open
	}

	void keyPressEvent(QKeyEvent *) { } // To ignore ESC key

private:
	Ui::KernelCompilationDialog *ui;
};

#endif	/* _KERNELCOMPILATIONDIALOG_H */
