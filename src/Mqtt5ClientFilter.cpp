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

#include "Mqtt5ClientFilter.h"

#include <cassert>

namespace cc_plugin_mqtt5_client_filter
{

namespace 
{

inline Mqtt5ClientFilter* asThis(void* data)
{
    return reinterpret_cast<Mqtt5ClientFilter*>(data);
}

} // namespace 
    

Mqtt5ClientFilter::Mqtt5ClientFilter() :
    m_client(::cc_mqtt5_client_alloc())
{
    // m_timer.setSingleShot(true);
    // connect(
    //     &m_timer, SIGNAL(timeout()),
    //     this, SLOT(sendDataPostponed()),
    //     Qt::QueuedConnection);

    ::cc_mqtt5_client_set_send_output_data_callback(m_client.get(), &Mqtt5ClientFilter::sendDataCb, this);
}

Mqtt5ClientFilter::~Mqtt5ClientFilter() noexcept = default;

bool Mqtt5ClientFilter::startImpl()
{
    return true; // TODO
}

void Mqtt5ClientFilter::socketConnectionReportImpl(bool connected)
{
    static_cast<void>(connected); // TODO
}

void Mqtt5ClientFilter::sendDataInternal(const unsigned char* buf, unsigned bufLen)
{
    auto dataInfo = cc_tools_qt::makeDataInfoTimed();
    dataInfo->m_data.assign(buf, buf + bufLen);
    // TODO: properties for publish
    reportDataToSend(std::move(dataInfo));
}

void Mqtt5ClientFilter::sendDataCb(void* data, const unsigned char* buf, unsigned bufLen)
{
    asThis(data)->sendDataInternal(buf, bufLen);
}

}  // namespace cc_plugin_mqtt5_client_filter


