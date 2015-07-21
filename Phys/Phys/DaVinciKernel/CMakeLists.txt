################################################################################
# Package: DaVinciKernel
################################################################################
gaudi_subdir(DaVinciKernel v12r2)

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

gaudi_add_module(DaVinciKernel
                 src/component/*.cpp
                 LINK_LIBRARIES PhysEvent GaudiAlgLib PartPropLib RelationsLib DaVinciInterfacesLib)

gaudi_add_dictionary(DaVinciKernel
                     dict/DaVinciKernelDict.h
                     dict/DaVinciKernelDict.xml
                     INCLUDE_DIRS Phys/DaVinciUtils
                     LINK_LIBRARIES PhysEvent GaudiAlgLib PartPropLib RelationsLib DaVinciInterfacesLib DaVinciKernelLib
                     OPTIONS "-U__MINGW32__")

gaudi_add_executable(Combiner1
                     tests/combiner1.cpp  
                     tests/Combine.cpp
                     INCLUDE_DIRS   Event
                     LINK_LIBRARIES GaudiKernel LHCbKernel PhysEvent )

gaudi_add_executable(Combiner2
                     tests/combiner2.cpp  
                     tests/Combine.cpp
                     INCLUDE_DIRS   Event
                     LINK_LIBRARIES GaudiKernel LHCbKernel PhysEvent )

gaudi_add_executable(Combiner3
                     tests/combiner3.cpp  
                     tests/Combine.cpp
                     INCLUDE_DIRS   Event
                     LINK_LIBRARIES GaudiKernel LHCbKernel PhysEvent )
