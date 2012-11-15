################################################################################
# Package: KernelSys
################################################################################
gaudi_subdir(KernelSys v12r1)

gaudi_depends_on_subdirs(Calo/CaloKernel
                         Kernel/LHCbKernel
                         Kernel/LHCbMath
                         Kernel/PartProp
                         Kernel/Relations
                         Kernel/XMLSummaryKernel
                         L0/L0MuonKernel
                         L0/ProcessorKernel
                         Muon/MuonKernel
                         Rich/RichKernel
                         ST/STKernel
                         Tf/TfKernel)

