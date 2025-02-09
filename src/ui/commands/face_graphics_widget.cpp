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

#include "face_graphics_widget.h"
#include "common/dbstring.h"
#include "ui_face_graphics_widget.h"
#include "ui/picker/picker_dialog.h"
#include "ui/picker/picker_faceset_widget.h"

FaceGraphicsWidget::FaceGraphicsWidget(ProjectData& project, QWidget *parent) :
    EventCommandBaseWidget(project, parent),
    ui(new Ui::FaceGraphicsWidget) {

    ui->setupUi(this);

    m_faceItem = new FaceSetGraphicsItem(project);
    ui->face_preview->scale(2.0, 2.0);
    ui->face_preview->setItem(m_faceItem);
    QObject::connect(ui->face_preview, &RpgGraphicsViewBase::clicked, this, &FaceGraphicsWidget::faceSetClicked);
    QObject::connect(ui->button_set, &QPushButton::clicked, this, &FaceGraphicsWidget::faceSetClicked);
    QObject::connect(ui->button_erase, &QPushButton::clicked, this, &FaceGraphicsWidget::faceSetReset);
    QObject::connect(ui->box_flip, &QCheckBox::clicked, this, [&](const bool checked) {
        ui->face_preview->item()->setFlipped(checked);
    });
}

FaceGraphicsWidget::~FaceGraphicsWidget()
{
	delete ui;
}

void FaceGraphicsWidget::setData(lcf::rpg::EventCommand* cmd) {
    EventCommandBaseWidget::setData(cmd);

    using Cmd = lcf::rpg::EventCommand::Code;
    if (cmd->string != "") {
        ui->face_preview->item()->setFlipped(cmd->parameters[2] != 0);
        ui->face_preview->item()->refresh(ToQString(cmd->string), cmd->parameters[0]);
        m_file = ToQString(cmd->string);
    }
    if (cmd->parameters[1] != 0) {
        ui->position_left->setChecked(true);
    } else {
        ui->position_right->setChecked(true);
    }
    ui->box_flip->setChecked(cmd->parameters[2] != 0);

}

void FaceGraphicsWidget::faceSetClicked() {
    auto* widget = new PickerFacesetWidget(0, this);
    PickerDialog dialog(m_project, FileFinder::FileType::Image, widget, this);
    QObject::connect(&dialog, &PickerDialog::fileSelected, [&](const QString& baseName) {
        m_faceItem->refresh(baseName, widget->index());
        m_file = baseName;
    });
    if (m_cmd->string != "") {
        dialog.setDirectoryAndFile(FACESET, ToQString(m_cmd->string));
    } else {
        dialog.setDirectory(FACESET);
    }
    dialog.exec();
}

void FaceGraphicsWidget::faceSetReset() {
    m_faceItem->refresh("", 0);
    m_file = "";
}

void FaceGraphicsWidget::apply() {
    m_cmd->string = ToDBString(m_file);
    m_cmd->parameters[0] = m_faceItem->index();
    m_cmd->parameters[1] = ui->position_right->isChecked() ? 0 : 1;
    m_cmd->parameters[2] = ui->box_flip->isChecked();
}
