################################################################################
# Package: CaloReco
################################################################################
gaudi_subdir(CaloReco v5r27)

gaudi_depends_on_subdirs(Calo/CaloDAQ
                         Calo/CaloInterfaces
                         Calo/CaloPIDs
                         Calo/CaloUtils
                         Kernel/Relations)

gaudi_add_module(CaloReco
                 src/*.cpp
                 INCLUDE_DIRS Calo/CaloDAQ
                 LINK_LIBRARIES CaloInterfaces CaloUtils RelationsLib)

gaudi_install_python_modules()

gaudi_env(SET CALORECOOPTS \${CALORECOROOT}/options)

