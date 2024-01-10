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

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTimer>

#include <list>
#include <memory>


namespace cc_plugin_mqtt5_client_filter
{

class Mqtt5ClientFilter : public QObject, public cc_tools_qt::Filter
{
    Q_OBJECT

public:
    Mqtt5ClientFilter();
    ~Mqtt5ClientFilter() noexcept;

    const QString& getClientId() const
    {
        return m_clientId;
    }

    void setClientId(const QString& val)
    {
        m_clientId = val;
    }

protected:
    virtual bool startImpl() override;
    //virtual QList<DataInfoPtr> recvDataImpl(DataInfoPtr dataPtr) override;
    //virtual QList<DataInfoPtr> sendDataImpl(DataInfoPtr dataPtr) override;
    virtual void socketConnectionReportImpl(bool connected) override;

private slots:

private:
    struct ClientDeleter
    {
        void operator()(CC_Mqtt5Client* ptr)
        {
            ::cc_mqtt5_client_free(ptr);
        }
    };
    
    using ClientPtr = std::unique_ptr<CC_Mqtt5Client, ClientDeleter>;

    void sendDataInternal(const unsigned char* buf, unsigned bufLen);

    static void sendDataCb(void* data, const unsigned char* buf, unsigned bufLen);

    ClientPtr m_client;
    QTimer m_timer;
    std::list<cc_tools_qt::DataInfoPtr> m_pendingData;
    QString m_clientId;
    bool m_firstConnect = true;
};

using Mqtt5ClientFilterPtr = std::shared_ptr<Mqtt5ClientFilter>;

inline
Mqtt5ClientFilterPtr makeMqtt5ClientFilter()
{
    return Mqtt5ClientFilterPtr(new Mqtt5ClientFilter());
}

}  // namespace cc_plugin_mqtt5_client_filter


