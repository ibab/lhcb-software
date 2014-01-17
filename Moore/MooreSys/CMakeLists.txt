################################################################################
# Package: MooreSys
################################################################################
gaudi_subdir(MooreSys v21r0)

gaudi_depends_on_subdirs(Hlt/HltMonitor
                         Hlt/IndependenceTests
                         Hlt/Moore
                         L0/L0App
                         Phys/Swimming)


gaudi_add_test(QMTest QMTEST)


set(setup_scripts)

set(SetupMoore_script "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/setupMoore_${CMAKE_PROJECT_VERSION}.sh")

add_custom_command(OUTPUT "${SetupMoore_script}"
                   COMMAND ${env_cmd} --xml ${env_xml}
                           python ${CMAKE_CURRENT_SOURCE_DIR}/scripts/CreateSetupMoore.py
                             "${SetupMoore_script}" ${CMAKE_PROJECT_VERSION})
list(APPEND setup_scripts "${SetupMoore_script}")

macro(gen_EFF_script flavor)
  set(eff_script "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/runMoore${flavor}Online_EFF.sh")
  set(cmd ${env_cmd} --xml ${env_xml}
          python ${CMAKE_CURRENT_SOURCE_DIR}/scripts/CreateRunMooreOnline_EFF.py
            "${eff_script}"
            "${SetupMoore_script}")
  if(NOT "${flavor}" STREQUAL "")
    set(cmd ${cmd} ${flavor})
  endif()
  add_custom_command(OUTPUT "${eff_script}"
                     COMMAND ${cmd}
                     DEPENDS "${SetupMoore_script}")
  list(APPEND setup_scripts "${eff_script}")
endmacro()

gen_EFF_script("")
gen_EFF_script("Hlt1")
gen_EFF_script("Hlt2")

install(PROGRAMS ${setup_scripts} DESTINATION scripts)

add_custom_target(MooreSetupScripts ALL DEPENDS ${setup_scripts})

# copy the TCK manifest for the _current_ release to the install area...
message(WARNING "installation of TCK manifest to be implemented")
#install(FILES ${HLTTCKROOT}/manifest/MOORE_$(version)
