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

#include "chipset_widget.h"
#include "common/dbstring.h"
#include "common/lcf_widget_binding.h"
#include "ui_chipset_widget.h"

#include <ui/common/rpg_model.h>

#include <ui/picker/picker_dialog.h>
#include <ui/picker/picker_image_widget.h>

ChipsetWidget::ChipsetWidget(ProjectData& project, QWidget *parent) :
	QWidget(parent),
    ui(new Ui::ChipsetWidget),
	m_project(project)
{
	ui->setupUi(this);

    LcfWidgetBinding::connect(this, ui->nameLineEdit);
    LcfWidgetBinding::connect(this, ui->chipsetLineEdit);
    m_lowerEditModeButtonGroup = new QButtonGroup(this);
    m_lowerEditModeButtonGroup->addButton(ui->lowerEditTerrainButton, Chipset::EDIT_MODE_TERRAIN);
    m_lowerEditModeButtonGroup->addButton(ui->lowerEditPassabilityButton, Chipset::EDIT_MODE_PASSABILITY);
    m_lowerEditModeButtonGroup->addButton(ui->lowerEditEdgesButton, Chipset::EDIT_MODE_EDGES);
    connect(m_lowerEditModeButtonGroup, &QButtonGroup::idClicked, &lower_scene, &ChipsetEditScene::setEditMode);

    m_sequenceButtonGroup = new QButtonGroup(this);
    m_sequenceButtonGroup->addButton(ui->waterSequence1232RadioButton, lcf::rpg::Chipset::AnimType_reciprocating);
    m_sequenceButtonGroup->addButton(ui->waterSequence123RadioButton, lcf::rpg::Chipset::AnimType_cyclic);

    m_speedButtonGroup = new QButtonGroup(this);
    m_speedButtonGroup->addButton(ui->waterSpeedSlowRadioButton, 0);
    m_speedButtonGroup->addButton(ui->waterSpeedFastRadioButton, 1);

    m_upperEditModeButtonGroup = new QButtonGroup(this);
    m_upperEditModeButtonGroup->addButton(ui->upperEditPassabilityButton, Chipset::EDIT_MODE_PASSABILITY);
    m_upperEditModeButtonGroup->addButton(ui->upperEditEdgesButton, Chipset::EDIT_MODE_EDGES);
    m_upperEditModeButtonGroup->addButton(ui->upperEditCounterButton, Chipset::EDIT_MODE_COUNTER);

    //FIXME: create modelholder to share one model instead of making a new one
    ui->terrainListView->setModel(new RpgModel<lcf::rpg::Terrain>(project));
    connect(ui->terrainListView->selectionModel(), &QItemSelectionModel::currentRowChanged, &lower_scene, &ChipsetEditScene::setTerrain);
}

ChipsetWidget::~ChipsetWidget() {
	delete ui;
}

void ChipsetWidget::setData(lcf::rpg::Chipset* chipset) {
    lower_scene.setData(chipset);
    upper_scene.setData(chipset);

    LcfWidgetBinding::bind(ui->nameLineEdit, chipset->name);
    LcfWidgetBinding::bind(ui->chipsetLineEdit, chipset->chipset_name);

    LcfWidgetBinding::bind(m_sequenceButtonGroup, chipset->animation_type);
    LcfWidgetBinding::bind(m_speedButtonGroup, chipset->animation_speed);

    drawLayers(ToQString(chipset->chipset_name));
    ui->lowerGraphicsView->setScene(&lower_scene);
    ui->upperGraphicsView->setScene(&upper_scene);
}

void ChipsetWidget::drawLayers(QString chipset) {
    lower_scene.set_chipset(chipset);
    lower_scene.draw_overview(RpgPainter::ALL_LOWER);
    upper_scene.force_chipset(lower_scene.share_chipset());
    upper_scene.draw_overview(RpgPainter::ALL_UPPER);
}

void ChipsetWidget::on_layerTabWidget_currentChanged(int index) {
    if (index) {
        disconnect(m_lowerEditModeButtonGroup, &QButtonGroup::idClicked, &lower_scene, &ChipsetEditScene::setEditMode);
        connect(m_upperEditModeButtonGroup, &QButtonGroup::idClicked, &upper_scene, &ChipsetEditScene::setEditMode);
    } else {
        connect(m_lowerEditModeButtonGroup, &QButtonGroup::idClicked, &lower_scene, &ChipsetEditScene::setEditMode);
        disconnect(m_upperEditModeButtonGroup, &QButtonGroup::idClicked, &upper_scene, &ChipsetEditScene::setEditMode);
    }
}


void ChipsetWidget::on_chipsetPushButton_clicked() {
    auto* widget = new PickerImageWidget(this);
    PickerDialog dialog(m_project, FileFinder::FileType::Image, widget, this);
    QObject::connect(&dialog, &PickerDialog::fileSelected, [&](const QString& baseName) {
        ui->chipsetLineEdit->setText(baseName);
    });
    dialog.setDirectoryAndFile(CHIPSET, ui->chipsetLineEdit->text());
    dialog.exec();
    drawLayers(ui->chipsetLineEdit->text());
}

