# Toolchain file that attempts to setup CMAKE_PREFIX_PATH and the tools versions by parsing the CMakeLists.txt
# file for athena_project() or gaudi_project() statements to determine the project hierarchy. Known tools are
# heptools and the tdaq projects (tdaq-common and dqm-common). The latter two make contributions to
# CMAKE_PREFIX_PATH, whereas the heptools tool doesn't.

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

## Hepler macro to debug/print variables in an uniform way.
macro(print_var var)
    #message(STATUS "${var} -> ${${var}}")
endmacro()

## set the CMAKE_PREFIX_PATH taking into account backward compatibility with
#  CMTPROJECTPATH
macro(init_search_path)
    # Append CMTPROJECTPATH to CMAKE_PREFIX_PATH (we prefer entries in CMAKE_PREFIX_PATH)
    print_var(CMAKE_PREFIX_PATH)
    file(TO_CMAKE_PATH "$ENV{CMTPROJECTPATH}" _cmt_pp)
    file(TO_CMAKE_PATH "$ENV{CMAKE_PREFIX_PATH}" _cmake_pp)
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${_cmake_pp} ${_cmt_pp})
    if(CMAKE_PREFIX_PATH)
        list(REMOVE_DUPLICATES CMAKE_PREFIX_PATH)
    endif()
    print_var(CMAKE_PREFIX_PATH)

    # Preset the CMAKE_MODULE_PATH from the environment, if not already defined.
    if(NOT CMAKE_MODULE_PATH)
        file(TO_CMAKE_PATH "$ENV{CMAKE_MODULE_PATH}" CMAKE_MODULE_PATH)
    endif()
    # Add local directories to the CMAKE_MODULE_PATH.
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_MODULE_PATH})
    if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/cmake)
        set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake ${CMAKE_MODULE_PATH})
    endif()
    print_var(CMAKE_MODULE_PATH)
endmacro()

## Initialize common variables.
macro(init)
    init_search_path()
    if(NOT BINARY_TAG)
        include(HEPToolsMacros)
        lcg_get_target_platform()
    endif()
endmacro()


include(CMakeParseArguments)

## Helper for recursion and ordering of found projects.
function(_internal_find_projects projects_var tools_var config_file)
    #message(STATUS "processing ${config_file}")
    set(collected_config ${collected_config} ${config_file})
    # Extract information from configuration file
    file(READ ${config_file} config_file_data)
    # Warning: this regular expression implies that 'gaudi_project' is not on the
    # first line of the file (but it is needed to ensure that it is not commented out)
    string(REGEX MATCH "[\r\n][ \t]*(gaudi_project|athena_project)[ \t]*\\(([^)]+)\\)" match ${config_file_data})
    set(match ${CMAKE_MATCH_2})
    if(NOT match)
        message(FATAL_ERROR "${config_file} does not contain gaudi_project or athena_project")
    endif()
    #message(STATUS "find_used_projects: match -> ${match}")
    # (replace space-type chars with spaces)
    string(REGEX REPLACE "[ \t\r\n]+" " " args "${match}")
    separate_arguments(args)
    CMAKE_PARSE_ARGUMENTS(PROJECT "" "" "USE;TOOLS;DATA" ${args})

    # get the project name and add it to the list of used projects
    list(GET PROJECT_UNPARSED_ARGUMENTS 0 proj_name)
    string(TOUPPER ${proj_name} proj_name)
    print_var(proj_name)
    set(projects ${proj_name} ${${projects_var}})

    # define cache variables for this project
    if(NOT ${proj_name}_CONFIG_FILE)
        get_filename_component(${proj_name}_CONFIG_FILE ${config_file} ABSOLUTE CACHE)
    endif()
    get_filename_component(${proj_name}_ROOT_DIR ${${proj_name}_CONFIG_FILE} PATH CACHE)

    # first we look for the tools we want (higher priority)
    print_var(PROJECT_TOOLS)
    while(PROJECT_TOOLS)
        # PROJECT_TOOLS format is "<tool> <vers1> <tool> <vers2>..."
        # we extract two entries per iteration
        list(GET PROJECT_TOOLS 0 name)
           list(GET PROJECT_TOOLS 1 version)
           list(REMOVE_AT PROJECT_TOOLS 0 1)
           # look for the directory of the tool
           find_file(${name}_DIR
                     NAMES ${name}/${version}/${BINARY_TAG}
                           ${name}/${version})
        if(${name}_DIR)
            set(tools ${name} ${tools})
        else()
            message(FATAL_ERROR "Cannot find tool ${name} ${version}")
        endif()
    endwhile()

    # then we look for the used projects
    print_var(PROJECT_USE)
    while(PROJECT_USE)
        # PROJECT_USE format is "<proj1> <vers1> <proj2> <vers2>..."
        # we extract two entries per iteration
        list(GET PROJECT_USE 0 name)
           list(GET PROJECT_USE 1 version)
           list(REMOVE_AT PROJECT_USE 0 1)
           string(TOUPPER ${name} name_upper)
           # look for the configuration file of the project
           find_file(${name_upper}_CONFIG_FILE NAMES CMakeLists.txt
                     PATH_SUFFIXES ${name}/${version}
                                   ${name_upper}/${name_upper}_${version}
                                   ${name}_${version}
                                   ${name})
        # recursion
        if(${name_upper}_CONFIG_FILE)
            # if the CMakeLists.txt of the other project was not found in the
            # suffixes, we get the CMakeLists.txt we started from (GAUDI-991)
            if(${name_upper}_CONFIG_FILE STREQUAL ${top_config_file} AND
               NOT proj_name STREQUAL name_upper)
              message(FATAL_ERROR "Cannot find project ${name} ${version}")
            endif()
            # protect against infinit recursion
            list(FIND collected_config ${${name_upper}_CONFIG_FILE} conf_pos)
            if(NOT conf_pos EQUAL -1)
              message(FATAL_ERROR "Infinite recursion detected at project ${name}")
            endif()
            _internal_find_projects(projects tools
                                    ${${name_upper}_CONFIG_FILE} ${name_upper})
        endif()
    endwhile()

    # propagate the full list of projects and tools to the caller
    set(${projects_var} ${projects} PARENT_SCOPE)
    set(${tools_var} ${tools} PARENT_SCOPE)
endfunction()

## Look for used projects
function(find_projects projects_var tools_var config_file)
    set(projects)
    set(tools)
    set(collected_config)
    # this is used to detect if a dependency is missing
    get_filename_component(top_config_file "${config_file}" ABSOLUTE)
    _internal_find_projects(projects tools ${config_file})
    if(projects)
        list(REMOVE_DUPLICATES projects)
        list(REVERSE projects)
    endif()
    if(tools)
        list(REMOVE_DUPLICATES projects)
        list(REVERSE tools)
    endif()
    print_var(projects)
    print_var(tools)
    set(${projects_var} ${projects} PARENT_SCOPE)
    set(${tools_var} ${tools} PARENT_SCOPE)
endfunction()

## Extend search paths with projects and tools
macro(set_paths_from_projects)
    # we need to reverse the list of arguments because we will prepend to the
    # search paths in a loop
    set(_entries ${ARGN})
    if(_entries)
        list(REVERSE _entries)
    endif()

    # prepare the helper variable (we add it to both CMAKE_PREFIX_PATH and
    # CMAKE_MODULE_PATH)
    set(_path)
    # and the variable for the extension of the toolchain (e.g. override externals)
    set(_extra_toolchains)
    foreach(_entry ${_entries})
        if(${_entry}_DIR) # it's a tool
            set(_root ${${_entry}_DIR})
        else() # it's a project
            string(TOUPPER "${_entry}" _entry)
            set(_root ${${_entry}_ROOT_DIR})
        endif()
        # we add the tool/project directory ...
        set(_path ${_root} ${_path}) # FIXME: is it needed?
        # ... and some optional extra entries
        foreach(_root ${_root}/cmake ${_root}/AtlasExternalsRelease/cmake
                      ${_root}/InstallArea/${BINARY_TAG}/cmake)
            if(EXISTS ${_root})
                set(_path ${_root} ${_path})
            endif()
        endforeach()
        if(EXISTS ${_root}/InstallArea/${BINARY_TAG}/cmake/extra-toolchain.cmake)
          list(APPEND _extra_toolchains ${_root}/InstallArea/${BINARY_TAG}/cmake/extra-toolchain.cmake)
        endif()
    endforeach()

    # set the real search paths variables
    set(CMAKE_PREFIX_PATH ${_path} ${CMAKE_PREFIX_PATH})
    set(CMAKE_MODULE_PATH ${_path} ${CMAKE_MODULE_PATH})
    # include all the toolchain extensions (they should be prepending entries to
    # CMAKE_PREFIX_PATH and CMAKE_MODULE_PATH)
    foreach(_extra_toolchain ${_extra_toolchains})
      include(${_extra_toolchain})
    endforeach()
endmacro()

## How to use these functions in a toolchain:
#
# init()
# find_projects(projects tools ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt)
#
# if(heptools_version)
#   use_heptools(${heptools_version})
# else()
#   inherit_heptools()
# endif()
#
# set_paths_from_projects(${tools} ${projects})
