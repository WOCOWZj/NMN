cmake_minimum_required(VERSION 3.16)
project(NMN LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# ---------------------------------------------------------------------------
# 依赖定位
#   默认值对应本项目当前的开发机；换机器时用 -D 覆盖即可，无需改文件：
#     cmake -B build -GNinja ^
#           -DNMN_QT_DIR="C:/Qt/6.9.3/mingw_64" ^
#           -DNMN_OPENCV_DIR="C:/opencv/build/x64/mingw/lib" ^
#           -DNMN_TESSERACT_RUNTIME="C:/Program Files/Tesseract-OCR"
# ---------------------------------------------------------------------------
set(NMN_QT_DIR "D:/Qt/6.9.3/mingw_64" CACHE PATH "Qt6 安装前缀（含 bin/ lib/ include/）")
set(NMN_OPENCV_DIR "D:/opencv/opencv/build/x64/mingw/lib" CACHE PATH "OpenCV 的 CMake 配置目录")
set(NMN_TESSERACT_RUNTIME "D:/Tesseract-OCR" CACHE PATH "Tesseract 运行时目录（libtesseract-*.dll 与 tessdata）")

list(APPEND CMAKE_PREFIX_PATH "${NMN_QT_DIR}")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

find_package(OpenCV REQUIRED COMPONENTS world
             PATHS "${NMN_OPENCV_DIR}"
             NO_DEFAULT_PATH)

message(STATUS "Qt6    : ${Qt6_DIR}")
message(STATUS "OpenCV : ${OpenCV_DIR} (${OpenCV_VERSION})")
message(STATUS "Tesseract 运行时 : ${NMN_TESSERACT_RUNTIME}")

# ---------------------------------------------------------------------------
# Tesseract：直接链接 libtesseract（不再 QProcess 调 tesseract.exe）
#   头文件与导入库的来源见 third_party/README.md
# ---------------------------------------------------------------------------
set(NMN_TESSERACT_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/include")
set(NMN_TESSERACT_LIB     "${CMAKE_CURRENT_SOURCE_DIR}/third_party/lib/libtesseract.dll.a")

if(NOT EXISTS "${NMN_TESSERACT_LIB}")
    message(FATAL_ERROR
        "缺少 ${NMN_TESSERACT_LIB}\n"
        "请按 third_party/README.md 生成导入库后再配置。")
endif()

# ---------------------------------------------------------------------------
# 目标
# ---------------------------------------------------------------------------
file(GLOB_RECURSE NMN_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE NMN_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE NMN_FORMS   CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.ui")

# 头文件一并列入源文件：保证 AUTOMOC 能扫描到子目录里的 Q_OBJECT
add_executable(${PROJECT_NAME} WIN32 ${NMN_SOURCES} ${NMN_HEADERS} ${NMN_FORMS})

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${NMN_TESSERACT_INCLUDE}")

target_compile_definitions(${PROJECT_NAME} PRIVATE TESS_IMPORTS)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    ${OpenCV_LIBS}
    "${NMN_TESSERACT_LIB}")

# MSVC 下强制 UTF-8，避免中文源码乱码
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

# ---------------------------------------------------------------------------
# 运行期 DLL
#   OpenCV 与 Tesseract 的 dll 都拷到 exe 同级目录，省得手动配 PATH
#   （Qt 的 dll 仍需 windeployqt 或对应 Qt 的 bin 目录在 PATH 中）
# ---------------------------------------------------------------------------
get_filename_component(NMN_OPENCV_BIN "${NMN_OPENCV_DIR}/../bin" ABSOLUTE)
file(GLOB NMN_OPENCV_DLLS "${NMN_OPENCV_BIN}/*.dll")
if(NMN_OPENCV_DLLS)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${NMN_OPENCV_DLLS} "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMENT "拷贝 OpenCV 运行库到输出目录")
endif()

file(GLOB NMN_TESSERACT_DLLS "${NMN_TESSERACT_RUNTIME}/*.dll")
if(NMN_TESSERACT_DLLS)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${NMN_TESSERACT_DLLS} "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMENT "拷贝 Tesseract 运行库到输出目录")
else()
    message(WARNING "在 ${NMN_TESSERACT_RUNTIME} 下没找到 DLL，运行时需要自行把该目录加入 PATH")
endif()
