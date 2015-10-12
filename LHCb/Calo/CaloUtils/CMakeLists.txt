################################################################################
# Package: CaloUtils
################################################################################
gaudi_subdir(CaloUtils v10r5p1)

gaudi_depends_on_subdirs(Calo/CaloInterfaces
                         Det/CaloDet
                         Event/DigiEvent
                         Event/L0Event
                         Event/MCEvent
                         Event/PhysEvent
                         Event/RecEvent
                         GaudiAlg
                         Kernel/LHCbMath)

find_package(AIDA)

gaudi_add_library(CaloUtils
                  src/*.cpp
                  PUBLIC_HEADERS CaloUtils
                  INCLUDE_DIRS AIDA Event/DigiEvent
                  LINK_LIBRARIES CaloInterfaces CaloDetLib L0Event MCEvent PhysEvent RecEvent GaudiAlgLib LHCbMathLib)

gaudi_add_dictionary(CaloUtils
                     dict/CaloUtilsDict.h
                     dict/CaloUtils.xml
                     INCLUDE_DIRS Event/DigiEvent
                     LINK_LIBRARIES CaloInterfaces CaloDetLib L0Event MCEvent PhysEvent RecEvent GaudiAlgLib LHCbMathLib CaloUtils
                     OPTIONS " -U__MINGW32__ ")

gaudi_install_python_modules()

