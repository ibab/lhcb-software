# GaudiObjDesc: Gaudi Object Description
#
# Enable the generation of classes from XML.

# Ensure that we have access to the Python interpreter.
find_package(PythonInterp REQUIRED)

#cmake_minimum_required(VERSION 2.8.5)

# locate
get_filename_component(_base ${CMAKE_CURRENT_LIST_FILE} PATH)
get_filename_component(_base ${_base} PATH)

find_program(god_cmd godII.py PATHS ${_base}/scripts ${binary_paths})
set(god_cmd ${PYTHON_EXECUTABLE} ${god_cmd})

find_file(GOD_DTD_FILE gdd.dtd
          HINTS ${_base}/xml_files ${_base}/data/GaudiObjDesc
          NO_SYSTEM_ENVIRONMENT_PATH)
find_path(GOD_DATA_DIR templates/header.tpl
          HINTS ${_base} ${_base}/data/GaudiObjDesc
          NO_SYSTEM_ENVIRONMENT_PATH)

# GaudiObjDesc functions
#-------------------------------------------------------------------------------
# god_build_headers(pattern1 pattern2 ...
#                   [DESTINATION dirname])
#
# Generate header files from XML descriptions and put them in the directory
# 'dirname' (default: 'Event').
#-------------------------------------------------------------------------------
function(god_build_headers)
  CMAKE_PARSE_ARGUMENTS(ARG "" "DESTINATION" "" ${ARGN})
  if(NOT ARG_DESTINATION)
    set(ARG_DESTINATION Event) # default value
  endif()

  # expand the list of XML files patterns
  set(xmlfiles)
  foreach(fp ${ARG_UNPARSED_ARGUMENTS})
    file(GLOB files ${fp})
    if(files)
      set(xmlfiles ${xmlfiles} ${files})
    else()
      set(xmlfiles ${xmlfiles} ${fp})
    endif()
  endforeach()

  set(dest ${CMAKE_BINARY_DIR}/include/${ARG_DESTINATION})
  file(MAKE_DIRECTORY ${dest})
  set(stamps)
  foreach(xmlfile ${xmlfiles})
    get_filename_component(fname ${xmlfile} NAME_WE)
    add_custom_command(OUTPUT ${fname}.obj2doth
                       COMMAND ${env_cmd}
                         --xml ${env_xml}
                       ${god_cmd} -g src -r ${GOD_DATA_DIR} -t ${GOD_DTD_FILE} -s ${dest} ${xmlfile}
                       COMMAND touch ${fname}.obj2doth
                       DEPENDS ${xmlfile}
                       COMMENT "Generating headers from ${xmlfile}")
    list(APPEND stamps ${fname}.obj2doth)
  endforeach()

  gaudi_get_package_name(package)
  add_custom_target(${package}Obj2doth ALL DEPENDS ${stamps})

  # notify the framework that this directory produces headers via this target
  # - for the other directories
  set_property(DIRECTORY APPEND
               PROPERTY GENERATED_HEADERS_TARGETS ${package}Obj2doth)
  # - for the current one
  set(required_local_genheader_targets ${package}Obj2doth PARENT_SCOPE)

  # Create a global target for all the obj2doth targets
  if(TARGET AllObj2doth)
    add_dependencies(AllObj2doth ${package}Obj2doth)
  else()
    add_custom_target(AllObj2doth DEPENDS ${package}Obj2doth)
  endif()

  gaudi_install_headers(${dest})
endfunction()

#-------------------------------------------------------------------------------
# god_build_dictionary(pattern1 pattern2 ...
#                      [HEADERS_DESTINATION dirname]
#                      [EXTEND header.h [selection.xml]]
#                      [LINK_LIBRARIES ...]
#                      [INCLUDE_DIRS ...]
#                      [OPTIONS ...])
#
# Generate Reflex dictionary from XML descriptions.
#-------------------------------------------------------------------------------
function(god_build_dictionary)
  CMAKE_PARSE_ARGUMENTS(ARG "" "HEADERS_DESTINATION" "EXTEND;LINK_LIBRARIES;INCLUDE_DIRS;OPTIONS" ${ARGN})

  gaudi_get_package_name(package)

  if(NOT TARGET ${package}Obj2doth)
    set(args ${ARG_UNPARSED_ARGUMENTS})
    if(ARG_HEADERS_DESTINATION)
      set(args ${args} DESTINATION ${ARG_HEADERS_DESTINATION})
    endif()
    #message(STATUS "god_build_headers(${args})")
    god_build_headers(${args})
  endif()

  # expand the list of XML files patterns
  set(xmlfiles)
  foreach(fp ${ARG_UNPARSED_ARGUMENTS})
    file(GLOB files ${fp})
    if(files)
      set(xmlfiles ${xmlfiles} ${files})
    else()
      set(xmlfiles ${xmlfiles} ${fp})
    endif()
  endforeach()

  if(ARG_HEADERS_DESTINATION)
    set(cmd_args -s ${ARG_HEADERS_DESTINATION})
  endif()

  set(dest ${CMAKE_CURRENT_BINARY_DIR}/god_dict)
  file(MAKE_DIRECTORY ${dest})
  set(header ${dest}/${package}_dictionary.h)
  set(selection ${dest}/${package}_selection.xml)

  add_custom_command(OUTPUT ${header} ${selection}
                     COMMAND ${env_cmd}
                       --xml ${env_xml}
                     ${god_cmd} ${cmd_args} -g dct -r ${GOD_DATA_DIR} -t ${GOD_DTD_FILE} -d ${dest} ${xmlfiles}
                     DEPENDS ${xmlfiles} ${package}Obj2doth
                     COMMENT "Generating dictionary from ${xmlfiles}")

  # copy the provided files
  if(ARG_EXTEND)
    list(LENGTH ARG_EXTEND count)
    if(count EQUAL 1)
      list(GET ARG_EXTEND 0 src_header)
      if(NOT IS_ABSOLUTE ${src_header})
        set(src_header ${CMAKE_CURRENT_SOURCE_DIR}/${src_header})
      endif()
      set(src_selection ${CMAKE_CURRENT_SOURCE_DIR}/xml/lcgdict/lcg_selection.xml)
    elseif(count EQUAL 2)
      list(GET ARG_EXTEND 0 src_header)
      if(NOT IS_ABSOLUTE ${src_header})
        set(src_header ${CMAKE_CURRENT_SOURCE_DIR}/${src_header})
      endif()
      list(GET ARG_EXTEND 1 src_selection)
      if(NOT IS_ABSOLUTE ${src_selection})
        set(src_selection ${CMAKE_CURRENT_SOURCE_DIR}/${src_selection})
      endif()
    else()
      message(FATAL_ERROR "Wrong number of arguments to option EXTEND, must be 1 or 2")
    endif()
    # Note: this depends on the xml files because every time one is modified, we
    # need to copy over again the source headers (actually it may not be needed,
    # but it is safer and GaudiObjDesc need to be improved).
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${package}_src_sel.copied
                       COMMAND cp -f ${src_header} ${header}
                       COMMAND cp -f ${src_selection} ${selection}
                       COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/${package}_src_sel.copied
                       DEPENDS ${src_header} ${src_selection} ${xmlfiles}
                       COMMENT "Copying custom files ${src_header} ${src_selection}")
    add_custom_command(OUTPUT ${header} ${selection}
                       DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${package}_src_sel.copied
                       APPEND)
  endif()

  gaudi_add_dictionary(${package} ${header} ${selection}
                       LINK_LIBRARIES GaudiObjDescLib GaudiKernel ${ARG_LINK_LIBRARIES}
                       INCLUDE_DIRS ${ARG_INCLUDE_DIRS}
                       OPTIONS ${ARG_OPTIONS})
endfunction()

