################################################################################
# Package: FlavourTagging
################################################################################
gaudi_subdir(FlavourTagging v13r3)

gaudi_depends_on_subdirs(Calo/CaloInterfaces
                         Event/PhysEvent
                         GaudiPython
                         Phys/DaVinciKernel
                         Phys/LoKi
                         Phys/LoKiPhys
                         PhysSel/PhysSelPython
                         Tr/TrackInterfaces)

find_package(ROOT COMPONENTS MLP Graf Hist Matrix TreePlayer Gpad Graf3d TMVA)

gaudi_add_module(FlavourTagging
                 src/*.cpp
                 PUBLIC_HEADERS src
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES CaloInterfaces PhysEvent GaudiPythonLib
                                DaVinciKernelLib LoKiPhysLib ROOT)

gaudi_add_executable(summaryof
                     summary/summaryof.cpp
                     INCLUDE_DIRS Tr/TrackInterfaces
                     LINK_LIBRARIES CaloInterfaces PhysEvent GaudiPythonLib
                                    DaVinciKernelLib LoKiPhysLib ROOT)

gaudi_install_python_modules()

gaudi_env(SET FLAVOURTAGGINGOPTS \${FLAVOURTAGGINGROOT}/options)

