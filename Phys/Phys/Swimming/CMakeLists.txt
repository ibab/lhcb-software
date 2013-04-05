################################################################################
# Package: Swimming
################################################################################
gaudi_subdir(Swimming v2r9)

gaudi_depends_on_subdirs(GaudiConf
                         GaudiKernel
                         Kernel/LHCbKernel
                         Phys/TisTosTobbing)

gaudi_add_library(Swimming
                  src/functions.cpp
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES GaudiKernel LHCbKernel TisTosTobbingLib)

gaudi_add_dictionary(Swimming
                     dict/functions.h
                     dict/functions.xml
                     LINK_LIBRARIES GaudiKernel LHCbKernel TisTosTobbingLib Swimming
                     OPTIONS "-U__MINGW32__")

gaudi_install_python_modules()

