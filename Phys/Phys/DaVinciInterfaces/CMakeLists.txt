################################################################################
# Package: DaVinciInterfaces
################################################################################
gaudi_subdir(DaVinciInterfaces v2r25)

gaudi_depends_on_subdirs(Calo/CaloUtils
                         Event/PhysEvent
                         GaudiAlg
                         GaudiKernel
                         Phys/DaVinciTypes)

gaudi_add_library(DaVinciInterfacesLib
                  src/*.cpp
                  PUBLIC_HEADERS Kernel
                  LINK_LIBRARIES CaloUtils PhysEvent GaudiKernel DaVinciTypesLib GaudiAlgLib)

gaudi_add_dictionary(DaVinciInterfaces
                     dict/DaVinciInterfacesDict.h
                     dict/DaVinciInterfacesDict.xml
                     LINK_LIBRARIES CaloUtils PhysEvent GaudiKernel DaVinciTypesLib DaVinciInterfacesLib
                     OPTIONS "-U__MINGW32__")

