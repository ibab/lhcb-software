from Boole.Configuration import *

boole = Boole() # call to ensure Boole is setup 

# kill xdst stuff
xdstPaths = [ 
        #added by Brunel:
    "/Event/Trigger",
    "/Event/Rich",
    "/Event/Calo",
    "/Event/Muon",
    "/Event/Other",
    "/Event/Rec",
    "/Event/pRec",
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
    '/Event/MC/TrackInfo',
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
    '/Event/Link/Rec/Track/Best',
    '/Event/Link/Rec/Track',
    '/Event/Link/Rec',
    '/Event/Link/Rec/Calo/Photons',
    '/Event/Link/Rec/Calo/Electrons',
    '/Event/Link/Rec/Calo/MergedPi0s',
    '/Event/Link/Rec/Calo/SplitPhotons',
    '/Event/Link/Rec/Calo',
    '/Event/pSim/Rich/DigitSummaries',
    '/Event/Rec/Header',
    '/Event/Rec/Status',
    '/Event/Rec/Summary',
    '/Event/Trigger/RawEvent',
    '/Event/Tracker/RawEvent',
    '/Event/Velo/RawEvent',
    '/Event/Calo/RawEvent',
    '/Event/Calo',
    '/Event/Muon/RawEvent',
    '/Event/Muon',
    '/Event/Rich/RawEvent',
    '/Event/Rich',
    '/Event/Other/RawEvent',
    '/Event/Other',
    '/Event/pRec/Track/Best',
    '/Event/pRec/Track/Muon',
    '/Event/pRec/Track',
    '/Event/pRec',
    '/Event/pRec/Rich/PIDs',
    '/Event/pRec/Rich',
    '/Event/pRec/Muon/MuonPID',
    '/Event/pRec/Muon',
    '/Event/pRec/Calo/Photons',
    '/Event/pRec/Calo/Electrons',
    '/Event/pRec/Calo/MergedPi0s',
    '/Event/pRec/Calo/SplitPhotons',
    '/Event/pRec/Calo',
    '/Event/pRec/ProtoP/Charged',
    '/Event/pRec/ProtoP/Neutrals',
    '/Event/pRec/ProtoP',
    '/Event/pRec/Vertex/Primary',
    '/Event/pRec/Vertex/V0',
    '/Event/pRec/Vertex']

extraLoad = ['/Event/Link/MC/Rich/Hits2MCRichOpticalPhotons',
             '/Event/Link/MC/Particles2MCRichTracks']

from Configurables import (TESCheck,EventNodeKiller)
initBoole = GaudiSequencer("InitBooleSeq")
xdstLoader = TESCheck("XDSTLoader")
xdstLoader.Inputs = xdstPaths+extraLoad
xdstLoader.Stop   = False # If not MC do not expect all of the entries
xdstLoader.OutputLevel = ERROR
xdstKiller = EventNodeKiller("XDSTKiller")
xdstKiller.Nodes = xdstPaths
xdstHandler = GaudiSequencer("XDSTLoverHandler")
xdstHandler.Members += [ xdstLoader, xdstKiller ]
xdstHandler.IgnoreFilterPassed = True # keep going
initBoole.Members += [ xdstHandler ]


from GaudiConf import IOHelper
def patch(): 
	OutputStream("DigiWriter").ItemList += ["/Event/Link/MC#1"] 
appendPostConfigAction(patch)
