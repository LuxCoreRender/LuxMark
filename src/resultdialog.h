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

#ifndef _RESULTDIALOG_H
#define	_RESULTDIALOG_H

#include <cstddef>

#include "luxmarkdefs.h"
#include "hardwaretree.h"

#include "ui_resultdialog.h"

class ResultDialog : public QDialog {
	Q_OBJECT

public:
	ResultDialog(
			LuxMarkAppMode mode,
			const char *sceneName,
			const double sampleSecs,
			const vector<BenchmarkDeviceDescription> &descs,
			QWidget *parent = NULL);
	~ResultDialog();

private:
	static void MD5ThreadImpl(ResultDialog *resultDialog);
	static void AddSceneFiles(ResultDialog *resultDialog,
			vector<boost::filesystem::path> &files,
			const boost::filesystem::path &path);

	Ui::ResultDialog *ui;

	const char *sceneName;
	double sampleSecs;
	const vector<BenchmarkDeviceDescription> &descs;
	DeviceListModel *deviceListModel;

	bool sceneValidation;
	boost::thread *md5Thread;

private slots:
	void submitResult();

    void setValidationLabel(const QString &text, const bool isOk);

signals:
	void validationLabelChanged(const QString &text, const bool isOk);
};

#endif	/* _RESULTDIALOG_H */

