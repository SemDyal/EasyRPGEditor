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

#include "chipset_edit_scene.h"
#include "chipset_edit_mode.h"
#include <QWidget>
#include <qbuttongroup.h>
#include <lcf/rpg/chipset.h>

class ProjectData;

namespace Ui {
class ChipsetWidget;
}

class ChipsetWidget : public QWidget
{
	Q_OBJECT

public:
	using value_type = lcf::rpg::Chipset;

    explicit ChipsetWidget(ProjectData& project, QWidget *parent = nullptr);
    ~ChipsetWidget();

	void setData(lcf::rpg::Chipset* chipset);
    void drawLayers(QString chipset);
signals:
    void lowerModeChanged(Chipset::EditMode editMode);
    void upperModeChanged(Chipset::EditMode editMode);
    void terrainChanged(int terrain);
private slots:
    void on_layerTabWidget_currentChanged(int index);

    void on_chipsetPushButton_clicked();

private:
    Ui::ChipsetWidget *ui;
	ProjectData& m_project;
    ChipsetEditScene lower_scene = ChipsetEditScene(Core::LOWER);
    ChipsetEditScene upper_scene = ChipsetEditScene(Core::UPPER);
    Chipset::EditMode m_lowerEditMode;
    Chipset::EditMode m_upperEditMode;
    QButtonGroup *m_lowerEditModeButtonGroup;
    QButtonGroup *m_sequenceButtonGroup;
    QButtonGroup *m_speedButtonGroup;
    QButtonGroup *m_upperEditModeButtonGroup;
};

