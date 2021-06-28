project(knut-lsp LANGUAGES CXX)

set(PROJECT_SOURCES
    client.h
    client.cpp
    clientbackend.h
    clientbackend.cpp
    json.h
    lsp_utils.h
    notificationmessage.h
    notificationmessage_json.h
    notifications.h
    requestmessage.h
    requestmessage_json.h
    requests.h
    types.h
    types_json.h
    types_json.cpp
)

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME}
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    Qt::Core
)
target_include_directories(${PROJECT_NAME}
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..
)