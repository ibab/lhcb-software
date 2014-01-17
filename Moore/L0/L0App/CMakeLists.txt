################################################################################
# Package: L0App
################################################################################
gaudi_subdir(L0App v2r0)

gaudi_depends_on_subdirs(DAQ/DAQSys
                         Det/CaloDet
                         Det/DDDB
                         Det/MuonDet
                         Event/DAQEvent
                         Event/L0Event
                         GaudiConf
                         GaudiKernel
                         Kernel/LHCbAlgs
                         L0/L0Base
                         L0/L0Calo
                         L0/L0DU
                         L0/L0Interfaces
                         L0/L0Muon
                         L0/L0MuonKernel
                         L0/ProcessorKernel)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
