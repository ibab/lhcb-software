################################################################################
# Package: ChargedProtoANNPID
################################################################################
gaudi_subdir(ChargedProtoANNPID v1r12)

gaudi_depends_on_subdirs(Event/MCEvent
                         Event/PhysEvent
                         Event/RecEvent
                         GaudiAlg
                         Rec/RecInterfaces)

find_package(NeuroBayesExpert)

#if(NEUROBAYESEXPERT_FOUND)
#endif()

find_package(ROOT COMPONENTS TMVA)

gaudi_add_module(ChargedProtoANNPID
                 src/*.cpp
                 INCLUDE_DIRS NeuroBayesExpert ROOT Rec/RecInterfaces
                 LINK_LIBRARIES NeuroBayesExpert ROOT MCEvent PhysEvent RecEvent GaudiAlgLib)

gaudi_install_python_modules()
