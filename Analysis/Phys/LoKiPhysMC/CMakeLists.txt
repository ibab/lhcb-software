################################################################################
# Package: LoKiPhysMC
################################################################################
gaudi_subdir(LoKiPhysMC v9r10)

gaudi_depends_on_subdirs(Event/LinkerEvent
                         Kernel/Relations
                         Phys/DaVinciMCKernel
                         Phys/LoKiGenMC
                         Phys/LoKiMC
                         Phys/LoKiPhys)

gaudi_add_library(LoKiPhysMCLib
                  src/*.cpp
                  PUBLIC_HEADERS LoKi
                  LINK_LIBRARIES LinkerEvent RelationsLib DaVinciMCKernelLib LoKiGenMCLib LoKiMCLib LoKiPhysLib)

gaudi_add_module(LoKiPhysMC
                 src/Components/*.cpp
                 LINK_LIBRARIES LinkerEvent RelationsLib DaVinciMCKernelLib LoKiGenMCLib LoKiMCLib LoKiPhysLib LoKiPhysMCLib)

gaudi_add_dictionary(LoKiPhysMC
                     dict/LoKiPhysMCDict.h
                     dict/LoKiPhysMC.xml
                     LINK_LIBRARIES LinkerEvent RelationsLib DaVinciMCKernelLib LoKiGenMCLib LoKiMCLib LoKiPhysLib LoKiPhysMCLib
                     OPTIONS " -U__MINGW32__ ")

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
