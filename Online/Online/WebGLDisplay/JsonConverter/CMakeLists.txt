################################################################################
# Package: JsonConverter
################################################################################
gaudi_subdir(JsonConverter v0r1)

gaudi_depends_on_subdirs(Tf/PatKernel
			 Tf/TfKernel
			 Det/STDet
			 Det/OTDet
			 Event/RecEvent
			 Event/TrackEvent
			 OT/OTDAQ
			 Kernel/PartProp
			 Rec/Brunel
			 Tr/TrackInterfaces
                         DAQ/MDF
			 GaudiAlg
                         Online/DIM
                         Online/OnlineKernel
                         Online/Gaucho)

gaudi_add_module(JsonConverter src/*.cpp
                 LINK_LIBRARIES GaudiKernel GaudiAlgLib )
