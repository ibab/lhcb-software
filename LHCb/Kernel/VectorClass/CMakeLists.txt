################################################################################
# Package: VectorClass
################################################################################
gaudi_subdir(VectorClass v1r16)

gaudi_depends_on_subdirs(GaudiPolicy)

gaudi_install_headers(VectorClass)

gaudi_add_library(VectorClassLib
                  src/*.cpp
                  PUBLIC_HEADERS VectorClass
                  INCLUDE_DIRS VectorClass)
