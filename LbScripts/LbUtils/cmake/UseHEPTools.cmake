
include(${CMAKE_CURRENT_LIST_DIR}/HEPToolsMacros.cmake)

# use_heptools(heptools_version)
#
# Look for the required version of the HEPTools toolchain and replace the
# current toolchain file with it.
#
# WARNING: this macro must be called from a toolchain file
#
macro(use_heptools heptools_version)
  # Deduce the LCG configuration tag from the system
  lcg_detect_host_platform()
  lcg_get_target_platform()

  # Find the toolchain descriptions (base and overrides)
  set(LCG_TOOLCHAIN_INFOS)
  file(TO_CMAKE_PATH "$ENV{CMTPROJECTPATH}" _cmt_pp)
  file(TO_CMAKE_PATH "$ENV{CMAKE_PREFIX_PATH}" _cmake_pp)
  foreach(_base_dir ${CMAKE_PREFIX_PATH} ${_cmake_pp} ${_cmt_pp})
    foreach(_name LCG_${heptools_version}_${BINARY_TAG}.txt
                  LCG_${heptools_version}_${LCG_platform}.txt
                  LCG_${heptools_version}_${LCG_system}.txt
                  LCG_externals_${BINARY_TAG}.txt
                  LCG_externals_${LCG_platform}.txt
                  LCG_externals_${LCG_system}.txt)
      if(EXISTS ${_base_dir}/${_name})
        set(LCG_TOOLCHAIN_INFOS ${_base_dir}/${_name} ${LCG_TOOLCHAIN_INFOS})
        break()
      elseif(EXISTS ${_base_dir}/LCG_${heptools_version}/${_name})
        set(LCG_TOOLCHAIN_INFOS ${_base_dir}/LCG_${heptools_version}/${_name} ${LCG_TOOLCHAIN_INFOS})
        break()
      endif()
    endforeach()
  endforeach()

  if(LCG_TOOLCHAIN_INFOS)
    # sanitize the list and extract the baseline file
    list(REMOVE_DUPLICATES LCG_TOOLCHAIN_INFOS)
    list(GET LCG_TOOLCHAIN_INFOS 0 LCG_TOOLCHAIN_INFO)
    list(REMOVE_AT LCG_TOOLCHAIN_INFOS 0)

    get_filename_component(LCG_releases ${LCG_TOOLCHAIN_INFO} PATH CACHE)
    set(LCG_external ${LCG_releases})
    if(LCG_releases MATCHES "LCG_${heptools_version}\$")
      get_filename_component(LCG_releases_base ${LCG_releases} PATH)
    else()
      set(LCG_releases_base ${LCG_releases})
    endif()

    message(STATUS "Using heptools ${heptools_version} from ${LCG_TOOLCHAIN_INFO}")

    if(LCG_TOOLCHAIN_INFOS)
      message(STATUS "with overrides from:")
      foreach(_info_file ${LCG_TOOLCHAIN_INFOS})
        message(STATUS "  ${_info_file}")
      endforeach()
    endif()

    if(LCG_FORCE_COMPILER)
      string(REPLACE "-" ";" _l "${LCG_FORCE_COMPILER}")
      lcg_set_compiler(${_l})
    endif()

    foreach(_info_file ${LCG_TOOLCHAIN_INFO} ${LCG_TOOLCHAIN_INFOS})
       file(STRINGS ${_info_file} _lcg_infos)
       foreach(_l ${_lcg_infos})
         if(_l MATCHES "^COMPILER:")
           string(REPLACE "COMPILER:" "" _l "${_l}")
           string(STRIP _l "${_l}")
           string(REGEX REPLACE " +" ";" _l "${_l}")
           if (NOT LCG_FORCE_COMPILER)
             lcg_set_compiler(${_l})
           endif()
           # We add the compiler to the path 
           lcg_set_lcg_system_compiler_path(${_l})
         else()
           if(NOT _l MATCHES "^((PLATFORM|VERSION|COMPILER):|#)")
             string(REGEX REPLACE "; *" ";" _l "${_l}")
             lcg_set_external(${_l})
           endif()
         endif()
       endforeach()
    endforeach()


    # Enable the right compiler (needs LCG_external)
    lcg_common_compilers_definitions()

    lcg_prepare_paths()

    lcg_find_common_tools()

    # Reset the cache variable to have proper documentation.
    #set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST}
    #    CACHE FILEPATH "The CMake toolchain file" FORCE)

  else()

    # try old toolchain style

    # Remove the reference to this file from the cache.
    unset(CMAKE_TOOLCHAIN_FILE CACHE)

    # Find the actual toolchain file.
    find_file(CMAKE_TOOLCHAIN_FILE
              NAMES heptools-${heptools_version}.cmake
              HINTS ENV CMTPROJECTPATH
              PATHS ${CMAKE_CURRENT_LIST_DIR}/cmake/toolchain
              PATH_SUFFIXES toolchain)

    if(NOT CMAKE_TOOLCHAIN_FILE)
      message(FATAL_ERROR "Cannot find heptools ${heptools_version}.")
    endif()

    # Reset the cache variable to have proper documentation.
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE}
        CACHE FILEPATH "The CMake toolchain file" FORCE)

    message(STATUS "Using heptools version ${heptools_version}")
    include(${CMAKE_TOOLCHAIN_FILE})

  endif()

endmacro()
