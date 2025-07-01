cmake_minimum_required(VERSION 3.19)
project(RestyCage LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

qt_add_executable(RestyCage
    WIN32 MACOSX_BUNDLE
    main.cpp
    restycagewindow.cpp
    restycagewindow.h
    restycagewindow.ui
    qjsonmodel.h qjsonmodel.cpp
    qutf8.h
)

target_link_libraries(RestyCage
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)

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
