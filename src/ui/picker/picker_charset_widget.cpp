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

#include "picker_charset_widget.h"
#include "ui_picker_charset_widget.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

PickerCharsetWidget::PickerCharsetWidget(int index, int pattern, int direction, bool showUpper, bool showControls, QWidget *parent) :
    PickerChildWidget(parent),
    m_index(index),
    m_pattern(pattern),
    m_direction(direction),
    m_showUpper(showUpper),
    m_showControls(showControls),
    ui(new Ui::PickerCharsetWidget) {
    if (showControls) {
        ui->setupUi(this);

        switch (m_pattern) {
        case 0:
            ui->patternLeftButton->setChecked(true);
            break;
        case 2:
            ui->patternRightButton->setChecked(true);
            break;
        default:
            ui->patternMiddleButton->setChecked(true);
            break;
        }

        switch (m_direction) {
        case 0:
            ui->directionUpButton->setChecked(true);
            break;
        case 1:
            ui->directionRightButton->setChecked(true);
            break;
        case 3:
            ui->directionLeftButton->setChecked(true);
            break;
        default:
            ui->directionDownButton->setChecked(true);
            break;
        }
    }
}

PickerCharsetWidget::~PickerCharsetWidget() {
    delete ui;
}

void PickerCharsetWidget::clicked(const QPointF& pos) {
    int x = static_cast<int>(pos.x() / cell_width);
    int y = static_cast<int>(pos.y() / cell_height);
	m_index = y * 4 + x;
	updateRect();
}

void PickerCharsetWidget::imageChanged(QPixmap image, QString filename) {
    m_image = image;
    m_filename = filename;
    redrawImage(image, filename);
}

void PickerCharsetWidget::redrawImage(QPixmap image, QString filename) {
    if (!m_pixmap) {
        m_view->setMinimumSize(196, 132);
        m_view->scale(2.0, 2.0);
        m_pixmap = new QGraphicsPixmapItem();
    }

    if (filename.startsWith("$")) {
        cell_width = image.width() / 12;
        cell_height = image.height() / 8;
    } else if (filename.isEmpty() && m_showUpper) {
        // empty filename means "use upper layer tile graphic"
        cell_width = cell_height = 16;
    } else {
        cell_width = 24;
        cell_height = 32;
    }

    QPixmap new_image(cell_width * 4, cell_height * 2);
    new_image.fill(QColor(0,0,0,0));

    QPainter* paint = new QPainter(&new_image);
    for (int i = 0; i < 8; ++i) {
        int src_x = ((i % 4) * cell_width * 3 + (m_pattern * cell_width));
        int src_y = (i >= 4) ? (m_direction + 4) * cell_height : m_direction * cell_height;
        int target_x = (i % 4) * cell_width;
        int target_y = i >= 4 ? cell_height : 0;
        paint->drawPixmap(QRect(target_x, target_y, cell_width, cell_height), image, QRect(src_x, src_y, cell_width, cell_height));
    }
    delete paint;

    m_pixmap->setPixmap(new_image);
    m_view->setItem(m_pixmap);
    updateRect();
}

void PickerCharsetWidget::updateRect() {
	if (!m_rect) {
		m_rect = new QGraphicsRectItem();

		QPen selPen(Qt::white);
		selPen.setWidth(2);
		m_rect->setPen(selPen);

		m_view->scene()->addItem(m_rect);
	}

    m_rect->setRect(QRect(
        (m_index % 4) * cell_width,
        m_index / 4 * cell_height,
        cell_width,
        cell_height));
}

void PickerCharsetWidget::on_directionUpButton_clicked()
{
    m_direction = 0;
    redrawImage(m_image, m_filename);
}


void PickerCharsetWidget::on_directionRightButton_clicked()
{
    m_direction = 1;
    redrawImage(m_image, m_filename);
}


void PickerCharsetWidget::on_directionLeftButton_clicked()
{
    m_direction = 3;
    redrawImage(m_image, m_filename);
}


void PickerCharsetWidget::on_directionDownButton_clicked()
{
    m_direction = 2;
    redrawImage(m_image, m_filename);
}


void PickerCharsetWidget::on_patternLeftButton_clicked()
{
    m_pattern = 0;
    redrawImage(m_image, m_filename);
}


void PickerCharsetWidget::on_patternMiddleButton_clicked()
{
    m_pattern = 1;
    redrawImage(m_image, m_filename);
}


void PickerCharsetWidget::on_patternRightButton_clicked()
{
    m_pattern = 2;
    redrawImage(m_image, m_filename);
}

