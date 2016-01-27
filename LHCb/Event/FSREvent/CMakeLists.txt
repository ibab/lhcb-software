################################################################################
# Package: FSREvent
################################################################################
gaudi_subdir(FSREvent v1r2)

gaudi_depends_on_subdirs(GaudiKernel
                         GaudiObjDesc
                         Kernel/LHCbKernel)

find_package(Boost)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

god_build_dictionary(xml/*.xml
                     EXTEND xml/lcgdict/lcg_selection.h
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES Boost GaudiKernel LHCbKernel)

