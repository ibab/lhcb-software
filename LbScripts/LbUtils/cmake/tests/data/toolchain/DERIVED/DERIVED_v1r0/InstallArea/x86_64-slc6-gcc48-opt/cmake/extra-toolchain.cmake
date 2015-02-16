message(STATUS "Entering ${CMAKE_CURRENT_LIST_FILE}")
# flag that we got called
set(derived_toolchain_called "YES")
# prepand something to check the order of calls
set(entries "DERIVED" ${entries})
