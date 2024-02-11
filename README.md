# MQTT5 Client Filter Plugin for the CommsChampion Tools
This project is a member of the [CommsChampion Ecosystem](https://commschamp.github.io/) and implements
**MQTT5 Filter** plugin for the [CommsChampion Tools](https://github.com/commschamp/cc_tools_qt). It
allows observation and debugging of the custom protocol messages exchanged using
[MQTT5](https://docs.oasis-open.org/mqtt/mqtt/v5.0/mqtt-v5.0.html) as the intermediate transport protocol in the common
environment of the [CommsChampion Tools](https://github.com/commschamp/cc_tools_qt).
Please read the [wiki page](https://github.com/commschamp/cc.mqtt5_client_filter.cc_tools_plugin/wiki) for the instructions on
how to configure and use the plugin.

# How to Build
This project uses [CMake](https://cmake.org/) as its build system. Please open the primary
[CMakeLists.txt](CMakeLists.txt) and review the available configuration options.

This project has the following direct and transitive external dependencies:

- [COMMS Library](https://github.com/commschamp/comms) (**direct** dependency) - Primary library for any protocol definition, some inner cmake scripts are re-used in the build.
- [cc.mqtt5.generated](https://github.com/commschamp/cc.mqtt5.generated) (**transitive** dependency) - Definition of the MQTT5 protocol (depends on the [COMMS Library](https://github.com/commschamp/comms)).
- [cc.mqtt5.libs](https://github.com/commschamp/cc.mqtt5.generated) (**direct** dependency) - MQTT5 client library (depends on the
[COMMS Library](https://github.com/commschamp/comms) and the [cc.mqtt5.generated](https://github.com/commschamp/cc.mqtt5.generated)).
- [CommsChampion Tools](https://github.com/commschamp/cc.mqtt5.generated) (**direct** dependency) - CommsChampion Tools and its library required for the plugin development.
- [Qt](https://www.qt.io/) (**direct** dependency) - Qt5/6 libraries.


All these dependencies are expected to be built externally and installation paths to the primary dependencies are expected to be passed
to the `cmake` invocation
using the **CMAKE_PREFIX_PATH** configuration variable:
```
cmake -DCMAKE_PREFIX_PATH=/path/to/comms/install\;/path/to/cc_tools_qt/install\;/path/to/cc.mqtt5.libs ...
```

There are [prepare_externals.sh](script/prepare_externals.sh) (for Linux) and [prepare_externals.bat](script/prepare_externals.bat) (for Windows)
scripts that can be used to pull and build all the required dependencies (excluding the [Qt](https://www.qt.io/) libraries).

This project requires minimal **C++17** standard as well as Qt **v5.15** or above to get properly compiled.

# Branching Model
This repository will follow the
[Successful Git Branching Model](http://nvie.com/posts/a-successful-git-branching-model/).

The **master** branch will always point to the latest release, the
development is performed on **develop** branch. As the result it is safe
to just clone the sources of this repository and use it without
any extra manipulations of looking for the latest stable version among the tags and
checking it out.

# Contact Information
For bug reports, feature requests, or any other question you may open an issue
here in **github** or e-mail me directly to: **arobenko@gmail.com**. I usually
respond within 24 hours.
