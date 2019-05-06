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

#ifndef _LUXCOREUIDIALOG_H
#define	_LUXCOREUIDIALOG_H

#ifndef Q_MOC_RUN
#include <cstddef>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include "luxmarkdefs.h"
#endif

#include "ui_luxcoreuidialog.h"

class LuxCoreUIDialog : public QDialog {
	Q_OBJECT

public:
	LuxCoreUIDialog(const char *sceneName, const boost::filesystem::path &exePath,
			QWidget *parent = NULL);
	~LuxCoreUIDialog();

protected:
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *) { } // To ignore ESC key

private:
	static void LuxCoreUIThreadImpl(LuxCoreUIDialog *luxCoreUIDialog);
	static void ExecCmd(const string &cmd);

	Ui::LuxCoreUIDialog *ui;
	const char *sceneName;
	const boost::filesystem::path exePath;
	boost::thread *luxCoreUIThread;

	bool toCloseDialog;

private slots:
	void luxCoreUIThreadDone();

signals:
	void signalLuxCoreUIThreadDone();
};

#endif	/* _LUXCOREUIDIALOG_H */
