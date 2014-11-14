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

#include <QDateTime>
#include <QTextStream>

#include "luxrays/core/utils.h"

#include "luxmarkcfg.h"
#include "submitdialog.h"

using namespace luxrays;

#define SD_MSG(a) { std::stringstream _SD_LOG_LOCAL_SS; _SD_LOG_LOCAL_SS << a;  ProgessMessage(QString(_SD_LOG_LOCAL_SS.str().c_str())); }
#define SD_LOG(a) { SD_MSG("<FONT COLOR=\"#0000ff\">" << a << "</FONT>"); }
#define SD_LOG_ERROR(a) { SD_MSG("<FONT COLOR=\"#ff0000\">" << a << "</FONT>"); }

SubmitDialog::SubmitDialog(
        const LuxMarkAppMode m,
        const char *scnName,
		const double sampSecs,
		const vector<BenchmarkDeviceDescription> ds,
		QWidget *parent) : QDialog(parent), ui(new Ui::SubmitDialog),
		mode(m), descs(ds) {
	sceneName = scnName;
	sampleSecs = sampSecs;

	state = INPUT;

	manager = NULL;
	reply = NULL;

	ui->setupUi(this);
	ui->genericButton->setText("&Submit");

	this->setWindowTitle("LuxMark v" LUXMARK_VERSION_MAJOR "." LUXMARK_VERSION_MINOR);
}

SubmitDialog::~SubmitDialog() {
	delete ui;
	delete reply;
	delete manager;
}

void SubmitDialog::ProgessMessage(const QString &msg) {
	QString buf;
	QTextStream ss(&buf);
	ss << QDateTime::currentDateTime().toString(tr("yyyy-MM-dd hh:mm:ss")) <<
			" - " << msg;

	ui->resultText->append(ss.readAll());
}

void SubmitDialog::genericButton() {
	if (state == INPUT) {
		// Send the result

		state = SUBMITTING;
		ui->genericButton->setText("&Cancel");

		// Data to submit
		const QString name = ui->nameEdit->text();
		const QString pwd =  ui->pwdEdit->text();
#if defined (WIN32)
		const QString os = "Windows";
#elif defined (__linux__)
		const QString os = "Linux";
#elif defined (__APPLE__)
		const QString os = "MacOS";
#else
		const QString os = "Unknown";
#endif
        const QString version = LUXMARK_VERSION_MAJOR "." LUXMARK_VERSION_MINOR;
		const QString scene = sceneName;
		const QString score = ToString(int(sampleSecs / 1000.0)).c_str();
		const QString note = ui->noteTextEdit->toPlainText();
		const QString devCount = ToString(descs.size()).c_str();

        QString benchMode;
        switch (mode) {
            case BENCHMARK_OCL_GPU:
            case BENCHMARK_OCL_CPUGPU:
            case BENCHMARK_OCL_CPU:
            case BENCHMARK_OCL_CUSTOM:
                benchMode = "OpenCL";
                break;
            case BENCHMARK_NATIVE:
                benchMode = "C++";
                break;
            default:
                benchMode = "Unknown";
        }

		// Delete manager/reply if required
		if (manager) {
			if (reply)
				delete reply;
			reply = NULL;

			delete manager;
			manager = NULL;
		}

		manager = new QNetworkAccessManager(this);

		// Create the http request
		SD_LOG("Creating HTTP request...");
		QNetworkRequest request;
		request.setUrl(QUrl("http://www.luxmark.info/submit_result"));
		request.setRawHeader("User-Agent", "LuxMark v" LUXMARK_VERSION_MAJOR "." LUXMARK_VERSION_MINOR);
		// Const's patch for Qt 4.8.1
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

		// Create the http request parameters
		QUrl params;
		params.addEncodedQueryItem("name", QUrl::toPercentEncoding(name));
		params.addEncodedQueryItem("password", QUrl::toPercentEncoding(pwd));

		SD_LOG("Submitted data:");

		params.addEncodedQueryItem("version", QUrl::toPercentEncoding(version));
		SD_LOG("version = " << version.toStdString());

		params.addEncodedQueryItem("os", QUrl::toPercentEncoding(os));
		SD_LOG("os = " << os.toStdString());

		params.addEncodedQueryItem("mode", QUrl::toPercentEncoding(benchMode));
		SD_LOG("mode = " << benchMode.toStdString());

		params.addEncodedQueryItem("scene_name", QUrl::toPercentEncoding(scene));
		SD_LOG("scene_name = " << scene.toStdString());

		params.addEncodedQueryItem("score", QUrl::toPercentEncoding(score));
		SD_LOG("score = " << score.toStdString());

		params.addEncodedQueryItem("note", QUrl::toPercentEncoding(note));
		SD_LOG("note = " << note.toStdString());

		params.addEncodedQueryItem("dev_count", QUrl::toPercentEncoding(devCount));
		SD_LOG("dev_count = " << devCount.toStdString());

		for (size_t i = 0; i < descs.size(); ++i) {
			SD_LOG("dev_platform_name = " << descs[i].platformName);
			params.addEncodedQueryItem("dev_platform_name[]", QUrl::toPercentEncoding(QString(descs[i].platformName.c_str())));

			SD_LOG("dev_platform_ver = " << descs[i].platformVersion);
			params.addEncodedQueryItem("dev_platform_ver[]", QUrl::toPercentEncoding(QString(descs[i].platformVersion.c_str())));

			SD_LOG("dev_name = " << descs[i].deviceName);
			params.addEncodedQueryItem("dev_name[]", QUrl::toPercentEncoding(QString(descs[i].deviceName.c_str())));

			SD_LOG("dev_type = " << descs[i].deviceType);
			params.addEncodedQueryItem("dev_type[]", QUrl::toPercentEncoding(QString(descs[i].deviceType.c_str())));

			SD_LOG("dev_units = " << descs[i].units);
			params.addEncodedQueryItem("dev_units[]", QUrl::toPercentEncoding(QString(ToString(descs[i].units).c_str())));

			SD_LOG("dev_clock = " << descs[i].clock);
			params.addEncodedQueryItem("dev_clock[]", QUrl::toPercentEncoding(QString(ToString(descs[i].clock).c_str())));

			SD_LOG("dev_native_float_vec_width = " << descs[i].nativeVectorWidthFloat);
			params.addEncodedQueryItem("dev_native_float_vec_width[]", QUrl::toPercentEncoding(QString(ToString(descs[i].nativeVectorWidthFloat).c_str())));

			SD_LOG("dev_global_mem = " << descs[i].globalMem);
			params.addEncodedQueryItem("dev_global_mem[]", QUrl::toPercentEncoding(QString(ToString(descs[i].globalMem).c_str())));

			SD_LOG("dev_local_mem = " << descs[i].localMem);
			params.addEncodedQueryItem("dev_local_mem[]", QUrl::toPercentEncoding(QString(ToString(descs[i].localMem).c_str())));

			SD_LOG("dev_constant_mem = " << descs[i].constantMem);
			params.addEncodedQueryItem("dev_constant_mem[]", QUrl::toPercentEncoding(QString(ToString(descs[i].constantMem).c_str())));
		}

		QByteArray data;
		data = params.encodedQuery();

		// Send the request
		SD_LOG("Posting result...");

		reply = manager->post(request, data);
		connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
		connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
				this, SLOT(httpError(QNetworkReply::NetworkError)));
	} else if (state == SUBMITTING) {
		// Cancel the result submission
		reply->abort();

		SD_LOG_ERROR("Submission aborted");
		state = INPUT;
		ui->genericButton->setText("&Submit");
	} else {
		// Done
		this->close();
	}
}

void SubmitDialog::httpFinished() {
    string result = QString(reply->readAll()).toStdString();
	//SD_LOG("httpFinished() - [" << result << "] [" << (result == "OK") << "]");

	if (result == "OK") {
		SD_LOG("Result successfully submitted !");
		state = DONE;
		ui->genericButton->setText("&Done");
	} else {
		SD_LOG_ERROR("Submission error: " << result);
		state = INPUT;
		ui->genericButton->setText("&Submit");
	}
}

void SubmitDialog::httpError(QNetworkReply::NetworkError error) {
	switch (error) {
		case QNetworkReply::ConnectionRefusedError:
			SD_LOG_ERROR("HTTP Error - the remote server refused the connection (the server is not accepting requests)");
			break;
		case QNetworkReply::HostNotFoundError:
			SD_LOG_ERROR("HTTP Error - the remote host name was not found (invalid hostname)");
			break;
		case QNetworkReply::TimeoutError:
			SD_LOG_ERROR("HTTP Error - the connection to the remote server timed out");
			break;
		default:
			SD_LOG_ERROR("HTTP Error " << error);
			break;
	}

	state = INPUT;
	ui->genericButton->setText("&Submit");
}
