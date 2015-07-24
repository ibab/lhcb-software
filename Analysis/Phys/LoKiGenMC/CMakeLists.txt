################################################################################
# Package: LoKiGenMC
################################################################################
gaudi_subdir(LoKiGenMC v9r2)

gaudi_depends_on_subdirs(Kernel/Relations
                         Phys/DaVinciMCKernel
                         Phys/LoKiGen
                         Phys/LoKiMC)

gaudi_add_library(LoKiGenMCLib
                  src/*.cpp
                  PUBLIC_HEADERS LoKi
                  LINK_LIBRARIES RelationsLib DaVinciMCKernelLib LoKiGenLib LoKiMCLib)

gaudi_add_module(LoKiGenMC
                 src/Components/*.cpp
                 LINK_LIBRARIES RelationsLib DaVinciMCKernelLib LoKiGenLib LoKiMCLib LoKiGenMCLib)

gaudi_add_dictionary(LoKiGenMC
                     dict/LoKiGenMCDict.h
                     dict/LoKiGenMC.xml
                     LINK_LIBRARIES RelationsLib DaVinciMCKernelLib LoKiGenLib LoKiMCLib LoKiGenMCLib
                     OPTIONS " -U__MINGW32__ ")

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
