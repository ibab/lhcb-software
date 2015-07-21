################################################################################
# Package: StrippingCache
################################################################################
gaudi_subdir(StrippingCache v1r0)

gaudi_depends_on_subdirs(Phys/LoKiArrayFunctors
                         Phys/LoKiHlt
                         Phys/StrippingAlgs
                         Phys/StrippingSelections
                         Phys/StrippingNeuroBayes
                         Phys/IncTopoVert
                         Phys/TMVASelections
                         Phys/DisplVertices
                         Phys/IsolationTools
                         Phys/StrippingArchive)

foreach(pack StrippingAlgs StrippingNeuroBayes IncTopoVert TMVASelections
             DisplVertices IsolationTools)
  # FIXME: this will be needed until GAUDI-1055 is fixed.
  if(EXISTS ${CMAKE_BINARY_DIR}/Phys/${pack}/genConf)
    gaudi_build_env(PREPEND LD_LIBRARY_PATH
                    ${CMAKE_BINARY_DIR}/Phys/${pack}/genConf/${pack}
                    PREPEND LD_LIBRARY_PATH
                    ${CMAKE_BINARY_DIR}/Phys/${pack})
  endif()
  if(TARGET ${pack}Conf)
    set(conf_deps ${conf_deps} ${pack}Conf)
  endif()
  if(TARGET ${pack}Conf)
    set(conf_deps ${conf_deps} ${pack}ComponentsList)
  endif()
endforeach()

# Build the cache
include(LoKiFunctorsCache)
loki_functors_cache(S21FunctorsCache
                    options/SuppressS21LogMessages.py
                    options/DV-Stripping21-Stripping.py options/SilenceErrors.py #options/DataType.py
                    LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                    DEPENDS ${conf_deps}
                    SPLIT 50)

loki_functors_cache(S20FunctorsCache
                    options/SuppressS21LogMessages.py
                    options/DV-Stripping20-Stripping.py options/SilenceErrors.py #options/DataType.py
                    LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                    DEPENDS ${conf_deps}
                    SPLIT 50)

