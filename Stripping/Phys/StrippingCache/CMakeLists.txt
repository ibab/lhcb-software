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

# Do not build for the -O0 platform, as this seems to have problems.
# To be investigated...
if ( NOT "$ENV{CMTCONFIG}" STREQUAL "x86_64-slc6-gcc48-do0" )

 MESSAGE( STATUS "StrippingCache Deps : " ${conf_deps} )

 include(LoKiFunctorsCache)

 loki_functors_cache(S23FunctorsCache
                     options/SuppressLogMessages.py
		     options/DV-Stripping23-Stripping.py options/SilenceErrors.py 
                     LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                     DEPENDS ${conf_deps}
                     SPLIT 50)
 
endif()
