cmake_minimum_required (VERSION 3.8)

project ("reckless")

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories("headers/")
include_directories("support/vendor/")
include_directories("support/include/")
include_directories("support/platform/")
include_directories("support/CarbonHeaders/")
include_directories("support/Frameworks/")
include_directories("support/Frameworks/CarbonCore")
include_directories("support/Frameworks/InputSprocket")
include_directories("support/Frameworks/QD")
include_directories("support/Frameworks/DrawSprocket")
include_directories("support/Frameworks/HIDUtilities")

# Add source to this project's executable.
set (SOURCES
        # Compatibility wrappers
        compat/iShockXForce.c
        compat/MacMemory.c
        compat/TextUtils.c
        compat/NumberFormatting.c
        compat/Sound.c

        # Game source
        source/main.c
#        source/appleevents.c
        source/AGL_DSp.c
        source/error.c
        source/gameendseq.c
        source/gameframe.c
        source/gameinitexit.c
#        source/HIDAccess.c  # replaced with compat/HIDAccess.c
        source/high.c
        source/initexit.c
        source/input.c
        source/interface.c
        source/IsPortListValid.c
        source/objectCollision.c
        source/objectControl.c
        source/objectPhysics.c
        source/objects.c
        source/packs.c
        source/particlefx.c
        source/pause.c
        source/preferences.c
        source/random.c
        source/register.c
        source/renderframe.c
#        source/renderframezoom.c  # looks like deadcode -- implemented in `renderfame.c`
        source/rle.c
        source/roaddraw.c
        source/screen.c
        source/screenfx.c
        source/sound.c
        source/sprites.c
        source/textfx.c
        compat/OSUtils.c)

add_executable(reckless ${SOURCES})
add_dependencies(reckless OpenGL::OpenGL)

target_link_libraries(reckless
        m
        #		-m32
        OpenGL::OpenGL
        ${SDL2_LIBRARIES})

target_compile_options(reckless PRIVATE
        #		-m32
        -Wno-multichar
        -Wno-builtin-declaration-mismatch
        )

target_compile_definitions(reckless PRIVATE
        PRAGMA_ONCE
        DARWIN
        __LITTLE_ENDIAN__
        __MACH__
        __APPLE_CC__
        __i386__
        __cmath__
        TARGET_API_MAC_OSX
        natural_t=uint32_t
        integer_t=int32_t

        # ignored
        __OPENSCRIPTING__
        __PRINT__
        __NAVIGATIONSERVICES__
        __COMMONPANELS__
        __HTMLRENDERING__
        __SPEECHRECOGNITION__
        __SECURITYHI__
        __HELP__
        __IMAGECAPTURE__
        )

# TODO: Add tests and install targets if needed.
