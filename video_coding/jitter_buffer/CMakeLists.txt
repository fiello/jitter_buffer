cmake_minimum_required (VERSION 2.8)

project (jitter_buffer CXX)

# library project itself
add_library (${jitter_buffer_OUTPUT}
   STATIC
   source/jitter_buffer.cc
   source/jitter_buffer_impl.cc
   source/frame_buffer.cc
   source/frame_fragment.cc
)
target_link_libraries (${jitter_buffer_OUTPUT})


# unit tests for the library
set (jitter_buffer_tests_OUTPUT jitter_buffer_tests)

add_executable (${jitter_buffer_tests_OUTPUT}
   tests/main.cc
   tests/fixture_jitter_buffer.cc
   tests/test_jitter_buffer.cc
)

target_link_libraries(
   ${jitter_buffer_tests_OUTPUT}
   ${jitter_buffer_OUTPUT}
   ${logger_OUTPUT}
   ${thread_pool_OUTPUT}
   ${Boost_LIBRARIES}
)
