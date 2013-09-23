# - Try to find ThePEG
# Defines:
#
#  ThePEG_FOUND
#  THEPEG_INCLUDE_DIR
#  THEPEG_INCLUDE_DIRS (not cached)
#  THEPEG_LIBRARY
#  THEPEG_LIBRARIES (not cached)
#  THEPEG_LIBRARY_DIRS (not cached)

find_library(THEPEG_LIBRARY NAMES ThePEG PATH_SUFFIXES ThePEG)

find_path(THEPEG_INCLUDE_DIR ThePEG/Repository/EventGenerator.h)

mark_as_advanced(THEPEG_INCLUDE_DIR THEPEG_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set ThePEG_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ThePEG DEFAULT_MSG THEPEG_INCLUDE_DIR THEPEG_LIBRARY)

set(THEPEG_LIBRARIES ${THEPEG_LIBRARY})
get_filename_component(THEPEG_LIBRARY_DIRS ${THEPEG_LIBRARY} PATH)

set(THEPEG_INCLUDE_DIRS ${THEPEG_INCLUDE_DIR})

mark_as_advanced(ThePEG_FOUND)
