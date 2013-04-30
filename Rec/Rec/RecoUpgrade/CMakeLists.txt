################################################################################
# Package: RecoUpgrade
################################################################################
gaudi_subdir(RecoUpgrade v3r1p1)

gaudi_depends_on_subdirs(Det/VPDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/PhysEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/MCInterfaces
                         Tf/PatAlgorithms
                         Tf/TfKernel
                         Tf/TrackSys
                         Tr/TrackFitEvent
                         Tr/TrackKernel)

gaudi_add_module(RecoUpgrade
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent Kernel/MCInterfaces Tf/TfKernel
                 LINK_LIBRARIES VPDetLib DAQEventLib LinkerEvent MCEvent PhysEvent GaudiAlgLib GaudiKernel TrackFitEvent TrackKernel)

gaudi_install_python_modules()

