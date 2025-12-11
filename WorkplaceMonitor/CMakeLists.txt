cmake_minimum_required(VERSION 3.16)

project(WorkplaceMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ¬ключаем автоматическую обработку Qt (moc, ui, rcc)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# --- Qt ---
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# --- OpenCV ---
set(OpenCV_DIR "C:/Qt/OpenCV-MinGW-Build-OpenCV-4.5.5-x64")  # ѕ–ќ¬≈–№, что путь точно существует!
find_package(OpenCV REQUIRED)

add_executable(WorkplaceMonitor
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    camera_thread.h
    camera_thread.cpp
)

target_link_libraries(WorkplaceMonitor
    Qt6::Widgets
    ${OpenCV_LIBS}
)
