################################################################################
# Package: StrippingCache
################################################################################
gaudi_subdir(StrippingCache v1r0)

gaudi_depends_on_subdirs(Phys/LoKiArrayFunctors
                         Phys/LoKiHlt)

# Install CMake modules
gaudi_install_cmake_modules()
# note: gaudi_install_cmake_modules only install .cmake files
install(FILES cmake/LoKiFunctorsCachePostActionOpts.py
        DESTINATION cmake)


# Build the cache
include(LoKiFunctorsCache)
loki_functors_cache(S21FunctorsCache
                    options/DV-Stripping21-Stripping.py options/DataType.py
                    LINK_LIBRARIES LoKiArrayFunctorsLib LoKiHltLib
                    SPLIT 50)
