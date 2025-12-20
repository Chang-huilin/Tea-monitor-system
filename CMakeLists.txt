cmake_minimum_required(VERSION 3.16)

project(teaSYS VERSION 0.1 LANGUAGES CXX)

# ===============================
# 基本编译设置
# ===============================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ===============================
# 查找 Qt
# ===============================
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Gui)

# ===============================
# eBUS SDK（相机）
# ===============================
set(EBUS_ROOT "D:/SDK/eBUS SDK")
set(EBUS_INCLUDE_DIR "${EBUS_ROOT}/Includes")
set(EBUS_LIB_DIR     "${EBUS_ROOT}/Libraries")

# ===============================
# OceanDirect SDK（光谱仪）
# ===============================
set(OCEANDIRECT_ROOT "C:/Program Files/Ocean Optics/OceanDirect SDK")
set(OCEANDIRECT_INCLUDE_DIR "${OCEANDIRECT_ROOT}/include")
set(OCEANDIRECT_LIB_DIR     "${OCEANDIRECT_ROOT}/lib")


# ===============================
# 工程源文件
# ===============================
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    core/camera_interface.h
    core/camera_service.h
    core/camera_service.cpp


    core/spectrometer_interface.h
    core/spectrometer_service.h
    core/spectrometer_service.cpp
)

# ===============================
# 可执行程序
# ===============================
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(teaSYS
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        readme.md
    )
else()
    add_executable(teaSYS
        ${PROJECT_SOURCES}
    )
endif()

# ===============================
# include / link
# ===============================
target_include_directories(teaSYS PRIVATE
    ${EBUS_INCLUDE_DIR}
    ${OCEAN_INCLUDE_DIR}
)

target_link_directories(teaSYS PRIVATE
    ${EBUS_LIB_DIR}
    ${OCEAN_LIB_DIR}
)

target_link_libraries(teaSYS PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    PvBase
    OceanDirect       # ← OceanDirect.dll 对应的 import lib
)
target_include_directories(teaSYS PRIVATE
    ${OCEANDIRECT_INCLUDE_DIR}
)

target_link_directories(teaSYS PRIVATE
    ${OCEANDIRECT_LIB_DIR}
)

# 说明：
# 1) 这里通常应该链接 OceanDirect.lib（MSVC 需要 import lib）
# 2) 如果你的 lib 目录里确实只有 OceanDirect.dll 而没有 .lib，先去确认 SDK 是否提供 .lib
target_link_libraries(teaSYS PRIVATE
    OceanDirect
)


# ===============================
# Windows
# ===============================
set_target_properties(teaSYS PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(teaSYS)
endif()

# 运行时把 DLL 拷贝到 exe 同目录（非常关键）
add_custom_command(TARGET teaSYS POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${OCEANDIRECT_LIB_DIR}/OceanDirect.dll"
            $<TARGET_FILE_DIR:teaSYS>
)
