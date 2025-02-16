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

#include "chipset_edit_scene.h"
#include "chipset_widget.h"
#include "common/dbstring.h"
#include <QGraphicsSceneMouseEvent>
#include <QSvgRenderer>
#include <QTextCharFormat>
#include <qgraphicseffect.h>

ChipsetEditScene::ChipsetEditScene(Core::Layer layer, lcf::rpg::Chipset *data, QObject *parent)
    : ChipsetScene{"", parent}
{
    m_layer = layer;
    m_data = data;
    QGraphicsOpacityEffect* opacity = new QGraphicsOpacityEffect;
    opacity->setOpacity(0.90);
    addItem(&m_overlay);
    m_overlay.setGraphicsEffect(opacity);
    if (layer == Core::LOWER) {
        m_editMode = Chipset::EDIT_MODE_TERRAIN;
    } else {
        m_editMode = Chipset::EDIT_MODE_PASSABILITY;
    }
}

void ChipsetEditScene::setData(lcf::rpg::Chipset *data) {
    m_data = data;
    set_chipset(ToQString(data->chipset_name));
    setEditMode(m_editMode);
}

void ChipsetEditScene::setTerrain(QModelIndex terrain, QModelIndex _) {
    m_terrain = terrain.row() + 1;
}

void ChipsetEditScene::setEditMode(int editMode) {
    QPixmap overlay(192, m_layer == Core::LOWER ? 864 : 768);
    overlay.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&overlay);
    painter.setRenderHint(QPainter::Antialiasing);
    int x = 0;
    int y = 0;

    if (editMode == Chipset::EDIT_MODE_TERRAIN) {
        if (m_layer == Core::UPPER) {
            throw "ChipsetEditScene: Cannot set terrain on upper layer!";
        }
        QPen outline;
        outline.setColor(Qt::black);
        outline.setWidth(5);
        QFont font;
        font.setBold(false);
        painter.setBrush(Qt::white);
        painter.setFont(font);
        for (int16_t i : m_data->terrain_data) {
            painter.setPen(outline);
            QRect bounds(x, y, 32, 32);
            QFontMetrics fm(font);
            int width = fm.horizontalAdvance(QString::number(i));
            QPainterPath path;
            path.addText(x + 16 - width / 2, y+21, font, QString::number(i));
            painter.drawPath(path);
            painter.setPen(Qt::white);
            painter.drawPath(path);
            x += 32;
            if (x == 192) {
                x = 0;
                y += 32;
            }
        }
    } else if (editMode == Chipset::EDIT_MODE_PASSABILITY) {
        for (uint8_t i : m_layer == Core::LOWER ? m_data->passable_data_lower : m_data->passable_data_upper) {
            if ((i >> 5) & 1) {
                painter.drawPixmap(x, y, m_pass_square.pixmap(32));
            } else if ((i >> 4) & 1) {
                painter.drawPixmap(x, y, m_pass_star.pixmap(32));
            } else if (!(i & 0b1111)) {
                painter.drawPixmap(x, y, m_pass_x.pixmap(32));
            } else {
                painter.drawPixmap(x, y, m_pass_o.pixmap(32));
            }
            x += 32;
            if (x == 192) {
                x = 0;
                y += 32;
            }
        }
    } else if (editMode == Chipset::EDIT_MODE_EDGES) {
        for (uint8_t i : m_layer == Core::LOWER ? m_data->passable_data_lower : m_data->passable_data_upper) {
            // down
            painter.drawPixmap(x, y, i & 1 ? m_counter_on.pixmap(32) : m_counter_off.pixmap(32));
            // left
            painter.drawPixmap(x, y, (i >> 1) & 1 ? m_counter_on.pixmap(32) : m_counter_off.pixmap(32));
            // right
            painter.drawPixmap(x, y, (i >> 2) & 1 ? m_counter_on.pixmap(32) : m_counter_off.pixmap(32));
            // up
            painter.drawPixmap(x, y, (i >> 3) & 1 ? m_counter_on.pixmap(32) : m_counter_off.pixmap(32));
            x += 32;
            if (x == 192) {
                x = 0;
                y += 32;
            }
        }
    } else if (editMode == Chipset::EDIT_MODE_COUNTER) {
        if (m_layer == Core::LOWER) {
            throw "ChipsetEditScene: Cannot set counter on lower layer!";
        }
        for (uint8_t i : m_data->passable_data_upper) {
            painter.drawPixmap(x, y, (i >> 6) & 1 ? m_counter_on.pixmap(32) : m_counter_off.pixmap(32));
            x += 32;
            if (x == 192) {
                x = 0;
                y += 32;
            }
        }
    }
    painter.end();
    m_overlay.setPixmap(overlay);

    m_editMode = Chipset::EditMode(editMode);
}

void ChipsetEditScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (!(sceneRect().marginsRemoved(QMarginsF(0,0,1,1))).contains(mouseEvent->scenePos())) {
        return;
    }
    if (m_editMode == Chipset::EDIT_MODE_TERRAIN) {
        int offset = ((int)mouseEvent->scenePos().y() / 32) * 6 + ((int)mouseEvent->scenePos().x() / 32) % 6;
        if (m_data->terrain_data.size() > offset && m_data->terrain_data[offset] != m_terrain && mouseEvent->buttons() == Qt::LeftButton) {
            m_data->terrain_data[offset] = m_terrain;
            setEditMode(Chipset::EDIT_MODE_TERRAIN);
        }
    }
}

void ChipsetEditScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (m_editMode == Chipset::EDIT_MODE_TERRAIN) {
        int offset = ((int)mouseEvent->scenePos().y() / 32) * 6 + ((int)mouseEvent->scenePos().x() / 32) % 6;
        if (m_data->terrain_data.size() > offset && m_data->terrain_data[offset] != m_terrain && mouseEvent->buttons() == Qt::LeftButton) {
            m_data->terrain_data[offset] = m_terrain;
            setEditMode(Chipset::EDIT_MODE_TERRAIN);
        }
    } else if (m_editMode == Chipset::EDIT_MODE_PASSABILITY) {
        int offset = ((int)mouseEvent->scenePos().y() / 32) * 6 + ((int)mouseEvent->scenePos().x() / 32) % 6;
        uint8_t &i = m_layer == Core::LOWER ? m_data->passable_data_lower[offset] : m_data->passable_data_upper[offset];
        if (mouseEvent->buttons() == Qt::LeftButton) {
            if ((i >> 5) & 1) { // square to o
                i = 0b1111;
            } else if ((i >> 4) & 1) { // star to square/o
                if (m_layer == Core::LOWER && 6 <= offset && offset <= 17) {
                    i = 0b100000;
                } else {
                    i = 0b1111;
                }
            } else if (!(i & 0b1111)) { // x to star
                i = 0b10000;
            } else { // o to x
                i = 0;
            }
        } else if (mouseEvent->buttons() == Qt::RightButton) {
            if ((i >> 5) & 1) { // square to star
                i = 0b10000;
            } else if ((i >> 4) & 1) { // star to x
                i = 0;
            } else if (!(i & 0b1111)) { // x to o
                i = 0b1111;
            } else { // o to square/star
                if (m_layer == Core::LOWER && 6 <= offset && offset <= 17) {
                    i = 0b100000;
                } else {
                    i = 0b10000;
                }
            }
        }
    } else if (m_editMode == Chipset::EDIT_MODE_EDGES) {
    } else if (m_editMode == Chipset::EDIT_MODE_COUNTER) {
        int offset = ((int)mouseEvent->scenePos().y() / 32) * 6 + ((int)mouseEvent->scenePos().x() / 32) % 6;
        if (m_data->passable_data_upper.size() > offset && mouseEvent->buttons() == Qt::LeftButton) {
            m_data->passable_data_upper[offset] ^= Chipset::PassableFlag::Counter;
        }
    }
    setEditMode(m_editMode);
}

void ChipsetEditScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    ChipsetEditScene::mousePressEvent(mouseEvent);
}
