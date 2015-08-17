################################################################################
# LoKiFunctorsCache
# -----------------
#
# This module provides a function to generate and build cache libraries for
# LoKi Functors.
# @see https://its.cern.ch/jira/browse/LHCBPS-1357
#
# Usage: loki_functors_cache(<cache library name> optionfile1 [optionfile2...]
#                            [LINK_LIBRARIES library1 library2...]
#                            [INCLUDE_DIRS dir1 dir2...]
#                            [DEPENDS target1 target2...]
#                            [SPLIT <N>]
#                            [FACTORIES factory1 factory2...])
#
# where:
# - <cache library name> defines the name of the generated library
# - optionfile1 [optionfile2...] are the options defining the functors
#                                (usually the options used for the application
#                                we need to generate the cache for)
# - LINK_LIBRARIES, INCLUDE_DIRS have the same meaning as in gaudi_add_module()
# - DEPENDS target1 target2... declare tergets to be build before we can
#                              generate the cache code (e.g. "genconf" steps)
# - SPLIT <N> stated the number of files to generate per factory (default: 1)
# - FACTORIES factory1 factory2... name of the hybrids factories that will be
#                                  invoked (default: CoreFactory, HltFactory,
#                                  HybridFactory)
#
# Note: the CMake option LOKI_BUILD_FUNCTOR_CACHE can be set to False to disable
#       generation/build of declared functor caches.
#
# @author Marco Clemencic <marco.clemencic@cern.ch>
#

set(LOKI_FUNCTORS_CACHE_POST_ACTION_OPTS
    ${CMAKE_CURRENT_LIST_DIR}/LoKiFunctorsCachePostActionOpts.py
    CACHE FILEPATH "Special options file for LoKi Functors cache generation.")
mark_as_advanced(LOKI_FUNCTORS_CACHE_POST_ACTION_OPTS)

option(LOKI_BUILD_FUNCTOR_CACHE "Enable building of LoKi Functors Caches."
       TRUE)

# Usage: loki_functors_cache(cache_name option_file_1 [option_file_2 ...])
function(loki_functors_cache name)
  # ignore cache declaration if requested
  if(NOT LOKI_BUILD_FUNCTOR_CACHE)
    return()
  endif()

  CMAKE_PARSE_ARGUMENTS(ARG "" "SPLIT"
                            "LINK_LIBRARIES;INCLUDE_DIRS;DEPENDS;FACTORIES" ${ARGN})

  if(NOT ARG_FACTORIES)
    # default values
    set(ARG_FACTORIES CoreFactory HltFactory Hlt1HltFactory Hlt2HltFactory HybridFactory)
  endif()

  # where all the files are generated
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${name}_srcs)

  # Output filename(s)
  foreach(factory_type ${ARG_FACTORIES})
    if(ARG_SPLIT)
      set(idx ${ARG_SPLIT})
      while(${idx} GREATER 0)
        string(LENGTH "${idx}" idx_length)
        if(idx_length EQUAL 1)
          set(idx_string "000${idx}")
        elseif(idx_length EQUAL 2)
          set(idx_string "00${idx}")
        elseif(idx_length EQUAL 3)
          set(idx_string "0${idx}")
        else()
          set(idx_string "${idx}")
        endif()
        set(output_names FUNCTORS_${factory_type}_${idx_string}.cpp ${output_names})
        set(outputs ${name}_srcs/FUNCTORS_${factory_type}_${idx_string}.cpp ${outputs})
        math(EXPR idx "${idx} - 1")
      endwhile()
    else()
      set(ARG_SPLIT 0)
      set(output_names FUNCTORS_${factory_type}_0001.cpp)
      set(outputs ${name}_srcs/FUNCTORS_${factory_type}_0001.cpp)
    endif()
  endforeach()

  set(inputs)
  foreach(input ${ARG_UNPARSED_ARGUMENTS})
    if((NOT IS_ABSOLUTE input) AND (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${input}))
      set(input ${CMAKE_CURRENT_SOURCE_DIR}/${input})
    endif()
    set(inputs ${inputs} ${input})
  endforeach()

  set(tmp_ext pkl)
  add_custom_command(OUTPUT ${name}_srcs/${name}.${tmp_ext}
                     COMMAND ${env_cmd} --xml ${env_xml}
                       gaudirun.py -n -o ${name}_srcs/${name}.${tmp_ext} ${inputs} ${LOKI_FUNCTORS_CACHE_POST_ACTION_OPTS}
                     DEPENDS ${inputs} ${ARG_DEPENDS}
                     COMMENT "Preprocess options for ${name}")

  add_custom_command(OUTPUT ${outputs}
                     COMMAND ${env_cmd} --xml ${env_xml} LOKI_GENERATE_CPPCODE=${ARG_SPLIT}
                       gaudirun.py ${name}.${tmp_ext}
                     COMMAND touch ${output_names}
                     WORKING_DIRECTORY ${name}_srcs
                     DEPENDS ${name}_srcs/${name}.${tmp_ext}
                     COMMENT "Generating sources for ${name}")

  gaudi_common_add_build(${outputs}
                         LINK_LIBRARIES ${ARG_LINK_LIBRARIES}
                         INCLUDE_DIRS ${ARG_INCLUDE_DIRS})

  add_library(${name} MODULE ${outputs})
  target_link_libraries(${name} GaudiPluginService
                                ${ARG_LINK_LIBRARIES})
  _gaudi_detach_debinfo(${name})

  gaudi_generate_componentslist(${name})

  #set_property(GLOBAL APPEND PROPERTY LOKI_FUNCTORS_CACHE_LIBRARIES ${name})

  gaudi_add_genheader_dependencies(${name})

  #----Installation details-------------------------------------------------------
  install(TARGETS ${name} LIBRARY DESTINATION lib OPTIONAL)
  #gaudi_export(MODULE ${name})
endfunction()
