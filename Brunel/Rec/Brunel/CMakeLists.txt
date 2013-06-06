################################################################################
# Package: Brunel
################################################################################
gaudi_subdir(Brunel v44r5)

gaudi_depends_on_subdirs(DAQ/MDF
                         Det/CaloDetXmlCnv
                         Det/DDDB
                         Det/DetDescSvc
                         Det/Magnet
                         Event/EventPacker
                         Gaudi
                         GaudiConf
                         GaudiKernel
                         Kernel/LHCbAlgs
                         LbcomSys
                         RecSys
                         Sim/SimComponents
                         Tools/XmlTools)

find_package(HepMC)

gaudi_install_python_modules()

gaudi_env(SET BRUNELOPTS \${BRUNELROOT}/options)


gaudi_add_test(QMTest QMTEST)
