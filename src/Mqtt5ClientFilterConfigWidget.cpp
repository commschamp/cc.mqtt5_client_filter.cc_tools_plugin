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

#include "Mqtt5ClientFilterConfigWidget.h"

#include <cassert>

namespace cc_plugin_mqtt5_client_filter
{

Mqtt5ClientFilterConfigWidget::Mqtt5ClientFilterConfigWidget(Mqtt5ClientFilter& filter, QWidget* parentObj) :
    Base(parentObj),
    m_filter(filter)
{
    m_ui.setupUi(this);

    m_ui.m_respTimeoutSpinBox->setValue(m_filter.config().m_respTimeout);
    m_ui.m_clientIdLineEdit->setText(m_filter.config().m_clientId);
    m_ui.m_usernameLineEdit->setText(m_filter.config().m_username);
    m_ui.m_passwordLineEdit->setText(m_filter.config().m_password);
    m_ui.m_cleanStartComboBox->setCurrentIndex(static_cast<int>(m_filter.config().m_forcedCleanStart));
    m_ui.m_subTopicsLineEdit->setText(m_filter.config().m_subTopics);
    m_ui.m_subQosSpinBox->setValue(m_filter.config().m_subQos);
    m_ui.m_pubTopicLineEdit->setText(m_filter.config().m_pubTopic);
    m_ui.m_pubQosSpinBox->setValue(m_filter.config().m_pubQos);
    m_ui.m_respTopicLineEdit->setText(m_filter.config().m_respTopic);

    connect(
        m_ui.m_respTimeoutSpinBox, qOverload<int>(&QSpinBox::valueChanged),
        this, &Mqtt5ClientFilterConfigWidget::respTimeoutUpdated);    

    connect(
        m_ui.m_clientIdLineEdit, &QLineEdit::textChanged,
        this, &Mqtt5ClientFilterConfigWidget::clientIdUpdated);

    connect(
        m_ui.m_usernameLineEdit, &QLineEdit::textChanged,
        this, &Mqtt5ClientFilterConfigWidget::usernameUpdated);        

    connect(
        m_ui.m_passwordLineEdit, &QLineEdit::textChanged,
        this, &Mqtt5ClientFilterConfigWidget::passwordUpdated);        

    connect(
        m_ui.m_cleanStartComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
        this, &Mqtt5ClientFilterConfigWidget::forcedCleanStartUpdated);           

    connect(
        m_ui.m_subTopicsLineEdit, &QLineEdit::textChanged,
        this, &Mqtt5ClientFilterConfigWidget::subTopicsUpdated);        

    connect(
        m_ui.m_subQosSpinBox, qOverload<int>(&QSpinBox::valueChanged),
        this, &Mqtt5ClientFilterConfigWidget::subQosUpdated);           

    connect(
        m_ui.m_pubTopicLineEdit, &QLineEdit::textChanged,
        this, &Mqtt5ClientFilterConfigWidget::pubTopicUpdated);        

    connect(
        m_ui.m_pubQosSpinBox, qOverload<int>(&QSpinBox::valueChanged),
        this, &Mqtt5ClientFilterConfigWidget::pubQosUpdated);   

    connect(
        m_ui.m_respTopicLineEdit, &QLineEdit::textChanged,
        this, &Mqtt5ClientFilterConfigWidget::respTopicUpdated);                 
}

Mqtt5ClientFilterConfigWidget::~Mqtt5ClientFilterConfigWidget() noexcept = default;

void Mqtt5ClientFilterConfigWidget::respTimeoutUpdated(int val)
{
    m_filter.config().m_respTimeout = static_cast<unsigned>(val);
}

void Mqtt5ClientFilterConfigWidget::clientIdUpdated(const QString& val)
{
    m_filter.config().m_clientId = val;
    m_filter.forceCleanStart();
}

void Mqtt5ClientFilterConfigWidget::usernameUpdated(const QString& val)
{
    m_filter.config().m_username = val;
}

void Mqtt5ClientFilterConfigWidget::passwordUpdated(const QString& val)
{
    m_filter.config().m_password = val;
}

void Mqtt5ClientFilterConfigWidget::forcedCleanStartUpdated(int val)
{
    m_filter.config().m_forcedCleanStart = (val > 0);
}

void Mqtt5ClientFilterConfigWidget::subTopicsUpdated(const QString& val)
{
    m_filter.config().m_subTopics = val;
    m_filter.forceCleanStart();
}

void Mqtt5ClientFilterConfigWidget::subQosUpdated(int val)
{
    m_filter.config().m_subQos = static_cast<unsigned>(val);
    m_filter.forceCleanStart();
}

void Mqtt5ClientFilterConfigWidget::pubTopicUpdated(const QString& val)
{
    m_filter.config().m_pubTopic = val;
}

void Mqtt5ClientFilterConfigWidget::pubQosUpdated(int val)
{
    m_filter.config().m_pubQos = static_cast<unsigned>(val);
}

void Mqtt5ClientFilterConfigWidget::respTopicUpdated(const QString& val)
{
    m_filter.config().m_respTopic = val;
}

}  // namespace cc_plugin_mqtt5_client_filter


