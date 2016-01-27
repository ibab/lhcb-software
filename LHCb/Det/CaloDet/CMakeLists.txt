################################################################################
# Package: CaloDet
################################################################################
gaudi_subdir(CaloDet v10r21)

gaudi_depends_on_subdirs(Calo/CaloKernel
                         Det/DetDesc
                         GaudiAlg
                         Kernel/LHCbKernel)

gaudi_add_library(CaloDetLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS CaloDet
                  INCLUDE_DIRS Calo/CaloKernel
                  LINK_LIBRARIES DetDescLib GaudiAlgLib LHCbKernel)

gaudi_add_module(CaloDet
                 src/component/*.cpp
                 INCLUDE_DIRS Calo/CaloKernel
                 LINK_LIBRARIES DetDescLib GaudiAlgLib LHCbKernel CaloDetLib)

gaudi_add_dictionary(CaloDet
                     dict/CaloDetDict.h
                     dict/CaloDetDict.xml
                     INCLUDE_DIRS Calo/CaloKernel
                     LINK_LIBRARIES DetDescLib GaudiAlgLib LHCbKernel CaloDetLib
                     OPTIONS "-U__MINGW32__")

gaudi_env(SET CALODETOPTS \${CALODETROOT}/options)

