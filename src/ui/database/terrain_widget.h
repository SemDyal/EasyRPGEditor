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

#include <QButtonGroup>
#include <QWidget>
#include <lcf/rpg/terrain.h>

class ProjectData;

namespace Ui {
class TerrainWidget;
}

class TerrainWidget : public QWidget
{
	Q_OBJECT

public:
	using value_type = lcf::rpg::Terrain;

	explicit TerrainWidget(ProjectData& project, QWidget *parent = nullptr);
	~TerrainWidget();

	void setData(lcf::rpg::Terrain* terrain);

private:
	Ui::TerrainWidget *ui;
	ProjectData& m_project;
    lcf::rpg::Terrain *m_current = nullptr;
    lcf::rpg::Terrain m_dummy;
    QButtonGroup* m_spriteButtonGroup;
    QButtonGroup* m_gridButtonGroup;
    QButtonGroup* m_backgroundButtonGroup;
};

