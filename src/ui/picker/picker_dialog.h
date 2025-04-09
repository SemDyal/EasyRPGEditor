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

#include <QDialog>
#include <QDir>
#include <QMouseEvent>

#include <ui/rpg_painter.h>

#include "common/filefinder.h"

class ProjectData;
class QFileSystemModel;
class QAbstractButton;
class PickerChildWidget;

namespace Ui {
class PickerDialog;
}

class PickerDialog : public QDialog {
	Q_OBJECT

public:
	explicit PickerDialog(ProjectData& project, FileFinder::FileType file_type = FileFinder::FileType::Default, PickerChildWidget* wrappedWidget = nullptr, QWidget *parent = nullptr);
	~PickerDialog() override;

    QPushButton* addActionButton(QString label);
    void makePreview();

    void setDirectory(const QString &dir);
	void setDirectoryAndFile(const QString& dir, const QString& initialFile);

    void setUpperTileFile();

signals:
	void fileSelected(QString baseName);

private slots:
	void on_filesystemView_clicked(const QModelIndex &index);
	void buttonClicked(QAbstractButton* button);
	void viewClicked(const QPointF& pos);

    void on_filesystemView_doubleClicked(const QModelIndex &index);

protected:
	Ui::PickerDialog *ui;

private:
	ProjectData& m_project;
	QDir m_dir;
	QFileInfo m_currentFile;
	QFileSystemModel* m_model = nullptr;
	FileFinder::FileType m_file_type;
    RpgPainter m_painter;
};
