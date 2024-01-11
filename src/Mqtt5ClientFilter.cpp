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

#include <QtCore/QDateTime>

#include <cassert>
#include <limits>
#include <iostream>
#include <string>

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
    m_timer.setSingleShot(true);
    connect(
        &m_timer, &QTimer::timeout,
        this, &Mqtt5ClientFilter::doTick);

    ::cc_mqtt5_client_set_send_output_data_callback(m_client.get(), &Mqtt5ClientFilter::sendDataCb, this);
    ::cc_mqtt5_client_set_broker_disconnect_report_callback(m_client.get(), &Mqtt5ClientFilter::brokerDisconnectedCb, this);
    ::cc_mqtt5_client_set_message_received_report_callback(m_client.get(), &Mqtt5ClientFilter::messageReceivedCb, this);
    ::cc_mqtt5_client_set_next_tick_program_callback(m_client.get(), &Mqtt5ClientFilter::nextTickProgramCb, this);
    ::cc_mqtt5_client_set_cancel_next_tick_wait_callback(m_client.get(), &Mqtt5ClientFilter::cancelTickProgramCb, this);
    ::cc_mqtt5_client_set_error_log_callback(m_client.get(), &Mqtt5ClientFilter::errorLogCb, nullptr);

    m_config.m_respTimeout = ::cc_mqtt5_client_get_default_response_timeout(m_client.get());
}

Mqtt5ClientFilter::~Mqtt5ClientFilter() noexcept = default;

bool Mqtt5ClientFilter::startImpl()
{
    auto ec = cc_mqtt5_client_init(m_client.get());
    if (ec != CC_Mqtt5ErrorCode_Success) {
        reportError(tr("Failed to initialize MQTT5 client"));
        return false;
    }

    return true; 
}

void Mqtt5ClientFilter::stopImpl()
{
    // ???
}

QList<cc_tools_qt::DataInfoPtr> Mqtt5ClientFilter::recvDataImpl(cc_tools_qt::DataInfoPtr dataPtr)
{
    m_recvDataPtr = std::move(dataPtr);
    m_inData.insert(m_inData.end(), m_recvDataPtr->m_data.begin(), m_recvDataPtr->m_data.end());
    auto consumed = ::cc_mqtt5_client_process_data(m_client.get(), m_inData.data(), static_cast<unsigned>(m_inData.size()));
    assert(consumed <= m_inData.size());
    m_inData.erase(m_inData.begin(), m_inData.begin() + consumed);
    m_recvDataPtr.reset();

    return QList<cc_tools_qt::DataInfoPtr>(); // TODO   
}

QList<cc_tools_qt::DataInfoPtr> Mqtt5ClientFilter::sendDataImpl(cc_tools_qt::DataInfoPtr dataPtr)
{
    m_sendDataPtr = std::move(dataPtr);

    return QList<cc_tools_qt::DataInfoPtr>(); // TODO
}

void Mqtt5ClientFilter::socketConnectionReportImpl(bool connected)
{
    if (connected) {
        socketConnected();
        return;
    }

    socketDisconnected();
}

void Mqtt5ClientFilter::doTick()
{
    assert(m_client);
    if (!m_client) {
        return;
    }

    ::cc_mqtt5_client_tick(m_client.get(), m_tickMs);
}

void Mqtt5ClientFilter::socketConnected()
{
    auto ec = cc_mqtt5_client_init(m_client.get());
    if (ec != CC_Mqtt5ErrorCode_Success) {
        reportError(tr("Failed to (re)initialize MQTT5 client"));
        return;
    }

    CC_Mqtt5ConnectHandle connect = cc_mqtt5_client_connect_prepare(m_client.get(), nullptr);
    if (connect == nullptr) {
        reportError(tr("Failed to allocate CONNECT message in MQTT5 client"));
        return;
    }    

    auto basicConfig = CC_Mqtt5ConnectBasicConfig();
    ::cc_mqtt5_client_connect_init_config_basic(&basicConfig);

    auto clientId = m_config.m_clientId.toStdString();
    
    if (!clientId.empty()) {
        basicConfig.m_clientId = clientId.c_str();
    }

    basicConfig.m_cleanStart = 
        (clientId.empty()) || 
        (clientId != m_prevClientId) ||
        (m_firstConnect);


    ec = ::cc_mqtt5_client_connect_config_basic(connect, &basicConfig);
    if (ec != CC_Mqtt5ErrorCode_Success) {
        reportError(tr("Failed to configure CONNECT message in MQTT5 client"));
        cc_mqtt5_client_connect_cancel(connect);
        return;
    }    

    ec = cc_mqtt5_client_connect_send(connect, &Mqtt5ClientFilter::connectCompleteCb, this);
    if (ec != CC_Mqtt5ErrorCode_Success) {
        reportError(tr("Failed to send MQTT5 CONNECT message"));
        cc_mqtt5_client_connect_cancel(connect);
        return;
    }
}

void Mqtt5ClientFilter::socketDisconnected()
{
    // TODO:
    assert(false);
}

void Mqtt5ClientFilter::sendDataInternal(const unsigned char* buf, unsigned bufLen)
{
    auto dataInfo = cc_tools_qt::makeDataInfoTimed();
    dataInfo->m_data.assign(buf, buf + bufLen);
    // TODO: properties for publish
    reportDataToSend(std::move(dataInfo));
}

void Mqtt5ClientFilter::brokerDisconnectedInternal()
{
    if (m_waitingForDisconnect) {
        return;
    }

    static const QString BrokerDisconnecteError = 
        tr("MQTT5 Broker is disconnected");

    reportError(BrokerDisconnecteError);
}

void Mqtt5ClientFilter::messageReceivedInternal(const CC_Mqtt5MessageInfo& info)
{
    static_cast<void>(info); // TODO;
    assert(m_recvDataPtr);
    auto dataInfo = cc_tools_qt::makeDataInfoTimed();
    // TODO
    assert(false);
    dataInfo->m_extraProperties = m_recvDataPtr->m_extraProperties;
    m_recvData.append(std::move(dataInfo));
}

void Mqtt5ClientFilter::nextTickProgramInternal(unsigned ms)
{
    assert(!m_timer.isActive());
    m_tickMs = ms;
    m_tickMeasureTs = QDateTime::currentMSecsSinceEpoch();
    m_timer.start(static_cast<int>(ms));
}

unsigned Mqtt5ClientFilter::cancelTickProgramInternal()
{
    assert(m_timer.isActive());
    m_timer.stop();
    auto now = QDateTime::currentMSecsSinceEpoch();
    assert(m_tickMeasureTs < now);
    auto diff = now - m_tickMeasureTs;
    assert(diff < std::numeric_limits<unsigned>::max());
    return static_cast<unsigned>(diff);
}

void Mqtt5ClientFilter::connectCompleteInternal(CC_Mqtt5AsyncOpStatus status, const CC_Mqtt5ConnectResponse* response)
{
    if (status != CC_Mqtt5AsyncOpStatus_Complete) {
        reportError(tr("Failed to connect to MQTT5 broker, status=") + QString::number(status));
        return;
    }

    assert(response != nullptr);
    if (response->m_reasonCode != CC_Mqtt5ReasonCode_Success) {
        reportError(tr("MQTT broker rejected connection with reasonCode=") + QString::number(response->m_reasonCode));
        return;        
    }

    std::cout << "!!! CONNECTED" << std::endl;
    // TODO: subscribe
}

void Mqtt5ClientFilter::sendDataCb(void* data, const unsigned char* buf, unsigned bufLen)
{
    asThis(data)->sendDataInternal(buf, bufLen);
}

void Mqtt5ClientFilter::brokerDisconnectedCb(void* data, [[maybe_unused]] const CC_Mqtt5DisconnectInfo* info)
{
    asThis(data)->brokerDisconnectedInternal();
}

void Mqtt5ClientFilter::messageReceivedCb(void* data, const CC_Mqtt5MessageInfo* info)
{
    assert(info != nullptr);
    if (info == nullptr) {
        return;
    }

    asThis(data)->messageReceivedInternal(*info);
}

void Mqtt5ClientFilter::nextTickProgramCb(void* data, unsigned ms)
{
    asThis(data)->nextTickProgramInternal(ms);
}

unsigned Mqtt5ClientFilter::cancelTickProgramCb(void* data)
{
    return asThis(data)->cancelTickProgramInternal();
}

void Mqtt5ClientFilter::errorLogCb([[maybe_unused]] void* data, const char* msg)
{
    std::cerr << "MQTT5 ERROR: " << msg << std::endl;
}

void Mqtt5ClientFilter::connectCompleteCb(void* data, CC_Mqtt5AsyncOpStatus status, const CC_Mqtt5ConnectResponse* response)
{
    asThis(data)->connectCompleteInternal(status, response);
}

}  // namespace cc_plugin_mqtt5_client_filter


