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

#include <vector>
#include <sstream>

#include <boost/algorithm/string/trim.hpp>

#include <QtCore/qabstractitemmodel.h>

#include "hardwaretree.h"
#include "mainwindow.h"
#include "libcpuid.h"
#include "luxcore/luxcore.h"

using namespace std;
using namespace luxrays;
using namespace luxcore;

//------------------------------------------------------------------------------
// Hardware tree view
//------------------------------------------------------------------------------

const int HardwareTreeItem::NATIVECPU_INDEX = -1;

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

HardwareTreeModel::HardwareTreeModel(MainWindow *w, const string &enabledDevices) :
		QAbstractItemModel() {
	// Build the gui
	win = w;

	rootItem = new HardwareTreeItem("Hardware");

	// Native CPU
	HardwareTreeItem *nativeDev = new HardwareTreeItem("Native CPU");
	rootItem->appendChild(nativeDev);

	// CUDA devices
	HardwareTreeItem *cudaDev = new HardwareTreeItem("CUDA");
	rootItem->appendChild(cudaDev);
	
	// OpenCL devices
	HardwareTreeItem *oclDev = new HardwareTreeItem("OpenCL");
	rootItem->appendChild(oclDev);

	HardwareTreeItem *oclCPUDev = new HardwareTreeItem("CPUs");
	oclDev->appendChild(oclCPUDev);
	HardwareTreeItem *oclGPUDev = new HardwareTreeItem("GPUs and Accelerators");
	oclDev->appendChild(oclGPUDev);

	// Retrieve native CPU information with LibCPUID
	HardwareTreeItem *nativeCPUNode = nullptr;
	if (cpuid_present()) {
		struct cpu_raw_data_t raw;
		if (cpuid_get_raw_data(&raw) < 0) {
			LM_LOG("Error detecting native CPU raw data: " << cpuid_error());

			nativeCPUNode = new HardwareTreeItem(HardwareTreeItem::NATIVECPU_INDEX, "Error detecting CPU raw data");
		} else {
			struct cpu_id_t data;
			if (cpu_identify(&raw, &data) < 0) {
				LM_LOG("Error identifying the native CPU: " << cpuid_error());

				nativeCPUNode = new HardwareTreeItem(HardwareTreeItem::NATIVECPU_INDEX, "Error identifying CPU");
			} else {
				string cpuName(data.brand_str);
				boost::trim(cpuName);
				nativeCPUNode = new HardwareTreeItem(HardwareTreeItem::NATIVECPU_INDEX, cpuName.c_str());
				
				stringstream ss;
				ss << "Vendor: " << data.vendor_str;
				nativeCPUNode->appendChild(new HardwareTreeItem(ss.str().c_str()));
				
				ss.str("");
				ss << "Cores: " << data.num_cores;
				nativeCPUNode->appendChild(new HardwareTreeItem(ss.str().c_str()));
				
				ss.str("");
				ss << "Logical cores: " << data.num_logical_cpus;
				nativeCPUNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

				ss.str("");
				ss << "Total logical cores: " << data.total_logical_cpus;
				nativeCPUNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

				ss.str("");
				ss << "Code name: " << data.cpu_codename;
				nativeCPUNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

				ss.str("");
				ss << "Features:";
				for (int i = 0; i < NUM_CPU_FEATURES; i++)
					if (data.flags[i])
						ss << " " << cpu_feature_str((cpu_feature_t)i);
				nativeCPUNode->appendChild(new HardwareTreeItem(ss.str().c_str()));
			}
		}
	} else
		nativeCPUNode = new HardwareTreeItem(0, "Unknown CPU");

	nativeCPUNode->setChecked(false);
	nativeDev->appendChild(nativeCPUNode);
	
	// Retrieve the hardware information with LuxCore
	const Properties hwDevDescs = GetOpenCLDeviceDescs();
	const vector<string> hwDevDescPrefixs = hwDevDescs.GetAllUniqueSubNames("opencl.device");

	// Build the list of devices
	bool hasCUDAdevs = false;
	for (size_t i = 0; i < hwDevDescPrefixs.size(); ++i) {
		const string &prefix = hwDevDescPrefixs[i];

		BenchmarkDeviceDescription deviceDesc;

		deviceDesc.deviceName = hwDevDescs.Get(prefix + ".name").Get<string>();
		deviceDesc.platformName = hwDevDescs.Get(prefix + ".platform.name").Get<string>();
		deviceDesc.platformVersion = hwDevDescs.Get(prefix + ".platform.version").Get<string>();
		deviceDesc.deviceType = hwDevDescs.Get(prefix + ".type").Get<string>();
		deviceDesc.units = hwDevDescs.Get(prefix + ".units").Get<int>();
		deviceDesc.clock = hwDevDescs.Get(prefix + ".clock").Get<int>();
		deviceDesc.nativeVectorWidthFloat = hwDevDescs.Get(prefix + ".nativevectorwidthfloat").Get<int>();
		deviceDesc.globalMem = hwDevDescs.Get(prefix + ".maxmemory").Get<unsigned long long>();
		deviceDesc.localMem = hwDevDescs.Get(prefix + ".localmemory").Get<unsigned long long>();
		deviceDesc.constantMem = hwDevDescs.Get(prefix + ".constmemory").Get<unsigned long long>();

		deviceDesc.isCUDA = (deviceDesc.deviceType == "CUDA_GPU");
		deviceDesc.isOpenCL = !deviceDesc.isCUDA;
		deviceDesc.isOpenCLCPU = deviceDesc.isOpenCL && (deviceDesc.deviceType == "OPENCL_CPU");

		deviceDescs.push_back(deviceDesc);

		hasCUDAdevs = hasCUDAdevs || deviceDesc.isCUDA;
	}

	// Create the tree nodes
	for (size_t i = 0; i < deviceDescs.size(); ++i) {
		const BenchmarkDeviceDescription &deviceDesc = deviceDescs[i];

		HardwareTreeItem *newNode = new HardwareTreeItem(i, deviceDesc.deviceName.c_str());

		stringstream ss;
		ss << "Platform: " << deviceDesc.platformName;
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		ss.str("");
		ss << "Platform Version: " << deviceDesc.platformVersion;
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		ss.str("");
		ss << "Type: " << deviceDesc.deviceType;
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		ss.str("");
		ss << "Compute Units: " << deviceDesc.units;
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		ss.str("");
		ss << "Clock: " << deviceDesc.clock << " MHz";
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		ss.str("");
		ss << "Preferred vector width: " << deviceDesc.nativeVectorWidthFloat;
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		ss.str("");
		ss << "Max. Global Memory: " << (deviceDesc.globalMem / 1024) << " Kbytes";
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		ss.str("");
		ss << "Local Memory: " << (deviceDesc.localMem / 1024) << " Kbytes";
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		ss.str("");
		ss << "Max. Constant Memory: " << (deviceDesc.constantMem / 1024) << " Kbytes";
		newNode->appendChild(new HardwareTreeItem(ss.str().c_str()));

		// The default mode is GPU-only
		bool enabledDev = (hasCUDAdevs && deviceDesc.isCUDA) ||
				(!hasCUDAdevs && deviceDesc.isOpenCL && !deviceDesc.isOpenCLCPU );

		if ((enabledDevices != "") && (i < enabledDevices.length()))
			enabledDev = (enabledDevices.at(i) == '1');

		newNode->setChecked(enabledDev);

		if (deviceDesc.isCUDA)
			cudaDev->appendChild(newNode);
		else {
			if (deviceDesc.isOpenCLCPU)
				oclCPUDev->appendChild(newNode);
			else
				oclGPUDev->appendChild(newNode);
		}
	}
	
	useNativeCPU = false;
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

			if (item->isNativeCPU())
				useNativeCPU = true;
			else
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

bool HardwareTreeModel::getUseNativeCPU() const {
	return useNativeCPU;
}

vector<BenchmarkDeviceDescription> HardwareTreeModel::getSelectedDeviceDescs(
    const LuxMarkAppMode mode) const {
    vector<BenchmarkDeviceDescription> descs;

    switch (mode) {
        case DEMO_LUXCOREUI: // TODO
		case BENCHMARK_CUDA_GPU:
		case STRESSTEST_CUDA_GPU:
            for (size_t i = 0; i < deviceSelection.size(); ++i) {
                if (deviceDescs[i].isCUDA)
                    descs.push_back(deviceDescs[i]);
            }
            break;
        case BENCHMARK_OCL_GPU:
		case STRESSTEST_OCL_GPU:
            for (size_t i = 0; i < deviceSelection.size(); ++i) {
                if (deviceDescs[i].isOpenCL && !deviceDescs[i].isOpenCLCPU)
                    descs.push_back(deviceDescs[i]);
            }
            break;
		case BENCHMARK_CUSTOM:
		case STRESSTEST_CUSTOM:
            for (size_t i = 0; i < deviceSelection.size(); ++i) {
                if (deviceSelection[i])
                    descs.push_back(deviceDescs[i]);
            }
            break;
        case BENCHMARK_NATIVE:
		case STRESSTEST_NATIVE:
        case PAUSE:
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
