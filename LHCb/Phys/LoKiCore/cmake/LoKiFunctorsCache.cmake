
set(LOKI_FUNCTORS_CACHE_POST_ACTION_OPTS
    ${CMAKE_CURRENT_LIST_DIR}/LoKiFunctorsCachePostActionOpts.py
    CACHE FILEPATH "Special options file for LoKi Functors cache generation.")
mark_as_advanced(LOKI_FUNCTORS_CACHE_POST_ACTION_OPTS)

# Usage: loki_functors_cache(cache_name option_file_1 [option_file_2 ...])
function(loki_functors_cache name)

  CMAKE_PARSE_ARGUMENTS(ARG "" "SPLIT"
                            "LIBRARIES;LINK_LIBRARIES;INCLUDE_DIRS;DEPENDS" ${ARGN})

  # Output filename(s)
  foreach(factory_type CoreFactory HltFactory HybridFactory)
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
        set(output FUNCTORS_${factory_type}_${idx_string}.cpp ${output})
        math(EXPR idx "${idx} - 1")
      endwhile()
    else()
      set(ARG_SPLIT 0)
      set(output FUNCTORS_${factory_type}_0001.cpp ${output})
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
  add_custom_command(OUTPUT ${name}.${tmp_ext}
                     COMMAND ${env_cmd} --xml ${env_xml}
                       gaudirun.py -n -o ${name}.${tmp_ext} ${inputs} ${LOKI_FUNCTORS_CACHE_POST_ACTION_OPTS}
                     DEPENDS ${inputs} ${ARG_DEPENDS})

  add_custom_command(OUTPUT ${output}
                     COMMAND ${env_cmd} --xml ${env_xml} LOKI_GENERATE_CPPCODE=${ARG_SPLIT}
                       gaudirun.py ${name}.${tmp_ext}
                     COMMAND touch ${output}
                     DEPENDS ${name}.${tmp_ext})

  gaudi_common_add_build(${output} LIBRARIES ${ARG_LIBRARIES}
                         LINK_LIBRARIES ${ARG_LINK_LIBRARIES} INCLUDE_DIRS ${ARG_INCLUDE_DIRS})

  add_library(${name} MODULE ${output})
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
