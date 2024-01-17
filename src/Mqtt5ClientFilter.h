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

#include <cc_tools_qt/Filter.h>

#include <cc_mqtt5_client/client.h>

#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTimer>

#include <list>
#include <memory>
#include <string>

namespace cc_plugin_mqtt5_client_filter
{

class Mqtt5ClientFilter final : public QObject, public cc_tools_qt::Filter
{
    Q_OBJECT

public:
    struct Config
    {
        unsigned m_respTimeout = 0U;
        QString m_clientId;
        QString m_username; 
        QString m_password; 
        QString m_subTopics = "#";
        int m_subQos = 2;
        QString m_pubTopic;
        QString m_respTopic;
        int m_pubQos = 0;
        bool m_forcedCleanStart = false;
    };

    Mqtt5ClientFilter();
    ~Mqtt5ClientFilter() noexcept;

    Config& config()
    {
        return m_config;
    }

    void forceCleanStart()
    {
        m_firstConnect = true;
    }

protected:
    virtual bool startImpl() override;
    virtual void stopImpl() override;
    virtual QList<cc_tools_qt::DataInfoPtr> recvDataImpl(cc_tools_qt::DataInfoPtr dataPtr) override;
    virtual QList<cc_tools_qt::DataInfoPtr> sendDataImpl(cc_tools_qt::DataInfoPtr dataPtr) override;
    virtual void socketConnectionReportImpl(bool connected) override;

private slots:
    void doTick();

private:
    struct ClientDeleter
    {
        void operator()(CC_Mqtt5Client* ptr)
        {
            ::cc_mqtt5_client_free(ptr);
        }
    };
    
    using ClientPtr = std::unique_ptr<CC_Mqtt5Client, ClientDeleter>;

    void socketConnected();
    void socketDisconnected();

    void sendDataInternal(const unsigned char* buf, unsigned bufLen);
    void brokerDisconnectedInternal();
    void messageReceivedInternal(const CC_Mqtt5MessageInfo& info);
    void nextTickProgramInternal(unsigned ms);
    unsigned cancelTickProgramInternal();
    void connectCompleteInternal(CC_Mqtt5AsyncOpStatus status, const CC_Mqtt5ConnectResponse* response);
    void subscribeCompleteInternal(CC_Mqtt5SubscribeHandle handle, CC_Mqtt5AsyncOpStatus status, const CC_Mqtt5SubscribeResponse* response);
    void publishCompleteInternal(CC_Mqtt5PublishHandle handle, CC_Mqtt5AsyncOpStatus status, const CC_Mqtt5PublishResponse* response);

    static void sendDataCb(void* data, const unsigned char* buf, unsigned bufLen);
    static void brokerDisconnectedCb(void* data, const CC_Mqtt5DisconnectInfo* info);
    static void messageReceivedCb(void* data, const CC_Mqtt5MessageInfo* info);
    static void nextTickProgramCb(void* data, unsigned ms);
    static unsigned cancelTickProgramCb(void* data);
    static void errorLogCb(void* data, const char* msg);
    static void connectCompleteCb(void* data, CC_Mqtt5AsyncOpStatus status, const CC_Mqtt5ConnectResponse* response);
    static void subscribeCompleteCb(void* data, CC_Mqtt5SubscribeHandle handle, CC_Mqtt5AsyncOpStatus status, const CC_Mqtt5SubscribeResponse* response);
    static void publishCompleteCb(void* data, CC_Mqtt5PublishHandle handle, CC_Mqtt5AsyncOpStatus status, const CC_Mqtt5PublishResponse* response);

    ClientPtr m_client;
    QTimer m_timer;
    std::list<cc_tools_qt::DataInfoPtr> m_pendingData;
    cc_tools_qt::DataInfo::DataSeq m_inData;
    Config m_config;
    std::string m_prevClientId;
    unsigned m_tickMs = 0U;
    qint64 m_tickMeasureTs = 0;
    cc_tools_qt::DataInfoPtr m_recvDataPtr;
    QList<cc_tools_qt::DataInfoPtr> m_recvData;
    cc_tools_qt::DataInfoPtr m_sendDataPtr;
    QList<cc_tools_qt::DataInfoPtr> m_sendData;
    bool m_firstConnect = true;
    bool m_socketConnected = false;
    bool m_waitingForDisconnect = false;
};

using Mqtt5ClientFilterPtr = std::shared_ptr<Mqtt5ClientFilter>;

inline
Mqtt5ClientFilterPtr makeMqtt5ClientFilter()
{
    return Mqtt5ClientFilterPtr(new Mqtt5ClientFilter());
}

}  // namespace cc_plugin_mqtt5_client_filter


