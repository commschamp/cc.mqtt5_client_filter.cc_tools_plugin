//
// Copyright 2014 - 2024 (C). Alex Robenko. All rights reserved.
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

#include "Mqtt5ClientFilter.h"

#include <cassert>

namespace cc_plugin_mqtt5_client_filter
{

Mqtt5ClientFilter::Mqtt5ClientFilter() :
    m_client(::cc_mqtt5_client_alloc())
{
    // m_timer.setSingleShot(true);
    // connect(
    //     &m_timer, SIGNAL(timeout()),
    //     this, SLOT(sendDataPostponed()),
    //     Qt::QueuedConnection);
}

Mqtt5ClientFilter::~Mqtt5ClientFilter() noexcept = default;


}  // namespace cc_plugin_mqtt5_client_filter


