################################################################################
# Package: Controller
################################################################################
gaudi_subdir(Controller v0r25)

gaudi_depends_on_subdirs(Online/OnlineKernel
                         Online/DIM
                         Online/XML
                         Online/UPI)

add_definitions(-DDD4HEP_USE_TINYXML)
gaudi_add_library(Controller src/*.cpp
                  PUBLIC_HEADERS FiniteStateMachine
                  LINK_LIBRARIES OnlineKernel dim XML)

macro(controller_add_exe name func)
  gaudi_add_executable(${name} ${ARGN} LINK_LIBRARIES Controller)
  set_property(TARGET ${name}
               APPEND PROPERTY COMPILE_DEFINITIONS FUNCTION=${func})
endmacro()

controller_add_exe(internal_fsm_test   internal_fsm_test   main/main.cpp)
controller_add_exe(external_fsm_test   external_fsm_test   main/main.cpp)
controller_add_exe(external_fsm_client external_fsm_client main/main.cpp)
controller_add_exe(fsm_upi             fsm_upi_controlmenu main/main.cpp main/ControlMenu.cpp)
target_link_libraries(fsm_upi UPI)
controller_add_exe(fsm_xml             fsm_xml_test        main/main.cpp)
target_link_libraries(fsm_xml XML)
controller_add_exe(fsm_ctrl            fsm_ctrl            main/fsm_ctrl.cpp)
controller_add_exe(fsm_dump            fsm_dump            main/main.cpp main/fsm_dump.cpp)
