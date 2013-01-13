cmake_minimum_required (VERSION 2.8)

project (VideoStreamingTestTask CXX)
set (project_ROOT ${PROJECT_SOURCE_DIR})

if (WIN32)
   find_package (Boost REQUIRED)
else ()
   find_package (Boost COMPONENTS REQUIRED thread)
   find_package (Boost COMPONENTS REQUIRED date_time)
endif ()

set (OUTPUT_DIRECTORY ${project_ROOT}/out_${CMAKE_SYSTEM_NAME})
set (LIBRARY_OUTPUT_PATH ${OUTPUT_DIRECTORY})
set (EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIRECTORY})

set (COMMON_INCLUDE_DIRECTORIES
   ${project_ROOT}
   ${project_ROOT}/tools
   ${project_ROOT}/third_party/gmock	# need this to compile with gmock properly
   ${Boost_INCLUDE_DIRS}
)

set (COMMON_LINK_DIRECTORIES
   ${Boost_LIBRARY_DIRS}
   ${LIBRARY_OUTPUT_PATH}
)

include_directories (${COMMON_INCLUDE_DIRECTORIES})
link_directories (${COMMON_LINK_DIRECTORIES})

set (logger_OUTPUT logger)
set (jitter_buffer_OUTPUT jitter_buffer)

add_subdirectory (video_coding)
add_subdirectory (tools/logger)

