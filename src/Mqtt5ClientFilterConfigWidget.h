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

#include "ui_Mqtt5ClientFilterConfigWidget.h"

#include "Mqtt5ClientFilter.h"

#include <QtWidgets/QWidget>


namespace cc_plugin_mqtt5_client_filter
{

class Mqtt5ClientFilterConfigWidget : public QWidget
{
    Q_OBJECT
    using Base = QWidget;

public:
    explicit Mqtt5ClientFilterConfigWidget(Mqtt5ClientFilter& filter, QWidget* parentObj = nullptr);
    ~Mqtt5ClientFilterConfigWidget() noexcept;

private slots:
    void refresh();
    void respTimeoutUpdated(int val);
    void clientIdUpdated(const QString& val);
    void usernameUpdated(const QString& val);
    void passwordUpdated(const QString& val);
    void passwordShowHideClicked(bool checked);
    void keepAliveUpdated(int val);
    void sessionExpiryUpdated(int val);
    void sessionExpiryInfiniteUpdated(int state);
    void sessionExpiryInfiniteUpdated(Qt::CheckState state);
    void topicAliasMaximumUpdated(int val);
    void forcedCleanStartUpdated(int val);
    void pubTopicUpdated(const QString& val);
    void pubQosUpdated(int val);
    void respTopicUpdated(const QString& val);
    void addSubscribe();
    void addTopicAlias();

private:
    using SubConfig = Mqtt5ClientFilter::SubConfig;
    using TopicAliasConfig = Mqtt5ClientFilter::TopicAliasConfig;

    void refreshSessionExpiryInterval();

    void refreshSubscribes();
    void addSubscribeWidget(SubConfig& config);    

    void refreshTopicAliases();
    void addTopicAliasWidget(TopicAliasConfig& config);

    Mqtt5ClientFilter& m_filter;
    Ui::Mqtt5ClientFilterConfigWidget m_ui;
};

}  // namespace cc_plugin_mqtt5_client_filter


