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

#include "map_scene.h"
#include <QGuiApplication>
#include <QAction>
#include <QDialogButtonBox>
#include <QGraphicsBlurEffect>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QScrollBar>
#include <QStatusBar>
#include <qmainwindow.h>
#include <lcf/rpg/event.h>
#include "common/image_loader.h"
#include "common/tileops.h"
#include "core.h"
#include "common/dbstring.h"
#include "ui/event/event_dialog.h"
#include "ui/other/run_game_dialog.h"
#include "ui/maptree/map_properties_dialog.h"
#include "undo_draw.h"
#include "undo_event.h"
#include <lcf/lmu/reader.h>
#include <lcf/lmt/reader.h>
#include <ui/common/palette_scene.h>

MapScene::MapScene(ProjectData& project, int id, QGraphicsView *view, PaletteScene *palette, QObject *parent) :
	QGraphicsScene(parent), m_project(project)
{
	m_init = false;
	m_view = view;
    m_palette = palette;
    m_view->setMouseTracking(true);
    m_undoStack = new QUndoStack(this);
	m_selectionTile = new QGraphicsRectItem(QRectF(QRect(0,32,32,32)));
	m_selecting = false;
	const auto& treeMap = project.treeMap();
	for (unsigned int i = 1; i < treeMap.maps.size(); i++)
		if (treeMap.maps[i].ID == id)
		{
			n_mapInfo = treeMap.maps[i];
			break;
		}
    // TODO: figure out why these shortcuts don't work
    new QShortcut(QKeySequence::Cut, this, SLOT(on_actionCut()));
    new QShortcut(QKeySequence::Copy, this, SLOT(on_actionCopy()));
    new QShortcut(QKeySequence::Paste, this, SLOT(on_actionPaste()));
	m_eventMenu = new QMenu(m_view);
	QList<QAction*> actions;
	actions << new QAction(QIcon(":/icons/share/old_playtest.png"),
						   tr("Start Game Here"),
						   this);
	actions << new QAction(QIcon(":/icons/share/old_edit.png"),
						   tr("Set Start Position"),
						   this);
	actions << new QAction(QIcon(":/icons/share/old_event_layer.png"),
						   tr("New Event"),
						   this);
	actions << new QAction(QIcon(":/icons/share/old_event_layer.png"),
						   tr("Edit Event"),
						   this);
	actions << new QAction(QIcon(":/icons/share/old_event_layer.png"),
						   tr("Copy Event"),
						   this);
	actions << new QAction(QIcon(":/icons/share/old_event_layer.png"),
						   tr("Cut Event"),
						   this);
	actions << new QAction(QIcon(":/icons/share/old_event_layer.png"),
						   tr("Paste Event"),
						   this);
	actions << new QAction(QIcon(":/icons/share/old_event_layer.png"),
						   tr("Delete Event"),
						   this);
	connect(actions[0],SIGNAL(triggered()),this,SLOT(on_actionRunHere()));
	connect(actions[1],SIGNAL(triggered()),this,SLOT(on_actionSetStartPosition()));
	connect(actions[2],SIGNAL(triggered()),this,SLOT(on_actionNewEvent()));
	connect(actions[3],SIGNAL(triggered()),this,SLOT(on_actionEditEvent()));
	connect(actions[4],SIGNAL(triggered()),this,SLOT(on_actionCopyEvent()));
	connect(actions[5],SIGNAL(triggered()),this,SLOT(on_actionCutEvent()));
	connect(actions[6],SIGNAL(triggered()),this,SLOT(on_actionPasteEvent()));
	connect(actions[7],SIGNAL(triggered()),this,SLOT(on_actionDeleteEvent()));

	m_eventMenu->addActions(actions);
    m_panorama = new QGraphicsPixmapItem();
	m_lowerpix = new QGraphicsPixmapItem();
    m_upperpix = new QGraphicsPixmapItem();
    addItem(m_panorama);
	addItem(m_lowerpix);
    addItem(m_upperpix);
    Load();
	QPen selPen(Qt::yellow);
	selPen.setWidth(2);
	m_selectionTile->setPen(selPen);
	m_selectionTile->setVisible(false);
	this->addItem(m_selectionTile);
	m_drawing = false;
	m_cancelled = false;
	m_selecting = false;
    QGraphicsOpacityEffect * effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(0.7);
    m_panorama->setGraphicsEffect(effect);
	m_lowerpix->setGraphicsEffect(effect);
	m_upperpix->setGraphicsEffect(new QGraphicsOpacityEffect(this));
	onLayerChanged();
    onToolChanged();
}

MapScene::~MapScene()
{
	delete m_lowerpix;
	delete m_upperpix;
	delete m_lines;
	delete m_selectionTile;
	delete m_undoStack;
    delete m_currentMapEvents;
}

void MapScene::Init()
{
	connect(m_view->verticalScrollBar(),
			SIGNAL(actionTriggered(int)),
			this,
			SLOT(on_user_interaction()));
	connect(m_view->horizontalScrollBar(),
			SIGNAL(actionTriggered(int)),
			this,
            SLOT(on_user_interaction()));
    connect(m_view->verticalScrollBar(),
            SIGNAL(valueChanged(int)),
            this,
            SLOT(redrawMap()));
    connect(m_view->horizontalScrollBar(),
            SIGNAL(valueChanged(int)),
            this,
            SLOT(redrawMap()));
	connect(m_view->verticalScrollBar(),
			SIGNAL(rangeChanged(int,int)),
			this,
			SLOT(redrawMap()));
	connect(m_view->horizontalScrollBar(),
			SIGNAL(rangeChanged(int,int)),
			this,
			SLOT(redrawMap()));
	connect(m_view->verticalScrollBar(),
			SIGNAL(valueChanged(int)),
			this,
			SLOT(on_view_V_Scroll()));
	connect(m_view->horizontalScrollBar(),
			SIGNAL(valueChanged(int)),
			this,
			SLOT(on_view_H_Scroll()));
	connect(&core(),
			SIGNAL(toolChanged()),
			this,
			SLOT(onToolChanged()));
	connect(&core(),
			SIGNAL(layerChanged()),
			this,
			SLOT(onLayerChanged()));
    connect(this->m_undoStack,
            SIGNAL(cleanChanged(bool)),
            this,
            SLOT(onCleanChanged(bool)));
	m_view->verticalScrollBar()->setValue(n_mapInfo.scrollbar_y *static_cast<int>(m_scale));
	m_view->horizontalScrollBar()->setValue(n_mapInfo.scrollbar_x * static_cast<int>(m_scale));
    m_init = true;
}

float MapScene::scale() const
{
	return m_scale;
}

QString MapScene::mapName() const
{
	return ToQString(n_mapInfo.name);
}

bool MapScene::isModified() const
{
    return (!m_undoStack->isClean());
}

int MapScene::id() const
{
	return n_mapInfo.ID;
}

int MapScene::chipsetId() const
{
	return m_map->chipset_id;
}

lcf::rpg::Map*MapScene::map() const
{
    return m_map.get();
}

void MapScene::setLayerData(Core::Layer layer, std::vector<short> data)
{
	if (layer == Core::LOWER)
	{
		m_lower = data;
		m_map->lower_layer = data;
	}
	else
	{
		m_upper = data;
		m_map->upper_layer = data;
	}
	redrawLayer(layer);
}

void MapScene::setEvent(int id, const lcf::rpg::Event &data) {
	for (unsigned int i = 0; i < m_map->events.size(); i++) {
		if (m_map->events[i].ID == id) {
            m_map->events[i] = data;
            return;
		}
	}

	m_map->events.push_back(data);
    redrawLayer(Core::UPPER);
}

void MapScene::deleteEvent(int id) {
    for (unsigned int i = 0; i < m_map->events.size(); i++) {
        if (m_map->events[i].ID == id) {
            m_map->events.erase(m_map->events.begin() + i);
            return;
        }
    }

    redrawLayer(Core::UPPER);
}

emhash8::HashMap<int, lcf::rpg::Event*> *MapScene::mapEvents()
{
    emhash8::HashMap<int, lcf::rpg::Event*> *events = new emhash8::HashMap<int, lcf::rpg::Event*>();
	for (unsigned int i = 0; i < m_map->events.size(); i++)
        events->try_set(m_map->events[i].ID, &m_map->events[i]);
	return events;
}

void MapScene::editMapProperties(QTreeWidgetItem *item)
{
	int old_width = m_map->width;
	int old_height = m_map->height;
	lcf::DBString old_name = n_mapInfo.name;

	MapPropertiesDialog dlg(m_project, n_mapInfo, *m_map, m_view);
	if (dlg.exec() == QDialog::Accepted) {
		if (m_map->width != old_width || m_map->height != old_height) {
			setLayerData(Core::LOWER, m_map->lower_layer);
			setLayerData(Core::UPPER, m_map->upper_layer);
			redrawGrid();
		}

		Save(true);
		redrawMap();
		setScale(m_scale);

		if (n_mapInfo.name != old_name) {
			item->setData(0, Qt::DisplayRole, ToQString(n_mapInfo.name));
		}
	}
}

void MapScene::redrawMap()
{
	if (!m_init)
		return;
	s_tileSize = core().tileSize() * static_cast<double>(m_scale);
    redrawPanorama();
	redrawLayer(Core::LOWER);
	redrawLayer(Core::UPPER);
}

void MapScene::setScale(float scale)
{
	float old_scale = m_scale;
	int map_x = m_view->horizontalScrollBar()->value();
	int map_y = m_view->verticalScrollBar()->value();

	m_scale = scale;
	m_lines->setScale(static_cast<double>(m_scale));
	m_selectionTile->setScale(static_cast<double>(m_scale));
	this->setSceneRect(0,
		0,
		m_map->width * core().tileSize() * static_cast<double>(m_scale),
		m_map->height * core().tileSize() * static_cast<double>(m_scale));

	if (m_view->horizontalScrollBar()->isVisible()) {
		m_view->horizontalScrollBar()->setValue((map_x + m_view->horizontalScrollBar()->pageStep() / 2.0) * m_scale / old_scale - m_view->horizontalScrollBar()->pageStep() / 2.0);
	} else {
		m_view->horizontalScrollBar()->setValue(m_view->horizontalScrollBar()->maximum() / 2.0);
	}
	if (m_view->verticalScrollBar()->isVisible()) {
		m_view->verticalScrollBar()->setValue((map_y + m_view->verticalScrollBar()->pageStep() / 2.0) * m_scale / old_scale - m_view->verticalScrollBar()->pageStep() / 2.0);
	} else {
		m_view->verticalScrollBar()->setValue(m_view->verticalScrollBar()->maximum() / 2.0);
	}
	redrawMap();
}

void MapScene::onLayerChanged()
{
	if (m_drawing)
		stopDrawing();
	if (m_selecting)
		stopSelecting();
    m_cancelled = false;
	switch (core().layer())
	{
	case Core::LOWER:
		m_lowerpix->graphicsEffect()->setEnabled(false);
		m_upperpix->graphicsEffect()->setEnabled(true);
		m_lines->setVisible(false);
		break;
	case Core::UPPER:
		m_lowerpix->graphicsEffect()->setEnabled(true);
		m_upperpix->graphicsEffect()->setEnabled(false);
		m_lines->setVisible(false);
		break;
	case Core::EVENT:
		m_lowerpix->graphicsEffect()->setEnabled(false);
		m_upperpix->graphicsEffect()->setEnabled(false);
		m_lines->setVisible(true);
		break;
//	  default:
//		  Q_ASSERT(false);
	}
}

void MapScene::onToolChanged()
{
	if (m_drawing)
		stopDrawing();
	if (m_selecting)
		stopSelecting();
    m_cancelled = false;
	switch (core().tool())
	{
	case (Core::ZOOM):
		m_lowerpix->setCursor(QCursor(QPixmap(":/icons/share/cur_zoom.png"),1,1));
		break;
	case (Core::PENCIL):
		m_lowerpix->setCursor(QCursor(QPixmap(":/icons/share/cur_pen.png"),1,1));
		break;
	case (Core::RECTANGLE):
		m_lowerpix->setCursor(QCursor(QPixmap(":/icons/share/cur_rectangle.png"),1,1));
		break;
	case (Core::CIRCLE):
		m_lowerpix->setCursor(QCursor(QPixmap(":/icons/share/cur_circle.png"),1,1));
		break;
	case (Core::FILL):
		m_lowerpix->setCursor(QCursor(QPixmap(":/icons/share/cur_fill.png"),1,1));
		break;
	}
}

void MapScene::onCleanChanged(bool clean) {
    emit mapCleanChanged(clean);
}

void MapScene::Save(bool properties_changed)
{
	if (!isModified() && !properties_changed)
		return;

	auto& treeMap = m_project.treeMap();
	for (unsigned int i = 1; i < treeMap.maps.size(); i++)
		if (treeMap.maps[i].ID == n_mapInfo.ID)
		{
			treeMap.maps[i] = n_mapInfo; //Apply info changes
			break;
		}
	// Remember last active map
	treeMap.active_node = n_mapInfo.ID;
	// FIXME: ProjectData.Project is Const
	core().project()->saveTreeMap();
    QString file = QString("Map%1.emu").arg(QString::number(n_mapInfo.ID), 4, u'0');
    lcf::LMU_Reader::PrepareSave(*m_map);
    // FIXME: ProjectData.Project is Const
	core().project()->saveMap(*m_map, n_mapInfo.ID);
    m_undoStack->setClean();
	emit mapSaved();
}
void MapScene::Load(bool revert)
{
	// FIXME: Many calls to core()
	const auto& treeMap = m_project.treeMap();
	for (unsigned int i = 1; i < treeMap.maps.size(); i++)
		if (treeMap.maps[i].ID == n_mapInfo.ID)
		{
			n_mapInfo = treeMap.maps[i]; //Revert info changes
			break;
		}

    std::unique_ptr<lcf::rpg::Map> map = m_project.project().loadMap(n_mapInfo.ID);
    if (map == nullptr) {
        qWarning()<<"Map loading failed!";
    }
    m_map = m_project.project().loadMap(n_mapInfo.ID);
    m_lower = m_map->lower_layer;
    m_upper = m_map->upper_layer;

	if (!revert) {
        redrawGrid();
    } else {
        redrawMap();
    }
    m_undoStack->clear();
}

void MapScene::undo()
{
	m_undoStack->undo();
    emit mapChanged();
}

void MapScene::redo()
{
    m_undoStack->redo();
    emit mapChanged();
}

void MapScene::on_actionCopy() {
    if (core().layer() == Core::EVENT) {
        on_actionCopyEvent();
    }
}

void MapScene::on_actionCut() {
    if (core().layer() == Core::EVENT) {
        on_actionCutEvent();
    }
}

void MapScene::on_actionPaste() {
    if (core().layer() == Core::EVENT) {
        on_actionPasteEvent();
    }
}

void MapScene::on_actionNewEvent()
{
	// Find first free id
	int id = getFirstFreeId();

	lcf::rpg::Event event;
	event.ID = id;
    event.name = ToDBString(QString("EV%1").arg(QString::number(id), 4, u'0'));
    event.x = cur_x;
    event.y = cur_y;
    lcf::rpg::EventPage page;
    page.ID = 1;
    page.character_index = m_palette->selection(0, 0) - 10000;
    event.pages.push_back(page);

    int result = EventDialog::edit(m_view, event, m_project, this);
	if (result != QDialogButtonBox::Cancel)
	{
		m_map->events.push_back(event);
        lcf::rpg::Event backup = event;
        m_undoStack->push(new UndoEvent(std::nullopt, event, this));
        redrawArea(Core::UPPER, event.x, event.y, event.x, event.y);
		emit mapChanged();
	}
}

void MapScene::on_actionEditEvent() {
	std::vector<lcf::rpg::Event>::iterator ev;
	for (ev = m_map->events.begin(); ev != m_map->events.end(); ++ev) {
		if (_index(cur_x, cur_y) == _index(ev->x, ev->y)) {
            lcf::rpg::Event backup = *ev;
            int result = EventDialog::edit(m_view, *ev, m_project, this);
			if (result != QDialogButtonBox::Cancel) {
                m_undoStack->push(new UndoEvent(backup, *ev, this));
				emit mapChanged();
			}
            redrawArea(Core::UPPER, ev->x, ev->y, ev->x, ev->y);
			return;
		}
	}
}

void MapScene::on_actionCopyEvent() {
	event_clipboard = *getEventAt(cur_x, cur_y);
	event_clipboard_set = true;
}

void MapScene::on_actionCutEvent() {
	on_actionCopyEvent();
	on_actionDeleteEvent();
}

void MapScene::on_actionPasteEvent() {
	int id = getFirstFreeId();

    lcf::rpg::Event backup = event_clipboard;
	lcf::rpg::Event event = event_clipboard;
	event.ID = id;
    event.name = ToDBString(QString("EV%1").arg(QString::number(id), 4, u'0'));
    event.x = cur_x;
    event.y = cur_y;

	m_map->events.push_back(event);
    m_undoStack->push(new UndoEvent(std::nullopt, event, this));
    redrawArea(Core::UPPER, event.x, event.y, event.x, event.y);
	emit mapChanged();
}

void MapScene::on_actionDeleteEvent()
{
	std::vector<lcf::rpg::Event>::iterator ev;
	for (ev = m_map->events.begin(); ev != m_map->events.end(); ++ev)
		if (_index(cur_x,cur_y) == _index(ev->x,ev->y))
			break;

	if (ev != m_map->events.end())
	{
		lcf::rpg::Event backup = *ev;
        m_undoStack->push(new UndoEvent(backup, std::nullopt, this));

		m_map->events.erase(ev);
        redrawArea(Core::UPPER, ev->x, ev->y, ev->x, ev->y);
		emit mapChanged();
	}
}

void MapScene::on_actionRunHere()
{
	core().runGameHere(id(), lst_x, lst_y);
}

void MapScene::on_actionSetStartPosition()
{
	auto& treeMap = m_project.treeMap();
	treeMap.start.party_map_id = this->id();
	treeMap.start.party_x = lst_x;
	treeMap.start.party_y = lst_y;
}

void MapScene::on_user_interaction()
{
	m_userInteraction = true;
}

void MapScene::on_view_V_Scroll()
{
	if (!m_userInteraction || !m_init)
		return;
	if (m_view->verticalScrollBar()->isVisible())
    {
		n_mapInfo.scrollbar_y = m_view->verticalScrollBar()->value() / static_cast<double>(m_scale);
    }
	m_userInteraction = false;
}

void MapScene::on_view_H_Scroll()
{
	if (!m_userInteraction || !m_init)
		return;
	if (m_view->horizontalScrollBar()->isVisible())
    {
		n_mapInfo.scrollbar_x = m_view->horizontalScrollBar()->value() / static_cast<double>(m_scale);
	}
	m_userInteraction = false;
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!in_bounds) {
        return;
    }
	if (event->button() == Qt::RightButton)
	{
		if (m_drawing)
		{
			stopDrawing();
			return;
		}
        if (core().tool() == Core::ZOOM && static_cast<double>(m_scale) > 0.125)
            setScale(m_scale/2);
        else if (sceneRect().contains(event->scenePos()) && core().layer() == Core::EVENT)
        {
            m_selecting = true;
            m_selectionTile->setVisible(true);
            m_selectionTile->setRect(QRectF(QRect(cur_x*core().tileSize(),cur_y*core().tileSize(),
                                                  core().tileSize(),core().tileSize())));
            lcf::rpg::Event *selection = getEventAt(cur_x, cur_y);
            m_eventMenu->actions()[2]->setEnabled(!selection);
            m_eventMenu->actions()[3]->setEnabled(selection);
            m_eventMenu->actions()[4]->setEnabled(selection);
            m_eventMenu->actions()[5]->setEnabled(selection);
            m_eventMenu->actions()[6]->setEnabled(!selection && event_clipboard_set);
            m_eventMenu->actions()[7]->setEnabled(selection);

			lst_x = cur_x;
			lst_y = cur_y;
			m_eventMenu->popup(event->screenPos());
		}

	}
	if (event->button() == Qt::LeftButton)
	{
		if (core().tool() == Core::ZOOM && static_cast<double>(m_scale) < 8.0) // Zoom
			setScale(m_scale*2);
		else if (core().layer() == Core::EVENT) // Select tile
		{
			m_selecting = true;
			m_selectionTile->setVisible(true);
			m_selectionTile->setRect(QRectF(QRect(cur_x*core().tileSize(),cur_y*core().tileSize(),
												  core().tileSize(),core().tileSize())));
		}
		else // Start drawing
		{

			fst_x = cur_x;
			fst_y = cur_y;
			switch(core().tool())
			{
			case Core::PENCIL:
				m_drawing = true;
				drawPen();
				break;
			case Core::RECTANGLE:
				m_drawing = true;
				drawRect();
				break;
			case Core::FILL:
				m_drawing = true;
				if (core().layer() == Core::LOWER)
                    drawFill(TileOps::translate(m_lower[static_cast<size_t>(_index(fst_x,fst_y))]),fst_x,fst_y);
				else if (core().layer() == Core::UPPER)
                    drawFill(TileOps::translate(m_upper[static_cast<size_t>(_index(fst_x,fst_y))]),fst_x,fst_y);
				updateArea(0, 0, m_map->width-1 ,m_map->height-1);
				break;
			default:
				break;
			}
		}
	}
}

void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (cur_x == static_cast<int>(event->scenePos().x() / s_tileSize) && cur_y == static_cast<int>(event->scenePos().y() / s_tileSize))
        return;
    int next_x = std::max(0, std::min(m_map->width - 1, static_cast<int>(event->scenePos().x() / s_tileSize)));
    int next_y = std::max(0, std::min(m_map->height - 1, static_cast<int>(event->scenePos().y() / s_tileSize)));
    processTools(next_x, next_y);
    cur_x = next_x;
    cur_y = next_y;
    in_bounds = sceneRect().contains(event->scenePos());

	// Update status bar
	QMainWindow* mw = qobject_cast<QMainWindow*>(parent());

	// Show coordinates of current tile
	QString status_msg = QString("(%0, %1)").arg(cur_x).arg(cur_y);

	if (core().layer() == Core::EVENT)
	{
		// Show events on current tile
		for (const lcf::rpg::Event& evt : m_map->events)
		{
			if (_index(cur_x,cur_y) == _index(evt.x,evt.y))
			{
				status_msg.append(QString(" - " + tr("Event") + " %0: %1").arg(evt.ID).arg(evt.name.c_str()));
			}
		}
	}

	mw->statusBar()->showMessage(status_msg);

}

void MapScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_cancelled && !event->buttons())
	{
		m_cancelled = false;
        m_drawing = false;
		return;
	}
	if (m_drawing && !m_cancelled)
	{
		m_drawing = false;
		if (core().layer() == Core::LOWER)
		{
			m_undoStack->push(new UndoDraw(Core::LOWER,
											m_map->lower_layer,
                                            m_lower,
											this));
			m_map->lower_layer = m_lower;
		}
		else
		{
			m_undoStack->push(new UndoDraw(Core::UPPER,
											m_map->upper_layer,
                                            m_upper,
											this));
			m_map->upper_layer = m_upper;
		}
		emit mapChanged();
	}
}

void MapScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (core().layer() != Core::EVENT || core().tool() == Core::ZOOM || event->button() == Qt::RightButton)
		return;

	// Do not allow putting events on invalid coordinates
	if (cur_x == -1 || cur_y == -1) {
		return;
	}

	if (getEventAt(cur_x, cur_y)) {
		on_actionEditEvent();
	} else {
		on_actionNewEvent();
	}
}

void MapScene::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) {
        if (core().layer() == Core::EVENT && getEventAt(cur_x, cur_y)) {
            on_actionDeleteEvent();
        }
    }
    processTools(cur_x, cur_y);
}

void MapScene::keyReleaseEvent(QKeyEvent *event) {
    processTools(cur_x, cur_y);
}

void MapScene::processTools(int next_x, int next_y) {
    if (m_drawing)
    {
        switch (core().tool())
        {
        case (Core::PENCIL):
            if (in_bounds){
                drawPen(next_x, next_y);
            }
            break;
        case (Core::RECTANGLE):
            drawRect(next_x, next_y);
            break;
        default:
            break;
        }
    }
}

int MapScene::_x(int index)
{
	return (index%m_map->width);
}

int MapScene::_y(int index)
{
	return (index/m_map->width);
}

int MapScene::_index(int x, int y)
{
	return (m_map->width*y+x);
}

void MapScene::redrawTile(const Core::Layer &layer,
								   const int &x,
								   const int &y,
								   const QRect &dest_rec)
{
	switch (layer)
	{
	case (Core::LOWER):
        m_painter.renderTile(m_lower[static_cast<size_t>(_index(x,y))],dest_rec);
		break;
	case (Core::UPPER):
        m_painter.renderTile(m_upper[static_cast<size_t>(_index(x,y))],dest_rec);
		break;
	default:
		break;
	}
}

void MapScene::stopDrawing()
{
	m_cancelled = true;
	m_drawing = false;
	m_lowerpix->setCursor(QCursor(QPixmap(":/icons/share/cur_cancel.png"),1,1));
	m_lower = m_map->lower_layer;
	m_upper = m_map->upper_layer;
	redrawLayer(Core::LOWER);
	redrawLayer(Core::UPPER);
}

void MapScene::stopSelecting()
{
	m_cancelled = true;
	m_selecting = false;
	m_lowerpix->setCursor(QCursor(QPixmap(":/icons/share/cur_cancel.png"),1,1));
	m_selectionTile->setVisible(false);
	//cancel selection...
}

void MapScene::updateArea(int x1, int y1, int x2, int y2, bool redraw)
{
    //Normalize
	if (x1 < 0)
		x1 = 0;
	if (y1 < 0)
		y1 = 0;
    if (x2 > m_map->width)
        x2 = m_map->width;
    if (y2 > m_map->height)
        y2 = m_map->height;

    // TODO: shift mode breaks on autotiles
    if (core().layer() == Core::LOWER && !QGuiApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier)) {
        for (int x = x1; x <= x2; x++) {
            for (int y = y1; y <= y2; y++) {
                if (!TileOps::isEblock(TileOps::translate(m_lower[static_cast<size_t>(_index(x, y))])))
                    m_lower[static_cast<size_t>(_index(x,y))] = bind(x, y);
            }

        }
    }
    if (redraw) {
        redrawArea(core().layer(), x1, y1, x2, y2);
    }
}

void MapScene::redrawArea(Core::Layer layer, int x1, int y1, int x2, int y2)
{
    QPixmap pix = layer == Core::LOWER ? m_lowerpix->pixmap() : m_upperpix->pixmap();
    int offset_x = m_view->horizontalScrollBar()->value() / s_tileSize;
    int offset_y = m_view->verticalScrollBar()->value() / s_tileSize;
    m_painter.beginPainting(pix);
    m_painter.setCompositionMode(QPainter::CompositionMode_Source);
    for (int x = x1; x <= x2; x++)
        for (int y = y1; y <= y2; y++)
        {
            if (x >= m_map->width || y >= m_map->height)
                continue;
            QRect dest_rect((x-offset_x)* s_tileSize,
                            (y-offset_y)* s_tileSize,
                            s_tileSize,
                            s_tileSize);
            redrawTile(layer, x, y, dest_rect);
        }
    if (layer == Core::UPPER)
    {
        m_painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        for (unsigned int i = 0; i < m_map->events.size(); i++)
        {
            if (x1 <= m_map->events[i].x && m_map->events[i].x <= x2
                && y1 <= m_map->events[i].y && m_map->events[i].y <= y2){
                QRect rect((m_map->events[i].x-offset_x)* s_tileSize,
                           (m_map->events[i].y-offset_y)* s_tileSize,
                           s_tileSize,
                           s_tileSize);
                m_painter.renderEvent(m_map->events[i], rect);
            }
        }
    }
    m_painter.endPainting();
    if (layer == Core::LOWER)
    {
        m_lowerpix->setPixmap(pix);
    }
    else
    {
        m_upperpix->setPixmap(pix);
    }
}

void MapScene::redrawLayer(Core::Layer layer)
{
    QSize size = getViewportContentSize();
	QPixmap pix(size);
    int start_x = m_view->horizontalScrollBar()->value()/s_tileSize;
    int start_y = m_view->verticalScrollBar()->value()/s_tileSize;
    int end_x = start_x+(size.width()-1)/s_tileSize;
    int end_y = start_y+(size.height()-1)/s_tileSize;
	pix.fill(QColor(0,0,0,0));
    m_painter.beginPainting(pix);
	for (int x = start_x; x <= end_x; x++)
		for (int y = start_y; y <= end_y; y++)
		{
			if (x >= m_map->width || y >= m_map->height)
				continue;
			QRect dest_rect((x-start_x)* s_tileSize,
					   (y-start_y)* s_tileSize,
					   s_tileSize,
					   s_tileSize);
			redrawTile(layer, x, y, dest_rect);
		}
	if (layer == Core::UPPER)
	{
        for (unsigned int i = 0; i < m_map->events.size(); i++)
		{
			QRect rect((m_map->events[i].x-start_x)* s_tileSize,
					   (m_map->events[i].y-start_y)* s_tileSize,
					   s_tileSize,
					   s_tileSize);
            m_painter.renderEvent(m_map->events[i], rect);
		}
	}
    m_painter.endPainting();
	if (layer == Core::LOWER)
	{
		m_lowerpix->setPixmap(pix);
        m_lowerpix->setPos(start_x*s_tileSize,start_y*s_tileSize);
	}
	else
	{
		m_upperpix->setPixmap(pix);
        m_upperpix->setPos(start_x*s_tileSize,start_y*s_tileSize);
	}
}

void MapScene::drawPen() {
    drawPen(cur_x, cur_y);
}

void MapScene::drawPen(int next_x, int next_y)
{
    for (int x = next_x; x < next_x + m_palette->selWidth(); x++)
        for (int y = next_y; y < next_y + m_palette->selHeight(); y++)
		{
			if (core().layer() == Core::LOWER)
                m_lower[static_cast<size_t>(_index(x,y))] = m_palette->selection(x-fst_x,y-fst_y);
			else if (core().layer() == Core::UPPER)
                m_upper[static_cast<size_t>(_index(x,y))] = m_palette->selection(x-fst_x,y-fst_y);
		}
    updateArea(next_x-1,next_y-1,next_x+m_palette->selWidth(),next_y+m_palette->selHeight());
    if (next_x - cur_x != 0 || next_y - cur_y != 0) {
        // TODO: replace this with a better line algorithm
        if (next_x - cur_x > 0) {next_x--;}
        else if (next_x - cur_x < 0) {next_x++;}
        if (next_y - cur_y > 0) {next_y--;}
        else if (next_y - cur_y < 0) {next_y++;}
        drawPen(next_x, next_y);
    }
}

void MapScene::drawRect() {
    drawRect(cur_x, cur_y);
}

void MapScene::drawRect(int next_x, int next_y)
{
	switch (core().layer())
	{
	case (Core::LOWER):
		m_lower = m_map->lower_layer;
		break;
	case (Core::UPPER):
		m_upper = m_map->upper_layer;
		break;
	default:
		break;
	}

    int x1 = fst_x > next_x ? next_x : fst_x;
    int x2 = fst_x > next_x ? fst_x : next_x;
    int y1 = fst_y > next_y ? next_y : fst_y;
    int y2 = fst_y > next_y ? fst_y : next_y;
    for (int x = x1; x <= x2; x++)
        for (int y = y1; y <= y2; y++)
		{
			if (core().layer() == Core::LOWER)
                m_lower[static_cast<size_t>(_index(x,y))] = m_palette->selection(x-fst_x,y-fst_y);
			else if (core().layer() == Core::UPPER)
                m_upper[static_cast<size_t>(_index(x,y))] = m_palette->selection(x-fst_x,y-fst_y);
        }
    updateArea(x1-1, y1-1, x2+1, y2+1, false);
    x1 = fst_x > cur_x ? cur_x : fst_x;
    x2 = fst_x > cur_x ? fst_x : cur_x;
    y1 = fst_y > cur_y ? cur_y : fst_y;
    y2 = fst_y > cur_y ? fst_y : cur_y;
    redrawArea(core().layer(), x1-1, y1-1, x2+1, y2+1);
}

void MapScene::drawFill(int terrain_id, int x, int y)
{
	if (x < 0 || x >= m_map->width || y < 0 || y >= m_map->height)
		return;
    if (terrain_id == m_palette->selection(x-fst_x,y-fst_y))
		return;
	switch (core().layer())
	{
	case (Core::LOWER):
        if (TileOps::translate(m_lower[static_cast<size_t>(_index(x,y))]) != terrain_id)
			return;
        m_lower[static_cast<size_t>(_index(x,y))] = m_palette->selection(x-fst_x,y-fst_y);
		break;
	case (Core::UPPER):
        if (TileOps::translate(m_upper[static_cast<size_t>(_index(x,y))]) != terrain_id)
			return;
        m_upper[static_cast<size_t>(_index(x,y))] = m_palette->selection(x-fst_x,y-fst_y);
		break;
	default:
		break;
	}
	drawFill(terrain_id, x, y-1);
	drawFill(terrain_id, x-1, y);
	drawFill(terrain_id, x+1, y);
	drawFill(terrain_id, x, y+1);
}

short MapScene::bind(int x, int y)
{
#define tile_u TileOps::translate(m_lower[static_cast<size_t>(_index(x, y-1))])
#define tile_d TileOps::translate(m_lower[static_cast<size_t>(_index(x, y+1))])
#define tile_l TileOps::translate(m_lower[static_cast<size_t>(_index(x-1, y))])
#define tile_r TileOps::translate(m_lower[static_cast<size_t>(_index(x+1, y))])
#define tile_ul TileOps::translate(m_lower[static_cast<size_t>(_index(x-1, y-1))])
#define tile_ur TileOps::translate(m_lower[static_cast<size_t>(_index(x+1, y-1))])
#define tile_dl TileOps::translate(m_lower[static_cast<size_t>(_index(x-1, y+1))])
#define tile_dr TileOps::translate(m_lower[static_cast<size_t>(_index(x+1, y+1))])
	int _code = 0, _scode = 0;
    int terrain_id = TileOps::translate(m_lower[static_cast<size_t>(_index(x, y))]);
	int u=0,d=0,l=0,r=0,ul=0,ur=0,dl=0,dr=0,sul=0,sur=0,sdl=0,sdr=0;
    if (TileOps::isDblock(terrain_id))
	{
		if (y > 0 && terrain_id != tile_u)
			u = UP;
		if (y < m_map->height-1 && terrain_id != tile_d)
			d = DOWN;
		if (x > 0 && terrain_id != tile_l)
			l = LEFT;
		if (x < m_map->width-1 && terrain_id != tile_r)
			r = RIGHT;
		if (u+l == 0 && x > 0 && y > 0 && terrain_id != tile_ul)
			ul = UPLEFT;
		if (u+r == 0 && x < m_map->width-1 && y > 0 && terrain_id != tile_ur)
			ur = UPRIGHT;
		if (d+l == 0 && x > 0 && y < m_map->height-1 && terrain_id != tile_dl)
			dl = DOWNLEFT;
		if (d+r == 0 && x < m_map->width-1 &&
				y < m_map->height-1 && terrain_id != tile_dr)
			dr = DOWNRIGHT;
		_code = u+d+l+r+ul+ur+dl+dr;
	}
    else if (TileOps::isWater(terrain_id) || TileOps::isAnimation(terrain_id))
	{
        if (y > 0 && (!TileOps::isWater(tile_u) &&
                      !TileOps::isAnimation(tile_u)))
			u = UP;
        if (y < m_map->height-1 && (!TileOps::isWater(tile_d) &&
                                          !TileOps::isAnimation(tile_d)))
			d = DOWN;
        if (x > 0 && (!TileOps::isWater(tile_l) &&
                      !TileOps::isAnimation(tile_l)))
			l = LEFT;
        if (x < m_map->width-1 && (!TileOps::isWater(tile_r) &&
                                         !TileOps::isAnimation(tile_r)))
			r = RIGHT;
        if ((u+l) == 0 && x > 0 && y > 0 && !TileOps::isWater(tile_ul) && !TileOps::isAnimation(tile_ul))
			ul = UPLEFT;
        if ((u+r) == 0 && x < m_map->width-1 && y > 0 && !TileOps::isWater(tile_ur) && !TileOps::isAnimation(tile_ur))
			ur = UPRIGHT;
        if ((d+l) == 0 && x > 0 && y < m_map->height-1 && !TileOps::isWater(tile_dl) && !TileOps::isAnimation(tile_dl))
			dl = DOWNLEFT;
		if ((d+r) == 0 && x < m_map->width-1 &&
                y < m_map->height-1 && !TileOps::isWater(tile_dr) && !TileOps::isAnimation(tile_dr))
			dr = DOWNRIGHT;
		_code = u+d+l+r+ul+ur+dl+dr;
		// DeepWater Special Corners
        if (TileOps::isDWater(terrain_id))
		{
            if (x > 0 && y > 0 && TileOps::isABWater(tile_u) && TileOps::isABWater (tile_l) && TileOps::isABWater (tile_ul))
				sul = UPLEFT;
            if (x < m_map->width-1 && y > 0 && TileOps::isABWater(tile_u) && TileOps::isABWater (tile_r) && TileOps::isABWater (tile_ur))
				sur = UPRIGHT;
            if (x > 0 && y < m_map->height-1 && TileOps::isABWater(tile_d) && TileOps::isABWater (tile_l) && TileOps::isABWater (tile_dl))
				sdl = DOWNRIGHT;
			if (x < m_map->width-1 && y < m_map->height-1 &&
                    TileOps::isABWater(tile_d) && TileOps::isABWater (tile_r) && TileOps::isABWater (tile_dr))
				sdr = DOWNLEFT;
		}
		else
		{
            if (x > 0 && y > 0 && TileOps::isDWater (tile_u) && TileOps::isDWater (tile_l) && TileOps::isWater(tile_ul))
				sul = UPLEFT;
            if (x < m_map->width-1 && y > 0 && TileOps::isDWater (tile_u) && TileOps::isDWater (tile_r) && TileOps::isWater(tile_ur))
				sur = UPRIGHT;
            if (x > 0 && y < m_map->height-1 && TileOps::isDWater (tile_d) && TileOps::isDWater (tile_l) && TileOps::isWater(tile_dl))
				sdl = DOWNRIGHT;
			if (x < m_map->width-1 && y < m_map->height-1 &&
                    TileOps::isDWater (tile_d) && TileOps::isDWater (tile_r) && TileOps::isWater(tile_dr))
				sdr = DOWNLEFT;
		}
		_scode = sul+sur+sdl+sdr;
	}
    return TileOps::translate(terrain_id, _code, _scode);
#undef tile_u
#undef tile_d
#undef tile_l
#undef tile_r
#undef tile_ul
#undef tile_ur
#undef tile_dl
#undef tile_dr
}

void MapScene::selectTile(int x, int y)
{
	cur_x = x;
	cur_y = y;
	std::unique_ptr<QGraphicsSceneMouseEvent> mpe{new QGraphicsSceneMouseEvent()};
	mpe->setButton(Qt::LeftButton);
	mousePressEvent(mpe.get());
}

void MapScene::centerOnTile(int x, int y)
{
	m_view->centerOn(x * s_tileSize, y * s_tileSize);
}

lcf::rpg::Event* MapScene::getEventAt(int x, int y) {
	std::vector<lcf::rpg::Event>::iterator ev;
	for (ev = m_map->events.begin(); ev != m_map->events.end(); ++ev) {
		if (_index(x, y) == _index(ev->x, ev->y)) {
			break;
		}
	}

	if (ev != m_map->events.end()) {
		return &(*ev);
	}

	return nullptr;
}

int MapScene::getFirstFreeId() {
	std::vector<lcf::rpg::Event>::iterator ev;
	int id = 1;
	for (;;++id) {
		bool valid = true;
		for (ev = m_map->events.begin(); ev != m_map->events.end(); ++ev) {
			if (ev->ID == id) {
				valid = false;
				break;
			}
		}
		if (valid) {
			break;
		}
	}

	return id;
}

void MapScene::redrawPanorama() {
    QSize panorama_size;
    if (!m_panoramaPixmap){
        QString panorama_name;
        if (m_map->parallax_flag) {
            panorama_name = m_map->parallax_name.c_str();
            panorama_size = setPanorama(panorama_name);
        } else {
            panorama_size = setPanorama(panorama_name);
        }
    } else {
        panorama_size = m_panoramaPixmap.size();
    }
    QSize size = getViewportContentSize();
    int panorama_width = (int)(((float)s_tileSize / 16) * panorama_size.width());
    int panorama_height = (int)(((float)s_tileSize / 16) * panorama_size.height());
    int start_x = m_view->horizontalScrollBar()->value()/panorama_width;
    int start_y = m_view->verticalScrollBar()->value()/panorama_height;
    int end_x = (start_x+(size.width()-1)/panorama_width)+1;
    int end_y = (start_y+(size.height()-1)/panorama_height)+1;
    int w_offset = m_view->horizontalScrollBar()->value()%panorama_width;
    int h_offset = m_view->verticalScrollBar()->value()%panorama_height;
    QPixmap pix(size);
    pix.fill(QColor(0,0,0,255));
    m_painter.beginPainting(pix);
    for (int x = start_x; x <= end_x; x++)
        for (int y = start_y; y <= end_y; y++)
        {
            QRect dest_rect(((x-start_x) * panorama_width) - w_offset,
                            ((y-start_y)* panorama_height) - h_offset,
                            panorama_width,
                            panorama_height);
            m_painter.drawPixmap(dest_rect, m_panoramaPixmap);
        }
    m_painter.endPainting();
    m_panorama->setPixmap(pix);
    m_panorama->setPos(m_view->horizontalScrollBar()->value(), m_view->verticalScrollBar()->value());
}

void MapScene::redrawGrid() {
	if (!grid_lines.empty()) {
		while (!grid_lines.empty()) {
			QGraphicsItem* line = grid_lines.takeLast();
			delete line;
		}
		destroyItemGroup(m_lines);
	}

	for (int x = 0; x <= m_map->width; x++) {
		grid_lines.append(new QGraphicsLineItem(x*core().tileSize(),
			0,
			x*core().tileSize(),
			m_map->height*core().tileSize()));
	}

	for (int y = 0; y <= m_map->height; y++) {
		grid_lines.append(new QGraphicsLineItem(0,
			y*core().tileSize(),
			m_map->width*core().tileSize(),
			y*core().tileSize()));
	}

	m_lines = createItemGroup(grid_lines);

	m_lines->setVisible(core().layer() == Core::EVENT);
}

QSize MapScene::getViewportContentSize() {
    QSize size = m_view->size();
    if (size.width() > m_map->width*s_tileSize)
        size.setWidth(m_map->width*s_tileSize);
    else
        size.setWidth(size.width()+s_tileSize);
    if (size.height() > m_map->height*s_tileSize)
        size.setHeight(m_map->height*s_tileSize);
    else
        size.setHeight(size.height()+s_tileSize);
    return size;
}

lcf::rpg::Event *MapScene::currentMapEvent(int eventID)
{
    lcf::rpg::Event *event = nullptr;
    if (m_currentMapEvents)
        event = m_currentMapEvents->get_or_return_default(eventID);
    if (!event)
    {
        event = new lcf::rpg::Event();
        event->name = "<?>";
    }
    return event;
}

void MapScene::setCurrentMapEvents(emhash8::HashMap<int, lcf::rpg::Event *> *events)
{
    m_currentMapEvents = events;
}

void MapScene::setTileset(int index) {
    m_painter.setChipset(ToQString(core().project()->database().chipsets[index-1].chipset_name));
}

QSize MapScene::setPanorama(QString name)
{
    QPixmap panorama = ImageLoader::Load(core().project()->findFile(PANORAMA, name, FileFinder::FileType::Image));
    if (!panorama)
        panorama = ImageLoader::Load(core().rtpPath(PANORAMA, name));
    if (!panorama)
    {
        panorama = QPixmap(640, 320);
        panorama.fill(Qt::black);
    }

    m_panoramaPixmap = panorama;
    return m_panoramaPixmap.size();
}
