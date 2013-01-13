cmake_minimum_required (VERSION 2.8)

project (logger CXX)

add_library (${logger_OUTPUT} 
   STATIC
   logger_impl.cc
)

target_link_libraries (
   ${logger_OUTPUT}   
)
