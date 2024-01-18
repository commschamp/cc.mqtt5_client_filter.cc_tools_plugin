//
// Copyright 2024 - 2025 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include "Mqtt5ClientFilterTopicAliasWidget.h"

#include <algorithm>


namespace cc_plugin_mqtt5_client_filter
{

Mqtt5ClientFilterTopicAliasWidget::Mqtt5ClientFilterTopicAliasWidget(Mqtt5ClientFilter& filter, TopicAliasConfig& config, QWidget* parentObj) : 
    Base(parentObj),
    m_filter(filter),
    m_config(config)
{
    m_ui.setupUi(this);

    m_ui.m_topicAliasLineEdit->setText(m_config.m_topic);

    connect(
        m_ui.m_topicAliasLineEdit, &QLineEdit::textChanged,
        this, &Mqtt5ClientFilterTopicAliasWidget::topicAliasUpdated);   

    connect(
        m_ui.m_delToolButton, &QToolButton::clicked,
        this, &Mqtt5ClientFilterTopicAliasWidget::delClicked);           
}

void Mqtt5ClientFilterTopicAliasWidget::topicAliasUpdated(const QString& val)
{
    m_config.m_topic = val;
}

void Mqtt5ClientFilterTopicAliasWidget::delClicked([[maybe_unused]] bool checked)
{
    auto& aliases = m_filter.config().m_topicAliases;
    auto iter = 
        std::find_if(
            aliases.begin(), aliases.end(), 
            [this](auto& info)
            {
                return &m_config == &info;
            });

    if (iter == aliases.end()) {
        assert(false); // should not happen
        return;
    }

    aliases.erase(iter);
    blockSignals(true);
    deleteLater();
}


}  // namespace cc_plugin_mqtt5_client_filter


