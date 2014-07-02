from Boole.Configuration import *

boole = Boole() # call to ensure Boole is setup 

# kill xdigi stuff
xdigiPaths = [ 
        #added by Brunel:
    "/Event/Trigger",
    "/Event/Rich",
    "/Event/Calo",
    "/Event/Muon",
    "/Event/Other",
    #added by Boole
    "/Event/Link/Raw",
    "/Event/DAQ",
    "/Event/pSim/Rich/DigitSummaries",
    "/Event/MC/TrackInfo",
    "/Event/MC/Muon",
    "/Event/MC/DigiHeader",
    #not always there, depends on the DataType and format
    "/Event/Link/Trig",
    "/Event/MC/Rich/DigitSummaries",
    "/Event/Prev/DAQ",
    "/Event/PrevPrev/DAQ",
    "/Event/Next/DAQ",
    "/Event/NextNext/DAQ"            
    '/Event/MC/DigiHeader',
    '/Event/MC/Muon',
    '/Event/MC/Muon/DigitsInfo',
    '/Event/Link/Raw/Velo/Clusters2MCHits',
    '/Event/Link/Raw/Velo/Clusters',
    '/Event/Link/Raw/Velo',
    '/Event/Link/Raw/TT/Clusters2MCHits',
    '/Event/Link/Raw/TT/Clusters',
    '/Event/Link/Raw/TT',
    '/Event/Link/Raw/IT/Clusters2MCHits',
    '/Event/Link/Raw/IT/Clusters',
    '/Event/Link/Raw/IT',
    '/Event/Link/Raw/OT/Times2MCHits',
    '/Event/Link/Raw/OT/Times',
    '/Event/Link/Raw/OT',
    '/Event/Link/Raw/Ecal/Digits',
    '/Event/Link/Raw/Ecal',
    '/Event/Link/Raw/Hcal/Digits',
    '/Event/Link/Raw/Hcal',
    '/Event/Link/Raw/Muon/Digits',
    '/Event/Link/Raw/Muon',
    '/Event/Link/Raw',
    '/Event/Trigger/RawEvent',
    '/Event/Calo/RawEvent',
    '/Event/Calo',
    '/Event/Muon/RawEvent',
    '/Event/Muon',
    '/Event/Rich/RawEvent',
    '/Event/Rich',
    '/Event/Other/RawEvent'
    ]

extraLoad = ['/Event/Link/MC/Rich/Hits2MCRichOpticalPhotons',
             '/Event/Link/MC/Particles2MCRichTracks']

from Configurables import (TESCheck,EventNodeKiller)
initBoole = GaudiSequencer("InitBooleSeq")
xdigiLoader = TESCheck("XDIGILoader")
xdigiLoader.Inputs = xdigiPaths+extraLoad
xdigiLoader.Stop   = False # If not MC do not expect all of the entries
xdigiLoader.OutputLevel = ERROR
xdigiKiller = EventNodeKiller("XDIGIKiller")
xdigiKiller.Nodes = xdigiPaths
xdigiHandler = GaudiSequencer("XDIGILoverHandler")
xdigiHandler.Members += [ xdigiLoader, xdigiKiller ]

xdigiHandler.IgnoreFilterPassed = True # keep going
initBoole.Members += [ xdigiHandler ]


from GaudiConf import IOHelper
def patch(): 
	OutputStream("DigiWriter").ItemList += ["/Event/Link/MC#1"] 
appendPostConfigAction(patch)
