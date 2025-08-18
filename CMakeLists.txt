cmake_minimum_required(VERSION 3.19)
project(RestyCage LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(RestyCage
    WIN32 MACOSX_BUNDLE
    main.cpp
    restycagewindow.cpp
    restycagewindow.h
    restycagewindow.ui
    qjsonmodel.h qjsonmodel.cpp
    qutf8.h

    keyvaluedialog.h keyvaluedialog.cpp keyvaluedialog.ui
    keyvaluefiletextdialog.h keyvaluefiletextdialog.cpp keyvaluefiletextdialog.ui
    queryform.h queryform.cpp queryform.ui
    Resources.qrc
    environmentform.h environmentform.cpp environmentform.ui
    keyvaluehandler.h keyvaluehandler.cpp
    queryserializer.h queryserializer.cpp
    environmentserializer.h environmentserializer.cpp
    constants.h
    namedialog.h namedialog.cpp namedialog.ui
    query.h query.cpp
    paramvalue.h paramvalue.cpp
)

target_link_libraries(RestyCage
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)
target_link_libraries(RestyCage PRIVATE Qt6::Core)
target_link_libraries(RestyCage PRIVATE Qt6::Core)
target_link_libraries(RestyCage PRIVATE Qt6::Core)

include(GNUInstallDirs)

install(TARGETS RestyCage
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET RestyCage
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
