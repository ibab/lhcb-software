################################################################################
# Package: FTDigitisation
################################################################################
gaudi_subdir(FTDigitisation v3r0)

gaudi_depends_on_subdirs(Det/FTDet
                         Event/FTEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         GaudiAlg
                         GaudiKernel)

find_package(GSL)

gaudi_add_module(FTDigitisation
                 src/*.cpp
                 INCLUDE_DIRS GSL Event/FTEvent
                 LINK_LIBRARIES GSL FTDetLib LinkerEvent MCEvent GaudiAlgLib GaudiKernel)

