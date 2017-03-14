project(taskbarswitch)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set(QT_MIN_VERSION "5.4.0")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

find_package(ECM 1.1.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMPackageConfigHelpers)
include(ECMOptionalAddSubdirectory)

set(QT_MIN_VERSION "5.5.0")
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    X11Extras
)

set(KF5_MIN_VERSION "5.24.0")
find_package(KF5 REQUIRED COMPONENTS
    DBusAddons
    GlobalAccel
    I18n
    WindowSystem
    XmlGui
)

# Set KI18n translation domain
add_definitions(-DTRANSLATION_DOMAIN=\"taskbarswitch\")

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

########### next target ###############

set(taskbarswitch_SRCS
    taskbarswitch.cpp)

add_library(kded_taskbarswitch MODULE ${taskbarswitch_SRCS})
set_target_properties(kded_taskbarswitch PROPERTIES OUTPUT_NAME taskbarswitch)
target_link_libraries(kded_taskbarswitch
    KF5::DBusAddons
    KF5::I18n
    KF5::GlobalAccel
    KF5::WindowSystem
    Qt5::X11Extras
    KF5::XmlGui
    )

########### install files ###############

install(TARGETS kded_taskbarswitch  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES taskbarswitch.desktop DESTINATION  ${SERVICES_INSTALL_DIR}/kded)
