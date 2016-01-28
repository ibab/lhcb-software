################################################################################
# Package: ChargedProtoANNPID
################################################################################
gaudi_subdir(ChargedProtoANNPID v2r13)

gaudi_depends_on_subdirs(Event/MCEvent
                         Event/PhysEvent
                         Event/RecEvent
                         GaudiAlg
                         Rec/RecInterfaces)

#find_package(NeuroBayesExpert)
#if(NEUROBAYESEXPERT_FOUND)
#  add_definitions(-D_ENABLE_NEUROBAYES)
#  include_directories(${NEUROBAYESEXPERT_INCLUDE_DIRS})
#else()
#  message(WARNING "Not using NeuroBayesExpert")
#endif()

find_package(ROOT COMPONENTS TMVA)

gaudi_add_module(ChargedProtoANNPID
                 src/*.cpp
                 INCLUDE_DIRS ROOT Rec/RecInterfaces
                 LINK_LIBRARIES ROOT MCEvent PhysEvent RecEvent GaudiAlgLib)

#if(NEUROBAYESEXPERT_FOUND)
#  target_link_libraries(ChargedProtoANNPID ${NEUROBAYESEXPERT_LIBRARIES})
#endif()

gaudi_install_python_modules()
