cmake_minimum_required(VERSION 3.19)
project(RestyCage LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network Sql)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(RestyCage
    WIN32 MACOSX_BUNDLE
    main.cpp
    forms/restycagewindow.cpp
    forms/restycagewindow.h
    forms/restycagewindow.ui
    qjsonmodel.h qjsonmodel.cpp
    qutf8.h
    dialogs/keyvaluedialog.h dialogs/keyvaluedialog.cpp dialogs/keyvaluedialog.ui
    dialogs/keyvaluefiletextdialog.h dialogs/keyvaluefiletextdialog.cpp dialogs/keyvaluefiletextdialog.ui
    forms/queryform.h forms/queryform.cpp forms/queryform.ui
    Resources.qrc
    forms/environmentform.h forms/environmentform.cpp forms/environmentform.ui
    keyvaluehandler.h keyvaluehandler.cpp


    constants.h
    dialogs/namedialog.h dialogs/namedialog.cpp dialogs/namedialog.ui
    db/query.h db/query.cpp
    db/paramvalue.h db/paramvalue.cpp
    db/db.h db/db.cpp
    db/environment.h db/environment.cpp
    db/collection.h db/collection.cpp
    dialogs/collectiondialog.h dialogs/collectiondialog.cpp dialogs/collectiondialog.ui
)

target_link_libraries(RestyCage
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
        Qt6::Sql
)
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
