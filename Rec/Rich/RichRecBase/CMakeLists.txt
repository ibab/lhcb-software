################################################################################
# Package: RichRecBase
################################################################################
gaudi_subdir(RichRecBase v4r21)

gaudi_depends_on_subdirs(Det/RichDet
                         Event/RecEvent
                         GaudiObjDesc
                         Rich/RichKernel)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_library(RichRecBase
                  src/event/*.cpp
                  src/base/*.cpp
                  PUBLIC_HEADERS RichRecBase
                  LINK_LIBRARIES RichDetLib RecEvent RichKernelLib)

gaudi_add_dictionary(RichRecBase
                     dict/RichRecBaseDict.h
                     dict/RichRecBaseDict.xml
                     LINK_LIBRARIES RichDetLib RecEvent RichKernelLib RichRecBase
                     OPTIONS "-U__MINGW32__ -DBOOST_DISABLE_ASSERTS")

