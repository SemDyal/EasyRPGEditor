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

#include "show_choices_widget.h"
#include "ui_show_choices_widget.h"
#include "lcf/rpg/eventcommand.h"
#include "common/dbstring.h"
#include "model/event_command_list.h"
#include <QLineEdit>

ShowChoicesWidget::ShowChoicesWidget(ProjectData& project, QWidget *parent) :
    EventCommandBaseWidget(project, parent),
    ui(new Ui::ShowChoicesWidget)
{
	ui->setupUi(this);

    QRadioButton* buttons[] = {ui->radioButton_1,ui->radioButton_2,ui->radioButton_3,ui->radioButton_4,ui->radioButton_5,ui->radioButton_6};
    for (int i = 0; i < 6; i++) {
        group.addButton(buttons[i], i);
    }
}

ShowChoicesWidget::~ShowChoicesWidget()
{
	delete ui;
}

void ShowChoicesWidget::setData(EventCommandList* commands) {
    EventCommandBaseWidget::setData(commands);

    using Cmd = lcf::rpg::EventCommand::Code;

    auto choices = ToQString(m_commands->command().string).split(QString("/"));
    QLineEdit* labels[] = {ui->choice1, ui->choice2, ui->choice3, ui->choice4};
    for (int i = 0; i < choices.length(); i++) {
        labels[i]->setText(choices[i]);
    }
    group.button(m_cmd->parameters[0])->setChecked(true);
}

void ShowChoicesWidget::apply() {
    auto show_choice_cmd = m_commands->command();
    auto old_choices = ToQString(show_choice_cmd.string).split(QString("/"));
    int previous_nb_choices = old_choices.size();
    int previous_nb_options = previous_nb_choices + (show_choice_cmd.parameters[0] == 5 ? 1 : 0);
    std::vector<int> choices_cmd_index(previous_nb_options, 0);
    int last = 0;

    int target_indent = m_cmd->indent;
    int current_option = 0;

    using Cmd = lcf::rpg::EventCommand::Code;
    for (size_t i = m_commands->index() + 1; i < m_commands->size(); ++i) {
        auto& cur_cmd = m_commands->commands()[i];

        if (cur_cmd.indent == target_indent) {
            if (static_cast<Cmd>(cur_cmd.code) == Cmd::ShowChoiceOption) {
                choices_cmd_index[current_option] = i;
                current_option++;
            }
            else if (static_cast<Cmd>(cur_cmd.code) == Cmd::ShowChoiceEnd) {
                last = i;
                break;
            }
        }
    }

    int insert_choices_before = (show_choice_cmd.parameters[0] == 5 ? choices_cmd_index[current_option - 1] : last);

    QString labels[] = {ui->choice1->text(), ui->choice2->text(), ui->choice3->text(), ui->choice4->text()};
    int nb_choices = 1;
    for (int i = 1; i < 4;i++) {
        if (labels[i] != "") {
            nb_choices = i + 1;
        }
    }

    QString short_choices = ui->choice1->text();
    for (int i = 1; i < nb_choices;i++) {
        short_choices += "/" + labels[i];
    }
    m_commands->command().string = ToDBString(short_choices);

    using Cmd = lcf::rpg::EventCommand::Code;


    if (show_choice_cmd.parameters[0] == 5 && group.checkedId() != 5) {
        m_commands->commands().erase(m_commands->commands().begin() + insert_choices_before, m_commands->commands().begin() + last);
    } else if (show_choice_cmd.parameters[0] != 5 && group.checkedId() == 5) {
        lcf::rpg::EventCommand option;
        option.code = static_cast<int>(Cmd::ShowChoiceOption);
        option.parameters = lcf::DBArray(1, 4);
        lcf::rpg::EventCommand end;
        end.code = static_cast<int>(Cmd::END);
        m_commands->commands().insert(m_commands->commands().begin() + last, option);
        m_commands->commands().insert(m_commands->commands().begin() + last + 1, end);
    }

    m_commands->command().parameters[0] = group.checkedId();

    for (int j = previous_nb_choices - 1; j >= nb_choices; j--) {
        int from = choices_cmd_index[j];
        int to = (j < previous_nb_choices - 1) ? choices_cmd_index[j + 1] : insert_choices_before;
        m_commands->commands().erase(m_commands->commands().begin() + from, m_commands->commands().begin() + to);
    }

    for (int j = nb_choices - 1; j >= previous_nb_choices; j--) {
        lcf::rpg::EventCommand option;
        option.code = static_cast<int>(Cmd::ShowChoiceOption);
        option.string = ToDBString(labels[j]);
        option.parameters = lcf::DBArray(1, j);
        lcf::rpg::EventCommand end;
        end.code = static_cast<int>(Cmd::END);
        m_commands->commands().insert(m_commands->commands().begin() + insert_choices_before, option);
        m_commands->commands().insert(m_commands->commands().begin() + insert_choices_before + 1, end);
    }

    for (int j = 0; j < previous_nb_choices && j < nb_choices; j++) {
        int from = choices_cmd_index[j];
        m_commands->commands()[from].string = ToDBString(labels[j]);
    }

}
