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

#ifndef _RESULTDIALOG_H
#define	_RESULTDIALOG_H

#ifndef Q_MOC_RUN
#include <cstddef>

#include <boost/filesystem.hpp>

#include "luxmarkdefs.h"
#include "hardwaretree.h"
#endif

#include "ui_resultdialog.h"

class ResultDialog : public QDialog {
	Q_OBJECT

public:
	ResultDialog(
			const LuxMarkAppMode mode,
			const char *sceneName,
			const double sampleSecs,
			const vector<BenchmarkDeviceDescription> descs,
			const unsigned char *frameBuffer,
			const u_int frameBufferWidth, const u_int frameBufferHeight,
			const bool singleRunExtInfo,
			QWidget *parent = NULL);
	~ResultDialog();

private:
	static void MD5ThreadImpl(ResultDialog *resultDialog);
	static void ImageThreadImpl(ResultDialog *resultDialog);
	static void AddSceneFiles(ResultDialog *resultDialog,
			vector<boost::filesystem::path> &files,
			const boost::filesystem::path &path);
	void PrintExtInfoAndExit();

	Ui::ResultDialog *ui;

	const LuxMarkAppMode mode;
	const char *sceneName;
	double sampleSec;
	const vector<BenchmarkDeviceDescription> descs;
	const unsigned char *frameBuffer;
	u_int frameBufferWidth, frameBufferHeight;
	DeviceListModel *deviceListModel;

	bool sceneValidationDone, sceneValidationOk;
	bool imageValidationDone, imageValidationOk;
	boost::thread *md5Thread, *imageThread;
	
	bool singleRunExtInfo;

private slots:
	void submitResult();

    void setSceneValidationLabel(const QString &text, const bool isDone, const bool isOk);
	void setImageValidationLabel(const QString &text, const bool isDone, const bool isOk);

signals:
	void sceneValidationLabelChanged(const QString &text, const bool isDone, const bool isOk);
	void imageValidationLabelChanged(const QString &text, const bool isDone, const bool isOk);
};

#endif	/* _RESULTDIALOG_H */

