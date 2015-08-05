################################################################################
# Package: SuperChic
################################################################################
gaudi_subdir(SuperChic v1r0)

gaudi_depends_on_subdirs(Gen/Generators)

include_directories(SuperChic)

gaudi_add_library(SuperChic
                  src/*.f
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES GeneratorsLib)

