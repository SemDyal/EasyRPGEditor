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

#include "palette_scene.h"
#include "common/tileops.h"
#include <QGraphicsDropShadowEffect>
#include <QPainter>

PaletteScene::PaletteScene(QObject *parent) :
	QGraphicsScene(parent),
	m_cancel(false),
	m_pressed(false)
{
    this->setSceneRect(QRect(0,0,96,448));
    m_selectionItem = new QGraphicsRectItem(QRectF(QRect(0,0,16,16)));
    last_selection = QRectF(QRect(0,0,16,16));
	this->addItem(m_selectionItem);
	m_tiles = new QGraphicsPixmapItem();
	this->addItem(m_tiles);
	m_tiles->setVisible(false);
	QPen selPen(Qt::yellow);
    selPen.setWidth(2);
	m_selectionItem->setPen(selPen);
	m_selectionItem->setVisible(false);
	QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    effect->setOffset(2);
	m_tiles->setGraphicsEffect(effect);
	m_tiles->stackBefore(m_selectionItem);
	setBackgroundBrush(QBrush(QPixmap(":/embedded/share/old_grid.png")));
    m_lowerSel.push_back(0);
    m_upperSel.push_back(10000);
    m_lowerSelW = 1;
    m_upperSelW = 1;
    m_lowerSelH = 1;
    m_upperSelH = 1;
    m_eventSel = 10000;
    m_lowerRect = QRectF(QRect(0, 0, 16, 16));
    m_upperRect = QRectF(QRect(0, 0, 16, 16));
}

short PaletteScene::selection(int off_x, int off_y)
{
    short result = 0;
    switch(core().layer())
    {
    case Core::LOWER:
        off_x %= m_lowerSelW;
        off_y %= m_lowerSelH;
        if (off_x < 0)
            off_x += m_lowerSelW;
        if (off_y < 0)
            off_y += m_lowerSelH;
        result = m_lowerSel[static_cast<size_t>(off_x+off_y*m_lowerSelW)];
        break;
    case Core::UPPER:
        off_x %= m_upperSelW;
        off_y %= m_upperSelH;
        if (off_x < 0)
            off_x += m_upperSelW;
        if (off_y < 0)
            off_y += m_upperSelH;
        result = m_upperSel[static_cast<size_t>(off_x+off_y*m_upperSelW)];
        break;
    case Core::EVENT:
        result = m_eventSel;
        break;
    }
    return result;
}

int PaletteScene::selWidth()
{
    switch(core().layer()) {
    case (Core::LOWER):
        return m_lowerSelW;
    case (Core::UPPER):
        return m_upperSelW;
    default:
        return 1;
    }
}

int PaletteScene::selHeight()
{
    switch(core().layer()) {
    case (Core::LOWER):
        return m_lowerSelH;
    case (Core::UPPER):
        return m_upperSelH;
    default:
        return 1;
    }
}

void PaletteScene::setSelection(std::vector<short> n_sel, int n_w, int n_h)
{
    if (static_cast<int>(n_sel.size()) != n_w * n_h)
        return;
    switch(core().layer())
    {
    case Core::LOWER:
        m_lowerSel = n_sel;
        m_lowerSelW = n_w;
        m_lowerSelH = n_h;
        break;
    case Core::UPPER:
        m_upperSel = n_sel;
        m_upperSelW = n_w;
        m_upperSelH = n_h;
        break;
    case Core::EVENT:
        m_eventSel = n_sel[0];
        break;
    }
}

void PaletteScene::onLayerChange()
{
	if (core().layer() == Core::LOWER)
	{
		m_tiles->setPixmap(m_lowerTiles);
        this->setSceneRect(QRect(0,0,96,448));
        //m_selectionItem->setRect();
	}
	else
	{
		m_tiles->setPixmap(m_upperTiles);
        this->setSceneRect(QRect(0,0,96,400));
        if (core().layer() == Core::UPPER) {
            //m_selectionItem->setRect();
        } else {
            //m_selectionItem->setRect();
        }
	}
	m_tiles->graphicsEffect()->setEnabled(core().layer() != Core::LOWER);
    recalculateSelectionRect();
}

void PaletteScene::onChipsetChange(std::shared_ptr<emhash8::HashMap<short, QPixmap>> chipset)
{
	m_tiles->setVisible(true);
	m_selectionItem->setVisible(true);
    m_lowerTiles = QPixmap(96, 448);
    m_upperTiles = QPixmap(96, 400);
    m_lowerTiles.fill(Qt::transparent);
    m_upperTiles.fill(Qt::transparent);
    m_painter.forceChipset(chipset);
    m_painter.beginPainting(m_lowerTiles);
    m_painter.renderTileOverview(RpgPainter::ALL_LOWER);
    m_painter.endPainting();
    m_painter.beginPainting(m_upperTiles);
    m_painter.renderTileOverview(RpgPainter::ALL_UPPER);
    m_painter.endPainting();
	onLayerChange();
}

void PaletteScene::updateSelectionRect()
{
	QRectF selRect;

    int small_x = (m_initial.x() <= m_current.x()) ? static_cast<int>(m_initial.x())/16 : static_cast<int>(m_current.x())/16;
    int big_x = (m_initial.x() >= m_current.x()) ? static_cast<int>(m_initial.x())/16 : static_cast<int>(m_current.x())/16;
    int small_y = (m_initial.y() <= m_current.y()) ? static_cast<int>(m_initial.y())/16 : static_cast<int>(m_current.y())/16;
    int big_y = (m_initial.y() >= m_current.y()) ? static_cast<int>(m_initial.y())/16 : static_cast<int>(m_current.y())/16;
    //keep inside the scene
    if (small_x < 0)
        small_x = 0;
    if (small_x > 5)
        small_x = 5;
    if (big_x > 5)
        big_x = 5;
    if (small_y < 0)
        small_y = 0;
    if (big_y > 27)
        big_y = 27;
    if (big_y - small_y > 5)
    {
        if (static_cast<int>(m_initial.y())/16 == small_y)
            big_y = small_y + 5;
        else
            small_y = big_y - 5;
    }
    if (core().layer() != Core::EVENT) {
        selRect = QRectF(QRect(small_x*16,small_y*16,(big_x-small_x+1)*16,(big_y-small_y+1)*16));
    } else {
        selRect = QRectF(QRect(small_x*16,small_y*16,16, 16));
    }

    if (core().layer() == Core::LOWER) {
        m_lowerRect = selRect;
    } else if (core().layer() == Core::UPPER) {
        m_upperRect = selRect;
    }

	m_selectionItem->setRect(selRect);
}

void PaletteScene::recalculateSelectionRect() {
    QRect bogus;
    switch (core().layer()) {
    case (Core::LOWER):
        m_selectionItem->setRect(m_lowerRect);
        break;
    case (Core::UPPER):
        m_selectionItem->setRect(m_upperRect);
        break;
    default:
        m_selectionItem->setRect(QRect(((m_eventSel - 10000) % 6) * 16, ((m_eventSel - 10000) / 6) * 16, 16, 16));
        break;
    }
}

void PaletteScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		cancelSelection();
		return;
	}
	else if (event->button() != Qt::LeftButton)
		return;
	m_initial = event->scenePos();
	m_current = event->scenePos();
	updateSelectionRect();
	QGraphicsScene::mousePressEvent(event);
}

void PaletteScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_cancel || core().layer() == Core::EVENT)
        return;
	else
		m_current = event->scenePos();

	updateSelectionRect();
	QGraphicsScene::mouseMoveEvent(event);
}

void PaletteScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && m_cancel)
	{
		m_cancel = false;
		return;
	}
	if (event->button() != Qt::LeftButton)
		return;

	std::vector<short> sel;
    int x = static_cast<int>(m_selectionItem->rect().left())/16;
    int y = static_cast<int>(m_selectionItem->rect().top())/16;
    int w = static_cast<int>(m_selectionItem->rect().width())/16;
    int h = static_cast<int>(m_selectionItem->rect().height())/16;
    for (int _y = y; _y < y+h; _y++)
        for (int _x = x; _x < x+w; _x++)
            if (core().layer() == Core::LOWER)
                sel.push_back(TileOps::translate(_x+_y*6, SAMPLE));
            else
                sel.push_back(TileOps::translate(_x+_y*6+162, SAMPLE));
    setSelection(sel, w, h);
    last_selection = m_selectionItem->boundingRect();
	QGraphicsScene::mouseReleaseEvent(event);
}

void PaletteScene::cancelSelection()
{
	m_cancel = true;
	m_selectionItem->setRect(last_selection);
}


