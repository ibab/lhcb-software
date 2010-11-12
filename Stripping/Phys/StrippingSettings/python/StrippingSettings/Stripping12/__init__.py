def streams():
	from StrippingSettings.Stripping12 import StreamCalibration
	from StrippingSettings.Stripping12 import StreamBhadron
	from StrippingSettings.Stripping12 import StreamDielectron
	from StrippingSettings.Stripping12 import StreamDimuon
	from StrippingSettings.Stripping12 import StreamMiniBias
	from StrippingSettings.Stripping12 import StreamSemileptonic
	from StrippingSettings.Stripping12 import StreamRadiative
	from StrippingSettings.Stripping12 import StreamEW
	from StrippingSettings.Stripping12 import StreamCharmControl
	from StrippingSettings.Stripping12 import StreamCharmMicroDST
	from StrippingSettings.Stripping12 import StreamCharmFull
	from StrippingSettings.Stripping12 import StreamLeptonicFull
	from StrippingSettings.Stripping12 import StreamLeptonicMicroDST

	allStreams = [
        	StreamCalibration.stream,
        	StreamBhadron.stream,
        	StreamDielectron.stream,
        	StreamDimuon.stream,
        	StreamMiniBias.stream,
        	StreamRadiative.stream,
        	StreamEW.stream,
        	StreamSemileptonic.stream,
        	StreamLeptonicFull.stream,
        	StreamLeptonicMicroDST.stream,
        	StreamCharmControl.stream,
        	StreamCharmFull.stream,
        	StreamCharmMicroDST.stream,
        	]
	return allStreams
	
