cmake_minimum_required(VERSION 3.14)
project(qtservice)

option(${PROJECT_NAME}_BUILD_SHARED "Build ${PROJECT_NAME} as shared libraries" true)
option(USE_QT_5 "Use Qt5" YES)

set(${PROJECT_NAME}-qt-components QtCore QtNetwork)

add_definitions(-DUNICODE)

if(USE_QT_5)
    find_package(Qt5 REQUIRED COMPONENTS Core Network)
else()
    set(QT_DONT_USE_QTGUI TRUE)
    find_package(Qt4 COMPONENTS ${${PROJECT_NAME}-qt-components})
    include(${QT_USE_FILE})
endif()

if(MSVC AND BUILD_SHARED_LIBS)
    add_definitions("/DQT_QTSERVICE_EXPORT=__declspec(dllexport)")
endif()

# generate moc files
set(CMAKE_AUTOMOC ON)

set(${PROJECT_NAME}-sources qtservice.cpp)
if(WIN32)
    set(${PROJECT_NAME}-sources ${${PROJECT_NAME}-sources} qtservice_win.cpp)
elseif(UNIX)
    set(${PROJECT_NAME}-sources ${${PROJECT_NAME}-sources} qtservice_unix.cpp qtunixsocket.cpp qtunixserversocket.cpp)
endif()

# for #include of generated moc files
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# for #include of moc headers from binary tree
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(${PROJECT_NAME} ${${PROJECT_NAME}-sources})

if(USE_QT_5)
    target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network)
else()
    target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})
endif()

install(TARGETS ${PROJECT_NAME}
    EXPORT flow-LibraryDependencies
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES qtservice.h QtServiceBase QtServiceController DESTINATION ${QT_INCLUDE_DIR}/${PROJECT_NAME})

if(UNIX)
    install(FILES qtunixserversocket.h qtunixsocket.h DESTINATION ${QT_INCLUDE_DIR}/${PROJECT_NAME})
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ProjectConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake @ONLY)

install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    DESTINATION share/cmake/${PROJECT_NAME} COMPONENT Development)

install(EXPORT flow-LibraryDependencies DESTINATION share/cmake/${PROJECT_NAME} COMPONENT Development)

set(QTSERVICE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    CACHE INTERNAL "${PROJECT_NAME} include directories"
)

set(QTSERVICE_LIBRARIES
    ${PROJECT_NAME}
    CACHE INTERNAL "${PROJECT_NAME} libraries"
)

