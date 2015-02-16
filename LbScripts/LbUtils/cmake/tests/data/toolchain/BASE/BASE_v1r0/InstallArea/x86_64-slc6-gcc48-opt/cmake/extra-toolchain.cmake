message(STATUS "Entering ${CMAKE_CURRENT_LIST_FILE}")
# flag that we got called
set(base_toolchain_called "YES")
# prepand something to check the order of calls
set(entries "BASE" ${entries})
