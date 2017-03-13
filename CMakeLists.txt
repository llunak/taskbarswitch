project(taskbarswitch)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DHAVE_CONFIG_H=1)
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

########### next target ###############

set(taskbarswitch_SRCS
    taskbarswitch.cpp)

kde4_add_plugin(kded_taskbarswitch ${taskbarswitch_SRCS})
target_link_libraries(kded_taskbarswitch ${KDE4_KDEUI_LIBS})

#kde4_add_executable(taskbarswitch ${taskbarswitch_SRCS})
#add_definitions(-DTASKBARSWITCH_STANDALONE)
#target_link_libraries(taskbarswitch ${KDE4_KDEUI_LIBS})


########### install files ###############

install(TARGETS kded_taskbarswitch  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES taskbarswitch.desktop DESTINATION  ${SERVICES_INSTALL_DIR}/kded)
