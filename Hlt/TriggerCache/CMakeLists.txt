################################################################################
# Package: TriggerCache
################################################################################
gaudi_subdir(TriggerCache v11r21)

gaudi_depends_on_subdirs(Phys/LoKiTrigger
                         Phys/LoKiArrayFunctors 
                         Phys/LoKiHlt) 

gaudi_add_library(TriggerCache
                  src/*.cpp
                  LINK_LIBRARIES LoKiHltLib LoKiArrayFunctorsLib LoKiTriggerLib )

gaudi_generate_componentslist(TriggerCache) 

