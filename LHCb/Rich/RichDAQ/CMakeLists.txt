################################################################################
# Package: RichDAQ
################################################################################
gaudi_subdir(RichDAQ v3r44)

gaudi_depends_on_subdirs(Det/RichDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Rich/RichKernel)

gaudi_add_module(RichDAQ
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent
                 LINK_LIBRARIES RichDetLib DAQEventLib RichKernelLib)

