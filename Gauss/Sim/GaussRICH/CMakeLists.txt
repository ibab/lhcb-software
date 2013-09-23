################################################################################
# Package: GaussRICH
################################################################################
gaudi_subdir(GaussRICH v14r0)

gaudi_depends_on_subdirs(Det/RichDet
                         Event/LinkerEvent
                         Event/MCEvent
                         Kernel/Relations
                         Rich/RichKernel
                         Sim/GaussTools)

find_package(AIDA)

include_directories(src/SensDet src/PhysProcess src/RichAction src/Misc
                    src/PhysPhotDet src/RichAnalysis /src/srcG4)

gaudi_add_library(GaussRICHLib
                  src/Misc/*.cpp
                  src/PhysPhotDet/*.cpp
                  src/srcG4/*.cc
                  src/SensDet/*.cpp
                  PUBLIC_HEADERS GaussRICH
                  INCLUDE_DIRS AIDA
                  LINK_LIBRARIES RichDetLib LinkerEvent MCEvent RelationsLib RichKernelLib GaussToolsLib)

gaudi_add_module(GaussRICH
                 src/*.cpp
                 src/PhysProcess/*.cpp
                 src/RichAnalysis/*.cpp
                 src/RichAction/*.cpp
                 src/Assoc/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES RichDetLib LinkerEvent MCEvent RelationsLib
                                RichKernelLib GaussToolsLib GaussRICHLib
                 GENCONF_PRELOAD GaussToolsGenConfHelperLib)

gaudi_env(SET GAUSSRICHOPTS \${GAUSSRICHROOT}/options)
