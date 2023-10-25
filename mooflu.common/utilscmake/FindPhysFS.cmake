find_path(PHYSFS_INCLUDE_DIR physfs.h
  HINTS
    ENV PHYSFSDIR
  PATH_SUFFIXES include/physfs include
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /opt
)

find_library(PHYSFS_LIBRARY
  NAMES physfs physfs-static # the one bundled with cmake does not look for static version
  HINTS
    ENV PHYSFSDIR
  PATH_SUFFIXES lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /opt
)
