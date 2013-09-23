################################################################################
# Package: GaussCalo
################################################################################
gaudi_subdir(GaussCalo v9r4)

gaudi_depends_on_subdirs(Det/CaloDet
                         Det/CaloDetXmlCnv
                         Event/MCEvent
                         Kernel/LHCbKernel
                         Sim/GaussTools)

find_package(AIDA)

gaudi_add_module(GaussCalo
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES CaloDetLib MCEvent LHCbKernel GaussToolsLib)

gaudi_env(SET GAUSSCALOOPTS \${GAUSSCALOROOT}/options)
