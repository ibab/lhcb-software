################################################################################# Package: LPair
################################################################################
gaudi_subdir(LPair v1r0)

gaudi_depends_on_subdirs(Gen/Generators)

include_directories(LPair)

gaudi_add_library(LPair
                  src/*.[fF]
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES GeneratorsLib)

