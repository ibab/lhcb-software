set(dirname ${package_name})
string(REPLACE "/" "_" targetname "$${dirname}")

set($${dirname}_DIR $${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Location of $${dirname}" FORCE)
set($${dirname}_XENV $${$${dirname}_DIR}/$${targetname}.xenv CACHE FILEPATH "$${dirname} environment file" FORCE)

if(EXISTS $${CMAKE_CURRENT_SOURCE_DIR}/Makefile)
  message(STATUS " building using make")
  add_custom_target($${targetname} ALL
                    make
                    COMMENT "building $${dirname}"
                    WORKING_DIRECTORY $${CMAKE_CURRENT_SOURCE_DIR})
elseif(EXISTS $${CMAKE_CURRENT_SOURCE_DIR}/cmt/requirements)
  message(STATUS " building using CMT")
  set(cmtdir $${CMAKE_CURRENT_SOURCE_DIR}/cmt)
  execute_process(COMMAND env PWD=$${cmtdir} cmt config
                  WORKING_DIRECTORY $${cmtdir})
  add_custom_target($${targetname} ALL
                    env PWD=$${cmtdir} cmt make
                    COMMENT "building $${dirname}"
                    WORKING_DIRECTORY $${cmtdir})
endif()
