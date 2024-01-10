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

    m_ui.m_clientIdLineEdit->setText(m_filter.getClientId());

    connect(
        m_ui.m_clientIdLineEdit, &QLineEdit::textChanged,
        this, &Mqtt5ClientFilterConfigWidget::clientIdUpdated);
}

Mqtt5ClientFilterConfigWidget::~Mqtt5ClientFilterConfigWidget() noexcept = default;

void Mqtt5ClientFilterConfigWidget::clientIdUpdated(const QString& val)
{
    m_filter.setClientId(val);
}


}  // namespace cc_plugin_mqtt5_client_filter


