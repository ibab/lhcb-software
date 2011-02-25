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
from StrippingSettings.Utils import lineBuilderConfiguration

# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")

#
# Import your stripping lines
#


# B->hh (lifetime biased)
from StrippingSelections.StrippingHb2Charged2Body import Hb2Charged2BodyLines
paramBhh = { 'PrescaleB2Charged2Body'   : 1,      'PrescaleB2PPbar' : 1,
             'MinPTB2Charged2Body'      : 1100,    'MinPTB2PPbar' : 900,
             'MinIPB2Charged2Body'      : 0.15,
             'MinIPChi2B2Charged2Body'  : 100,     'MinIPChi2B2PPbar' : 16,
             'TrChi2'                   : 5,
             'PIDppi'                   : 0,
             'PIDpk'                    : -2,
             'MaxPTB2Charged2Body'      : 2700,   'MaxPTB2PPbar' : 2400,
             'MaxIPB2Charged2Body'      : 0.3,
             'MaxIPChi2B2Charged2Body'  : 200,    'MaxIPChi2B2PPbar' : 36,
             'CombMassLow'              : 4800,
             'CombMassHigh'             : 5800,   'CombMassWindow' : 200,
             'DOCA'                     : 0.08,   'VertexChi2B2PPbar' : 16,
             'BPT'                      : 1200,   'BPTB2PPbar' : 1000,
             'BIP'                      : 0.06,
             'BIPChi2B2Charged2Body'    : 12,     'BIPChi2B2PPbar' : 36,
             'BDIRA'                    : 0.9995,
             'BTAU'                     : 0.0009,
             'MassLow'                  : 4800,
             'MassHigh'                 : 5800
             }
confBhh          = Hb2Charged2BodyLines("Bhh", paramBhh)
stream.appendLines(confBhh.lines())

# B->hh (lifetime unbiased)
from StrippingSelections import StrippingB2hhLTUnbiased
paramBhhUnbiased = lineBuilderConfiguration('StrippingDev', 'B2hhLTUnbiased' )['CONFIG']
paramBhhUnbiased['NetCut'] = (0.85 + 1.0)/2.0
print "NetCut %lf" % paramBhhUnbiased['NetCut']


confBhhUnbiased  = StrippingB2hhLTUnbiased.StrippingB2hhLTUnbiasedConf("BhhLTUnbiased", paramBhhUnbiased)
stream.appendLines(confBhhUnbiased.lines())


# B->hhh
from StrippingSelections.StrippingBu3hFrom2h import Bu3hFrom2hBuilder
paramsBu3hFrom2h = {
    'MaxTrSIZE':  None,
    '_h_PT': 700. ,
    '_h_P': 5000. ,
    '_h_IPCHI2': 16. ,
    '_h_TRCHI2DOF': 3.0 ,
    '_2hLoose_DOCA': .12 ,
    '_2h_PTmax': 2000. ,
    '_2h_PTsum':4000.  ,
    '_2h_PVIPCHI2sum': 100. ,
    '_2h_FDCHI2': 150. ,
    '_2h_DIRA': 0.99 ,
    '_2h_CORRMmax': 6000. ,
    '_2h_CORRMmin': 4000. ,
    '_2h_CharmVetoIPCHI2': 16. ,
    '_2h_CharmVetoM': 2500. ,
    '_2h_TRKCHIDOFmin': 3. ,
    '_2h_Mmin': 3500. ,
    '_2h_PVDOCAmin': 1.0 ,
    '_2h_CHI2': 14.0 ,
    '_2h_PTRANS': 4000. ,
    '_3h_DOCA': .14 ,
    '_3h_PTmax': 2000 ,
    '_3h_DIRA': .9998 ,
    '_3h_FDCHI2': 150. ,
    '_3h_PVDOCAmin': 1.0 ,
    '_3h_CHI2': 14.0 ,
    '_3h_IP': 0.1 ,
    '_3h_PT': 1000. ,
    '_3h_PTsum':4250.  ,
    '_3h_PVIPCHI2sum': 150. ,
    '_3h_Charge': 1 ,
    '_3h_CORRMmax': 7000. ,
    '_3h_CORRMmin': 4000. ,
    '_3h_Mmax': 6000. ,
    '_3h_Mmin': 4000. ,
    '2hLinePrescale': 1. ,
    '2hLinePostscale': 1 ,
    '3hLinePrescale': 1 ,
    '3hLinePostscale': 1}

bu3hfrom2hBuilder = Bu3hFrom2hBuilder("bu3hfrom2hBuilder", paramsBu3hFrom2h)
stream.appendLines( bu3hfrom2hBuilder.lines() )


#
# remaining config
#
from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

# Configure the stripping using the same options as in Reco06-Stripping10
sc = StrippingConf( Streams = [ stream ],
                    MaxCandidates = 50000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents )
sc.OutputType = "ETC"                    # Can be either "ETC" or "DST"

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

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

from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True



DaVinci().PrintFreq = 2000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().ETCFile = "etc.root"
DaVinci().EvtMax = 50000
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
DaVinci().MoniSequence += [ seq ]            # Append the TagCreator to DaVinci
DaVinci().DataType = "2010"
DaVinci().InputType = 'SDST'
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/RUN_81430_RealData+Reco08-Stripping12_90000000_SDST.py")
