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
        m_outline.setColor(Qt::black);
        m_outline.setWidth(5);
        m_font.setBold(false);

    } else {
        m_editMode = Chipset::EDIT_MODE_PASSABILITY;
    }
}

void ChipsetEditScene::drawTerrainPiece(int x, int y, int16_t value, bool highlight) {
    if (m_numberCache.contains(value)) {
        m_painter.drawPixmap(x, y, 32, 32, m_numberCache[value]);
    } else {
        QPixmap numberPixmap(32, 32);
        numberPixmap.fill(Qt::transparent);
        QPainter num_painter;
        num_painter.begin(&numberPixmap);
        num_painter.setRenderHint(QPainter::Antialiasing);
        num_painter.setBrush(Qt::white);
        num_painter.setFont(m_font);
        num_painter.setPen(m_outline);
        QRect bounds(0, 0, 32, 32);
        QFontMetrics fm(m_font);
        int width = fm.horizontalAdvance(QString::number(value));
        QPainterPath path;
        path.addText(16 - width / 2, 21, m_font, QString::number(value));
        num_painter.drawPath(path);
        num_painter.setPen(highlight ? Qt::yellow : Qt::white);
        num_painter.drawPath(path);
        num_painter.end();
        m_painter.drawPixmap(x, y, 32, 32, numberPixmap);
        m_numberCache[value] = numberPixmap;
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
    m_painter.begin(&overlay);
    m_painter.setRenderHint(QPainter::Antialiasing);
    int x = 0;
    int y = 0;

    if (editMode == Chipset::EDIT_MODE_TERRAIN) {
        if (m_layer == Core::UPPER) {
            throw "ChipsetEditScene: Cannot set terrain on upper layer!";
        }
        for (int16_t i : m_data->terrain_data) {
            drawTerrainPiece(x, y, i, false);
            x += 32;
            if (x == 192) {
                x = 0;
                y += 32;
            }
        }
    } else if (editMode == Chipset::EDIT_MODE_PASSABILITY) {
        for (uint8_t i : m_layer == Core::LOWER ? m_data->passable_data_lower : m_data->passable_data_upper) {
            if ((i >> 5) & 1) {
                m_painter.drawPixmap(x, y, m_pass_square.pixmap(32));
            } else if ((i >> 4) & 1) {
                m_painter.drawPixmap(x, y, m_pass_star.pixmap(32));
            } else if (!(i & 0b1111)) {
                m_painter.drawPixmap(x, y, m_pass_x.pixmap(32));
            } else {
                m_painter.drawPixmap(x, y, m_pass_o.pixmap(32));
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
            if (i & 1) {
                m_painter.drawPixmap(x, y, m_edges_down.pixmap(32));
            } else {
                m_painter.drawPixmap(x, y+10, m_counter_off.pixmap(32));
            }
            // left
            if ((i >> 1) & 1) {
                m_painter.drawPixmap(x, y, m_edges_left.pixmap(32));
            } else {
                m_painter.drawPixmap(x-10, y, m_counter_off.pixmap(32));
            }
            // right
            if ((i >> 2) & 1) {
                m_painter.drawPixmap(x, y, m_edges_right.pixmap(32));
            } else {
                m_painter.drawPixmap(x+10, y, m_counter_off.pixmap(32));
            }
            // up
            if ((i >> 3) & 1) {
                m_painter.drawPixmap(x, y, m_edges_up.pixmap(32));
            } else {
                m_painter.drawPixmap(x, y-10, m_counter_off.pixmap(32));
            }
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
            m_painter.drawPixmap(x, y, (i >> 6) & 1 ? m_counter_on.pixmap(32) : m_counter_off.pixmap(32));
            x += 32;
            if (x == 192) {
                x = 0;
                y += 32;
            }
        }
    }
    m_painter.end();
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
    int offset = ((int)mouseEvent->scenePos().y() / 32) * 6 + ((int)mouseEvent->scenePos().x() / 32) % 6;
    if (m_editMode == Chipset::EDIT_MODE_TERRAIN) {
        if (m_data->terrain_data.size() > offset && m_data->terrain_data[offset] != m_terrain && mouseEvent->buttons() == Qt::LeftButton) {
            m_data->terrain_data[offset] = m_terrain;
            drawTerrainPiece((int)mouseEvent->scenePos().x(), (int)mouseEvent->scenePos().y(), m_terrain, false);
        }
    } else if (m_editMode == Chipset::EDIT_MODE_PASSABILITY) {
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
        uint8_t &i = m_layer == Core::LOWER ? m_data->passable_data_lower[offset] : m_data->passable_data_upper[offset];
        int x = (int)mouseEvent->scenePos().x() % 32;
        int y = (int)mouseEvent->scenePos().y() % 32;
        // down
        if (( 8 <= x && x <= 23 && 24 <= y && y <= 31 )
            || ( 9 <= x && x <= 22 && y == 23 )
            || ( 10 <= x && x <= 21 && y == 22 )
            || ( 11 <= x && x <= 20 && y == 21 )
            || ( 12 <= x && x <= 19 && y == 20 )) {
            i ^= 1;
        }
        // left
        if (( 0 <= x && x <= 7 && 8 <= y && y <= 23 )
            || ( x == 8 && 9 <= y && y <= 22 )
            || ( x == 9 && 10 <= y && y <= 21 )
            || ( x == 9 && 11 <= y && y <= 20 )
            || ( x == 9 && 12 <= y && y <= 19 )) {
            i ^= (1 << 1);
        }
        // right
        if (( 24 <= x && x <= 32 && 8 <= y && y <= 23 )
            || ( x == 23 && 9 <= y && y <= 22 )
            || ( x == 22 && 10 <= y && y <= 21 )
            || ( x == 21 && 11 <= y && y <= 20 )
            || ( x == 20 && 12 <= y && y <= 19 )) {
            i ^= (1 << 2);
        }
        // up
        if (( 8 <= x && x <= 23 && 0 <= y && y <= 7 )
            || ( 9 <= x && x <= 22 && y == 8 )
            || ( 10 <= x && x <= 21 && y == 9 )
            || ( 11 <= x && x <= 20 && y == 10 )
            || ( 12 <= x && x <= 19 && y == 11 )) {
            i ^= (1 << 3);
        }
    } else if (m_editMode == Chipset::EDIT_MODE_COUNTER) {
        if (m_data->passable_data_upper.size() > offset && mouseEvent->buttons() == Qt::LeftButton) {
            m_data->passable_data_upper[offset] ^= Chipset::PassableFlag::Counter;
        }
    }
    setEditMode(m_editMode);
}

void ChipsetEditScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    ChipsetEditScene::mousePressEvent(mouseEvent);
}
