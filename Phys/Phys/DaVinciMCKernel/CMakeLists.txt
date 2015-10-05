################################################################################
# Package: DaVinciMCKernel
################################################################################
gaudi_subdir(DaVinciMCKernel v4r5)

gaudi_depends_on_subdirs(Associators/MCAssociators
                         Event/GenEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/PhysEvent
                         Event/RecEvent
                         Phys/DaVinciInterfaces
                         Phys/LoKiCore)

gaudi_add_library(DaVinciMCKernelLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS Kernel
                  INCLUDE_DIRS Associators/MCAssociators
                  LINK_LIBRARIES GenEvent LinkerEvent MCEvent PhysEvent RecEvent DaVinciInterfacesLib LoKiCoreLib)

gaudi_add_dictionary(DaVinciMCKernel
                     dict/DaVinciMCKernelDict.h
                     dict/DaVinciMCKernelDict.xml
                     INCLUDE_DIRS Associators/MCAssociators
                     LINK_LIBRARIES GenEvent LinkerEvent MCEvent PhysEvent RecEvent DaVinciInterfacesLib LoKiCoreLib DaVinciMCKernelLib
                     OPTIONS "-U__MINGW32__")

