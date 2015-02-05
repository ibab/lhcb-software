################################################################################
# Package: StrippingCache
################################################################################
gaudi_subdir(StrippingCache v1r0)

gaudi_depends_on_subdirs(Phys/LoKiArrayFunctors 
                         Phys/LoKiHlt) 

gaudi_add_library(StrippingCache
                  src/*.cpp
                  LINK_LIBRARIES LoKiHltLib LoKiArrayFunctorsLib)

gaudi_generate_componentslist(StrippingCache) 

