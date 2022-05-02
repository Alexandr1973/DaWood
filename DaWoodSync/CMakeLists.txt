cmake_minimum_required(VERSION 3.5)

project(DaWood_Sync LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS SerialPort REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS sql REQUIRED)
#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Charts REQUIRED)
#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS PrintSupport REQUIRED)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/app_win32.rc")

if(ANDROID)
  add_library(DaWood_Sync SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
  )
else()
  add_executable(DaWood_Sync WIN32
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    myserialport.cpp
    myserialport.h
    crc32.cpp
    crc32.h
    tblconfig.cpp
    tblconfig.h
    utilites.cpp
    utilites.h
    device.cpp
    device.h
    dataprocessing.cpp
    dataprocessing.h
    thermo.cpp
    thermo.h
    pc_database.cpp
    pc_database.h
    piezo.cpp
    piezo.h
    devlogs.cpp
    devlogs.h
    firmware_read.cpp
    firmware_read.h
    inklin.cpp
    inklin.h
    resource.qrc
    ${APP_ICON_RESOURCE_WINDOWS}
  )
endif()

target_link_libraries(DaWood_Sync PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(DaWood_Sync PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort)
target_link_libraries(DaWood_Sync PRIVATE Qt${QT_VERSION_MAJOR}::Sql)
#target_link_libraries(DaWood_Sync PRIVATE Qt${QT_VERSION_MAJOR}::Charts)
#target_link_libraries(DaWood_Sync PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)

get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

add_custom_command(TARGET DaWood_Sync POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
        env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
            "$<TARGET_FILE:DaWood_Sync>"
    COMMENT "Running windeployqt..."
)
