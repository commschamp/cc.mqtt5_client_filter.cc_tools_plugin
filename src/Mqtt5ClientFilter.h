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

class Mqtt5ClientFilter : public QObject, public cc_tools_qt::Filter
{
    Q_OBJECT

public:
    struct Config
    {
        unsigned m_respTimeout = 0U;
        QString m_clientId;
    };

    Mqtt5ClientFilter();
    ~Mqtt5ClientFilter() noexcept;

    Config& config()
    {
        return m_config;
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

    static void sendDataCb(void* data, const unsigned char* buf, unsigned bufLen);
    static void brokerDisconnectedCb(void* data, const CC_Mqtt5DisconnectInfo* info);
    static void messageReceivedCb(void* data, const CC_Mqtt5MessageInfo* info);
    static void nextTickProgramCb(void* data, unsigned ms);
    static unsigned cancelTickProgramCb(void* data);
    static void errorLogCb(void* data, const char* msg);
    static void connectCompleteCb(void* data, CC_Mqtt5AsyncOpStatus status, const CC_Mqtt5ConnectResponse* response);

    ClientPtr m_client;
    QTimer m_timer;
    std::list<cc_tools_qt::DataInfoPtr> m_pendingData;
    cc_tools_qt::DataInfo::DataSeq m_inData;
    Config m_config;
    std::string m_prevClientId;
    unsigned m_tickMs = 0U;
    qint64 m_tickMeasureTs = 0U;
    cc_tools_qt::DataInfoPtr m_recvDataPtr;
    QList<cc_tools_qt::DataInfoPtr> m_recvData;
    cc_tools_qt::DataInfoPtr m_sendDataPtr;
    QList<cc_tools_qt::DataInfoPtr> m_sendData;
    bool m_firstConnect = true;
    bool m_waitingForDisconnect = false;
};

using Mqtt5ClientFilterPtr = std::shared_ptr<Mqtt5ClientFilter>;

inline
Mqtt5ClientFilterPtr makeMqtt5ClientFilter()
{
    return Mqtt5ClientFilterPtr(new Mqtt5ClientFilter());
}

}  // namespace cc_plugin_mqtt5_client_filter

