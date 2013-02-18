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

find_package(NeuroBayesExpert)

gaudi_install_headers(Kernel)

gaudi_add_module(JetTagging
                 src/*.cpp
                 INCLUDE_DIRS NeuroBayesExpert
                 LINK_LIBRARIES LoKiAlgo LHCbMathLib LoKiPhysMCLib DaVinciInterfacesLib NeuroBayesExpert)

gaudi_add_dictionary(JetTagging
                     dict/JetTaggingInterface.h
                     dict/JetTaggingInterface.xml
                     LINK_LIBRARIES GaudiKernel TrackEvent RecEvent PhysEvent
                     OPTIONS " -U__MINGW32__ ")

gaudi_install_python_modules()

