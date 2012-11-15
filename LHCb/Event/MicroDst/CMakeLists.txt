################################################################################
# Package: MicroDst
################################################################################
gaudi_subdir(MicroDst v1r0)

gaudi_depends_on_subdirs(Event/PhysEvent
                         GaudiKernel
                         GaudiPython)

gaudi_add_library(MicroDstLib
                  src/*.cpp
                  PUBLIC_HEADERS Kernel
                  LINK_LIBRARIES PhysEvent GaudiKernel GaudiPythonLib)

gaudi_add_dictionary(MicroDst
                     dict/MicroDstDict.h
                     dict/MicroDstDict.xml
                     LINK_LIBRARIES PhysEvent GaudiKernel GaudiPythonLib MicroDstLib
                     OPTIONS "-U__MINGW32__")

gaudi_install_python_modules()

