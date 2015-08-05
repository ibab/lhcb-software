################################################################################
# Package: PatChecker
################################################################################
gaudi_subdir(PatChecker v3r16)

gaudi_depends_on_subdirs(Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/PhysEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/MCInterfaces
                         Tf/PatKernel
                         Tf/TfKernel)

find_package(AIDA)
find_package(GSL)

gaudi_add_module(PatChecker
                 src/*.cpp
                 INCLUDE_DIRS AIDA GSL Event/DigiEvent Kernel/MCInterfaces Tf/PatKernel Tf/TfKernel
                 LINK_LIBRARIES GSL LinkerEvent MCEvent PhysEvent GaudiAlgLib GaudiKernel)

