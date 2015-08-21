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

# Print the dependencies
#MESSAGE( STATUS "StrippingCache Deps : " ${conf_deps} )

# Build the caches

# Do not build for the -O0 platform, as this seems to have problems.
# To be investigated...
if ( NOT "$ENV{CMTCONFIG}" STREQUAL "x86_64-slc6-gcc48-do0" )

 # Append build path with component list so each cache build skips functors already cached.
 gaudi_build_env( PREPEND LD_LIBRARY_PATH ${CMAKE_BINARY_DIR}/Phys/StrippingCache )

 # Unique stamp for library names
 string( MD5 name_stamp ${CMAKE_BINARY_DIR} )
 set( name_stamp _${name_stamp} )
 
 include(LoKiFunctorsCache)

 set( s23name S23FunctorCache${name_stamp} )
 loki_functors_cache(${s23name}
                     options/SuppressLogMessages.py
                     options/DV-Stripping23-Stripping.py 
                     options/SilenceErrors.py 
                     LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                     DEPENDS ${conf_deps}
                     SPLIT 50)

 set ( s22name S22FunctorCache${name_stamp} )
 loki_functors_cache(${s22name}
                     options/SuppressLogMessages.py
                     options/DV-Stripping22-Stripping.py 
                     options/SilenceErrors.py 
                     LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                     DEPENDS ${s23name}
                     SPLIT 20)

 set ( s21name S21FunctorCache${name_stamp} )
 loki_functors_cache(${s21name}
                     options/SuppressLogMessages.py
                     options/DV-Stripping21-Stripping.py 
                     options/SilenceErrors.py 
                     LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                     DEPENDS ${s22name}
                     SPLIT 50)

 set ( s20r3name S20r3FunctorCache${name_stamp} )
 loki_functors_cache(${s20r3name}
                     options/SuppressLogMessages.py
                     options/DV-Stripping20r3-Stripping.py 
                     options/SilenceErrors.py 
                     LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                     DEPENDS ${s21name}
                     SPLIT 20)

 set ( s20r2name S20r2FunctorCache${name_stamp} )
 loki_functors_cache(${s20r2name}
                     options/SuppressLogMessages.py
                     options/DV-Stripping20r2-Stripping.py 
                     options/SilenceErrors.py 
                     LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                     DEPENDS ${s20r3name}
                     SPLIT 20)

 set ( s20r0p3name S20r0p3FunctorCache${name_stamp} )
 loki_functors_cache(${s20r0p3name}
                     options/SuppressLogMessages.py
                     options/DV-Stripping20r0p3-Stripping.py 
                     options/SilenceErrors.py 
                     LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                     DEPENDS ${s20r2name}
                     SPLIT 20)

 set ( s20r0p2name S20r0p2FunctorCache${name_stamp} )
 loki_functors_cache(${s20r0p2name}
                     options/SuppressLogMessages.py
                     options/DV-Stripping20r0p2-Stripping.py 
                     options/SilenceErrors.py 
                     LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                     DEPENDS ${s20r0p3name}
                     SPLIT 20)

 set ( s20r0p1name S20r0p1FunctorCache${name_stamp} )
 loki_functors_cache(${s20r0p1name}
                     options/SuppressLogMessages.py
                     options/DV-Stripping20r0p1-Stripping.py 
                     options/SilenceErrors.py 
                     LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                     DEPENDS ${s20r0p2name}
                     SPLIT 20)

 set ( s20name S20FunctorCache${name_stamp} )
 loki_functors_cache(${s20name}
                     options/SuppressLogMessages.py
                     options/DV-Stripping20-Stripping.py 
                     options/SilenceErrors.py 
                     LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                     DEPENDS ${s20r0p1name}
                     SPLIT 50)

endif()
