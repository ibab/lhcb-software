################################################################################
# Package: GlobalReco
################################################################################
gaudi_subdir(GlobalReco v6r54)

gaudi_depends_on_subdirs(Calo/CaloInterfaces
                         Calo/CaloUtils
                         Det/CaloDet
                         Event/RecEvent
                         GaudiAlg
                         Kernel/Relations
                         Rec/ChargedProtoANNPID
                         Tr/TrackInterfaces)

gaudi_add_module(GlobalReco
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES CaloInterfaces CaloUtils CaloDetLib RecEvent GaudiAlgLib RelationsLib)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
