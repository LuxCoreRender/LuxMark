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

#include <QFile>
#include <QCryptographicHash>

#include "luxmarkcfg.h"
#include "resultdialog.h"
#include "submitdialog.h"
#include "luxmarkapp.h"

ResultDialog::ResultDialog(LuxMarkAppMode mode,
		const char *scnName, const double sampSecs,
		const vector<BenchmarkDeviceDescription> &ds,
		QWidget *parent) : QDialog(parent),
		ui(new Ui::ResultDialog), descs(ds) {
	sceneName = scnName;
	sampleSecs = sampSecs;
	sceneValidation = false;

	ui->setupUi(this);

	this->setWindowTitle("LuxMark v" LUXMARK_VERSION_MAJOR "." LUXMARK_VERSION_MINOR);
	ui->modeLabel->setText(LuxMarkAppMode2String(mode).c_str());
	ui->sceneLabel->setText(sceneName);

	deviceListModel = new DeviceListModel(descs);
	ui->deviceListView->setModel(deviceListModel);

	ui->resultLCD->display(int(sampleSecs / 1000.0));

	if ((strcmp(sceneName, SCENE_ROOM) != 0) &&
			(strcmp(sceneName, SCENE_SALA) !=0) &&
			(strcmp(sceneName, SCENE_LUXBALL_HDR) !=0))
		ui->submitButton->setEnabled(false);

	// To re-enable only with official release
	ui->submitButton->setEnabled(false);

	QObject::connect(this, SIGNAL(validationLabelChanged(const QString &, const bool)),
			this, SLOT(setValidationLabel(const QString &, const bool)));

	// Check if it ione of the official benchmarks
	if (string(sceneName) == SCENE_LUXBALL_HDR) {
		// Start the md5 validation thread
		md5Thread = new boost::thread(boost::bind(ResultDialog::MD5ThreadImpl, this));
	} else {
		ui->validationLabel->setText("N/A");
		ui->validationLabel->setStyleSheet("QLabel { color : green; }");
		md5Thread = NULL;
	}
}

ResultDialog::~ResultDialog() {
	if (md5Thread) {
		md5Thread->join();
		delete md5Thread;
	}
	
	delete ui;
	delete deviceListModel;
}

void ResultDialog::submitResult() {
	// TODO: wait for the end of all pending processes
	SubmitDialog *dialog = new SubmitDialog(sceneName, sampleSecs, descs);
	dialog->exec();
	delete dialog;
}

void ResultDialog::setValidationLabel(const QString &text, const bool isOk) {
	ui->validationLabel->setText(text);
	if (isOk)
		ui->validationLabel->setStyleSheet("QLabel { color : green; }");
	else
		ui->validationLabel->setStyleSheet("QLabel { color : red; }");

	sceneValidation = isOk;
}

void ResultDialog::AddSceneFiles(ResultDialog *resultDialog,
		vector<boost::filesystem::path> &files, const boost::filesystem::path &path) {
    for (boost::filesystem::directory_iterator it(path); it != boost::filesystem::directory_iterator(); ++it) {
		const boost::filesystem::path &fileName = it->path();

		if (boost::filesystem::is_directory(fileName)) {
			// Avoid LuxVR scene directory
			if (fileName.filename() != "luxvr-scene")
				AddSceneFiles(resultDialog, files, fileName);
		} else if (boost::filesystem::is_regular_file(fileName)) {
			// Check if it is one of the scene file extensions
			const string ext = fileName.extension().generic_string();
			if ((ext == ".lxs") || (ext == ".lxm") || (ext == ".lxo") || (ext == ".lxv") || (ext == ".ply") ||
					 (ext == ".jpg") || (ext == ".png") || (ext == ".hdr")) {
				const QString label(("Selecting file [" + fileName.filename().generic_string() + "]").c_str());
				emit resultDialog->validationLabelChanged(label, false);
				files.push_back(fileName);
			}
		}
	}
}

void ResultDialog::MD5ThreadImpl(ResultDialog *resultDialog) {
	// Begin the md5 scene validation process
	emit resultDialog->validationLabelChanged("Starting...", false);

	try {
		// Extract the scene directory name
		boost::filesystem::path scenePath = boost::filesystem::path(resultDialog->sceneName).parent_path();
		LM_LOG("MD5 Validation scene path: [" << scenePath << "]");

		// Build the list of files to validate
		vector<boost::filesystem::path> files;
		AddSceneFiles(resultDialog, files, scenePath);
        sort(files.begin(), files.end());
		LM_LOG("MD5 Validation selected files: [" << scenePath << "]");
		BOOST_FOREACH(boost::filesystem::path fileName, files) {
			LM_LOG("  [" << fileName << "]");
		}

		// Validate each file
		QCryptographicHash hash(QCryptographicHash::Md5);
		LM_LOG("MD5 Validated files: [" << scenePath << "]");
		BOOST_FOREACH(boost::filesystem::path fileName, files) {
			LM_LOG("  [" << fileName << "]");

			const QString label(("Validating file [" + fileName.filename().generic_string() + "]").c_str());
			emit resultDialog->validationLabelChanged(label, false);

			// Read all file
			QFile file(fileName.c_str());
			hash.addData(file.readAll());
			file.close();
		}

		// Check the result
		const string md5 = QString(hash.result().toHex()).toStdString();
		LM_LOG("Scene files MD5: [" << md5 << "]");

		if (string(resultDialog->sceneName) == SCENE_LUXBALL_HDR) {
			if (md5 == "d41d8cd98f00b204e9800998ecf8427e")
				emit resultDialog->validationLabelChanged("OK", true);
			else
				emit resultDialog->validationLabelChanged("Failed", false);
		} else {
			// This should never happen, the thread should have not been started at all
			LM_LOG("Internal error in ResultDialog::MD5ThreadImpl()");
			emit resultDialog->validationLabelChanged("Failed", false);
		}
	} catch (exception &err) {
		LM_ERROR("VALIDATION ERROR: " << err.what());

		emit resultDialog->validationLabelChanged("Error", false);
	}
}
