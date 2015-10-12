################################################################################
# Package: L0Muon
################################################################################
gaudi_subdir(L0Muon v9r14p1)

gaudi_depends_on_subdirs(Det/DetDesc
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/L0Event
                         L0/L0Base
                         L0/L0Interfaces
                         L0/L0MuonKernel
                         L0/ProcessorKernel
                         DAQ/DAQKernel
                         Muon/MuonDAQ
                         Muon/MuonKernel)

find_package(Boost COMPONENTS system filesystem)

gaudi_add_module(L0Muon
                 src/component/*.cpp
                 INCLUDE_DIRS Boost Event/DigiEvent L0/L0Interfaces Muon/MuonDAQ
                 LINK_LIBRARIES Boost DetDescLib DAQEventLib DAQKernelLib L0Event L0Base L0MuonKernel ProcessorKernel MuonKernelLib)

