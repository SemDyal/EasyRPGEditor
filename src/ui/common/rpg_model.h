/*
 * This file is part of EasyRPG Editor.
 *
 * EasyRPG Editor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * EasyRPG Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EasyRPG Editor. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QAbstractListModel>
#include <QPixmap>
#include <QMessageBox>

#include <lcf/rpg/database.h>
#include "core.h"
#include "common/dbstring.h"
#include "common/filefinder.h"
#include "common/image_loader.h"
#include "model/rpg_reflect.h"

template<typename LCF>
class RpgModel : public QAbstractListModel {
public:
	/** Model fetches automatically the data from the Project based on LCF */
	RpgModel(ProjectData& project, QObject *parent = nullptr) :
			QAbstractListModel(parent), m_project(project), m_data(RpgReflect<LCF>::items(project.database())) {}
	/** Model takes the data from the passed vector */
	RpgModel(ProjectData& project, std::vector<LCF>& data, QObject *parent = nullptr) :
			QAbstractListModel(parent), m_project(project), m_data(data) {}
	int rowCount(const QModelIndex& = QModelIndex()) const override { return m_data.size(); }
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
	QVariant data(const QModelIndex &index, int role) const override;
    std::vector<LCF>& lcfData() const {
		return m_data;
	}

private:
	ProjectData& m_project;
	std::vector<LCF>& m_data;
};

enum ModelData {
	ModelDataId = Qt::UserRole,
	ModelDataObject
};

template<class LCF>
QVariant RpgModel<LCF>::data(const QModelIndex &index, int role) const {
	if (!index.isValid()) {
		return QVariant();
	} else if (role == Qt::DisplayRole || role == Qt::EditRole) {
		auto data = m_data[index.row()];
		return QString("%1: %2").arg(data.ID, 4, 10, QChar('0')).arg(ToQString(data.name));
	} else if (role == Qt::DecorationRole) {
		return typename RpgReflect<LCF>::model_type(m_project, m_data[index.row()]).preview();
	} else if (role == Qt::UserRole) {
		return m_data[index.row()].ID;
	} else if (role == ModelDataObject) {
		return QVariant::fromValue(&m_data[index.row()]);
	}

	return QVariant();
}

// Small note about this: these don't correct the IDs of the remaining items.
// This means this only really works for appending or removing from the end.
// At the time of writing these are the only two use cases, but if you implement
// random access insertion and deletion, you'll have to handle that to prevent bugs.
// Oh, and *do* warn on that; lots of things in RM2K3Script rely on hardcoded location. :p
template <class LCF>
bool RpgModel<LCF>::insertRows(int row, int count, const QModelIndex &parent) {
    beginInsertRows(parent, row, count);
    for (int i = row + 1; i < row + count + 1; i++) {
        LCF empty;
        empty.ID = i;
        m_data.push_back(empty);
    }
    endInsertRows();
    return true;
}

template <class LCF>
bool RpgModel<LCF>::removeRows(int row, int count, const QModelIndex &parent) {
    beginRemoveRows(parent, row, count);
    m_data.erase(m_data.begin() + row, m_data.begin() + row + count);
    endRemoveRows();
    return true;
}
