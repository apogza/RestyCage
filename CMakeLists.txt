cmake_minimum_required(VERSION 3.19)
project(RestyCage LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network Sql)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(RestyCage
    WIN32 MACOSX_BUNDLE
    main.cpp
    forms/resty_cage_window.cpp
    forms/resty_cage_window.h
    forms/resty_cage_window.ui
    models/treeview_item.h models/treeview_item.cpp
    dialogs/key_value_dialog.h dialogs/key_value_dialog.cpp dialogs/key_value_dialog.ui
    dialogs/key_value_file_text_dialog.h dialogs/key_value_file_text_dialog.cpp dialogs/key_value_file_text_dialog.ui
    forms/query_form.h forms/query_form.cpp forms/query_form.ui
    Resources.qrc
    forms/environment_form.h forms/environment_form.cpp forms/environment_form.ui
    key_value_handler.h key_value_handler.cpp
    constants.h
    dialogs/name_dialog.h dialogs/name_dialog.cpp dialogs/name_dialog.ui
    db/query.h db/query.cpp
    db/param_value.h db/param_value.cpp
    db/db.h db/db.cpp
    db/environment.h db/environment.cpp
    db/collection.h db/collection.cpp
    dialogs/collection_dialog.h dialogs/collection_dialog.cpp dialogs/collection_dialog.ui
    db/basic_query_auth.h db/basic_query_auth.cpp
    db/bearer_query_auth.h db/bearer_query_auth.cpp
    db/query_binary_body.h db/query_binary_body.cpp
    db/query_raw_body.h db/query_raw_body.cpp
    network_helper.h network_helper.cpp
)

target_link_libraries(RestyCage
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
        Qt::Sql
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
