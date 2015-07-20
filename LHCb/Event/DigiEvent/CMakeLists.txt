################################################################################
# Package: DigiEvent
################################################################################
gaudi_subdir(DigiEvent v4r4)

gaudi_depends_on_subdirs(GaudiObjDesc
                         Kernel/LHCbKernel
                         Kernel/LHCbMath)

find_package(Boost)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

god_build_dictionary(xml/*.xml
                     EXTEND dict/lcgDict.h
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES Boost LHCbKernel LHCbMathLib)

