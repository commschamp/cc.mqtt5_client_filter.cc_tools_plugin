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


#pragma once

#include "ui_Mqtt5ClientFilterTopicAliasWidget.h"

#include "Mqtt5ClientFilter.h"

#include <QtWidgets/QWidget>


namespace cc_plugin_mqtt5_client_filter
{

class Mqtt5ClientFilterTopicAliasWidget : public QWidget
{
    Q_OBJECT
    using Base = QWidget;

public:
    using TopicAliasConfig = Mqtt5ClientFilter::TopicAliasConfig;

    explicit Mqtt5ClientFilterTopicAliasWidget(Mqtt5ClientFilter& filter, TopicAliasConfig& config, QWidget* parentObj = nullptr);
    ~Mqtt5ClientFilterTopicAliasWidget() noexcept = default;

private slots:
    void topicAliasUpdated(const QString& val);
    void delClicked(bool checked);

private:
    Mqtt5ClientFilter& m_filter;
    TopicAliasConfig& m_config;
    Ui::Mqtt5ClientFilterTopicAliasWidget m_ui;
};

}  // namespace cc_plugin_mqtt5_client_filter


