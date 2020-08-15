# CMakeList.txt : CMake project for RecklessDriving, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
set (SOURCES
	AGL_DSp.c
	# appleevents.c
	error.c
	gameendseq.c
	gameframe.c
	gameinitexit.c
	HIDAccess.c
	high.c
	initexit.c
	input.c
	interface.c
	IsPortListValid.c
	objectCollision.c
	objectControl.c
	objectPhysics.c
	objects.c
	packs.c
	particlefx.c
	pause.c
	preferences.c
	random.c
	register.c
	renderframe.c
#	renderframezoom.c  # looks like deadcode -- implemented in `renderfame.c`
	rle.c
	roaddraw.c
	screen.c
	screenfx.c
	sound.c
	sprites.c
	textfx.c
)

add_executable (reckless "main.c" ${SOURCES})
add_dependencies(reckless OpenGL::OpenGL)

target_link_libraries(reckless
		m
#		-m32
		OpenGL::OpenGL)

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
