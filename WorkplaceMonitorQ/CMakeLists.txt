cmake_minimum_required(VERSION 3.16)
project(WorkplaceMonitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(OpenCV_DIR "C:/Qt/opencv-4.9.0-windows/opencv/build")
find_package(OpenCV REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)


qt_standard_project_setup()

qt_add_executable(WorkplaceMonitor
    main.cpp
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
    camera_thread.h
    camera_thread.cpp
)

# сярюмнбйю гюбхяхлняреи дкъ жекх: WorkplaceMonitor
target_link_libraries(
    # жекэ:
    WorkplaceMonitor

    # опхбъгйю ахакхнрей:
    PUBLIC Qt6::Widgets
    PRIVATE ${OpenCV_LIBS}
)
#C:/Qt/opencv-4.9.0-windows/opencv/build
