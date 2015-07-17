################################################################################
# Package: StrippingCache
################################################################################
gaudi_subdir(StrippingCache v1r0)

gaudi_depends_on_subdirs(Phys/LoKiArrayFunctors
                         Phys/LoKiHlt)

# Build the cache
include(LoKiFunctorsCache)
loki_functors_cache(S21FunctorsCache
                    options/DV-Stripping21-Stripping.py #options/DataType.py
                    LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                    SPLIT 50)

loki_functors_cache(S20FunctorsCache
                    options/DV-Stripping20-Stripping.py #options/DataType.py
                    LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                    SPLIT 50)

