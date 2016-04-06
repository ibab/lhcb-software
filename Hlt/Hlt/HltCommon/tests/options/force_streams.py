from Configurables import HltOutputSvc, HltOutputSequence

outputSvc = HltOutputSvc()
outStream = HltOutputSequence()
outStream.ForceStreams = {'' : outputSvc.Filters['Full']}

