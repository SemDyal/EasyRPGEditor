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

#include "terrain_widget.h"
#include "common/lcf_widget_binding.h"
#include "ui_terrain_widget.h"

TerrainWidget::TerrainWidget(ProjectData& project, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TerrainWidget),
	m_project(project)
{
	ui->setupUi(this);

    m_spriteButtonGroup = new QButtonGroup(this);
    m_spriteButtonGroup->addButton(ui->spriteNormalButton, lcf::rpg::Terrain::BushDepth_normal);
    m_spriteButtonGroup->addButton(ui->spriteHalfRadioButton, lcf::rpg::Terrain::BushDepth_half);
    m_spriteButtonGroup->addButton(ui->spriteThirdRadioButton, lcf::rpg::Terrain::BushDepth_third);
    m_spriteButtonGroup->addButton(ui->spriteSemiRadioButton, lcf::rpg::Terrain::BushDepth_full);

    m_gridButtonGroup = new QButtonGroup(this);
    m_gridButtonGroup->addButton(ui->gridShallowRadioButton, 0);
    m_gridButtonGroup->addButton(ui->gridDeepRadioButton, 1);

    m_backgroundButtonGroup = new QButtonGroup(this);
    m_backgroundButtonGroup->addButton(ui->backgroundBackgroundRadioButton, lcf::rpg::Terrain::BGAssociation_background);
    m_backgroundButtonGroup->addButton(ui->backgroundFrameRadioButton, lcf::rpg::Terrain::BGAssociation_frame);

    LcfWidgetBinding::connect(this, ui->nameLineEdit);
    LcfWidgetBinding::connect<int32_t>(this, ui->damageSpinBox);
    LcfWidgetBinding::connect<int32_t>(this, ui->encounterRateSpinBox);
    // TODO: footstep sound
    LcfWidgetBinding::connect<bool>(this, ui->footstepDamageCheckBox);

    LcfWidgetBinding::connect<bool>(this, ui->oddsPreemptiveCheckBox);
    LcfWidgetBinding::connect<int32_t>(this, ui->oddsPreemptiveSpinBox);
    LcfWidgetBinding::connect<bool>(this, ui->oddsBackCheckBox);
    LcfWidgetBinding::connect<int32_t>(this, ui->oddsBackSpinBox);
    LcfWidgetBinding::connect<bool>(this, ui->oddsPincerCheckBox);
    LcfWidgetBinding::connect<int32_t>(this, ui->oddsPincerSpinBox);
    LcfWidgetBinding::connect<bool>(this, ui->oddsSurpriseCheckBox);
    LcfWidgetBinding::connect<int32_t>(this, ui->oddsSurpriseSpinBox);

    LcfWidgetBinding::connect<bool>(this, ui->vehicleBoatPassCheckBox);
    LcfWidgetBinding::connect<bool>(this, ui->vehicleShipPassCheckBox);
    LcfWidgetBinding::connect<bool>(this, ui->vehicleAirshipPassCheckBox);
    LcfWidgetBinding::connect<bool>(this, ui->vehicleAirshipLandCheckBox);
    LcfWidgetBinding::connect<int32_t>(this, m_spriteButtonGroup);
    LcfWidgetBinding::connect<int32_t>(this, m_gridButtonGroup);

    LcfWidgetBinding::connect<int32_t>(this, m_backgroundButtonGroup);
    // TODO: background_name
    // TODO: foreground name
    LcfWidgetBinding::connect<bool>(this, ui->fgFrameHCheckBox);
    LcfWidgetBinding::connect<int32_t>(this, ui->fgFrameHSpinBox);
    LcfWidgetBinding::connect<bool>(this, ui->fgFrameVCheckBox);
    LcfWidgetBinding::connect<int32_t>(this, ui->fgFrameVSpinBox);
    // TODO: background name
    LcfWidgetBinding::connect<bool>(this, ui->bgFrameHCheckBox);
    LcfWidgetBinding::connect<int32_t>(this, ui->bgFrameHSpinBox);
    LcfWidgetBinding::connect<bool>(this, ui->bgFrameVCheckBox);
    LcfWidgetBinding::connect<int32_t>(this, ui->bgFrameVSpinBox);

}

TerrainWidget::~TerrainWidget()
{
	delete ui;
}

void TerrainWidget::setData(lcf::rpg::Terrain* terrain)
{
    m_current = terrain ? terrain : &m_dummy;

    LcfWidgetBinding::bind(ui->nameLineEdit, m_current->name);
    LcfWidgetBinding::bind(ui->damageSpinBox, m_current->damage);
    LcfWidgetBinding::bind(ui->encounterRateSpinBox, m_current->encounter_rate);
    // TODO: footstep sound
    LcfWidgetBinding::bind(ui->footstepDamageCheckBox, m_current->on_damage_se);

    LcfWidgetBinding::bind<bool>(ui->oddsPreemptiveCheckBox, m_current->special_flags.back_enemies);
    LcfWidgetBinding::bind<int32_t>(ui->oddsPreemptiveSpinBox, m_current->special_back_enemies);
    LcfWidgetBinding::bind<bool>(ui->oddsBackCheckBox, m_current->special_flags.back_party);
    LcfWidgetBinding::bind<int32_t>(ui->oddsBackSpinBox, m_current->special_back_party);
    LcfWidgetBinding::bind<bool>(ui->oddsPincerCheckBox, m_current->special_flags.lateral_enemies);
    LcfWidgetBinding::bind<int32_t>(ui->oddsPincerSpinBox, m_current->special_lateral_enemies);
    LcfWidgetBinding::bind<bool>(ui->oddsSurpriseCheckBox, m_current->special_flags.lateral_party);
    LcfWidgetBinding::bind<int32_t>(ui->oddsSurpriseSpinBox, m_current->special_lateral_party);

    LcfWidgetBinding::bind(ui->vehicleBoatPassCheckBox, m_current->boat_pass);
    LcfWidgetBinding::bind(ui->vehicleShipPassCheckBox, m_current->ship_pass);
    LcfWidgetBinding::bind(ui->vehicleAirshipLandCheckBox, m_current->airship_land);
    LcfWidgetBinding::bind(ui->vehicleAirshipLandCheckBox, m_current->airship_land);
    LcfWidgetBinding::bind(m_spriteButtonGroup, m_current->bush_depth);
    LcfWidgetBinding::bind(m_gridButtonGroup, m_current->grid_location);

    LcfWidgetBinding::bind(m_backgroundButtonGroup, m_current->background_type);
    // TODO: background_name
    // TODO: foreground name
    LcfWidgetBinding::bind<bool>(ui->fgFrameHCheckBox, m_current->background_a_scrollh);
    LcfWidgetBinding::bind<int32_t>(ui->fgFrameHSpinBox, m_current->background_a_scrollh_speed);
    LcfWidgetBinding::bind<bool>(ui->fgFrameVCheckBox, m_current->background_a_scrollv);
    LcfWidgetBinding::bind<int32_t>(ui->fgFrameVSpinBox, m_current->background_a_scrollv_speed);
    // TODO: background name
    LcfWidgetBinding::bind<bool>(ui->bgFrameHCheckBox, m_current->background_b_scrollh);
    LcfWidgetBinding::bind<int32_t>(ui->bgFrameHSpinBox, m_current->background_b_scrollh_speed);
    LcfWidgetBinding::bind<bool>(ui->bgFrameVCheckBox, m_current->background_b_scrollv);
    LcfWidgetBinding::bind<int32_t>(ui->bgFrameVSpinBox, m_current->background_b_scrollv_speed);



    this->setEnabled(terrain != nullptr);
}
