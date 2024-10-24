cmake_minimum_required(VERSION 3.14)
project(qtservice)

option(${PROJECT_NAME}_BUILD_SHARED "Build ${PROJECT_NAME} as shared libraries" ON)
option(USE_QT_5 "Use Qt5" ON)

set(${PROJECT_NAME}_QT_COMPONENTS Core Network)

add_definitions(-DUNICODE)

if(USE_QT_5)
    find_package(Qt5 REQUIRED COMPONENTS ${${PROJECT_NAME}_QT_COMPONENTS})
else()
    set(QT_DONT_USE_QTGUI TRUE)
    find_package(Qt4 COMPONENTS ${${PROJECT_NAME}_QT_COMPONENTS})
    include(${QT_USE_FILE})
endif()

if(MSVC AND BUILD_SHARED_LIBS)
    add_definitions("/DQT_QTSERVICE_EXPORT=__declspec(dllexport)")
endif()

set(CMAKE_AUTOMOC ON)

set(SOURCES
    src/qtservice.cpp
    src/qtservice_unix.cpp
    src/qtunixsocket.cpp
    src/qtunixserversocket.cpp
)

set(HEADERS
    src/qtservice.h
    src/qtservice_p.h
    src/qtunixsocket.h
    src/qtunixserversocket.h
)

if(WIN32)
    list(APPEND SOURCES src/qtservice_win.cpp)
    target_link_libraries(QtService PRIVATE user32)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_library(QtService STATIC ${SOURCES} ${HEADERS})

if(USE_QT_5)
    target_link_libraries(QtService Qt5::Core Qt5::Network)
else()
    target_link_libraries(QtService ${QT_LIBRARIES})
endif()

install(TARGETS QtService
    EXPORT qtservice-LibraryDependencies
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES src/qtservice.h src/QtServiceBase src/QtServiceController
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

if(UNIX)
    install(FILES src/qtunixserversocket.h src/qtunixsocket.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ProjectConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    DESTINATION share/cmake/${PROJECT_NAME} COMPONENT Development)

install(EXPORT qtservice-LibraryDependencies
    DESTINATION share/cmake/${PROJECT_NAME} COMPONENT Development)

set(QTSERVICE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    CACHE INTERNAL "${PROJECT_NAME} include directories"
)

set(QTSERVICE_LIBRARIES
    QtService
    CACHE INTERNAL "${PROJECT_NAME} libraries"
)
