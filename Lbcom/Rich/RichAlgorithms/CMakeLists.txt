################################################################################
# Package: RichAlgorithms
################################################################################
gaudi_subdir(RichAlgorithms v1r9)

gaudi_depends_on_subdirs(Det/RichDet
                         Rich/RichKernel)

gaudi_add_module(RichAlgorithms
                 src/*.cpp
                 LINK_LIBRARIES RichDetLib RichKernelLib)

