################################################################################
# Package: AlignKernel
################################################################################
gaudi_subdir(AlignKernel v3r8)

gaudi_depends_on_subdirs(GaudiAlg
                         GaudiGSL
                         GaudiKernel)

# helper function used as fall-back solution to find BLAS/LAPACK
macro(simple_find_lib _name)
  find_library(${_name}_LIB ${_name})
  if(${_name}_LIB)
    message(STATUS "Found ${_name}: ${${_name}_LIB}")
    set(${_name}_LIBRARIES ${${_name}_LIB})
    set(${_name}_FOUND True CACHE INTERNAL "${_name} library found")
  else()
    message(FATAL_ERROR "Cannot find ${_name}")
  endif()
endmacro()

find_package(GSL)

# Try regular LAPACK finder
find_package(LAPACK QUIET)

# Fall-back solution for HEPTools versions of LAPACK and BLAS
if(NOT LAPACK_FOUND)
  simple_find_lib(BLAS)
  simple_find_lib(LAPACK)
  set(LAPACK_LIBRARIES ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES})
endif()


gaudi_add_library(AlignKernel
                  src/*.cpp
                  PUBLIC_HEADERS AlignKernel
                  INCLUDE_DIRS GSL
                  LINK_LIBRARIES GSL LAPACK gfortran GaudiAlgLib GaudiGSLLib GaudiKernel)

gaudi_add_dictionary(AlignKernel
                     dict/AlignKernelDict.h
                     dict/AlignKernelDict.xml
                     INCLUDE_DIRS GSL
                     LINK_LIBRARIES GSL GaudiAlgLib GaudiGSLLib GaudiKernel AlignKernel
                     OPTIONS "-U__MINGW32__")

