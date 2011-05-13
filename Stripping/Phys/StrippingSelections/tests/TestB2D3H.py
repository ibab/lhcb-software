#!/usr/bin/env python


# Test single line of the stripping
#QUESTION if there is a question
#ADDED if there is an addition or change

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci, DecayTreeTuple
from StrippingConf.Configuration import StrippingConf

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from StrippingSelections import StrippingB2D3H as StrippingB2D3H #ADDED

StrippingB2D3H = StrippingB2D3H.B2D3HAllLinesConf("StrippingB2D3H",StrippingB2D3H.confdict) #ADDED

from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")

stream.appendLines( 
                     StrippingB2D3H.lines()
                    )


from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

# Configure the stripping using the same options as in Reco06-Stripping10
sc = StrippingConf( Streams = [ stream ],
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents )
sc.OutputType = "ETC"                    # Can be either "ETC" or "DST"

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

# Mimic the new default tracking cuts
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts = { "Chi2Cut" : [0,5] }

# Configure the ETC writing step
from Configurables import EventTuple, TupleToolSelResults
from Configurables import TupleToolStripping

tag = EventTuple("TagCreator")
tag.EvtColsProduce = True
tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolStripping"  ]
tag.addTool( TupleToolStripping )
tag.TupleToolStripping.StrippigReportsLocations = "/Event/Strip/Phys/DecReports"

# Remove the microbias and beam gas etc events before doing the tagging step
regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
from Configurables import LoKi__HDRFilter
filterHLT = LoKi__HDRFilter("FilterHLT",Code = regexp )

seq = GaudiSequencer("TagSeq")
seq.Members = [tag]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())

#---->
# vanyas suggestion of correlation matrix here:
from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())
#<----

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

DaVinci().PrintFreq = 2000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().ETCFile = "etc.root"
DaVinci().EvtMax = 50000   #Originally 200k
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
DaVinci().MoniSequence += [ seq ]            # Append the TagCreator to DaVinci
DaVinci().DataType = "2010"
DaVinci().InputType = 'SDST'
#importOptions("$STRIPPINGSELECTIONSROOT/tests/data/RUN_79646_RealData+Reco06-Stripping10_90000000_SDST.py")
#more statistics (need to add += in the data .py file):
#importOptions("$STRIPPINGSELECTIONSROOT/tests/data/RUN_79647_RealData+Reco06-Stripping10_90000000_SDST.py")
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/RUN_81430_RealData+Reco08-Stripping12_90000000_SDST.py")


#DaVinci().Simulation = True
