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

#include <boost/lexical_cast.hpp>
#include <QtGui/qevent.h>

#include "luxmarkcfg.h"
#include "luxcoreuidialog.h"
#include "mainwindow.h"

LuxCoreUIDialog::LuxCoreUIDialog(const char *name,
		const boost::filesystem::path &path, QWidget *parent) :
		QDialog(parent), ui(new Ui::LuxCoreUIDialog), sceneName(name), exePath(path),
		toCloseDialog(false) {
	ui->setupUi(this);

	QObject::connect(this, SIGNAL(signalLuxCoreUIThreadDone()),
			this, SLOT(luxCoreUIThreadDone()));

	luxCoreUIThread = new boost::thread(boost::bind(LuxCoreUIDialog::LuxCoreUIThreadImpl, this));
}

LuxCoreUIDialog::~LuxCoreUIDialog() {
	luxCoreUIThread->join();
	delete luxCoreUIThread;

	delete ui;
}

void LuxCoreUIDialog::closeEvent(QCloseEvent *event) {
	if (toCloseDialog)
		event->accept(); // Close window
	else
		event->ignore(); // Keep window
}

void LuxCoreUIDialog::luxCoreUIThreadDone() {
	toCloseDialog = true;

	// Close the dialog
	close();
}

#if defined(WIN32)
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

void LuxCoreUIDialog::ExecCmd(const string &cmd) {
	string execCmd = cmd;
#if defined(WIN32)
	// Wrap whole command line in quotes so it becomes one parameter
	execCmd = "\"" + execCmd + "\"";
#endif
	LM_LOG("Exec: [" << execCmd << "]");

	FILE *pipe = POPEN(execCmd.c_str(), "r");
	if (!pipe)
		throw runtime_error("Unable to execute command: " + cmd);

	LM_LOG("Command output:");

	string line = "";
	for (;;) {
		int c = fgetc(pipe);
		if (c == EOF) {
			LM_LOG(line);
			break;
		} else if (c == '\r') {
			// Nothing to do
		} else if (c == '\n') {
			LM_LOG(line);
			line = "";
		} else
			line += boost::lexical_cast<unsigned char>((unsigned char)c);
		
	}
	PCLOSE(pipe);
}

void LuxCoreUIDialog::LuxCoreUIThreadImpl(LuxCoreUIDialog *luxCoreUIDialog) {
	// Looks for LuxCoreUI command
	LM_LOG("Lux CoreUIexecutable path: [" << luxCoreUIDialog->exePath << "]");
	boost::filesystem::path luxCoreUIPath = luxCoreUIDialog->exePath / "luxcoreui";

	if (!boost::filesystem::exists(luxCoreUIPath))
		luxCoreUIPath = luxCoreUIDialog->exePath / "luxcoreui.exe";

	if (!boost::filesystem::exists(luxCoreUIPath)) {
		LM_LOG("Unable to find LuxCoreUI executable");
	} else {
		LM_LOG("LuxCoreUI path: [" << luxCoreUIPath << "]");
		const string luxCoreUI = luxCoreUIPath.make_preferred().string();
		LM_LOG("LuxCoreUI native path: [" << luxCoreUI << "]");

		const string luxCoreUICmd = "\"" + luxCoreUI + "\" "
			"-D renderengine.type PATHOCL "
			" \"" + luxCoreUIDialog->sceneName + "\"" + " 2>&1";

		// Start LuxCoreUI
		ExecCmd(luxCoreUICmd);

		LM_LOG("LuxCoreUI done");
	}
	
	luxCoreUIDialog->signalLuxCoreUIThreadDone();
}
