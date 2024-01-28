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


#include "Mqtt5ClientFilterSubConfigWidget.h"

#include <algorithm>

namespace cc_plugin_mqtt5_client_filter
{

Mqtt5ClientFilterSubConfigWidget::Mqtt5ClientFilterSubConfigWidget(Mqtt5ClientFilter& filter, SubConfig& config, QWidget* parentObj) : 
    Base(parentObj),
    m_filter(filter),
    m_config(config)
{
    m_ui.setupUi(this);

    m_ui.m_topicLineEdit->setText(m_config.m_topic);
    m_ui.m_maxQosSpinBox->setValue(m_config.m_maxQos);
    m_ui.m_noLocalComboBox->setCurrentIndex(static_cast<int>(m_config.m_noLocal));
    m_ui.m_retainAsPublishedComboBox->setCurrentIndex(static_cast<int>(m_config.m_retainAsPublished));
    m_ui.m_retainHandlingComboBox->setCurrentIndex(m_config.m_retainHandling);

    connect(
        m_ui.m_topicLineEdit, &QLineEdit::textChanged,
        this, &Mqtt5ClientFilterSubConfigWidget::topicUpdated);   

    connect(
        m_ui.m_maxQosSpinBox, qOverload<int>(&QSpinBox::valueChanged),
        this, &Mqtt5ClientFilterSubConfigWidget::maxQosUpdated);  

    connect(
        m_ui.m_noLocalComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
        this, &Mqtt5ClientFilterSubConfigWidget::noLocalUpdated);   

    connect(
        m_ui.m_retainAsPublishedComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
        this, &Mqtt5ClientFilterSubConfigWidget::retainAsPublishedUpdated);    

    connect(
        m_ui.m_retainHandlingComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
        this, &Mqtt5ClientFilterSubConfigWidget::retainHandlingUpdated);                            

    connect(
        m_ui.m_delToolButton, &QToolButton::clicked,
        this, &Mqtt5ClientFilterSubConfigWidget::delClicked);           
}

void Mqtt5ClientFilterSubConfigWidget::topicUpdated(const QString& val)
{
    m_config.m_topic = val;
    m_filter.forceCleanStart();
}

void Mqtt5ClientFilterSubConfigWidget::maxQosUpdated(int val)
{
    m_config.m_maxQos = val;
    m_filter.forceCleanStart();
}

void Mqtt5ClientFilterSubConfigWidget::noLocalUpdated(int val)
{
    m_config.m_noLocal = val > 0;
    m_filter.forceCleanStart();
}

void Mqtt5ClientFilterSubConfigWidget::retainAsPublishedUpdated(int val)
{
    m_config.m_retainAsPublished = val > 0;
    m_filter.forceCleanStart();
}

void Mqtt5ClientFilterSubConfigWidget::retainHandlingUpdated(int val)
{
    m_config.m_retainHandling = val;
    m_filter.forceCleanStart();
}

void Mqtt5ClientFilterSubConfigWidget::delClicked([[maybe_unused]] bool checked)
{
    auto& subs = m_filter.config().m_subscribes;
    auto iter = 
        std::find_if(
            subs.begin(), subs.end(), 
            [this](auto& info)
            {
                return &m_config == &info;
            });

    if (iter == subs.end()) {
        assert(false); // should not happen
        return;
    }

    subs.erase(iter);
    m_filter.forceCleanStart();
    blockSignals(true);
    deleteLater();
}


}  // namespace cc_plugin_mqtt5_client_filter


