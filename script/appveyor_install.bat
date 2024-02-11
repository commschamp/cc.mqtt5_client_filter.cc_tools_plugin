IF "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2019" (
    set TOOLCHAIN=msvc16
    set QT_SUBDIR=msvc2019
    set CMAKE_GENERATOR=Visual Studio 16 2019
    IF "%QT_MAJOR%"=="6" (
        set QT_VER=6.4
    ) ELSE (
        set QT_VER=5.15
    )
    IF "%PLATFORM%"=="x86" (
        echo Performing x86 build in VS2019
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"
        set CMAKE_PLATFORM=Win32
    ) ELSE (
        echo Performing amd64 build in VS2019
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
        set CMAKE_PLATFORM=x64
    )

) ELSE IF "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2022" (
    set TOOLCHAIN=msvc17
    set QT_SUBDIR=msvc2019
    set CMAKE_GENERATOR=Visual Studio 17 2022
    IF "%QT_MAJOR%"=="6" (
        set QT_VER=6.4
    ) ELSE (
        set QT_VER=5.15
    )
    IF "%PLATFORM%"=="x86" (
        echo Performing x86 build in VS2022
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"
        set CMAKE_PLATFORM=Win32
    ) ELSE (
        echo Performing amd64 build in VS2022
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
        set CMAKE_PLATFORM=x64
    )  
) ELSE (
    echo Toolchain %TOOLCHAIN% is not supported
    exit -1
)

set QTDIR_PREFIX=C:/Qt/%QT_VER%
IF "%PLATFORM%"=="x86" (
    set QTDIR_SUFFIX=
) ELSE (
    set QTDIR_SUFFIX=_64
)

set QTDIR=%QTDIR_PREFIX%/%QT_SUBDIR%%QTDIR_SUFFIX%
IF NOT EXIST %QTDIR% (
    echo WARNING: %QTDIR% does not exist!!!
    set QTDIR=%QTDIR_PREFIX%/msvc2015%QTDIR_SUFFIX%
)

echo Using Qt%QT_MAJOR% from %QTDIR%

