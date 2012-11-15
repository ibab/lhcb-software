################################################################################
# Package: BcmDet
################################################################################
gaudi_subdir(BcmDet v1r3)

gaudi_depends_on_subdirs(Det/DetDesc
                         Det/DetDescCnv
                         GaudiKernel)

gaudi_add_library(BcmDetLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS BcmDet
                  LINK_LIBRARIES DetDescLib DetDescCnvLib GaudiKernel)

gaudi_add_module(BcmDet
                 src/component/*.cpp
                 LINK_LIBRARIES DetDescLib DetDescCnvLib GaudiKernel BcmDetLib)

gaudi_add_dictionary(BcmDet
                     dict/BcmDetDict.h
                     dict/BcmDetDict.xml
                     LINK_LIBRARIES DetDescLib DetDescCnvLib GaudiKernel BcmDetLib
                     OPTIONS "-U__MINGW32__")

