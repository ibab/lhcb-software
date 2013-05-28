################################################################################
# Package: PrAlgorithms
################################################################################
gaudi_subdir(PrAlgorithms v1r5)

gaudi_depends_on_subdirs(Det/FTDet
                         Event/DAQEvent
                         Event/RecEvent
                         Pr/PrKernel)

gaudi_add_module(PrAlgorithms
                 src/*.cpp
                 LINK_LIBRARIES FTDetLib DAQEventLib RecEvent PrKernel)

