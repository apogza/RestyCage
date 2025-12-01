cmake_minimum_required(VERSION 3.19)
project(RestyCage LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network Sql)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(RestyCage
    main.cpp
    Resources.qrc
    forms/resty_cage_window.cpp
    forms/resty_cage_window.h
    forms/resty_cage_window.ui
    models/treeview_item.h models/treeview_item.cpp
    dialogs/key_value_dialog.h dialogs/key_value_dialog.cpp dialogs/key_value_dialog.ui
    dialogs/key_value_file_text_dialog.h dialogs/key_value_file_text_dialog.cpp dialogs/key_value_file_text_dialog.ui
    forms/query_form.h forms/query_form.cpp forms/query_form.ui
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
    ui/json_highlighter.h ui/json_highlighter.cpp
    db/migrations/migration.h db/migrations/migration.cpp
    db/migrations/add_uid_migration.h db/migrations/add_uid_migration.cpp
)

target_link_libraries(RestyCage
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
        Qt::Sql
)
target_link_libraries(RestyCage PRIVATE Qt6::Core)

