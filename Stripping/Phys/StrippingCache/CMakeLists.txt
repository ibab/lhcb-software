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
                         Phys/StrippingAlgs
                         Phys/StrippingNeuroBayes
                         Phys/IncTopoVert
                         Phys/TMVASelections
                         Phys/DisplVertices
                         Phys/IsolationTools
                         Phys/RelatedInfoTools)

# Do not build for the -O0 platform, as this seems to have problems.
# To be investigated...
if ( NOT "$ENV{CMTCONFIG}" STREQUAL "x86_64-slc6-gcc48-do0" )

 foreach(pack DaVinci StrippingAlgs StrippingNeuroBayes IncTopoVert TMVASelections
              DisplVertices IsolationTools RelatedInfoTools)
   # FIXME: this will be needed until GAUDI-1055 is fixed.
   if(EXISTS ${CMAKE_BINARY_DIR}/Phys/${pack}/genConf/${pack})
     gaudi_build_env(PREPEND PYTHONPATH ${CMAKE_BINARY_DIR}/Phys/${pack}/genConf/${pack})
   endif()
 #  if(TARGET ${pack}Conf)
 #    set(deps ${conf_deps} ${pack}Conf )
 #  endif()
 #  if(TARGET ${pack}ComponentsList)
 #    set(deps ${conf_deps} ${pack}ComponentsList )
 #  endif()
 endforeach()

 # Append build path with component list so each cache build skips functors already cached.
 gaudi_build_env( PREPEND LD_LIBRARY_PATH ${CMAKE_BINARY_DIR}/Phys/StrippingCache )

 # Unique stamp for library names
 string( MD5 name_stamp ${CMAKE_BINARY_DIR} )
 MESSAGE( STATUS "NameStamp : " ${CMAKE_BINARY_DIR} " MD5-> " ${name_stamp} )
 set( name_stamp _${name_stamp} )
 
 # Import the cache creation module
 include(LoKiFunctorsCache)

 # First cache depends on DaVinci, so they are built last
 set ( conf_deps ${deps} DaVinciConfUserDB )

 # Build the caches, most recent first
 foreach(strip 23 22 21 20r3 20r2 20r0p3 20r0p2 20r0p1 20)

  # Stripping cache name
  set ( sname S${strip}FunctorCache${name_stamp} )

  # Stripping options file to use. If exists use AppConfig file, otherwise try local
  #set ( optsfile $ENV{APPCONFIGROOT}/options/DaVinci/DV-Stripping${strip}-Stripping.py )
  #if ( NOT EXISTS ${optsfile} )
  set ( optsfile ${CMAKE_CURRENT_SOURCE_DIR}/options/DV-Stripping${strip}-Stripping.py )
  #endif()

  # cache build if options exist
  if ( EXISTS ${optsfile} )

    # Print the dependencies
    MESSAGE( STATUS "S"${strip} " : " ${optsfile} )

    # Build !
    loki_functors_cache(${sname}
                        options/SuppressLogMessages.py
                        ${optsfile}
                        options/SilenceErrors.py 
                        LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                        DEPENDS ${conf_deps}
                        SPLIT 50)

    # Update dependency for next cache
    set ( conf_deps ${deps} ${sname} )

  endif()

 endforeach()
 
endif()
