cmake_minimum_required (VERSION 3.8)

project ("reckless")

find_package(opengl REQUIRED)

include_directories("headers")
include_directories("support/platform/")
include_directories("support/CarbonHeaders/")
include_directories("support/Frameworks/")

add_subdirectory ("source")
