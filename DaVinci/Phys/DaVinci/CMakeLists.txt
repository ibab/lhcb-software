################################################################################
# Package: DaVinci
################################################################################
gaudi_subdir(DaVinci v37r1p2)

gaudi_depends_on_subdirs(AnalysisSys
                         Calo/CaloDAQ
                         Det/DDDB
                         Event/LinkerInstances
                         GaudiConf
                         GaudiKernel
                         GaudiPython
                         GaudiSvc
                         LbcomSys
                         PhysSys
                         RecSys
                         Sim/SimComponents
                         StrippingSys)

find_package(HepMC)

set_property(DIRECTORY PROPERTY CONFIGURABLE_USER_MODULES DaVinci.Configuration)
gaudi_install_python_modules()

gaudi_env(SET DAVINCIOPTS \${DAVINCIROOT}/options)
