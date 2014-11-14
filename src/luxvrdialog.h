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

#ifndef _LUXVRDIALOG_H
#define	_LUXVRDIALOG_H

#ifndef Q_MOC_RUN
#include <cstddef>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include "luxmarkdefs.h"
#endif

#include "ui_luxvrdialog.h"

class LuxVRDialog : public QDialog {
	Q_OBJECT

public:
	LuxVRDialog(const char *sceneName, const boost::filesystem::path &exePath,
			QWidget *parent = NULL);
	~LuxVRDialog();

protected:
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *) { } // To ignore ESC key

private:
	static void LuxVRThreadImpl(LuxVRDialog *luxvrDialog);
	static void ExecCmd(const string &cmd);

	Ui::LuxVRDialog *ui;
	const char *sceneName;
	const boost::filesystem::path exePath;
	boost::thread *luxvrThread;

	bool toCloseDialog;

private slots:
	void luxVRThreadDone();

signals:
	void signalLuxVRThreadDone();
};

#endif	/* _LUXVRDIALOG_H */

