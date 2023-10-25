# Find libRocket

find_library(ROCKETCORE_LIBRARY
	NAMES RocketCore
	PATH_SUFFIXES lib64 lib
	PATHS
)

find_library(ROCKETCORE_LIBRARY_DEBUG
	NAMES RocketCore_d
	PATH_SUFFIXES lib64 lib
	PATHS
)

find_library(ROCKETCONTROLS_LIBRARY
	NAMES RocketControls
	PATH_SUFFIXES lib64 lib
	PATHS
)

find_library(ROCKETCONTROLS_LIBRARY_DEBUG
	NAMES RocketControls_d
	PATH_SUFFIXES lib64 lib
	PATHS
)

find_library(ROCKETDEBUGGER_LIBRARY
	NAMES RocketDebugger
	PATH_SUFFIXES lib64 lib
	PATHS
)

find_library(ROCKETDEBUGGER_LIBRARY_DEBUG
	NAMES RocketDebugger_d
	PATH_SUFFIXES lib64 lib
	PATHS
)

set(ROCKET_LIBRARY
	optimized ${ROCKETCONTROLS_LIBRARY}
	optimized ${ROCKETCORE_LIBRARY}
	optimized ${ROCKETDEBUGGER_LIBRARY}
	debug ${ROCKETCONTROLS_LIBRARY_DEBUG}
	debug ${ROCKETCORE_LIBRARY_DEBUG}
	debug ${ROCKETDEBUGGER_LIBRARY_DEBUG}
)

# Find ogg/vorbis

find_library(OGG_LIBRARY
	NAMES ogg libogg libogg_static
	PATH_SUFFIXES lib64 lib
	PATHS
)

find_library(VORBIS_LIBRARY
	NAMES vorbis libvorbis libvorbis_static
	PATH_SUFFIXES lib64 lib
	PATHS
)

find_library(VORBISFILE_LIBRARY
	NAMES vorbisfile libvorbisfile libvorbisfile_static
	PATH_SUFFIXES lib64 lib
	PATHS
)
