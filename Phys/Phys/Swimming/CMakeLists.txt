################################################################################
# Package: Swimming
################################################################################
gaudi_subdir(Swimming v3r8)

gaudi_depends_on_subdirs(Event/SwimmingEvent
                         GaudiConf
                         GaudiKernel
                         Kernel/LHCbKernel
                         Det/DetCond
                         Phys/TisTosTobbing)

gaudi_add_library(Swimming
                  src/Lib/*.cpp
                  PUBLIC_HEADERS Swimming
                  LINK_LIBRARIES GaudiKernel LHCbKernel TisTosTobbingLib)

gaudi_add_module(SwimmingComponent
                 src/Component/*.cpp
                 LINK_LIBRARIES GaudiKernel LHCbKernel TisTosTobbingLib)

gaudi_add_dictionary(Swimming
                     dict/functions.h
                     dict/functions.xml
                     LINK_LIBRARIES GaudiKernel LHCbKernel TisTosTobbingLib Swimming
                     OPTIONS "-U__MINGW32__")

gaudi_add_dictionary(SwimmingHacks
                     dict/hacks.h
                     dict/hacks.xml
                     LINK_LIBRARIES GaudiKernel LHCbKernel TisTosTobbingLib Swimming
                     OPTIONS "-U__MINGW32__")

gaudi_install_python_modules()

