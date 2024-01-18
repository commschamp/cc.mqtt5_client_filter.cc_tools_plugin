cmake_minimum_required (VERSION 3.11)
project ("cc_plugin_mqtt5_client_filter")

# Available options
option (OPT_WARN_AS_ERR "Treat warning as error" ON)
option (OPT_USE_CCACHE "Use ccache on UNIX systems if it's available" ON)

# Extra configuration variables
set (CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to use")
set (OPT_MAJOR_QT_VERSION 5 CACHE STRING "Major Qt version")


#######################################################################

find_package(LibComms REQUIRED NO_MODULE)
find_package(cc_tools_qt REQUIRED NO_MODULE)
find_package(cc_mqtt5_client REQUIRED NO_MODULE)
find_package(Qt${OPT_MAJOR_QT_VERSION} REQUIRED COMPONENTS Widgets Core)

if (Qt${OPT_MAJOR_QT_VERSION}_VERSION VERSION_LESS 5.15)
    message(FATAL_ERROR "Minimum supported Qt version is 5.15!")
endif()

#######################################################################

# Compiler options
set (compile_opts)
if (OPT_WARN_AS_ERR)
    list (APPEND compile_opts WARN_AS_ERR)
endif ()

set (ccache_opt)
if ((UNIX) AND (OPT_USE_CCACHE))
    list (APPEND compile_opts USE_CCACHE)
endif ()

include (${LibComms_DIR}/CC_Compile.cmake)
cc_compile(${compile_opts})
cc_msvc_force_warn_opt("/W4")

#######################################################################

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include(GNUInstallDirs)
set (PLUGIN_INSTALL_REL_DIR ${CMAKE_INSTALL_LIBDIR}/cc_tools_qt/plugin)
set (PLUGIN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${PLUGIN_INSTALL_REL_DIR})

set (src
    src/Mqtt5ClientFilter.cpp
    src/Mqtt5ClientFilterConfigWidget.cpp
    src/Mqtt5ClientFilterConfigWidget.ui
    src/Mqtt5ClientFilterPlugin.cpp
    src/Mqtt5ClientFilterTopicAliasWidget.cpp
    src/ui.qrc
)

add_library (${CMAKE_PROJECT_NAME} MODULE ${src})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE cc::cc_mqtt5_client cc::cc_tools_qt Qt::Widgets Qt::Core)
install (
    TARGETS ${CMAKE_PROJECT_NAME}
    DESTINATION ${PLUGIN_INSTALL_DIR})


