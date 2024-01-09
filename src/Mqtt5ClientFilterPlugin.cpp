//
// Copyright 2015 - 2024 (C). Alex Robenko. All rights reserved.
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

#include "Mqtt5ClientFilterPlugin.h"

#include <memory>
#include <cassert>

#include "Mqtt5ClientFilter.h"

namespace cc_plugin_mqtt5_client_filter
{

Mqtt5ClientFilterPlugin::Mqtt5ClientFilterPlugin()
{
    pluginProperties()
        .setFiltersCreateFunc(
            [this]()
            {
                createFilterIfNeeded();
                cc_tools_qt::PluginProperties::ListOfFilters result;
                result.append(m_filter);
                return result;
            });
}

Mqtt5ClientFilterPlugin::~Mqtt5ClientFilterPlugin() noexcept = default;

void Mqtt5ClientFilterPlugin::createFilterIfNeeded()
{
    if (m_filter) {
        return;
    }

    m_filter = makeMqtt5ClientFilter();
}

}  // namespace cc_plugin_mqtt5_client_filter


