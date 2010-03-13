project(hikki)

add_subdirectory(markdown)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include_directories(${KDE4_INCLUDES})
set(src main.cpp
        mainwindow.cpp
        note.cpp
        notebook.cpp
        webkitpart.cpp
        proxyaction.cpp)
kde4_add_executable(hikki ${src})

target_link_libraries(hikki markdown ${KDE4_KDEUI_LIBS} kparts ktexteditor QtWebKit)
install(TARGETS hikki ${INSTALL_TARGETS_DEFAULT_ARGS})
