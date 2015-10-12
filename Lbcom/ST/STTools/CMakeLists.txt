################################################################################
# Package: STTools
################################################################################
gaudi_subdir(STTools v4r18)

gaudi_depends_on_subdirs(Det/STDet
                         Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/LHCbKernel
                         ST/STKernel
                         ST/STTELL1Event)

find_package(AIDA)

gaudi_add_module(STTools
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/DigiEvent
                 LINK_LIBRARIES STDetLib LinkerEvent MCEvent TrackEvent GaudiAlgLib GaudiKernel LHCbKernel STKernelLib STTELL1Event)

gaudi_install_python_modules()

