################################################################################
# Package: KalmanFilter
################################################################################
gaudi_subdir(KalmanFilter v1r15)

gaudi_depends_on_subdirs(Calo/CaloUtils
                         Event/PhysEvent
                         Event/RecEvent
                         GaudiKernel
                         Kernel/LHCbMath
                         Phys/DaVinciInterfaces
                         Phys/DaVinciTypes)

gaudi_add_library(KalmanFilter
                  src/*.cpp
                  PUBLIC_HEADERS KalmanFilter Kernel
                  LINK_LIBRARIES CaloUtils PhysEvent RecEvent GaudiKernel LHCbMathLib DaVinciInterfacesLib DaVinciTypesLib)

gaudi_add_dictionary(KalmanFilter
                     dict/KalmanFilterDict.h
                     dict/KalmanFilter.xml
                     LINK_LIBRARIES CaloUtils PhysEvent RecEvent GaudiKernel LHCbMathLib DaVinciInterfacesLib DaVinciTypesLib KalmanFilter
                     OPTIONS "-U__MINGW32__")

