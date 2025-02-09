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

#include "show_message_widget.h"
#include "ui_show_message_widget.h"
#include "common/dbstring.h"
#include "model/event_command_list.h"
#include "lcf/rpg/eventcommand.h"

ShowMessageWidget::ShowMessageWidget(ProjectData &project, QWidget *parent) :
    EventCommandBaseWidget(project, parent),
    ui(new Ui::ShowMessageWidget) {

    ui->setupUi(this);
}

ShowMessageWidget::~ShowMessageWidget() {
    delete ui;
}

void ShowMessageWidget::setData(EventCommandList* commands) {
    EventCommandBaseWidget::setData(commands);

    using Cmd = lcf::rpg::EventCommand::Code;

    ui->message->append(ToQString(m_cmd->string));

    for (size_t i = commands->index() + 1; i < commands->size(); ++i) {
        auto& cur_cmd = commands->commands()[i];

        if (static_cast<Cmd>(cur_cmd.code) != Cmd::ShowMessage_2) {
            break;
        }

        ui->message->append(ToQString(cur_cmd.string));
    }
}

void ShowMessageWidget::apply() {
    auto message = ui->message->toPlainText();
    auto lines = message.split(QString("\n"));

    m_cmd->string = ToDBString(lines[0]);

    using Cmd = lcf::rpg::EventCommand::Code;

    size_t i = m_commands->index() + 1;
    for (int curr_line = 1; curr_line < lines.size(); ++curr_line) {

        if (i < m_commands->size()) { // Attempt to replace the next ShowMessage_2
            auto& cur_cmd = m_commands->commands()[i];

            if (static_cast<Cmd>(cur_cmd.code) != Cmd::ShowMessage_2) {
                lcf::rpg::EventCommand cmd;
                cmd.code = (int32_t) Cmd::ShowMessage_2;
                cmd.indent = m_commands->command().indent;
                m_commands->commands().insert(m_commands->commands().begin() + i, cmd);
            }
        } else {
            lcf::rpg::EventCommand cmd;
            cmd.code = (int32_t) Cmd::ShowMessage_2;
            cmd.indent = m_commands->command().indent;
            m_commands->commands().insert(m_commands->commands().begin() + i, cmd);
        }

        m_commands->commands()[i].string = ToDBString(lines[curr_line]);
        ++i;
    }

    int after = m_commands->index() + lines.length();
    auto& cur_cmd = m_commands->commands()[after];
    while (static_cast<Cmd>(cur_cmd.code) == Cmd::ShowMessage_2) {
        m_commands->commands().erase(m_commands->commands().begin() + after);
        cur_cmd = m_commands->commands()[after];
    }
}
