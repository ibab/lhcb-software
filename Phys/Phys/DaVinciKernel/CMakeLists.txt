################################################################################
# Package: DaVinciKernel
################################################################################
gaudi_subdir(DaVinciKernel v11r1)

gaudi_depends_on_subdirs(Event/PhysEvent
                         GaudiAlg
                         Kernel/PartProp
                         Kernel/Relations
                         Phys/DaVinciInterfaces
                         Phys/DaVinciUtils)

gaudi_add_library(DaVinciKernelLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS Kernel
                  INCLUDE_DIRS Phys/DaVinciUtils
                  LINK_LIBRARIES PhysEvent GaudiAlgLib PartPropLib RelationsLib DaVinciInterfacesLib)

gaudi_add_dictionary(DaVinciKernel
                     dict/DaVinciKernelDict.h
                     dict/DaVinciKernelDict.xml
                     INCLUDE_DIRS Phys/DaVinciUtils
                     LINK_LIBRARIES PhysEvent GaudiAlgLib PartPropLib RelationsLib DaVinciInterfacesLib DaVinciKernelLib
                     OPTIONS "-U__MINGW32__")

