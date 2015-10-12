################################################################################
# Package: CaloAssociators
################################################################################
gaudi_subdir(CaloAssociators v6r14p2)

gaudi_depends_on_subdirs(Calo/CaloUtils
                         Det/CaloDet
                         Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/RecEvent
                         GaudiAlg
                         Kernel/Relations)

gaudi_add_module(CaloAssociators
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent
                 LINK_LIBRARIES CaloUtils CaloDetLib LinkerEvent MCEvent RecEvent GaudiAlgLib RelationsLib)

gaudi_install_python_modules()

gaudi_env(SET CALOASSOCIATORSOPTS \${CALOASSOCIATORSROOT}/options)

