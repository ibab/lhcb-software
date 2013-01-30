################################################################################
# Package: ChargedProtoANNPID
################################################################################
gaudi_subdir(ChargedProtoANNPID v1r10)

gaudi_depends_on_subdirs(Event/MCEvent
                         Event/PhysEvent
                         Event/RecEvent
                         GaudiAlg
                         Rec/RecInterfaces)

find_package(NeuroBayesExpert)

gaudi_add_module(ChargedProtoANNPID
                 src/*.cpp
                 INCLUDE_DIRS NeuroBayesExpert Rec/RecInterfaces
                 LINK_LIBRARIES NeuroBayesExpert MCEvent PhysEvent RecEvent GaudiAlgLib)

gaudi_install_python_modules()

