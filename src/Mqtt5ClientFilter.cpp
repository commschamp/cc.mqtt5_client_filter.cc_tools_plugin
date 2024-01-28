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

#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QList>

#include <cassert>
#include <cstdint>
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

const QString& topicProp()
{
    static const QString Str("mqtt5.topic");
    return Str;
}

const QString& qosProp()
{
    static const QString Str("mqtt5.qos");
    return Str;
}

const QString& retainedProp()
{
    static const QString Str("mqtt5.retained");
    return Str;    
}

const QString& contentTypeProp()
{
    static const QString Str("mqtt5.content_type");
    return Str;
}

const QString& correlationDataProp()
{
    static const QString Str("mqtt5.correlation_data");
    return Str;
}

const QString& formatProp()
{
    static const QString Str("mqtt5.format");
    return Str;
}

const QString& expiryIntervalProp()
{
    static const QString Str("mqtt5.expiry_interval");
    return Str;
}

const QString& responseTopicProp()
{
    static const QString Str("mqtt5.response_topic");
    return Str;
}

const QString& subIdsProp()
{
    static const QString Str("mqtt5.sub_ids");
    return Str;
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

    ec = ::cc_mqtt5_client_set_default_response_timeout(m_client.get(), m_config.m_respTimeout);
    if (ec != CC_Mqtt5ErrorCode_Success) {
        reportError(tr("Failed to update MQTT5 default response timeout"));
        return false;
    }    

    return true; 
}

void Mqtt5ClientFilter::stopImpl()
{
    if (!::cc_mqtt5_client_is_connected(m_client.get())) {
        return;
    }

    CC_Mqtt5DisconnectHandle disconnect = ::cc_mqtt5_client_disconnect_prepare(m_client.get(), nullptr);
    if (disconnect == nullptr) {
        reportError(tr("Failed to allocate DISCONNECT message in MQTT5 client"));
        return;
    }    

    auto config = CC_Mqtt5DisconnectConfig();
    ::cc_mqtt5_client_disconnect_init_config(&config);
    auto ec = ::cc_mqtt5_client_disconnect_config(disconnect, &config);
    if (ec != CC_Mqtt5ErrorCode_Success) {
        reportError(tr("Failed to configure MQTT5 disconnect with ec=") + QString::number(ec));
        return;
    }    

    ec = cc_mqtt5_client_disconnect_send(disconnect);
    if (ec != CC_Mqtt5ErrorCode_Success) {
        reportError(tr("Failed to send disconnect with ec=") + QString::number(ec));
        return;
    }    
}

QList<cc_tools_qt::DataInfoPtr> Mqtt5ClientFilter::recvDataImpl(cc_tools_qt::DataInfoPtr dataPtr)
{
    m_recvData.clear();
    m_recvDataPtr = std::move(dataPtr);
    m_inData.insert(m_inData.end(), m_recvDataPtr->m_data.begin(), m_recvDataPtr->m_data.end());
    auto consumed = ::cc_mqtt5_client_process_data(m_client.get(), m_inData.data(), static_cast<unsigned>(m_inData.size()));
    assert(consumed <= m_inData.size());
    m_inData.erase(m_inData.begin(), m_inData.begin() + consumed);
    m_recvDataPtr.reset();
    return std::move(m_recvData);
}

QList<cc_tools_qt::DataInfoPtr> Mqtt5ClientFilter::sendDataImpl(cc_tools_qt::DataInfoPtr dataPtr)
{
    m_sendData.clear();

    if (!m_socketConnected) {
        reportError(tr("Cannot send MQTT5 data when socket is not connected"));
        return m_sendData;
    }

    if (!::cc_mqtt5_client_is_connected(m_client.get())) {
        m_pendingData.push_back(std::move(dataPtr));
        return m_sendData;
    }

    std::string topic = m_config.m_pubTopic.toStdString();
    auto& props = dataPtr->m_extraProperties;
    if (props.contains(topicProp())) {
        topic = props[topicProp()].value<QString>().toStdString();
    }
    else {
        props[topicProp()] = QString::fromStdString(topic);
    }

    auto qos = m_config.m_pubQos;
    if (props.contains(qosProp())) {
        qos = props[qosProp()].value<int>();
    }
    else {
        props[qosProp()] = qos;
    }

    CC_Mqtt5ErrorCode ec = CC_Mqtt5ErrorCode_Success;
    CC_Mqtt5PublishHandle publish = ::cc_mqtt5_client_publish_prepare(m_client.get(), &ec);
    if (publish == NULL) {
        reportError(tr("Publish allocation failed with ec=") + QString::number(ec));
        return m_sendData;
    }

    auto basicConfig = CC_Mqtt5PublishBasicConfig();
    ::cc_mqtt5_client_publish_init_config_basic(&basicConfig);

    basicConfig.m_topic = topic.c_str();
    basicConfig.m_data = dataPtr->m_data.data();
    basicConfig.m_dataLen = static_cast<decltype(basicConfig.m_dataLen)>(dataPtr->m_data.size());
    basicConfig.m_qos = static_cast<decltype(basicConfig.m_qos)>(qos);    
    ec = ::cc_mqtt5_client_publish_config_basic(publish, &basicConfig);
    if (ec != CC_Mqtt5ErrorCode_Success) {
        reportError(tr("Failed to configure MQTT5 publish with ec=") + QString::number(ec));
        return m_sendData;
    }    

    // TODO: configure properties

    auto respTopic = m_config.m_respTopic.toStdString();
    bool hasExtra = 
        (!respTopic.empty());

    if (hasExtra) {
        auto extraConfig = CC_Mqtt5PublishExtraConfig();
        ::cc_mqtt5_client_publish_init_config_extra(&extraConfig);
        
        if (!respTopic.empty()) {
            extraConfig.m_responseTopic = respTopic.c_str();
        }

        ec = ::cc_mqtt5_client_publish_config_extra(publish, &extraConfig);
        if (ec != CC_Mqtt5ErrorCode_Success) {
            reportError(tr("Failed to configure extra properties for MQTT5 publish with ec=") + QString::number(ec));
        }           
    }

    m_sendDataPtr = std::move(dataPtr);

    ec = ::cc_mqtt5_client_publish_send(publish, &publishCompleteCb, this);
    if (ec != CC_Mqtt5ErrorCode_Success) {
        reportError(tr("Failed to send MQTT5 publish with ec=") + QString::number(ec));
        m_sendDataPtr.reset();
        return m_sendData;        
    }

    m_sendDataPtr.reset();
    return std::move(m_sendData);
}

void Mqtt5ClientFilter::socketConnectionReportImpl(bool connected)
{
    m_socketConnected = connected;
    if (connected) {
        socketConnected();
        return;
    }

    socketDisconnected();
}

void Mqtt5ClientFilter::doTick()
{
    assert(m_tickMeasureTs > 0);
    m_tickMeasureTs = 0;

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
    auto username = m_config.m_username.toStdString();
    auto password = m_config.m_username.toStdString();
    
    if (!clientId.empty()) {
        basicConfig.m_clientId = clientId.c_str();
    }

    basicConfig.m_cleanStart = 
        (m_config.m_forcedCleanStart) ||
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
        return;
    }

    m_prevClientId = clientId;
}

void Mqtt5ClientFilter::socketDisconnected()
{
    m_waitingForDisconnect = true;
    ::cc_mqtt5_client_notify_network_disconnected(m_client.get(), true);
}

void Mqtt5ClientFilter::sendPendingData()
{
    for (auto& dataPtr : m_pendingData) {
        sendDataImpl(std::move(dataPtr));
    }
    m_pendingData.clear();
}

void Mqtt5ClientFilter::registerTopicAliases()
{
    for (auto& info : m_config.m_topicAliases) {
        if (info.m_topic.isEmpty()) {
            continue;
        }

        auto topic = info.m_topic.toStdString();
        ::cc_mqtt5_client_pub_topic_alias_alloc(m_client.get(), topic.c_str(), static_cast<std::uint8_t>(info.m_qos0Rep));
    }
}

void Mqtt5ClientFilter::sendDataInternal(const unsigned char* buf, unsigned bufLen)
{
    auto dataInfo = cc_tools_qt::makeDataInfoTimed();
    dataInfo->m_data.assign(buf, buf + bufLen);
    if (!m_sendDataPtr) {
        reportDataToSend(std::move(dataInfo));
        return;
    }

    dataInfo->m_extraProperties = m_sendDataPtr->m_extraProperties;
    m_sendData.append(std::move(dataInfo));
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
    assert(m_recvDataPtr);
    auto dataInfo = cc_tools_qt::makeDataInfoTimed();
    if (info.m_dataLen > 0U) {
        dataInfo->m_data.assign(info.m_data, info.m_data + info.m_dataLen);
    }
    auto& props = dataInfo->m_extraProperties;
    props = m_recvDataPtr->m_extraProperties;
    assert(info.m_topic != nullptr);
    props[topicProp()] = info.m_topic;
    props[qosProp()] = static_cast<int>(info.m_qos);
    props[retainedProp()] = info.m_retained;

    if (info.m_contentType != nullptr) {
        props[contentTypeProp()] = info.m_contentType;
    }

    if (info.m_correlationDataLen > 0U) {
        assert(info.m_correlationData != nullptr);
        props[correlationDataProp()] = QByteArray(reinterpret_cast<const char*>(info.m_correlationData), static_cast<int>(info.m_correlationDataLen));
    }

    if (info.m_format != CC_Mqtt5PayloadFormat_Unspecified) {
        props[formatProp()] = static_cast<int>(info.m_format);
    }

    if (info.m_messageExpiryInterval != 0U) {
        props[expiryIntervalProp()] = static_cast<int>(info.m_messageExpiryInterval);
    }

    if (info.m_responseTopic != nullptr) {
        props[responseTopicProp()] = info.m_responseTopic;
    }

    if (info.m_subIdsCount > 0U) {
        assert(info.m_subIds != nullptr);
        props[subIdsProp()] = QVariant::fromValue(QList<int>(info.m_subIds, info.m_subIds + info.m_subIdsCount));
    }    

    // TODO: user props

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
    assert(m_tickMeasureTs > 0);
    assert(m_timer.isActive());
    m_timer.stop();
    auto now = QDateTime::currentMSecsSinceEpoch();
    assert(m_tickMeasureTs <= now);
    auto diff = now - m_tickMeasureTs;
    assert(diff < std::numeric_limits<unsigned>::max());
    m_tickMeasureTs = 0U;
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

    m_firstConnect = false;

    registerTopicAliases();
    sendPendingData();

    if (response->m_sessionPresent) {
        return;
    }

    CC_Mqtt5SubscribeHandle subscribe = ::cc_mqtt5_client_subscribe_prepare(m_client.get(), nullptr);
    if (subscribe == nullptr) {
        reportError(tr("Failed to allocate SUBSCRIBE message in MQTT5 client"));
        return;
    }    

    for (auto& sub : m_config.m_subscribes) {
        auto topicStr = sub.m_topic.trimmed().toStdString();

        auto topicConfig = CC_Mqtt5SubscribeTopicConfig();
        ::cc_mqtt5_client_subscribe_init_config_topic(&topicConfig);
        topicConfig.m_topic = topicStr.c_str();
        topicConfig.m_maxQos = static_cast<decltype(topicConfig.m_maxQos)>(sub.m_maxQos);
        topicConfig.m_retainHandling = static_cast<decltype(topicConfig.m_retainHandling)>(sub.m_retainHandling);
        topicConfig.m_noLocal = sub.m_noLocal;   
        topicConfig.m_retainAsPublished = sub.m_retainAsPublished;   

        auto ec = ::cc_mqtt5_client_subscribe_config_topic(subscribe, &topicConfig);
        if (ec != CC_Mqtt5ErrorCode_Success) {
            reportError(
                QString("%1 \"%2\", ec=%3").arg(tr("Failed to configure topic")).arg(sub.m_topic).arg(ec));
            continue;
        }  
    }

    auto ec = cc_mqtt5_client_subscribe_send(subscribe, &Mqtt5ClientFilter::subscribeCompleteCb, this);
    if (ec != CC_Mqtt5ErrorCode_Success) {
        reportError(tr("Failed to send MQTT5 SUBSCRIBE message"));
        return;
    }    
}

void Mqtt5ClientFilter::subscribeCompleteInternal([[maybe_unused]] CC_Mqtt5SubscribeHandle handle, CC_Mqtt5AsyncOpStatus status, const CC_Mqtt5SubscribeResponse* response)
{
    if (status != CC_Mqtt5AsyncOpStatus_Complete) {
        reportError(tr("Failed to subsribe to MQTT5 topics, status=") + QString::number(status));
        return;
    }  

    assert (response != nullptr);
    for (auto idx = 0U; idx < response->m_reasonCodesCount; ++idx) {
        if (response->m_reasonCodes[idx] < CC_Mqtt5ReasonCode_UnspecifiedError) {
            continue;
        }

        reportError(tr("MQTT broker rejected subscribe with reasonCode=") + QString::number(response->m_reasonCodes[idx]));
    }       
}

void Mqtt5ClientFilter::publishCompleteInternal([[maybe_unused]] CC_Mqtt5PublishHandle handle, CC_Mqtt5AsyncOpStatus status, const CC_Mqtt5PublishResponse* response)
{
    if (status != CC_Mqtt5AsyncOpStatus_Complete) {
        reportError(tr("Failed to publish to MQTT5 broker, status=") + QString::number(status));
        return;
    }

    if (response == nullptr) {
        return;
    }

    if (CC_Mqtt5ReasonCode_UnspecifiedError <= response->m_reasonCode) {
        reportError(tr("MQTT broker rejected publish with reasonCode=") + QString::number(response->m_reasonCode));
        return;        
    }    
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

void Mqtt5ClientFilter::subscribeCompleteCb(void* data, CC_Mqtt5SubscribeHandle handle, CC_Mqtt5AsyncOpStatus status, const CC_Mqtt5SubscribeResponse* response)
{
    asThis(data)->subscribeCompleteInternal(handle, status, response);
}

void Mqtt5ClientFilter::publishCompleteCb(void* data, CC_Mqtt5PublishHandle handle, CC_Mqtt5AsyncOpStatus status, const CC_Mqtt5PublishResponse* response)
{
    asThis(data)->publishCompleteInternal(handle, status, response);
}

}  // namespace cc_plugin_mqtt5_client_filter


