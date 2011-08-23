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

##
## B->hh (lifetime biased)
##
from StrippingSelections.StrippingHb2Charged2Body import Hb2Charged2BodyLines
paramBhh = { 
    'PrescaleB2Charged2Body'   : 1,      'PrescaleB2PPbar' : 1,
    'MinPTB2Charged2Body'      : 1000,    'MinPTB2PPbar' : 900,
    'MinIPB2Charged2Body'      : 0.12,
    'MinIPChi2B2Charged2Body'  : 100,     'MinIPChi2B2PPbar' : 9,
    'TrChi2'                   : 5,
    'PIDppi'                   : 0,
    'PIDpk'                    : -2,
    'MaxPTB2Charged2Body'      : 2500,   'MaxPTB2PPbar' : 2100,
    'MaxIPB2Charged2Body'      : 0.25,
    'MaxIPChi2B2Charged2Body'  : 200,    'MaxIPChi2B2PPbar' : 25,
    'CombMassLow'              : 4800,
    'CombMassHigh'             : 5800,   'CombMassWindow' : 200,
    'DOCA'                     : 0.08,   'VertexChi2B2PPbar' : 16,
    'BPT'                      : 1200,   'BPTB2PPbar' : 1000,
    'BIP'                      : 0.1,
    'BIPChi2B2Charged2Body'    : 12,     'BIPChi2B2PPbar' : 36,
    'BDIRA'                    : 0.9995,
    'BTAU'                     : 0.0006,
    'MassLow'                  : 4800,
    'MassHigh'                 : 5800
       }
confBhh          = Hb2Charged2BodyLines("Bhh", paramBhh)
stream.appendLines(confBhh.lines())

##
## B->hh (lifetime unbiased)
##
from StrippingSelections import StrippingB2hhLTUnbiased

paramBhhUnbiased ={
    'TrackChi2'               :    5.0
 , 'DaughterPtMin'         :    1.5 # GeV
 , 'DaughterPtMax'         :    2.0 # GeV
 , 'DaughterPMin'          :   10.0 # GeV
 , 'DaughterPIDKMax'       :    0.1
 , 'BMassMin'              :    5.0 # GeV
 , 'BMassMax'              :    6.0 # GeV
 , 'DOCA'                  :    0.07
 , 'VertexChi2'            :   25.0
 , 'PrescaleLoose'         :    0.005
 , 'PostscaleLoose'        :    1.0
 , 'PrescaleNB'            :    1.0
 , 'PostscaleNB'           :    1.0
 , 'NetCut'                :    0.9
 , 'BLifetime'             :    0.3
 }
confBhhUnbiased  = StrippingB2hhLTUnbiased.StrippingB2hhLTUnbiasedConf("BhhLTUnbiased", paramBhhUnbiased)
stream.appendLines(confBhhUnbiased.lines())

##
## B->hhh
##
from StrippingSelections.StrippingBu2hhh import Bu2hhhBuilder
from StrippingSelections.StrippingBu2hhh import config_params as config_params_Bu2hhh
bu2hhhbuilder = Bu2hhhBuilder('Bu2hhh',config_params_Bu2hhh)

stream.appendLines( bu2hhhbuilder.lines() )

##
## B-> hhpi0
##
from StrippingSelections.StrippingB2HHPi0 import StrippingB2HHPi0Conf
paramsB2HHPi0 = lineBuilderConfiguration('stripping13', 'B2HHPi0' )['CONFIG']
confB2HHPi0 = StrippingB2HHPi0Conf("confB2HHPi0",paramsB2HHPi0)
stream.appendLines( confB2HHPi0.lines() )

##
## B->Kshh
##
from StrippingSelections.StrippingB2KShh import B2KShhConf
from StrippingSelections.StrippingB2KShh import default_config as paramB2Kshh
confB2Kshh = B2KShhConf("confB2Kshh", paramB2Kshh)
stream.appendLines( confB2Kshh.lines())

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
#sc.OutputType = "ETC"                    # Can be either "ETC" or "DST"

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

# Remove the microbias and beam gas etc events before doing the tagging step
regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
from Configurables import LoKi__HDRFilter
filterHLT = LoKi__HDRFilter("FilterHLT",Code = regexp )

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())

from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

from Configurables import CondDB
CondDB(UseOracle = True)
DaVinci().DDDBtag  = "head-20110722"
DaVinci().CondDBtag = "head-20110722"

DaVinci().PrintFreq = 2000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 50000
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
DaVinci().DataType  = "2011"
DaVinci().InputType = "SDST"

importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco11a_Run97120_SDSTs.py")
