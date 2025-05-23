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

#ifdef Q_OS_WIN
#define PLAYER "Player.exe"
#else
#define PLAYER "easyrpg-player"
#endif

#include "defines.h"

#include <QPixmap>
#include <QPainter>
#include <QListWidget>
#include <lcf/rpg/map.h>
#include <lcf/rpg/chipset.h>
#include <vendor/hash_table8.hpp>
#include "ui/other/run_game_dialog.h"
#include "model/project.h"

class Core : public QObject
{
	Q_OBJECT

public:

	enum Layer
	{
		LOWER,
		UPPER,
		EVENT
	};

	enum Tool
	{
		ZOOM,
		PENCIL,
		RECTANGLE,
		CIRCLE,
		FILL
	};

	Core();

	static Core* getCore();

    QSize loadPanorama(QString name);

	int tileSize();
	void setTileSize(int tileSize);

	QString rtpPath(const QString &folder, const QString &filename = QString()) const;

	Layer layer();
	void setLayer(const Layer &layer);

	Tool tool();
    void setTool(const Tool &tool);

	QString gameTitle();
	void setGameTitle(const QString &gameTitle);

	QPixmap createDummyPixmap(int width, int height);

	QString defDir() const;
	void setDefDir(const QString &defDir);

	void setRtpDir(const QString &n_path);

	void cacheEvent(const lcf::rpg::Event* ev, QString key);

	void runGame();
	void runGameHere(int map_id, int x, int y);
	void runBattleTest(int troop_id);

	std::shared_ptr<Project>& project();
    const std::shared_ptr<Project>& project() const;
    emhash8::HashMap<QString, QPixmap> &getEventCache();

signals:
	void toolChanged();

	void layerChanged();

	void chipsetChanged();

private:
	int m_tileSize;
	QString m_defDir;
    QString m_rtpDir;
	Layer m_layer;
    Tool m_tool;
    emhash8::HashMap<QString, QPixmap> m_eventCache;
    static Core *core_instance;
	RunGameDialog *m_runGameDialog;
    std::shared_ptr<Project> m_project;
};

Core& core();
