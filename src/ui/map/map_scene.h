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
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QMap>
#include <QMenu>
#include <QUndoStack>
#include <QTreeWidgetItem>
#include <memory>
#include <qevent.h>
#include <lcf/rpg/map.h>
#include <lcf/rpg/mapinfo.h>
#include <qshortcut.h>
#include <ui/rpg_painter.h>
#include <ui/common/palette_scene.h>
#include "common/dbstring.h"
#include "core.h"

class ProjectData;

class MapScene : public QGraphicsScene
{
	Q_OBJECT
public:
    explicit MapScene(ProjectData& project, int id, QGraphicsView *view, PaletteScene *palette, QObject *parent = nullptr);
	~MapScene();

	void Init();
	float scale() const;
	void selectTile(int x, int y);
	void centerOnTile(int x, int y);
	void setScale(float scale);
	QString mapName() const;
	bool isModified() const;
	int id() const;
    int chipsetId() const;
    lcf::rpg::Map*map() const;
	void setLayerData(Core::Layer layer, std::vector<short> data);
    void setEvent(int id, const lcf::rpg::Event &data);
    void deleteEvent(int id);
    emhash8::HashMap<int, lcf::rpg::Event *> *mapEvents();
	void editMapProperties(QTreeWidgetItem *item);

    inline bool canUndo() {return m_undoStack->canUndo();};
    inline bool canRedo() {return m_undoStack->canRedo();};
    inline bool isClean() {return m_undoStack->isClean();};

    lcf::rpg::Event *currentMapEvent(int eventID);
    void loadEvents();
    void setCurrentMapEvents(emhash8::HashMap<int, lcf::rpg::Event *> *events);
    void setTileset(int index);
    QSize setPanorama(QString name);
    inline std::shared_ptr<emhash8::HashMap<short, QPixmap>> &sharePainterTiles() { return m_painter.sharePainterTiles(); };
    friend class UndoDraw;
    friend class UndoEvent;


signals:

	void mapChanged();

	void mapSaved();

    void mapCleanChanged(bool clean);

public slots:
	void redrawMap();

	void onLayerChanged();

	void onToolChanged();

    void onCleanChanged(bool clean);

	void Save(bool properties_changed = false);

    void Load(bool revert = false);

    void undo();
    void redo();

private slots:
	void on_actionRunHere();

	void on_actionSetStartPosition();

    void on_actionCopy();
    void on_actionCut();
    void on_actionPaste();

	void on_actionNewEvent();

	void on_actionEditEvent();

	void on_actionCopyEvent();

	void on_actionCutEvent();

	void on_actionPasteEvent();

	void on_actionDeleteEvent();

	void on_user_interaction();

    void on_view_V_Scroll();

    void on_view_H_Scroll();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    void processTools(int next_x, int next_y);
	int _x(int index);
	int _y(int index);
	int _index(int x, int y);
	void redrawTile(const Core::Layer &layer,
					const int &x,
					const int &y,
					const QRect &dest_rec);
	void stopDrawing();
	void stopSelecting();
    void updateArea(int x1, int y1, int x2, int y2, bool redraw = true);
    void redrawArea(Core::Layer layer, int x1, int y1, int x2, int y2);
	void redrawLayer(Core::Layer layer);
    void drawPen();
    void drawPen(int next_x, int next_y);
	void drawRect();
    void drawRect(int next_x, int next_y);
	void drawFill(int terrain_id, int x, int y);
	short bind(int x, int y);
	lcf::rpg::Event* getEventAt(int x, int y);
	int getFirstFreeId();
	void redrawPanorama();
	void redrawGrid();
    QSize getViewportContentSize();


	QMenu *m_eventMenu;
    QGraphicsPixmapItem *m_panorama;
	QGraphicsPixmapItem *m_lowerpix;
	QGraphicsPixmapItem *m_upperpix;
	QGraphicsItemGroup* m_lines;
	QGraphicsRectItem* m_selectionTile;
	QUndoStack *m_undoStack;
	std::unique_ptr<lcf::rpg::Map> m_map;
	lcf::rpg::MapInfo n_mapInfo; //To store unsaved changes
	std::vector<short> m_lower;
	std::vector<short> m_upper;
	float m_scale;
	bool m_init = false;
	int s_tileSize;
    int cur_x = -1;
    int cur_y = -1;
    bool in_bounds;
	int fst_x;
	int fst_y;
	int lst_x;
	int lst_y;
	QGraphicsView* m_view;
    bool m_drawing = false;
	bool m_cancelled;
	bool m_selecting;
	bool m_userInteraction = false;
	ProjectData& m_project;
	lcf::rpg::Event event_clipboard;
	bool event_clipboard_set = false;
	QList<QGraphicsItem*> grid_lines;
    RpgPainter m_painter;
    QPixmap m_panoramaPixmap;
    emhash8::HashMap<int, lcf::rpg::Event*> *m_currentMapEvents;
    int m_lastHScrollPos;
    int m_lastVScrollPos;
    PaletteScene *m_palette;

};

