################################################################################
# Package: TsaAlgorithms
################################################################################
gaudi_subdir(TsaAlgorithms v3r31)

gaudi_depends_on_subdirs(Calo/CaloUtils
                         Kernel/Relations
                         ST/STKernel
                         Tf/TsaKernel
                         Tr/TrackFitEvent
                         Tr/TrackInterfaces)

gaudi_add_module(TsaAlgorithms
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES CaloUtils RelationsLib STKernelLib TsaKernel TrackFitEvent)

gaudi_install_python_modules()

