################################################################################
# Package: RichDigiQC
################################################################################
gaudi_subdir(RichDigiQC v3r15)

gaudi_depends_on_subdirs(Det/RichDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/MCEvent
                         Kernel/MCInterfaces
                         Rich/RichKernel)

gaudi_add_module(RichDigiQC
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/DigiEvent Kernel/MCInterfaces
                 LINK_LIBRARIES RichDetLib DAQEventLib MCEvent RichKernelLib)

gaudi_env(SET RICHDIGIQCOPTS \${RICHDIGIQCROOT}/options)

