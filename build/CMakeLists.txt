# Минимальная версия CMake, необходимая для работы файла
cmake_minimum_required(VERSION 3.10)

# Определение проекта
project(AppServer)

# Добавление исполняемого файла, созданного из файла main.cpp
add_executable(AppServer ../source/main.cpp)

# Опционально: если нужно подключить библиотеку (например, как с Boost)
# target_link_libraries(AppServer PRIVATE Boost::program_options)