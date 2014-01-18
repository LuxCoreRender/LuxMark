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

#include <boost/lexical_cast.hpp>
#include <QtGui/qevent.h>

#include "luxmarkcfg.h"
#include "luxvrdialog.h"
#include "mainwindow.h"

LuxVRDialog::LuxVRDialog(const char *name,
		const boost::filesystem::path &path, QWidget *parent) :
		QDialog(parent), ui(new Ui::LuxVRDialog), sceneName(name), exePath(path),
		toCloseDialog(false) {
	ui->setupUi(this);

	QObject::connect(this, SIGNAL(signalLuxVRThreadDone()),
			this, SLOT(luxVRThreadDone()));

	luxvrThread = new boost::thread(boost::bind(LuxVRDialog::LuxVRThreadImpl, this));
}

LuxVRDialog::~LuxVRDialog() {
	luxvrThread->join();
	delete luxvrThread;

	delete ui;
}

void LuxVRDialog::closeEvent(QCloseEvent *event) {
	if (toCloseDialog)
		event->accept(); // Close window
	else
		event->ignore(); // Keep window
}

void LuxVRDialog::luxVRThreadDone() {
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

void LuxVRDialog::ExecCmd(const string &cmd) {
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

void LuxVRDialog::LuxVRThreadImpl(LuxVRDialog *luxvrDialog) {
	// Looks for LuxVR command
	LM_LOG("Lux executable path: [" << luxvrDialog->exePath << "]");
	boost::filesystem::path luxvrPath = luxvrDialog->exePath / "slg4";
	if (!boost::filesystem::exists(luxvrPath))
		luxvrPath = luxvrDialog->exePath / "slg4.exe";
	if (!boost::filesystem::exists(luxvrPath))
		luxvrPath = luxvrDialog->exePath / "LuxMark.app/Contents/MacOS/slg4"; // OSX application bundle
	if (!boost::filesystem::exists(luxvrPath))
		throw runtime_error("Unable to find SLG4 executable");
	LM_LOG("LuxVR path: [" << luxvrPath << "]");
	const string luxvr = luxvrPath.make_preferred().string();
	LM_LOG("LuxVR native path: [" << luxvr << "]");

	const string luxvrCmd = "\"" + luxvr + "\" "
        "-R "
        "-D renderengine.type RTPATHOCL "
        "-D screen.refresh.interval 50 "
        //"-D opencl.devices.select 10000 "
        " \"" + luxvrDialog->sceneName + "\"" + " 2>&1";

	// Start LuxVR
	ExecCmd(luxvrCmd);

	LM_LOG("LuxVR done");
	
	luxvrDialog->signalLuxVRThreadDone();
}
