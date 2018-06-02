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
	renderframezoom.c
	rle.c
	roaddraw.c
	screen.c
	screenfx.c
	sound.c
	sprites.c
	textfx.c
)

add_executable (reckless "main.c" ${SOURCES})

# TODO: Add tests and install targets if needed.
