################################################################################
# Package: FlavourTagging
################################################################################
gaudi_subdir(FlavourTagging v12r12p1)

gaudi_depends_on_subdirs(Calo/CaloInterfaces
                         Event/PhysEvent
                         GaudiPython
                         Phys/DaVinciKernel
                         PhysSel/PhysSelPython
                         Tr/TrackInterfaces)

gaudi_add_module(FlavourTagging
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES CaloInterfaces PhysEvent GaudiPythonLib DaVinciKernelLib)

gaudi_add_executable(summaryof
                     summary/summaryof.cpp
                     INCLUDE_DIRS Tr/TrackInterfaces
                     LINK_LIBRARIES CaloInterfaces PhysEvent GaudiPythonLib DaVinciKernelLib)

gaudi_install_python_modules()

gaudi_env(SET FLAVOURTAGGINGOPTS \${FLAVOURTAGGINGROOT}/options)

