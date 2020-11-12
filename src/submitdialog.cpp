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

#include <QDateTime>
#include <QTextStream>
#include <QUrlQuery>

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
		const QString score = QString::number(int(sampleSecs / 1000.0));
		const QString note = ui->noteTextEdit->toPlainText();
		const QString devCount = QString::number(descs.size());

        QString benchMode;
        switch (mode) {
            case BENCHMARK_CUDA_GPU:
                benchMode = "CUDA";
                break;
            case BENCHMARK_OCL_GPU:
                benchMode = "OpenCL";
                break;
            case BENCHMARK_NATIVE:
                benchMode = "C++";
                break;
            case BENCHMARK_CUSTOM:
                benchMode = "Custom";
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
        QUrlQuery paramsQuery(params);
		paramsQuery.addQueryItem("name", QUrl::toPercentEncoding(name));
		paramsQuery.addQueryItem("password", QUrl::toPercentEncoding(pwd));

		SD_LOG("Submitted data:");

		paramsQuery.addQueryItem("version", QUrl::toPercentEncoding(version));
		SD_LOG("version = " << version.toStdString());

		paramsQuery.addQueryItem("os", QUrl::toPercentEncoding(os));
		SD_LOG("os = " << os.toStdString());

		paramsQuery.addQueryItem("mode", QUrl::toPercentEncoding(benchMode));
		SD_LOG("mode = " << benchMode.toStdString());

		paramsQuery.addQueryItem("scene_name", QUrl::toPercentEncoding(scene));
		SD_LOG("scene_name = " << scene.toStdString());

		paramsQuery.addQueryItem("score", QUrl::toPercentEncoding(score));
		SD_LOG("score = " << score.toStdString());

		paramsQuery.addQueryItem("note", QUrl::toPercentEncoding(note));
		SD_LOG("note = " << note.toStdString());

		paramsQuery.addQueryItem("dev_count", QUrl::toPercentEncoding(devCount));
		SD_LOG("dev_count = " << devCount.toStdString());

		for (size_t i = 0; i < descs.size(); ++i) {
			SD_LOG("dev_platform_name = " << descs[i].platformName);
			paramsQuery.addQueryItem("dev_platform_name[]", QUrl::toPercentEncoding(QString(descs[i].platformName.c_str())));

			SD_LOG("dev_platform_ver = " << descs[i].platformVersion);
			paramsQuery.addQueryItem("dev_platform_ver[]", QUrl::toPercentEncoding(QString(descs[i].platformVersion.c_str())));

			SD_LOG("dev_name = " << descs[i].deviceName);
			paramsQuery.addQueryItem("dev_name[]", QUrl::toPercentEncoding(QString(descs[i].deviceName.c_str())));

			SD_LOG("dev_type = " << descs[i].deviceType);
			paramsQuery.addQueryItem("dev_type[]", QUrl::toPercentEncoding(QString(descs[i].deviceType.c_str())));

			SD_LOG("dev_units = " << descs[i].units);
			paramsQuery.addQueryItem("dev_units[]", QUrl::toPercentEncoding(QString::number(descs[i].units)));

			SD_LOG("dev_clock = " << descs[i].clock);
			paramsQuery.addQueryItem("dev_clock[]", QUrl::toPercentEncoding(QString::number(descs[i].clock)));

			SD_LOG("dev_native_float_vec_width = " << descs[i].nativeVectorWidthFloat);
			paramsQuery.addQueryItem("dev_native_float_vec_width[]", QUrl::toPercentEncoding(QString::number(descs[i].nativeVectorWidthFloat)));

			SD_LOG("dev_global_mem = " << descs[i].globalMem);
			paramsQuery.addQueryItem("dev_global_mem[]", QUrl::toPercentEncoding(QString::number(descs[i].globalMem)));

			SD_LOG("dev_local_mem = " << descs[i].localMem);
			paramsQuery.addQueryItem("dev_local_mem[]", QUrl::toPercentEncoding(QString::number(descs[i].localMem)));

			SD_LOG("dev_constant_mem = " << descs[i].constantMem);
			paramsQuery.addQueryItem("dev_constant_mem[]", QUrl::toPercentEncoding(QString::number(descs[i].constantMem)));
		}

        params.setQuery(paramsQuery);
		QByteArray data;
		data = params.query(QUrl::FullyEncoded).toUtf8();

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
