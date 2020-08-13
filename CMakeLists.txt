cmake_minimum_required (VERSION 3.8)

project ("reckless")

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

include_directories("headers/")
include_directories("vendor/")
include_directories("support/include/")
include_directories("support/platform/")
include_directories("support/CarbonHeaders/")
include_directories("support/Frameworks/")
include_directories("support/Frameworks/CarbonCore")
include_directories("support/Frameworks/InputSprocket")
include_directories("support/Frameworks/QD")
include_directories("support/Frameworks/DrawSprocket")
include_directories("support/Frameworks/HIDUtilities")

add_subdirectory ("source")
