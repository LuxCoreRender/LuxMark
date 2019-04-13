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

#include <vector>
#include <sstream>

#include <QtCore/qabstractitemmodel.h>

#include "hardwaretree.h"
#include "mainwindow.h"
#include "luxcore/luxcore.h"

using namespace std;
using namespace luxrays;
using namespace luxcore;

//------------------------------------------------------------------------------
// Hardware tree view
//------------------------------------------------------------------------------

HardwareTreeItem::HardwareTreeItem(const QVariant &data, HardwareTreeItem *parent) {
	deviceIndex = 0;
	parentItem = parent;
	itemData = data;
	checkable = false;
	checked = false;
}

HardwareTreeItem::HardwareTreeItem(const int index, const QVariant &data,
		HardwareTreeItem *parent) {
	deviceIndex = index;
	parentItem = parent;
	itemData = data;
	checkable = true;
	checked = true;
}

HardwareTreeItem::~HardwareTreeItem() {
	qDeleteAll(childItems);
}

void HardwareTreeItem::appendChild(HardwareTreeItem *item) {
	item->parentItem = this;
	childItems.append(item);
}

HardwareTreeItem *HardwareTreeItem::child(int row) {
	return childItems.value(row);
}

int HardwareTreeItem::childCount() const {
	return childItems.count();
}

int HardwareTreeItem::columnCount() const {
	return 1;
}

QVariant HardwareTreeItem::data(int column) const {
	return itemData;
}

HardwareTreeItem *HardwareTreeItem::parent() {
	return parentItem;
}

int HardwareTreeItem::row() const {
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<HardwareTreeItem *>(this));

	return 0;
}

//------------------------------------------------------------------------------
// HardwareTreeModel
//------------------------------------------------------------------------------

HardwareTreeModel::HardwareTreeModel(MainWindow *w) : QAbstractItemModel() {
	// Build the gui
	win = w;

	rootItem = new HardwareTreeItem("Hardware");

	// OpenCL devices
	HardwareTreeItem *oclDev = new HardwareTreeItem("OpenCL");
	rootItem->appendChild(oclDev);

	HardwareTreeItem *oclCPUDev = new HardwareTreeItem("CPUs");
	oclDev->appendChild(oclCPUDev);
	HardwareTreeItem *oclGPUDev = new HardwareTreeItem("GPUs and Accelerators");
	oclDev->appendChild(oclGPUDev);

	// Retrieve the hardware information with LuxCore
	const Properties oclDevDescs = GetOpenCLDeviceDescs();
	const vector<string> oclDevDescPrefixs = oclDevDescs.GetAllUniqueSubNames("opencl.device");

	for (size_t i = 0; i < oclDevDescPrefixs.size(); ++i) {
		const string &prefix = oclDevDescPrefixs[i];

		BenchmarkDeviceDescription deviceDesc;

		deviceDesc.deviceName = oclDevDescs.Get(prefix + ".name").Get<string>();
		HardwareTreeItem *newNode = new HardwareTreeItem(i, deviceDesc.deviceName.c_str());

		stringstream ss;
		deviceDesc.platformName = oclDevDescs.Get(prefix + ".platform.name").Get<string>();
		ss << "Platform: " << deviceDesc.platformName;
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		ss.str("");
		deviceDesc.platformVersion = oclDevDescs.Get(prefix + ".platform.version").Get<string>();
		ss << "Platform Version: " << deviceDesc.platformVersion;
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		ss.str("");
		deviceDesc.deviceType = oclDevDescs.Get(prefix + ".type").Get<string>();
		ss << "Type: " << deviceDesc.deviceType;
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		ss.str("");
		deviceDesc.units = oclDevDescs.Get(prefix + ".units").Get<int>();
		ss << "Compute Units: " << deviceDesc.units;
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		ss.str("");
		deviceDesc.clock = oclDevDescs.Get(prefix + ".clock").Get<int>();
		ss << "Clock: " << deviceDesc.clock << " MHz";
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		ss.str("");
		deviceDesc.nativeVectorWidthFloat = oclDevDescs.Get(prefix + ".nativevectorwidthfloat").Get<int>();
		ss << "Preferred vector width: " << deviceDesc.nativeVectorWidthFloat;
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		ss.str("");
		deviceDesc.globalMem = oclDevDescs.Get(prefix + ".maxmemory").Get<unsigned long long>();
		ss << "Max. Global Memory: " << (deviceDesc.globalMem / 1024) << " Kbytes";
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		ss.str("");
		deviceDesc.localMem = oclDevDescs.Get(prefix + ".localmemory").Get<unsigned long long>();
		ss << "Local Memory: " << (deviceDesc.localMem / 1024) << " Kbytes";
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		ss.str("");
		deviceDesc.constantMem = oclDevDescs.Get(prefix + ".constmemory").Get<unsigned long long>();
		ss << "Max. Constant Memory: " << (deviceDesc.constantMem / 1024) << " Kbytes";
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		const bool isCPUDev = (deviceDesc.deviceType == "CPU");
		if (isCPUDev) {
			// The default mode is GPU-only
			newNode->setChecked(false);
			oclCPUDev->appendChild(newNode);
		} else {
			newNode->setChecked(true);
			oclGPUDev->appendChild(newNode);
		}

		deviceDescs.push_back(deviceDesc);
		deviceSelection.push_back(!isCPUDev);
		isCPU.push_back(isCPUDev);
	}
}

HardwareTreeModel::~HardwareTreeModel() {
	delete rootItem;
}

int HardwareTreeModel::columnCount(const QModelIndex &parent) const {
	if (parent.isValid())
		return static_cast<HardwareTreeItem *>(parent.internalPointer())->columnCount();
	else
		return rootItem->columnCount();
}

QVariant HardwareTreeModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid())
		return QVariant();

	if (role == Qt::DisplayRole) {
		HardwareTreeItem *item = static_cast<HardwareTreeItem *>(index.internalPointer());

		return item->data(index.column());
	} else if (role ==  Qt::CheckStateRole) {
		HardwareTreeItem *item = static_cast<HardwareTreeItem *>(index.internalPointer());

		if (item->isCheckable()) {
			if (item->isChecked())
				return Qt::Checked;
			else
				return Qt::Unchecked;
		} else
			return QVariant();
	} else if (role == Qt::ForegroundRole) {
		HardwareTreeItem *item = static_cast<HardwareTreeItem *>(index.internalPointer());

		if (item->isCheckable()) {
			// It is a device node
			return QVariant(QColor(Qt::blue));
		} else {
			if (item->childCount() == 0) {
				return QVariant(QColor(Qt::darkGray));
			} else {
				// It is a leaf
				return QVariant();
			}
		}
	} else
		return QVariant();
}

QModelIndex HardwareTreeModel::index(int row, int column, const QModelIndex &parent) const {
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	HardwareTreeItem *parentItem;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<HardwareTreeItem*>(parent.internalPointer());

	HardwareTreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex HardwareTreeModel::parent(const QModelIndex &index) const {
	if (!index.isValid())
		return QModelIndex();

	HardwareTreeItem *childItem = static_cast<HardwareTreeItem*>(index.internalPointer());
	HardwareTreeItem *parentItem = childItem->parent();

	if (parentItem == rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int HardwareTreeModel::rowCount(const QModelIndex &parent) const {
	HardwareTreeItem *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<HardwareTreeItem*>(parent.internalPointer());

	return parentItem->childCount();
}

Qt::ItemFlags HardwareTreeModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	HardwareTreeItem *item = static_cast<HardwareTreeItem *>(index.internalPointer());

	if (item->isCheckable())
		return Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	else
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool HardwareTreeModel::setData(const QModelIndex &index, const QVariant &value,
		int role) {
	if (!index.isValid())
		return false;

	if (role == Qt::CheckStateRole) {
		HardwareTreeItem *item = static_cast<HardwareTreeItem *>(index.internalPointer());

		const bool v = (value == Qt::Checked);
		if (v != item->isChecked()) {
			item->setChecked(v);

			// Set pause mode
			win->Pause();

			deviceSelection[item->getDeviceIndex()] = v;
		}

		return true;
	} else
		return false;
}

string HardwareTreeModel::getDeviceSelectionString() const {
	stringstream ss;

	for (size_t i = 0; i < deviceSelection.size(); ++i)
		ss << (deviceSelection[i] ? "1" : "0");

	return ss.str();
}

vector<BenchmarkDeviceDescription> HardwareTreeModel::getSelectedDeviceDescs(
    const LuxMarkAppMode mode) const {
    vector<BenchmarkDeviceDescription> descs;

    switch (mode) {
        case DEMO_LUXVR:
        case STRESSTEST_OCL_GPU:
        case BENCHMARK_OCL_GPU:
            for (size_t i = 0; i < deviceSelection.size(); ++i) {
                if (!isCPU[i])
                    descs.push_back(deviceDescs[i]);
            }
            break;
        case STRESSTEST_OCL_CPUGPU:
        case BENCHMARK_OCL_CPUGPU:
            for (size_t i = 0; i < deviceSelection.size(); ++i)
                descs.push_back(deviceDescs[i]);
            break;
        case STRESSTEST_OCL_CPU:
        case BENCHMARK_OCL_CPU:
            for (size_t i = 0; i < deviceSelection.size(); ++i) {
                if (isCPU[i])
                    descs.push_back(deviceDescs[i]);
            }
            break;
        case BENCHMARK_OCL_CUSTOM:
            for (size_t i = 0; i < deviceSelection.size(); ++i) {
                if (deviceSelection[i])
                    descs.push_back(deviceDescs[i]);
            }
            break;
        case PAUSE:
        case BENCHMARK_NATIVE:
            break;
    }

    return descs;
}

//------------------------------------------------------------------------------
// DeviceTreeModel
//------------------------------------------------------------------------------

DeviceListModel::DeviceListModel(const vector<BenchmarkDeviceDescription> &descs) :
			QAbstractItemModel() {
	rootItem = new HardwareTreeItem("Devices");

	for (size_t i = 0; i < descs.size(); ++i) {
		const BenchmarkDeviceDescription &desc = descs[i];

		stringstream ss;
		ss << desc.deviceName << " [" << desc.deviceType << ", " <<
				desc.units << ", " << desc.clock << " MHz]";

		HardwareTreeItem *newNode = new HardwareTreeItem(ss.str().c_str());
		rootItem->appendChild(newNode);
	}
}

DeviceListModel::~DeviceListModel() {
	delete rootItem;
}

int DeviceListModel::columnCount(const QModelIndex &parent) const {
	if (parent.isValid())
		return static_cast<HardwareTreeItem *>(parent.internalPointer())->columnCount();
	else
		return rootItem->columnCount();
}

QVariant DeviceListModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid())
		return QVariant();

	if (role == Qt::DisplayRole) {
		HardwareTreeItem *item = static_cast<HardwareTreeItem *>(index.internalPointer());

		return item->data(index.column());
	} else if (role == Qt::ForegroundRole) {
		return QVariant(QColor(Qt::blue));
	} else
		return QVariant();
}

QModelIndex DeviceListModel::index(int row, int column, const QModelIndex &parent) const {
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	HardwareTreeItem *parentItem;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<HardwareTreeItem*>(parent.internalPointer());

	HardwareTreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex DeviceListModel::parent(const QModelIndex &index) const {
	if (!index.isValid())
		return QModelIndex();

	HardwareTreeItem *childItem = static_cast<HardwareTreeItem*>(index.internalPointer());
	HardwareTreeItem *parentItem = childItem->parent();

	if (parentItem == rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int DeviceListModel::rowCount(const QModelIndex &parent) const {
	HardwareTreeItem *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<HardwareTreeItem*>(parent.internalPointer());

	return parentItem->childCount();
}

Qt::ItemFlags DeviceListModel::flags(const QModelIndex &index) const {
	return 0;
}
