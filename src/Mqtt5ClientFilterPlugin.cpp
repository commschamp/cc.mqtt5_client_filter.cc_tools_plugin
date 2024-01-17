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

#include "Mqtt5ClientFilterPlugin.h"

#include "Mqtt5ClientFilter.h"
#include "Mqtt5ClientFilterConfigWidget.h"

#include <cassert>
#include <memory>
#include <type_traits>

namespace cc_plugin_mqtt5_client_filter
{

namespace 
{

const QString MainConfigKey("cc_plugin_mqtt5_client_filter");
const QString RespTimeoutSubKey("resp_timeout");
const QString ClientIdSubKey("client_id");
const QString UsernameSubKey("username");
const QString PasswordSubKey("password");
const QString ForceCleanStartSubKey("force_clean_start");
const QString SubTopicsSubKey("sub_topics");
const QString SubQosSubKey("sub_qos");
const QString PubTopicSubKey("pub_topic");
const QString PubQosSubKey("pub_qos");


template <typename T>
void getFromConfigMap(const QVariantMap& subConfig, const QString& key, T& val)
{
    using Type = std::decay_t<decltype(val)>;
    auto var = subConfig.value(key);
    if (var.isValid() && var.canConvert<Type>()) {
        val = var.value<Type>();
    }    
}

} // namespace 
    

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
            })
        .setConfigWidgetCreateFunc(
            [this]()
            {
                createFilterIfNeeded();
                return new Mqtt5ClientFilterConfigWidget(*m_filter);
            })            
        ;
}

Mqtt5ClientFilterPlugin::~Mqtt5ClientFilterPlugin() noexcept = default;

void Mqtt5ClientFilterPlugin::getCurrentConfigImpl(QVariantMap& config)
{
    createFilterIfNeeded();
    assert(m_filter);

    QVariantMap subConfig;
    subConfig.insert(RespTimeoutSubKey, m_filter->config().m_respTimeout);
    subConfig.insert(ClientIdSubKey, m_filter->config().m_clientId);
    subConfig.insert(UsernameSubKey, m_filter->config().m_username);
    subConfig.insert(PasswordSubKey, m_filter->config().m_password);
    subConfig.insert(ForceCleanStartSubKey, m_filter->config().m_forcedCleanStart);
    subConfig.insert(SubTopicsSubKey, m_filter->config().m_subTopics);
    subConfig.insert(SubQosSubKey, m_filter->config().m_subQos);
    subConfig.insert(PubTopicSubKey, m_filter->config().m_pubTopic);
    subConfig.insert(PubQosSubKey, m_filter->config().m_pubQos);
    config.insert(MainConfigKey, QVariant::fromValue(subConfig));
}

void Mqtt5ClientFilterPlugin::reconfigureImpl(const QVariantMap& config)
{
    auto subConfigVar = config.value(MainConfigKey);
    if ((!subConfigVar.isValid()) || (!subConfigVar.canConvert<QVariantMap>())) {
        return;
    }

    createFilterIfNeeded();
    assert(m_filter);

    auto subConfig = subConfigVar.value<QVariantMap>();

    getFromConfigMap(subConfig, RespTimeoutSubKey, m_filter->config().m_respTimeout);
    getFromConfigMap(subConfig, ClientIdSubKey, m_filter->config().m_clientId);
    getFromConfigMap(subConfig, UsernameSubKey, m_filter->config().m_username);
    getFromConfigMap(subConfig, PasswordSubKey, m_filter->config().m_password);
    getFromConfigMap(subConfig, ForceCleanStartSubKey, m_filter->config().m_forcedCleanStart);
    getFromConfigMap(subConfig, SubTopicsSubKey, m_filter->config().m_subTopics);
    getFromConfigMap(subConfig, SubQosSubKey, m_filter->config().m_subQos);
    getFromConfigMap(subConfig, PubTopicSubKey, m_filter->config().m_pubTopic);
    getFromConfigMap(subConfig, PubQosSubKey, m_filter->config().m_pubQos);
}

void Mqtt5ClientFilterPlugin::createFilterIfNeeded()
{
    if (m_filter) {
        return;
    }

    m_filter = makeMqtt5ClientFilter();
}

}  // namespace cc_plugin_mqtt5_client_filter


