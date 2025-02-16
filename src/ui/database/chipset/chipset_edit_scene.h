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

#include "chipset_edit_mode.h"
#include <ui/common/chipset_scene.h>
#include "core.h"

class ChipsetEditScene : public ChipsetScene
{
    Q_OBJECT

public:
    explicit ChipsetEditScene(Core::Layer layer, lcf::rpg::Chipset *data = nullptr, QObject *parent = nullptr);
    void setData(lcf::rpg::Chipset *data);
public slots:
    void setEditMode(int editMode);
    void setTerrain(QModelIndex terrain, QModelIndex _);
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
private:
    lcf::rpg::Chipset *m_data;
    Chipset::EditMode m_editMode;
    Core::Layer m_layer;
    QGraphicsPixmapItem m_overlay;
    int16_t m_terrain = 1;

    QIcon m_pass_o = QIcon(":/passability/passability_o");
    QIcon m_pass_x = QIcon(":/passability/passability_x");
    QIcon m_pass_square = QIcon(":/passability/passability_square");
    QIcon m_pass_star = QIcon(":/passability/passability_star");
    QIcon m_counter_on = QIcon(":/passability/passability_counter_on");
    QIcon m_counter_off = QIcon(":/passability/passability_counter_off");
};
