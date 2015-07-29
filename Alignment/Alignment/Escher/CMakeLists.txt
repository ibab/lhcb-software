################################################################################
# Package: Escher
################################################################################
gaudi_subdir(Escher v5r6)

gaudi_depends_on_subdirs(Alignment/AlignEvent
                         Alignment/AlignKernel
                         Alignment/AlignSolvTools
                         Alignment/AlignTrTools
                         Alignment/AlignmentDBVisualisationTool
                         Alignment/AlignmentInterfaces
                         Alignment/AlignmentTools
                         Alignment/TAlignment
                         Alignment/VeloAlignment
                         DAQ/MDF
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
                         Phys/CommonParticles
                         Phys/DaVinciKernel
                         RecSys
                         Sim/SimComponents
                         Tools/XmlTools)

find_package(HepMC)
find_package(pytools)

gaudi_install_python_modules()
gaudi_install_scripts()

gaudi_env(SET ESCHEROPTS \${ESCHERROOT}/options)


gaudi_add_test(QMTest QMTEST)
