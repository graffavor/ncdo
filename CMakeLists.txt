cmake_minimum_required(VERSION 3.2)

project(ncdo VERSION 1.0 DESCRIPTION "NCurses tool for managing todo.txt todo lists")

set(SOURCE_FILES src/main.cpp include/term.h include/config.h src/config.cpp src/term.cpp src/td/todo.cpp include/td/todo.h include/hook.h include/ui/task_list.h src/ui/task_list.cpp include/ui.h include/ui/editor.h src/ui/editor.cpp include/util.h include/ui/component.h src/ui/component.cpp include/ui/data_list.h src/ui/data_list.cpp include/ui/charcodes.h include/ui/menu.h src/ui/menu.cpp include/ui/help.h src/ui/help.cpp include/td/task.h include/includes.h include/ui/exit_popup.h src/ui/exit_popup.cpp include/ui/textutil.h src/ui/textutil.cpp)

if (APPLE)
    set(CMAKE_PREFIX_PATH "/usr/local/opt/ncurses")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(Boost_USE_STATIC_LIBS ON)
set(CURSES_NEED_WIDE ON)

include(GNUInstallDirs)

find_package(Curses REQUIRED)
find_package(Boost COMPONENTS program_options date_time REQUIRED)

include_directories(include ${CURSES_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
include_directories(${CURSES_INCLUDE_DIR})

add_compile_options(-std=c++14 -Wall -Wextra -pedantic)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE include src ${CURSES_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES} ${Boost_LIBRARIES})