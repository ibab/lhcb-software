################################################################################
# Package: SuperChic
################################################################################
gaudi_subdir(SuperChic v1r0)

gaudi_depends_on_subdirs(Gen/Generators)

gaudi_add_library(SuperChic
                  src/*.f
                  PUBLIC_HEADERS SuperChic
                  LINK_LIBRARIES GeneratorsLib)

