################################################################################
# Package: DaVinciMonitors
################################################################################
gaudi_subdir(DaVinciMonitors v1r23)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/HltEvent
                         Event/LinkerEvent
                         Event/RecEvent
                         Event/TrackEvent
                         Kernel/PartProp
                         Muon/MuonTools
                         Phys/DaVinciKernel
                         Phys/LoKiArrayFunctors
                         Phys/LoKiPhys)

gaudi_add_module(DaVinciMonitors
                 src/*.cpp
                 LINK_LIBRARIES MuonDetLib HltEvent LinkerEvent RecEvent TrackEvent PartPropLib DaVinciKernelLib LoKiArrayFunctorsLib LoKiPhysLib)

gaudi_install_python_modules()

