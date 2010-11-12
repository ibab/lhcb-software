def streams():
	''' These options were taken from Stripping11 AppConfig file'''
	from StrippingSettings.Stripping11 import StreamCalibration
	from StrippingSettings.Stripping11 import StreamBhadron
	from StrippingSettings.Stripping11 import StreamCharm
	from StrippingSettings.Stripping11 import StreamDielectron
	from StrippingSettings.Stripping11 import StreamDimuon
	from StrippingSettings.Stripping11 import StreamMiniBias
	from StrippingSettings.Stripping11 import StreamSemileptonic
	from StrippingSettings.Stripping11 import StreamRadiative
	from StrippingSettings.Stripping11 import StreamEW
	from StrippingSettings.Stripping11 import StreamCharmMicroDST
	from StrippingSettings.Stripping11 import StreamLeptonicMicroDST

	streamBhadron =  StreamBhadron.stream
	streamCharm   =  StreamCharm.stream

	from StrippingSelections.StrippingB2twobody_prompt import B2twobody_promptLine
	streamBhadron.appendLines( B2twobody_promptLine() ) 

	from StrippingSelections.StrippingD2hhh_conf import StrippingD2hhhConf
	streamCharm.appendLines( [
    		StrippingD2hhhConf().stripDs2PPP() ,
    		StrippingD2hhhConf().stripDs2KKP() ,
    		StrippingD2hhhConf().stripDs2KPPos()
    		] )

	# Now need a hack to rename the streams
	# First, rename the "LeptonicMicroDST" stream to "Leptonic"
	# Files will then end with "leptonic.mdst"
	from StrippingConf.StrippingStream import StrippingStream
	streamLeptonic = StrippingStream("Leptonic")
	streamLeptonic.appendLines( StreamLeptonicMicroDST.stream.lines )

	# Now create a new stream for the full charm, called "CharmControl"
	# Files will end with "charmcontrol.dst"
	streamCharmControl = StrippingStream("CharmControl")
	streamCharmControl.appendLines( StreamCharm.stream.lines )

	# Finally, rename the "CharmMicroDST" stream to "Charm"
	# Files will end with "charm.mdst"
	streamCharmNew = StrippingStream("Charm")
	streamCharmNew.appendLines( StreamCharmMicroDST.stream.lines )

	allStreams = [
    		StreamCalibration.stream, 
    		streamBhadron,
    		streamCharmControl,
    		StreamDielectron.stream,
    		StreamDimuon.stream,
    		StreamMiniBias.stream,
    		StreamSemileptonic.stream,
    		StreamRadiative.stream,
    		StreamEW.stream,
    		streamCharmNew,
    		streamLeptonic
    		]

	return allStreams

