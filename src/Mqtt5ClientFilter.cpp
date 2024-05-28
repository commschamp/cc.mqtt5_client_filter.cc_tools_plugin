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
#include <QtCore/QVariant>

#include <cassert>
#include <chrono>
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

const QString& aliasTopicProp()
{
    static const QString Str("mqtt.topic");
    return Str;
}

const QString& qosProp()
{
    static const QString Str("mqtt5.qos");
    return Str;
}

const QString& aliasQosProp()
{
    static const QString Str("mqtt.qos");
    return Str;
}

const QString& retainedProp()
{
    static const QString Str("mqtt5.retained");
    return Str;    
}

const QString& aliasRetainedProp()
{
    static const QString Str("mqtt.retained");
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

const QString& userPropsProp()
{
    static const QString Str("mqtt5.user_props");
    return Str;
}

const QString& clientProp()
{
    static const QString Str("mqtt5.client");
    return Str;    
}

const QString& aliasClientProp()
{
    static const QString Str("mqtt.client");
    return Str;    
}

const QString& usernameProp()
{
    static const QString Str("mqtt5.username");
    return Str;    
}

const QString& aliasUsernameProp()
{
    static const QString Str("mqtt.username");
    return Str;    
}

const QString& passwordProp()
{
    static const QString Str("mqtt5.password");
    return Str;    
}

const QString& aliasPasswordProp()
{
    static const QString Str("mqtt.password");
    return Str;    
}

const QString& pubTopicProp()
{
    static const QString Str("mqtt5.pub_topic");
    return Str;    
}

const QString& aliasPubTopicProp()
{
    static const QString Str("mqtt.pub_topic");
    return Str;    
}

const QString& pubQosProp()
{
    static const QString Str("mqtt5.pub_qos");
    return Str;    
}

const QString& aliasPubQosProp()
{
    static const QString Str("mqtt.pub_qos");
    return Str;    
}

const QString& respTopicProp()
{
    static const QString Str("mqtt5.resp_topic");
    return Str;    
}

const QString& subscribesProp()
{
    static const QString Str("mqtt5.subscribes");
    return Str;    
}

const QString& aliasSubscribesProp()
{
    static const QString Str("mqtt.subscribes");
    return Str;    
}

const QString& subscribesRemoveProp()
{
    static const QString Str("mqtt5.subscribes_remove");
    return Str;    
}

const QString& aliasSubscribesRemoveProp()
{
    static const QString Str("mqtt.subscribes_remove");
    return Str;    
}

const QString& subscribesClearProp()
{
    static const QString Str("mqtt5.subscribes_clear");
    return Str;    
}

const QString& aliasSubscribesClearProp()
{
    static const QString Str("mqtt.subscribes_clear");
    return Str;    
}

const QString& keySubProp()
{
    static const QString Str("key");
    return Str;
}

const QString& valueSubProp()
{
    static const QString Str("value");
    return Str;
}

const QString& topicSubProp()
{
    static const QString Str("topic");
    return Str;
}

const QString& qosSubProp()
{
    static const QString Str("qos");
    return Str;
}

const QString& noLocalSubProp()
{
    static const QString Str("no_local");
    return Str;
}

const QString& retainAsPublishedSubProp()
{
    static const QString Str("retain_as_published");
    return Str;
}


const QString& retainHandlingSubProp()
{
    static const QString Str("retain_handling");
    return Str;
}

QVariantMap toVariantMap(const CC_Mqtt5UserProp& prop)
{
    QVariantMap map;
    map[keySubProp()] = QString(prop.m_key);
    map[valueSubProp()] = QString(prop.m_value);
    return map;
}

QByteArray parseBinDataStr(const QString& str)
{
    QByteArray result;
    for (auto idx = 0; idx < str.size(); ++idx) {
        auto byteStr = str.mid(idx, 2);
        result.append(static_cast<char>(byteStr.toUInt(nullptr, 16)));
    }

    return result;
}

std::string getOutgoingTopic(const QVariantMap& props, const QString configVal)
{
    if (props.contains(topicProp())) {
        return props[topicProp()].value<QString>().toStdString();
    }

    if (props.contains(aliasTopicProp())) {
        return props[aliasTopicProp()].value<QString>().toStdString();
    }

    return configVal.toStdString();
}

int getOutgoingQos(const QVariantMap& props, int configVal)
{
    if (props.contains(qosProp())) {
        return props[qosProp()].value<int>();
    }

    if (props.contains(aliasQosProp())) {
        return props[aliasQosProp()].value<int>();
    }

    return configVal;
}

bool getOutgoingRetained(const QVariantMap& props)
{
    if (props.contains(retainedProp())) {
        return props[retainedProp()].value<bool>();
    }

    if (props.contains(aliasRetainedProp())) {
        return props[aliasRetainedProp()].value<bool>();
    }    

    return false;
}

const QString& errorCodeStr(CC_Mqtt5ErrorCode ec)
{
    static const QString Map[] = {
        /* CC_Mqtt5ErrorCode_Success */ "Success",
        /* CC_Mqtt5ErrorCode_InternalError */ "Internal Error",
        /* CC_Mqtt5ErrorCode_NotIntitialized */ "Not Initialized",
        /* CC_Mqtt5ErrorCode_Busy */ "Busy",
        /* CC_Mqtt5ErrorCode_NotConnected */ "Not Connected",
        /* CC_Mqtt5ErrorCode_AlreadyConnected */ "Already Connected",
        /* CC_Mqtt5ErrorCode_BadParam */ "Bad Parameter",
        /* CC_Mqtt5ErrorCode_InsufficientConfig */ "Insufficient Config",
        /* CC_Mqtt5ErrorCode_OutOfMemory */ "Out of Memory",
        /* CC_Mqtt5ErrorCode_BufferOverflow */ "Buffer Overflow",
        /* CC_Mqtt5ErrorCode_NotSupported */ "Feature is Not Supported",
        /* CC_Mqtt5ErrorCode_RetryLater */ "Retry later",
        /* CC_Mqtt5ErrorCode_Terminating */ "Terminating",
        /* CC_Mqtt5ErrorCode_NetworkDisconnected */ "Network is Disconnected",
        /* CC_Mqtt5ErrorCode_NotAuthenticated */ "Not Authenticated",
        /* CC_Mqtt5ErrorCode_PreparationLocked */ "Preparation Locked",
    };
    static const std::size_t MapSize = std::extent<decltype(Map)>::value;
    static_assert(MapSize == CC_Mqtt5ErrorCode_ValuesLimit);

    auto idx = static_cast<unsigned>(ec);
    if (MapSize <= idx) {
        static const QString UnknownStr("Unknown");
        return UnknownStr;
    }

    return Map[idx];
}

const QString& statusStr(CC_Mqtt5AsyncOpStatus status)
{
    static const QString Map[] = {
        /* CC_Mqtt5AsyncOpStatus_Complete */ "Complete",
        /* CC_Mqtt5AsyncOpStatus_InternalError */ "Internal Error",
        /* CC_Mqtt5AsyncOpStatus_Timeout */ "Timeout",
        /* CC_Mqtt5AsyncOpStatus_ProtocolError */ "Protocol Error",
        /* CC_Mqtt5AsyncOpStatus_Aborted */ "Aborted",
        /* CC_Mqtt5AsyncOpStatus_BrokerDisconnected */ "Broker Disconnected",
        /* CC_Mqtt5AsyncOpStatus_OutOfMemory */ "Out of Memory",
        /* CC_Mqtt5AsyncOpStatus_BadParam */ "Bad Parameter",
    };
    static const std::size_t MapSize = std::extent<decltype(Map)>::value;
    static_assert(MapSize == CC_Mqtt5AsyncOpStatus_ValuesLimit);

    auto idx = static_cast<unsigned>(status);
    if (MapSize <= idx) {
        static const QString UnknownStr("Unknown");
        return UnknownStr;
    }

    return Map[idx];
}

std::vector<std::uint8_t> parsePassword(const QString& password)
{
    std::vector<std::uint8_t> result;
    result.reserve(password.size());

    for (auto idx = 0; idx < password.size();) {
        if (((idx + 1) < password.size()) && (password[idx] == '\\') && (password[idx + 1] == '\\')) {
            result.push_back(static_cast<std::uint8_t>('\''));
            idx += 2;
            continue;
        }

        if ((password.size() <= (idx + 4)) || 
            (password[idx] != '\\') || 
            (password[idx + 1] != 'x')) {
            result.push_back(static_cast<std::uint8_t>(password[idx].cell()));
            idx += 1;
            continue;
        }

        result.push_back(static_cast<std::uint8_t>(password.mid(idx + 2, 2).toUInt(nullptr, 16)));
        idx += 4;
    }

    return result;
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
    auto ec = ::cc_mqtt5_client_set_default_response_timeout(m_client.get(), m_config.m_respTimeout);
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
        reportError(tr("Failed to configure MQTT5 disconnect with error: ") + errorCodeStr(ec));
        return;
    }    

    ec = cc_mqtt5_client_disconnect_send(disconnect);
    if (ec != CC_Mqtt5ErrorCode_Success) {
        reportError(tr("Failed to send disconnect with error: ") + errorCodeStr(ec));
        return;
    }    
}

QList<cc_tools_qt::DataInfoPtr> Mqtt5ClientFilter::recvDataImpl(cc_tools_qt::DataInfoPtr dataPtr)
{
    m_recvData.clear();
    m_recvDataPtr = std::move(dataPtr);
    m_inData.insert(m_inData.end(), m_recvDataPtr->m_data.begin(), m_recvDataPtr->m_data.end());
    auto consumed = ::cc_mqtt5_client_process_data(m_client.get(), m_inData.data(), static_cast<unsigned>(m_inData.size()));
    if (3 <= getDebugOutputLevel()) {
        std::cout << '[' << currTimestamp() << "] (" << debugNameImpl() << "): consumed bytes: " << consumed << "/" << m_inData.size() << std::endl;
    }    
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

    auto& props = dataPtr->m_extraProperties;
    std::string topic = getOutgoingTopic(props, m_config.m_pubTopic);
    props[topicProp()] = QString::fromStdString(topic);
    
    auto qos = getOutgoingQos(props, m_config.m_pubQos);
    props[qosProp()] = qos;

    auto retained = getOutgoingRetained(props);
    props[retainedProp()] = retained;

    if (2 <= getDebugOutputLevel()) {
        std::cout << '[' << currTimestamp() << "] (" << debugNameImpl() << "): publish: " << topic << std::endl;
    }     

    CC_Mqtt5ErrorCode ec = CC_Mqtt5ErrorCode_Success;
    CC_Mqtt5PublishHandle publish = ::cc_mqtt5_client_publish_prepare(m_client.get(), &ec);
    if (publish == NULL) {
        reportError(tr("Publish allocation failed with error: ") + errorCodeStr(ec));
        return m_sendData;
    }

    auto basicConfig = CC_Mqtt5PublishBasicConfig();
    ::cc_mqtt5_client_publish_init_config_basic(&basicConfig);

    basicConfig.m_topic = topic.c_str();
    basicConfig.m_data = dataPtr->m_data.data();
    basicConfig.m_dataLen = static_cast<decltype(basicConfig.m_dataLen)>(dataPtr->m_data.size());
    basicConfig.m_qos = static_cast<decltype(basicConfig.m_qos)>(qos);    
    basicConfig.m_retain = retained;
    ec = ::cc_mqtt5_client_publish_config_basic(publish, &basicConfig);
    if (ec != CC_Mqtt5ErrorCode_Success) {
        reportError(tr("Failed to configure MQTT5 publish with error: ") + errorCodeStr(ec));
        return m_sendData;
    }    

    auto respTopic = m_config.m_respTopic.toStdString();
    if (props.contains(responseTopicProp())) {
        respTopic = props.value(responseTopicProp()).toString().toStdString();
    }

    auto contentType = props.value(contentTypeProp()).toString().toStdString();

    auto correlationData = parseBinDataStr(props.value(correlationDataProp()).toString());

    bool hasFormat = props.contains(formatProp());
    bool hasExpiryInterval = props.contains(expiryIntervalProp());

    bool hasExtra = 
        (!respTopic.empty()) || 
        (!contentType.empty()) ||
        (!correlationData.isEmpty()) ||
        (hasFormat) || 
        (hasExpiryInterval);

    if (hasExtra) {
        auto extraConfig = CC_Mqtt5PublishExtraConfig();
        ::cc_mqtt5_client_publish_init_config_extra(&extraConfig);
        
        if (!respTopic.empty()) {
            extraConfig.m_responseTopic = respTopic.c_str();
        }

        if (!contentType.empty()) {
            extraConfig.m_contentType = contentType.c_str();
        }

        if (!correlationData.isEmpty()) {
            extraConfig.m_correlationData = reinterpret_cast<const std::uint8_t*>(correlationData.constData());
            extraConfig.m_correlationDataLen = static_cast<decltype(extraConfig.m_correlationDataLen)>(correlationData.size());
        }

        if (hasFormat) {
            extraConfig.m_format = static_cast<decltype(extraConfig.m_format)>(props.value(formatProp()).toUInt());
        }

        if (hasExpiryInterval) {
            extraConfig.m_messageExpiryInterval = props.value(formatProp()).toUInt();
        }        

        ec = ::cc_mqtt5_client_publish_config_extra(publish, &extraConfig);
        if (ec != CC_Mqtt5ErrorCode_Success) {
            reportError(tr("Failed to configure extra properties for MQTT5 publish with error: ") + errorCodeStr(ec));
        }           
    }

    auto userPropsVar = props.value(userPropsProp());
    if (userPropsVar.isValid()) {
        auto userProps = userPropsVar.value<QVariantList>();
        for (auto& propMapVar : userProps) {
            auto map = propMapVar.value<QVariantMap>();
            auto keyVar = map.value(keySubProp());
            auto valueVar = map.value(valueSubProp());

            if ((!keyVar.isValid()) || (!valueVar.isValid())) {
                reportError("Invalid user property configuration in message extra properties, ignoring");
                continue;
            }

            auto key = keyVar.value<QString>().toStdString();
            auto value = valueVar.value<QString>().toStdString();

            CC_Mqtt5UserProp prop;
            prop.m_key = key.c_str();
            prop.m_value = value.c_str();

            ec = ::cc_mqtt5_client_publish_add_user_prop(publish, &prop);
            if (ec != CC_Mqtt5ErrorCode_Success) {
                reportError(tr("Failed to add publish user property with error: ") + errorCodeStr(ec));
                continue;
            }            
        }
    }

    m_sendDataPtr = std::move(dataPtr);

    ec = ::cc_mqtt5_client_publish_send(publish, &publishCompleteCb, this);
    if (ec != CC_Mqtt5ErrorCode_Success) {
        reportError(tr("Failed to send MQTT5 publish with error: ") + errorCodeStr(ec));
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

void Mqtt5ClientFilter::applyInterPluginConfigImpl(const QVariantMap& props)
{
    bool updated = false;

    {
        static const QString* ClientProps[] = {
            &aliasClientProp(),
            &clientProp(),
        };

        for (auto* p : ClientProps) {
            auto var = props.value(*p);
            if ((var.isValid()) && (var.canConvert<QString>())) {
                m_config.m_clientId = var.value<QString>();
                updated = true;
            }
        }
    }

    {
        static const QString* UsernameProps[] = {
            &aliasUsernameProp(),
            &usernameProp(),
        };

        for (auto* p : UsernameProps) {
            auto var = props.value(*p);
            if ((var.isValid()) && (var.canConvert<QString>())) {
                m_config.m_username = var.value<QString>();
                updated = true;
            }
        }
    }
    
    {
        static const QString* PasswordProps[] = {
            &aliasPasswordProp(),
            &passwordProp(),
        };

        for (auto* p : PasswordProps) {
            auto var = props.value(*p);
            if ((var.isValid()) && (var.canConvert<QString>())) {
                m_config.m_password = var.value<QString>();
                updated = true;
            }
        }  
    }

    {
        static const QString* PubTopicProps[] = {
            &aliasPubTopicProp(),
            &pubTopicProp(),
        };

        for (auto* p : PubTopicProps) {
            auto var = props.value(*p);
            if ((var.isValid()) && (var.canConvert<QString>())) {
                m_config.m_pubTopic = var.value<QString>();
                updated = true;
            }
        }  
    }  

    {
        static const QString* PubQosProps[] = {
            &aliasPubQosProp(),
            &pubQosProp(),
        };

        for (auto* p : PubQosProps) {
            auto var = props.value(*p);
            if ((var.isValid()) && (var.canConvert<int>())) {
                m_config.m_pubQos = var.value<int>();
                updated = true;
            }
        }  
    }  

    {
        static const QString* RespTopicProps[] = {
            &respTopicProp(),
        };

        for (auto* p : RespTopicProps) {
            auto var = props.value(*p);
            if ((var.isValid()) && (var.canConvert<QString>())) {
                m_config.m_respTopic = var.value<QString>();
                updated = true;
            }
        }  
    }     

    {
        static const QString* SubscribesRemoveProps[] = {
            &aliasSubscribesRemoveProp(),
            &subscribesRemoveProp(),
        };

        for (auto* p : SubscribesRemoveProps) {
            auto var = props.value(*p);
            if ((!var.isValid()) || (!var.canConvert<QVariantList>())) {
                continue;
            }

            auto subList = var.value<QVariantList>();

            for (auto idx = 0; idx < subList.size(); ++idx) {
                auto& subVar = subList[idx];
                if ((!subVar.isValid()) || (!subVar.canConvert<QVariantMap>())) {
                    continue;
                }

                auto subMap = subVar.value<QVariantMap>();
                auto topicVar = subMap.value(topicSubProp());
                if ((!topicVar.isValid()) || (!topicVar.canConvert<QString>())) {
                    continue;
                }

                auto topic = topicVar.value<QString>();

                auto iter = 
                    std::find_if(
                        m_config.m_subscribes.begin(), m_config.m_subscribes.end(),
                        [&topic](const auto& info)
                        {
                            return topic == info.m_topic;
                        });
                
                if (iter != m_config.m_subscribes.end()) {
                    m_config.m_subscribes.erase(iter);
                    updated = true;
                    forceCleanStart();                    
                }
            }
        }  
    }  

    {
        static const QString* SubscribesClearProps[] = {
            &aliasSubscribesClearProp(),
            &subscribesClearProp(),
        };

        for (auto* p : SubscribesClearProps) {
            auto var = props.value(*p);
            if ((!var.isValid()) || (!var.canConvert<bool>())) {
                continue;
            }

            if ((!var.value<bool>()) || (m_config.m_subscribes.empty())) {
                continue;
            }

            m_config.m_subscribes.clear();
            updated = true;
        }  
    }           

    {
        static const QString* SubscribesProps[] = {
            &aliasSubscribesProp(),
            &subscribesProp(),
        };

        for (auto* p : SubscribesProps) {
            auto var = props.value(*p);
            if ((!var.isValid()) || (!var.canConvert<QVariantList>())) {
                continue;
            }

            auto subList = var.value<QVariantList>();

            for (auto idx = 0; idx < subList.size(); ++idx) {
                auto& subVar = subList[idx];
                if ((!subVar.isValid()) || (!subVar.canConvert<QVariantMap>())) {
                    continue;
                }

                auto subMap = subVar.value<QVariantMap>();
                auto topicVar = subMap.value(topicSubProp());
                if ((!topicVar.isValid()) || (!topicVar.canConvert<QString>())) {
                    continue;
                }

                auto topic = topicVar.value<QString>();

                auto iter = 
                    std::find_if(
                        m_config.m_subscribes.begin(), m_config.m_subscribes.end(),
                        [&topic](const auto& info)
                        {
                            return topic == info.m_topic;
                        });
                
                if (iter == m_config.m_subscribes.end()) {
                    iter = m_config.m_subscribes.insert(m_config.m_subscribes.end(), SubConfig());
                    iter->m_topic = topic;
                }

                auto& subConfig = *iter;
                auto qosVar = subMap.value(qosSubProp());
                if (qosVar.isValid() && qosVar.canConvert<int>()) {
                    subConfig.m_maxQos = qosVar.value<int>();
                }

                auto retainHandlingVar = subMap.value(retainHandlingSubProp());
                if (retainHandlingVar.isValid() && retainHandlingVar.canConvert<int>()) {
                    subConfig.m_retainHandling = retainHandlingVar.value<int>();
                }

                auto noLocalVar = subMap.value(noLocalSubProp());
                if (noLocalVar.isValid() && noLocalVar.canConvert<bool>()) {
                    subConfig.m_noLocal = noLocalVar.value<bool>();
                }  

                auto retainAsPublishedVar = subMap.value(retainAsPublishedSubProp());
                if (retainAsPublishedVar.isValid() && retainAsPublishedVar.canConvert<bool>()) {
                    subConfig.m_retainAsPublished = retainAsPublishedVar.value<bool>();
                }                                       
            }
            
            updated = true;
            forceCleanStart();
        }  
    }              

    if (updated) {
        emit sigConfigChanged();
    }
}

const char* Mqtt5ClientFilter::debugNameImpl() const
{
    return "mqtt v5 client filter";
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
    if (2 <= getDebugOutputLevel()) {
        std::cout << '[' << currTimestamp() << "] (" << debugNameImpl() << "): socket connected report" << std::endl;
    }

    auto basicConfig = CC_Mqtt5ConnectBasicConfig();
    ::cc_mqtt5_client_connect_init_config_basic(&basicConfig);

    auto clientId = m_config.m_clientId.toStdString();
    auto username = m_config.m_username.toStdString();
    auto password = parsePassword(m_config.m_password);
    
    if (!clientId.empty()) {
        basicConfig.m_clientId = clientId.c_str();
    }

    basicConfig.m_username = username.c_str();
    basicConfig.m_password = password.data();
    basicConfig.m_passwordLen = static_cast<decltype(basicConfig.m_passwordLen)>(password.size());
    basicConfig.m_keepAlive = m_config.m_keepAlive;
    basicConfig.m_cleanStart = 
        (m_config.m_forcedCleanStart) ||
        (clientId.empty()) || 
        (clientId != m_prevClientId) ||
        (m_firstConnect);

    auto extraConfig = CC_Mqtt5ConnectExtraConfig();
    ::cc_mqtt5_client_connect_init_config_extra(&extraConfig);
    extraConfig.m_sessionExpiryInterval = m_config.m_sessionExpiryInterval;
    if (m_config.m_sessionExpiryInfinite) {
        extraConfig.m_sessionExpiryInterval = CC_MQTT5_SESSION_NEVER_EXPIRES;
    }
    extraConfig.m_topicAliasMaximum = m_config.m_topicAliasMaximum;

    auto ec = 
        cc_mqtt5_client_connect_full(
            m_client.get(), 
            &basicConfig, 
            nullptr, 
            &extraConfig, 
            nullptr, 
            &Mqtt5ClientFilter::connectCompleteCb, 
            this);

    if (ec != CC_Mqtt5ErrorCode_Success) {
        reportError(tr("Failed to initiate MQTT v5 connection"));
        return;
    }    

    m_prevClientId = clientId;
}

void Mqtt5ClientFilter::socketDisconnected()
{
    if (2 <= getDebugOutputLevel()) {
        std::cout << '[' << currTimestamp() << "] (" << debugNameImpl() << "): socket disconnected report" << std::endl;
    }

    ::cc_mqtt5_client_notify_network_disconnected(m_client.get());
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
    if (3 <= getDebugOutputLevel()) {
        std::cout << '[' << currTimestamp() << "] (" << debugNameImpl() << "): sending " << bufLen << " bytes" << std::endl;
    }

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
    static const QString BrokerDisconnecteError = 
        tr("MQTT5 Broker is disconnected");

    reportError(BrokerDisconnecteError);
}

void Mqtt5ClientFilter::messageReceivedInternal(const CC_Mqtt5MessageInfo& info)
{
    if (2 <= getDebugOutputLevel()) {
        std::cout << '[' << currTimestamp() << "] (" << debugNameImpl() << "): app message received: " << info.m_topic << std::endl;
    }

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

    if (info.m_userPropsCount > 0U) {
        assert(info.m_userProps != nullptr);
        QVariantList userProps;
        for (auto idx = 0U; idx < info.m_userPropsCount; ++idx) {
            userProps.append(toVariantMap(info.m_userProps[idx]));
        }

        props[userPropsProp()] = QVariant::fromValue(userProps);
    }

    m_recvData.append(std::move(dataInfo));
}

void Mqtt5ClientFilter::nextTickProgramInternal(unsigned ms)
{
    if (3 <= getDebugOutputLevel()) {
        std::cout << '[' << currTimestamp() << "] (" << debugNameImpl() << "): tick request: " << ms << std::endl;
    }

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

    if (3 <= getDebugOutputLevel()) {
        std::cout << '[' << currTimestamp() << "] (" << debugNameImpl() << "): cancel tick: " << diff << std::endl;
    }
    return static_cast<unsigned>(diff);
}

void Mqtt5ClientFilter::connectCompleteInternal(CC_Mqtt5AsyncOpStatus status, const CC_Mqtt5ConnectResponse* response)
{
    if (status != CC_Mqtt5AsyncOpStatus_Complete) {
        reportError(tr("Failed to connect to MQTT5 broker with status: ") + statusStr(status));
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

    if (m_config.m_subscribes.empty()) {
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
        reportError(tr("Failed to subsribe to MQTT5 topics with status: ") + statusStr(status));
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
        reportError(tr("Failed to publish to MQTT5 broker with status: ") + statusStr(status));
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

void Mqtt5ClientFilter::brokerDisconnectedCb(
    void* data, 
    [[maybe_unused]] CC_Mqtt5BrokerDisconnectReason reason, 
    [[maybe_unused]] const CC_Mqtt5DisconnectInfo* info)
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
    auto timestamp = std::chrono::high_resolution_clock::now();
    auto sinceEpoch = timestamp.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(sinceEpoch).count();
    std::cerr << '[' << milliseconds << "] MQTT ERROR: " << msg << std::endl;
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


