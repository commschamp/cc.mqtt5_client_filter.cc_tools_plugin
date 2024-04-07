rem Input
rem BUILD_DIR - Main build directory
rem GENERATOR - CMake generator
rem PLATFORM - CMake generator platform
rem QTDIR - Path to Qt installation
rem EXTERNALS_DIR - (Optional) Directory where externals need to be located
rem COMMS_REPO - (Optional) Repository of the COMMS library
rem COMMS_TAG - (Optional) Tag of the COMMS library
rem CC_TOOLS_QT_REPO - (Optional) Repository of the cc_tools_qt
rem CC_TOOLS_QT_TAG - (Optional) Tag of the cc_tools_qt
rem CC_MQTT5_GENERATED_REPO - (Optional) Repository of the cc.mqtt5.generated
rem CC_MQTT5_GENERATED_TAG - (Optional) Tag of the cc.mqtt5.generated
rem CC_MQTT5_LIBS_REPO - (Optional) Repository of the cc.mqtt5.libs
rem CC_MQTT5_LIBS_TAG - (Optional) Tag of the cc.mqtt5.libs
rem COMMON_INSTALL_DIR - (Optional) Common directory to perform installations
rem COMMON_BUILD_TYPE - (Optional) CMake build type
rem COMMON_CXX_STANDARD - (Optional) CMake C++ standard
rem COMMON_QT_VER - (Optional) common Qt major version

rem -----------------------------------------------------

if [%BUILD_DIR%] == [] echo "BUILD_DIR hasn't been specified" & exit /b 1

if NOT [%GENERATOR%] == [] set GENERATOR_PARAM=-G %GENERATOR%

if NOT [%PLATFORM%] == [] set PLATFORM_PARAM=-A %PLATFORM%

if [%EXTERNALS_DIR%] == [] set EXTERNALS_DIR=%BUILD_DIR%/externals

if [%COMMS_REPO%] == [] set COMMS_REPO="https://github.com/commschamp/comms.git"

if [%COMMS_TAG%] == [] set COMMS_TAG="master"

if [%CC_TOOLS_QT_REPO%] == [] set CC_TOOLS_QT_REPO="https://github.com/commschamp/cc_tools_qt.git"

if [%CC_TOOLS_QT_TAG%] == [] set CC_TOOLS_QT_TAG="master"

if [%CC_MQTT5_GENERATED_REPO%] == [] set CC_MQTT5_GENERATED_REPO="https://github.com/commschamp/cc.mqtt5.generated.git"

if [%CC_MQTT5_GENERATED_TAG%] == [] set CC_MQTT5_GENERATED_TAG="master"

if [%CC_MQTT5_LIBS_REPO%] == [] set CC_MQTT5_LIBS_REPO="https://github.com/commschamp/cc.mqtt5.libs.git"

if [%CC_MQTT5_LIBS_TAG%] == [] set CC_MQTT5_LIBS_TAG="master"

if [%COMMON_BUILD_TYPE%] == [] set COMMON_BUILD_TYPE=Debug

if [%COMMON_CXX_STANDARD%] == [] set COMMON_CXX_STANDARD=17

if [%COMMON_QT_VER%] == [] set COMMON_QT_VER=5

set COMMS_SRC_DIR=%EXTERNALS_DIR%/comms
set COMMS_BUILD_DIR=%BUILD_DIR%/externals/comms/build
set COMMS_INSTALL_DIR=%COMMS_BUILD_DIR%/install
if NOT [%COMMON_INSTALL_DIR%] == [] set COMMS_INSTALL_DIR=%COMMON_INSTALL_DIR%

set CC_TOOLS_QT_SRC_DIR=%EXTERNALS_DIR%/cc_tools_qt
set CC_TOOLS_QT_BUILD_DIR=%BUILD_DIR%/externals/cc_tools_qt/build
set CC_TOOLS_QT_INSTALL_DIR=%CC_TOOLS_QT_BUILD_DIR%/install
if NOT [%COMMON_INSTALL_DIR%] == [] set CC_TOOLS_QT_INSTALL_DIR=%COMMON_INSTALL_DIR%

set CC_MQTT5_GENERATED_SRC_DIR=%EXTERNALS_DIR%/cc.mqtt5.generated
set CC_MQTT5_GENERATED_BUILD_DIR=%BUILD_DIR%/externals/cc.mqtt5.generated/build
set CC_MQTT5_GENERATED_INSTALL_DIR=%CC_MQTT5_GENERATED_BUILD_DIR%/install
if NOT [%COMMON_INSTALL_DIR%] == [] set CC_MQTT5_GENERATED_INSTALL_DIR=%COMMON_INSTALL_DIR%

set CC_MQTT5_LIBS_SRC_DIR=%EXTERNALS_DIR%/cc.mqtt5.libs
set CC_MQTT5_LIBS_BUILD_DIR=%BUILD_DIR%/externals/cc.mqtt5.libs/build
set CC_MQTT5_LIBS_INSTALL_DIR=%CC_MQTT5_LIBS_BUILD_DIR%/install
if NOT [%COMMON_INSTALL_DIR%] == [] set CC_MQTT5_LIBS_INSTALL_DIR=%COMMON_INSTALL_DIR%

rem ----------------------------------------------------

mkdir "%EXTERNALS_DIR%"

if exist %COMMS_SRC_DIR%/.git (
    echo "Updating COMMS library..."
    cd "%COMMS_SRC_DIR%"
    git fetch --all
    git checkout .    
    git checkout %COMMS_TAG%
    git pull --all
    if %errorlevel% neq 0 exit /b %errorlevel%    
) else (
    echo "Cloning COMMS library..."
    git clone -b %COMMS_TAG% %COMMS_REPO% %COMMS_SRC_DIR%
    if %errorlevel% neq 0 exit /b %errorlevel%
)

:comms_build
echo "Building COMMS library..."
mkdir "%COMMS_BUILD_DIR%"
cd %COMMS_BUILD_DIR%
cmake %GENERATOR_PARAM% %PLATFORM_PARAM% -S %COMMS_SRC_DIR% -B %COMMS_BUILD_DIR% -DCMAKE_INSTALL_PREFIX=%COMMS_INSTALL_DIR% -DCMAKE_BUILD_TYPE=%COMMON_BUILD_TYPE% -DCMAKE_CXX_STANDARD=%COMMON_CXX_STANDARD%
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build %COMMS_BUILD_DIR% --config %COMMON_BUILD_TYPE% --target install
if %errorlevel% neq 0 exit /b %errorlevel%

rem ----------------------------------------------------

if exist %CC_TOOLS_QT_SRC_DIR%/.git (
    echo "Updating cc_tools_qt..."
    cd %CC_TOOLS_QT_SRC_DIR%
    git fetch --all
    git checkout .    
    git checkout %CC_TOOLS_QT_TAG%
    git pull --all    
) else (
    echo "Cloning cc_tools_qt ..."
    git clone -b %CC_TOOLS_QT_TAG% %CC_TOOLS_QT_REPO% %CC_TOOLS_QT_SRC_DIR%
    if %errorlevel% neq 0 exit /b %errorlevel%
)

echo "Building cc_tools_qt ..."
mkdir "%CC_TOOLS_QT_BUILD_DIR%"
cd %CC_TOOLS_QT_BUILD_DIR%
cmake %GENERATOR_PARAM% %PLATFORM_PARAM% -S %CC_TOOLS_QT_SRC_DIR% -B %CC_TOOLS_QT_BUILD_DIR% -DCMAKE_INSTALL_PREFIX=%CC_TOOLS_QT_INSTALL_DIR% ^
    -DCMAKE_BUILD_TYPE=%COMMON_BUILD_TYPE% -DCC_TOOLS_QT_BUILD_APPS=OFF -DCMAKE_PREFIX_PATH=%COMMS_INSTALL_DIR%;%QTDIR% ^
    -DCMAKE_CXX_STANDARD=%COMMON_CXX_STANDARD% %CC_TOOLS_QT_VERSION_OPT%
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build %CC_TOOLS_QT_BUILD_DIR% --config %COMMON_BUILD_TYPE% --target install
if %errorlevel% neq 0 exit /b %errorlevel%

rem ----------------------------------------------------

if exist %CC_MQTT5_GENERATED_SRC_DIR%/.git(
    echo "Updating cc.mqtt5.generated..."
    cd %CC_MQTT5_GENERATED_SRC_DIR%
    git fetch --all
    git checkout .
    git checkout %CC_MQTT5_GENERATED_TAG%
    git pull --all
) else (
    echo "Cloning cc.mqtt5.generated ..."
    git clone -b %CC_MQTT5_GENERATED_TAG% %CC_MQTT5_GENERATED_REPO% %CC_MQTT5_GENERATED_SRC_DIR%
    if %errorlevel% neq 0 exit /b %errorlevel%
)

echo "Building cc.mqtt5.generated ..."
mkdir "%CC_MQTT5_GENERATED_BUILD_DIR%"
cd %CC_MQTT5_GENERATED_BUILD_DIR%
cmake %GENERATOR_PARAM% %PLATFORM_PARAM% -S %CC_MQTT5_GENERATED_SRC_DIR% -B %CC_MQTT5_GENERATED_BUILD_DIR% -DCMAKE_INSTALL_PREFIX=%CC_MQTT5_GENERATED_INSTALL_DIR% -DCMAKE_BUILD_TYPE=%COMMON_BUILD_TYPE% -DOPT_REQUIRE_COMMS_LIB=OFF -DCMAKE_PREFIX_PATH=%COMMS_INSTALL_DIR% -DCMAKE_CXX_STANDARD=%COMMON_CXX_STANDARD%
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build %CC_MQTT5_GENERATED_BUILD_DIR% --config %COMMON_BUILD_TYPE% --target install
if %errorlevel% neq 0 exit /b %errorlevel%

rem ----------------------------------------------------

if exist %CC_MQTT5_LIBS_SRC_DIR%/.git (
    echo "Updating cc.mqtt5.libs..."
    cd %CC_MQTT5_LIBS_SRC_DIR%
    git fetch --all
    git checkout .
    git checkout %CC_MQTT5_LIBSD_TAG%
    git pull --all
) else (
    echo "Cloning cc.mqtt5.libs ..."
    git clone -b %CC_MQTT5_LIBS_TAG% %CC_MQTT5_LIBS_REPO% %CC_MQTT5_LIBS_SRC_DIR%
    if %errorlevel% neq 0 exit /b %errorlevel%
)
    
echo "Building cc.mqtt5.libs ..."
mkdir "%CC_MQTT5_LIBS_BUILD_DIR%"
cd %CC_MQTT5_LIBS_BUILD_DIR%
cmake %GENERATOR_PARAM% %PLATFORM_PARAM% -S %CC_MQTT5_LIBS_SRC_DIR% -B %CC_MQTT5_LIBS_BUILD_DIR% -DCMAKE_INSTALL_PREFIX=%CC_MQTT5_LIBS_INSTALL_DIR% -DCMAKE_BUILD_TYPE=%COMMON_BUILD_TYPE% -DCMAKE_PREFIX_PATH=%COMMS_INSTALL_DIR%;%CC_MQTT5_GENERATED_INSTALL_DIR% -DCMAKE_CXX_STANDARD=%COMMON_CXX_STANDARD% -DCC_MQTT5_CLIENT_LIB_FORCE_PIC=ON -DCC_MQTT5_CLIENT_APPS=OFF
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build %CC_MQTT5_LIBS_BUILD_DIR% --config %COMMON_BUILD_TYPE% --target install
if %errorlevel% neq 0 exit /b %errorlevel%
