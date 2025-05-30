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

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <ui/rpg_painter.h>
#include "core.h"

class PaletteScene : public QGraphicsScene
{
	Q_OBJECT
public:
	explicit PaletteScene(QObject *parent = nullptr);

    short selection(int off_x, int off_y);
    int selWidth();
    int selHeight();
    void setSelection(std::vector<short> n_sel, int n_w, int n_h);
signals:

public slots:
	void onLayerChange();
    void onChipsetChange(std::shared_ptr<emhash8::HashMap<short, QPixmap> >);
protected:
	void updateSelectionRect();
    void recalculateSelectionRect();

	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	void cancelSelection();

	bool m_cancel;
	bool m_pressed;
	QGraphicsPixmapItem* m_tiles;
	QGraphicsRectItem* m_selectionItem;
	QPixmap m_lowerTiles;
	QPixmap m_upperTiles;
    std::vector<short> m_lowerSel;
    std::vector<short> m_upperSel;
    short m_eventSel;
    int m_lowerSelW;
    int m_lowerSelH;
    int m_upperSelW;
    int m_upperSelH;
	QRectF last_selection;
	QPointF m_initial;
	QPointF m_current;
    RpgPainter m_painter;

    QRectF m_lowerRect;
    QRectF m_upperRect;
};

