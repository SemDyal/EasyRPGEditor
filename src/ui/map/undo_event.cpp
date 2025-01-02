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

#include "undo_event.h"

UndoEvent::UndoEvent(
    std::optional<lcf::rpg::Event> before,
    std::optional<lcf::rpg::Event> after,
    MapScene *scene,
    QUndoCommand *parent
) :
QUndoCommand(parent),
m_before(before),
m_after(after),
m_scene(scene)
{}

void UndoEvent::undo()
{
    if (m_after) {
        if (m_before) {
            // changed
            m_scene->setEvent(m_before.value().ID, m_before.value());
        } else {
            // new
            m_scene->deleteEvent(m_after.value().ID);
        }
        m_scene->redrawArea(Core::UPPER, m_after.value().x, m_after.value().y, m_after.value().x, m_after.value().y);
    } else {
        //removed
        m_scene->setEvent(m_before.value().ID, m_before.value());
        m_scene->redrawArea(Core::UPPER, m_before.value().x, m_before.value().y, m_before.value().x, m_before.value().y);
    }
}

void UndoEvent::redo()
{
    if (m_after) {
        if (m_before) {
            // changed
            m_scene->setEvent(m_after.value().ID, m_after.value());
        } else {
            // new
            m_scene->setEvent(m_after.value().ID, m_after.value());
        }
        m_scene->redrawArea(Core::UPPER, m_after.value().x, m_after.value().y, m_after.value().x, m_after.value().y);
    } else {
        //removed
        m_scene->deleteEvent(m_before.value().ID);
        m_scene->redrawArea(Core::UPPER, m_before.value().x, m_before.value().y, m_before.value().x, m_before.value().y);
    }
}
