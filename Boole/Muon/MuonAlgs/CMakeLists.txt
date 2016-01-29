################################################################################
# Package: MuonAlgs
################################################################################
gaudi_subdir(MuonAlgs v6r14)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/DigiEvent
                         Event/MCEvent
                         GaudiAlg
                         Muon/MuonKernel)

gaudi_add_module(MuonAlgs
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent
                 LINK_LIBRARIES MuonDetLib MCEvent GaudiAlgLib MuonKernelLib)

