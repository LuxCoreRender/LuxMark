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

#ifndef _HARDWARETREE_H
#define _HARDWARETREE_H

#ifndef Q_MOC_RUN
#include <vector>

#include <boost/thread/thread.hpp>

#include <QVariant>
#include <QModelIndex>
#include <QAbstractItemModel>

#include "luxmarkdefs.h"
#endif

using namespace std;

class MainWindow;


//------------------------------------------------------------------------------
// Hardware tree view
//------------------------------------------------------------------------------

class HardwareTreeItem {
public:
	static const int NATIVECPU_INDEX;

	HardwareTreeItem(const QVariant &data,
			HardwareTreeItem *parent = 0);
	HardwareTreeItem(const int deviceIndex, const QVariant &data,
			HardwareTreeItem *parent = 0);
	~HardwareTreeItem();

	bool isNativeCPUNode() const { return (deviceIndex == NATIVECPU_INDEX); }
	bool isOptixNode() const {
		return (itemData.toString().toStdString() == "Use Optix/RTX");
	}

	int getDeviceIndex() const { return deviceIndex; }
	

	void appendChild(HardwareTreeItem *child);

	HardwareTreeItem *child(int row);
	int childCount() const;
	int columnCount() const;
	QVariant data(int column) const;
	int row() const;
	HardwareTreeItem *parent();

	bool isEnabled() const { return enabled; }
	void setEnabled(const bool e) {  enabled = e; }
	
	bool isCheckable() const { return checkable; }
	void setCheckable(const bool c) { checkable = c; }

	bool isChecked() const { return checked; }
	void setChecked(const bool c) { checked = c; }

private:
	int deviceIndex;

	QList<HardwareTreeItem *> childItems;
	QVariant itemData;
	HardwareTreeItem *parentItem;

	bool enabled, checkable, checked;
};

//------------------------------------------------------------------------------
// HardwareTreeModel
//------------------------------------------------------------------------------

typedef struct {
	string platformName;
	string platformVersion;
	string deviceName;
	string deviceType;
	int units;
	int clock;
    int nativeVectorWidthFloat;
	unsigned long long globalMem;
	unsigned long long localMem;
	unsigned long long constantMem;
	int cudaMajorVersion, cudaMinorVersion;
	bool isOpenCL, isOpenCLCPU, isCUDA;

	bool enabledDevice, useOptix;
} BenchmarkDeviceDescription;


class HardwareTreeModel : public QAbstractItemModel {
	Q_OBJECT

public:
	HardwareTreeModel(MainWindow *win, const bool useCPU,
			const string &enabledDevices, const string &enabledOptixDevices);
	~HardwareTreeModel();

	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	QVariant data(const QModelIndex &index, int role) const;
	QModelIndex index(int row, int column,
			const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;

	string getDeviceSelectionString() const;
	string getOptixSelectionString() const;
	bool getUseNativeCPU() const;
    vector<BenchmarkDeviceDescription> getSelectedDeviceDescs(const LuxMarkAppMode mode) const;

private:
	MainWindow *win;

	HardwareTreeItem *rootItem;

    vector<BenchmarkDeviceDescription> deviceDescs;
	bool useNativeCPU;
};

//------------------------------------------------------------------------------
// DeviceTreeModel
//------------------------------------------------------------------------------

class DeviceListModel : public QAbstractItemModel {
	Q_OBJECT

public:
	DeviceListModel(const vector<BenchmarkDeviceDescription> &devDescs);
	~DeviceListModel();

	QVariant data(const QModelIndex &index, int role) const;
	QModelIndex index(int row, int column,
			const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;

private:
	MainWindow *win;

	HardwareTreeItem *rootItem;
};

#endif // _HARDWARETREE_H
