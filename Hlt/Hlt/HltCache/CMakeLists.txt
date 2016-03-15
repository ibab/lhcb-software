################################################################################
# Package: StrippingCache
################################################################################
gaudi_subdir(HltCache v1r0)

gaudi_depends_on_subdirs(Hlt/Moore
                         Phys/LoKiArrayFunctors
                         Phys/LoKiHlt
                         Phys/LoKiPhys
                         Phys/LoKiAlgo
                         Phys/LoKiCore
                         Phys/LoKiTrigger
                         Phys/TisTosTobbing
                         Phys/RelatedInfoTools
                         Hlt/HltDAQ
                         Hlt/HltRawData
                         Hlt/HltConf
                         Hlt/HltTracking
                         Hlt/HltSettings
                         Hlt/Hlt1Lines
                         Hlt/Hlt2Lines
                         Hlt/HltServices
                         Hlt/HltLine)


foreach(pack Hlt/Moore Hlt/HltConf Hlt/HltTracking Hlt/HltSettings Hlt/Hlt1Lines
             Hlt/Hlt2Lines Hlt/HltServices Hlt/HltDAQ Hlt/HltRawData Hlt/HltLine
             Phys/LoKiTrigger Phys/TisTosTobbing Phys/RelatedInfoTools)
  # FIXME: this will be needed until GAUDI-1055 is fixed.
  if(EXISTS ${CMAKE_BINARY_DIR}/${pack}/genConf/${pack})
    gaudi_build_env(PREPEND PYTHONPATH ${CMAKE_BINARY_DIR}/${pack}/genConf/${pack})
  endif()
endforeach()

# Import the cache creation module
include(LoKiFunctorsCache)

set(conf_deps ${deps} MooreConfUserDB)

#options/SuppressLogMessages.py
#options/SilenceErrors.py

set(LOKI_FUNCTORS_CACHE_POST_ACTION_OPTS
    ${CMAKE_CURRENT_SOURCE_DIR}/options/LoKiFunctorsCacheHltPostActionOpts.py)
    
loki_functors_cache(Hlt_pp_Draft2016_FunctorCache
                    options/Physics_pp_Draft2016.py
                    options/DisableLoKiCacheFunctors.py
                    FACTORIES CoreFactory TrackFunctorFactory HltFactory TrackFactory
                              LoKi__Hybrid__HltFactory
                              Hlt1CoreFactory Hlt1HltFactory Hlt1Factory Hlt1HybridFactory
                              Hlt2TrackFunctorFactory Hlt2CoreFactory  Hlt2HybridFactory
                              Hlt2HltFactory
                    LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib LoKiTriggerLib
                    DEPENDS ${conf_deps}
                    SPLIT 10)
