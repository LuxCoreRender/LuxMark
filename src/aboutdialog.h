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

#ifndef _ABOUTDIALOG_H
#define	_ABOUTDIALOG_H

#include <cstddef>

#include "ui_aboutdialog.h"

class AboutDialog : public QDialog {
	Q_OBJECT

public:
	AboutDialog(QWidget *parent = NULL);
	~AboutDialog();

private:
	Ui::AboutDialog *ui;
};

#endif	/* _ABOUTDIALOG_H */

