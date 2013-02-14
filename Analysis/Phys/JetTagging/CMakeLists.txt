################################################################################
# Package: JetTagging
################################################################################
gaudi_subdir(JetTagging v1r0)

gaudi_depends_on_subdirs(Phys/LoKiJets
			 Phys/LoKiAlgo
			 Kernel/LHCbMath
			 Phys/JetAccessories
			 Phys/JetAccessoriesMC
			 Phys/LoKiPhysMC
			 Phys/DaVinciInterfaces
			 )

gaudi_add_module(JetTagging
                 src/*.cpp
                 LINK_LIBRARIES   LoKiJetsLib LoKiAlgoLib LHCbMathLib JetAccessoriesLib JetAccessoriesMCLib LoKiPhysMCLib DaVinciInterfacesLib)

