################################################################################
# Package: StrippingCache
################################################################################
gaudi_subdir(StrippingCache v1r0)

gaudi_depends_on_subdirs(Phys/DaVinci
                         Phys/LoKiArrayFunctors
                         Phys/LoKiHlt
                         Phys/LoKiPhys
                         Phys/LoKiAlgo
                         Phys/LoKiCore
                         Phys/DSTWriters
                         Phys/DaVinciFilters
                         Phys/CommonParticles
                         Phys/StrippingAlgs
                         Phys/StrippingSelections
                         Phys/StrippingNeuroBayes
                         Phys/IncTopoVert
                         Phys/TMVASelections
                         Phys/DisplVertices
                         Phys/IsolationTools
                         Phys/RelatedInfoTools
                         Phys/StrippingArchive)

foreach(pack DaVinci StrippingAlgs StrippingNeuroBayes IncTopoVert TMVASelections
             DisplVertices IsolationTools RelatedInfoTools)
  # FIXME: this will be needed until GAUDI-1055 is fixed.
  if(EXISTS ${CMAKE_BINARY_DIR}/Phys/${pack}/genConf)
    gaudi_build_env(PREPEND LD_LIBRARY_PATH
                    ${CMAKE_BINARY_DIR}/Phys/${pack}/genConf/${pack}
                    PREPEND LD_LIBRARY_PATH
                    ${CMAKE_BINARY_DIR}/Phys/${pack})
  endif()
#  if(TARGET ${pack}Conf)
#    set(conf_deps ${conf_deps} ${pack}Conf )
#  endif()
#  if(TARGET ${pack}ComponentsList)
#    set(conf_deps ${conf_deps} ${pack}ComponentsList )
#  endif()
endforeach()

# Explicitly depend on DaVinci, to make sure it is built first
set(conf_deps ${conf_deps} DaVinciConfUserDB )

# Build the caches

MACRO (TODAY RESULT)
   EXECUTE_PROCESS(COMMAND "date" "+%S/%M/%H/%d/%m/%Y" OUTPUT_VARIABLE ${RESULT})
   string(REGEX REPLACE "(..)/(..)/(..)/(..)/(..)/..(..).*" "\\1\\2\\3\\4\\5\\6" ${RESULT} ${${RESULT}})
ENDMACRO (TODAY)

# Do not build for the -O0 platform, as this seems to have problems.
# To be investigated...
if ( NOT "$ENV{CMTCONFIG}" STREQUAL "x86_64-slc6-gcc48-do0" )

 MESSAGE( STATUS "StrippingCache Deps : " ${conf_deps} )

 TODAY( time_date )

 include(LoKiFunctorsCache)

 set( s23name S23FunctorCache_${time_date} )
 loki_functors_cache(${s23name}
                     options/SuppressLogMessages.py
                     options/DV-Stripping23-Stripping.py 
                     options/SilenceErrors.py 
                     LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                     DEPENDS ${conf_deps}
                     SPLIT 50)

 set ( s22name S22FunctorCache_${time_date} )
 loki_functors_cache(${s22name}
                     options/SuppressLogMessages.py
                     options/DV-Stripping22-Stripping.py 
                     options/SilenceErrors.py 
                     LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                     DEPENDS ${s23name}
                     SPLIT 50)

 set ( s21name S21FunctorCache_${time_date} )
 loki_functors_cache(${s21name}
                     options/SuppressLogMessages.py
                     options/DV-Stripping21-Stripping.py 
                     options/SilenceErrors.py 
                     LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                     DEPENDS ${s22name}
                     SPLIT 50)

 set ( s20name S20FunctorCache_${time_date} )
 loki_functors_cache(${s20name}
                     options/SuppressLogMessages.py
                     options/DV-Stripping20-Stripping.py 
                     options/SilenceErrors.py 
                     LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                     DEPENDS ${s21name}
                     SPLIT 50)

endif()
