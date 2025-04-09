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

#include "picker_dialog.h"
#include "ui_picker_dialog.h"
#include "model/project.h"
#include "common/image_loader.h"
#include "picker_child_widget.h"
#include <QFileSystemModel>
#include <QGraphicsScene>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <qpushbutton.h>
#include <core.h>

PickerDialog::PickerDialog(ProjectData &project, FileFinder::FileType file_type, PickerChildWidget* wrappedWidget, QWidget *parent) :
		QDialog(parent),
		ui(new Ui::PickerDialog),
		m_project(project),
		m_file_type(file_type) {
	ui->setupUi(this);
	wrappedWidget->setParent(this);
	ui->wrappedWidget = wrappedWidget;
	ui->sidebar->insertWidget(1, wrappedWidget);

	m_model = new QFileSystemModel(this);
	m_model->setReadOnly(true);
	ui->filesystemView->setModel(m_model);

    // Hide columns except name
    for (int i = 1; i <= 3; ++i) {
        ui->filesystemView->setColumnHidden(i, true);
    }

	QObject::connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PickerDialog::buttonClicked);

	if (file_type == FileFinder::FileType::Image) {
		ui->wrappedWidget->setView(ui->graphicsView);
		QObject::connect(ui->graphicsView, &RpgGraphicsViewBase::clicked, this, &PickerDialog::viewClicked);
	} else {
		ui->graphicsView->setVisible(false);
	}
}

PickerDialog::~PickerDialog() {
	delete ui;
}

QPushButton* PickerDialog::addActionButton(QString label) {
    auto* button = new QPushButton(label);
    ui->viewLayout->addWidget(button);
    return button;
}

void PickerDialog::makePreview() {
    ui->buttonBox->hide();
    setModal(false);
}

void PickerDialog::buttonClicked(QAbstractButton* button) {
	switch (ui->buttonBox->standardButton(button)) {
		case QDialogButtonBox::Ok:
			emit fileSelected(m_currentFile.baseName());
			QDialog::accept();
			break;
		case QDialogButtonBox::Cancel:
		case QDialogButtonBox::Close:
			QDialog::reject();
			break;
		case QDialogButtonBox::Apply:
			emit fileSelected(m_currentFile.baseName());
			break;
		case QDialogButtonBox::Help:
			// TODO
			break;
		default:
			break;
	}
}

void PickerDialog::setUpperTileFile() {
    // intentionally set an invalid index to hide selection
    ui->filesystemView->setCurrentIndex(QModelIndex());
    // intentionally create an empty fileinfo to spoof base name for saving
    m_currentFile = QFileInfo();
    // manually override the graphic
    QPixmap tileOverview(96, 384);
    tileOverview.fill(Qt::transparent);
    m_painter.beginPainting(tileOverview);
    m_painter.renderTileOverview(RpgPainter::ALL_UPPER);
    m_painter.endPainting();
    ui->wrappedWidget->imageChanged(tileOverview, "");
}

void PickerDialog::setDirectory(const QString &dir) {
    m_dir.setPath(dir);
    QString path = m_project.project().findDirectory(dir);
    m_model->setRootPath(path);
    ui->filesystemView->setRootIndex(m_model->index(path));
}

void PickerDialog::setDirectoryAndFile(const QString &dir, const QString& initialFile) {
    m_dir.setPath(dir);
    QString path = m_project.project().findDirectory(dir);
	QString file = m_project.project().findFile(dir, initialFile, m_file_type);
	m_model->setRootPath(path);
	ui->filesystemView->setRootIndex(m_model->index(path));

	QModelIndex index = m_model->index(file);
	ui->filesystemView->setCurrentIndex(index);

    m_currentFile = QFileInfo(file);

	ui->wrappedWidget->fileChanged(m_currentFile.absoluteFilePath());

	if (m_file_type == FileFinder::FileType::Image) {
		QPixmap image = ImageLoader::Load(m_currentFile.absoluteFilePath());
        ui->wrappedWidget->imageChanged(image, m_currentFile.fileName());
	}
}

void PickerDialog::on_filesystemView_clicked(const QModelIndex &index) {
	m_currentFile = m_model->fileInfo(index);
	ui->wrappedWidget->fileChanged(m_currentFile.absoluteFilePath());
	if (m_file_type == FileFinder::FileType::Image) {
		QPixmap image = ImageLoader::Load(m_currentFile.absoluteFilePath());
        ui->wrappedWidget->imageChanged(image, m_currentFile.fileName());
	}
}

void PickerDialog::on_filesystemView_doubleClicked(const QModelIndex &index) {
    m_currentFile = m_model->fileInfo(index);
    ui->wrappedWidget->fileDoubleClicked(m_currentFile.absoluteFilePath());
    if (m_file_type == FileFinder::FileType::Image) {
        QPixmap image = ImageLoader::Load(m_currentFile.absoluteFilePath());
        ui->wrappedWidget->imageChanged(image, m_currentFile.fileName());
    }
}

void PickerDialog::viewClicked(const QPointF& pos) {
	ui->wrappedWidget->clicked(pos);
}



