# $Id: $
# Test your line(s) of the stripping


#
# for Physics WG CP Gamma/Loop
#
# contact: V. Vagnoni, U. Kerzel
#

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

#
# Import your stripping lines
#

# B-> hh (lifetime unbiased, mass hypothesis Bs->KK)
from StrippingSelections.StrippingB2hhLTUnbiased import StrippingB2hhLTUnbiasedConf as B2hhLTUnbiased

# B-> hh (lifetime biased, mass hypothesis Bd->pi pi),  B-> pp
from StrippingSelections.StrippingHb2Charged2Body import Hb2Charged2BodyLines

# B-> hhh
from StrippingSelections import StrippingBu2hhh
from StrippingSelections.StrippingB2KShh import StrippingB2KShhConf

# B-> hhh (for testing)
from StrippingSelections import StrippingBu3hFrom2h


# B -> hhpi0
from StrippingSelections.StrippingB2HHPi0 import StrippingB2HHPi0Conf

#
# Now build the stream
#

# for the BHadron stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")

stream.appendLines( Hb2Charged2BodyLines().lines() )

stream.appendLines( [StrippingB2KShhConf().B2KSLLhh(),
                     StrippingB2KShhConf().B2KSDDhh(),
                     StrippingBu2hhh.line
                     ])

stream.appendLines( [StrippingBu3hFrom2h.line1,
                     StrippingBu3hFrom2h.line2
                     ])

stream.appendLines( [B2hhLTUnbiased().StripB2hhLTUnbiasedLoose(),
                     B2hhLTUnbiased().StripB2hhLTUnbiased(),
                     B2hhLTUnbiased().StripB2hhLTUnbiasedTrigger()] )

stream.appendLines( [StrippingB2HHPi0Conf().B2HHPi0_Merged(),
                     StrippingB2HHPi0Conf().B2HHPi0_Resolved()
                     ])

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

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

DaVinci().PrintFreq = 500
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().ETCFile = "etc.root"
DaVinci().EvtMax = 100000
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().MoniSequence += [ seq ]                     # Append the TagCreator to DaVinci
DaVinci().DataType = "2010"
DaVinci().InputType = 'SDST'
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/RUN_79646_RealData+Reco06-Stripping10_90000000_SDST.py")

