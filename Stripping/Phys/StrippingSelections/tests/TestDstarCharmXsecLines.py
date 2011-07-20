# $Id: $
# Test your line(s) of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")

# Import your stripping lines. Replace the lines below appropriately.
#from StrippingSelections.StrippingB2XMuMuSS import B2XMuMuSSConf as builder
#from StrippingSelections.StrippingB2XMuMuSS import config_params as config_params
#lb = builder('B2XMuMuSS',config_params)

#stream.appendLines( lb.lines() )

from StrippingSelections.StrippingD02HHForXSec import StrippingD02HHForXSecConf
from StrippingSelections.StrippingDstar2D0Pi_D02KPiForXSec import StrippingDstar2D0Pi_D02KPiForXSecConf
from StrippingSelections.StrippingD02K3PiForXSec import D02K3PiForXSecConf
from StrippingSelections.StrippingD02KPiGeoForXSec import StrippingD02KPiGeoForXSecConf
#from StrippingSelections.StrippingD02HHForXSec import config_params as config_params

config_paramsD02HH =  {
     'Daug_PT_MIN'               : 250.0
     , 'Daug_TRCHI2DOF_MAX'        :   9.0
     , 'Daug_MIPCHI2DV_MIN'        :   9.0
     , 'D0_BPVVDCHI2_MIN'          :  16.0
     , 'D0_BPVDIRA_MIN'            :   0.9999
     , 'D0_BPVIPCHI2_MAX'          : 100.0
     , 'D0_ADAMASS_WIN'            :  80.0
     , 'D0_ADMASS_WIN'             :  75.0
     , 'Dstar_AMDiff_MAX'          : 160.0
     , 'Dstar_MDiff_MAX'           : 155.0
     , 'Dstar_VCHI2VDOF_MAX'       : 100.0
     , 'HltFilter'          : "HLT_PASS_RE('Hlt1.*')"#Hlt1MB.*
     , 'PrescaleD02HH'             :   0.1
     , 'PrescaleDstar2D0Pi_D02HH'  :   1.0
     , 'PostscaleD02HH'            :   1.0
     , 'PostscaleDstar2D0Pi_D02HH' :   1.0
     }
lb_D02HH = StrippingD02HHForXSecConf('D02HHForXsec',config_paramsD02HH)

stream.appendLines( lb_D02HH.lines() )
config_paramsDstarD02HH =  {
    'Daug_TRCHI2DOF_MAX'        :  10.0
    , 'Daug_BPVIPCHI2_MIN'        :   6.0
    , 'K_PIDK_MIN'                :   0.0
    , 'Pi_PIDK_MAX'               :   0.0
    , 'D0_ADAMASS_WIN'            :  80.0
    , 'D0_ADMASS_WIN'             :  75.0
    , 'D0_VCHI2VDOF_MAX'          :  25.0
    , 'D0_BPVLTIME_MIN'           :0.0002 
    , 'D0_BPVLTFITCHI2_MAX'       : 100.0
    , 'Dstar_AMDiff_MAX'          : 160.0
    , 'Dstar_VCHI2VDOF_MAX'       : 100.0
    , 'Dstar_MDiff_MAX'           : 155.0
    , 'HltFilter'          : "HLT_PASS_RE('Hlt1.*')"#Hlt1MB.*
    , 'PrescaleD02HH'             :   0.1
    , 'PrescaleDstar2D0Pi_D02HH'  :   1.0
    , 'PostscaleD02HH'            :   1.0
    , 'PostscaleDstar2D0Pi_D02HH' :   1.0
    }


lb_DstarD02HH =StrippingDstar2D0Pi_D02KPiForXSecConf('DstarD02KPiForXsec', config_paramsDstarD02HH)
stream.appendLines( lb_DstarD02HH.lines() )

config_paramsK3Pi=  {
'D0Cuts' : { 'CombMassWin' : 80
                 ,'MassWin' : 75
                 ,'MaxDOCA' : 0.25
                 ,'DauPtAllTracks' : 250
                 ,'DauPtThreeTracks' : 300
                 ,'DauPtTwoTracks' : 350
                 ,'DauPtOneTrack' : 400
                 ,'DauP' : 3000
                 ,'DauIPchi2AllTracks' : 1.5
                 ,'DauIPchi2ThreeTracks' : 3.5
                 ,'DauIPchi2TwoTracks' : 7
                 ,'DauIPchi2OneTrack' : 7
                 ,'FDchi2' : 16
                 ,'DIRA' : 0.99975
                 ,'VtxChi2DOF' : 20
                 ,'DauTrackChi2DOF' : 4
                 ,'ApplyKaonPIDK' : True
                 ,'KaonPIDK' : 5
                 ,'ApplyPionPIDK' : False
                 ,'PionPIDK' : 0
                 }
    ,'DstarCuts' : { 'CombDeltaMLower' : -8.25
                     ,'CombDeltaMUpper' : 16.5
                     ,'DeltaMLower' : -7.5
                     ,'DeltaMUpper' : 15
                     ,'DOCA' : 0.45
                     ,'SlowPiTrackChi2DOF' : 4
                     ,'VtxChi2DOF' : 100 }
    ,'HltFilter' : {'Tagged' : "HLT_PASS_RE('Hlt1.*')"#Hlt1MB.*
                    ,'Untagged' : "HLT_PASS_RE('Hlt1.*')"#Hlt1MB.*
                    }
    ,'CheckPV' : {'Tagged' : True
                  ,'Untagged' : True }
    ,'Prescale' : {'Tagged' : 0.1
                   ,'Untagged' : 0.1 }
    ,'Postscale' : {'Tagged' : 1
                    ,'Untagged' : 1 }
    ,'ApplyGECs' : {'Tagged' : False
                    ,'Untagged' : False }
    ,'MaxLongTracks' : {'Tagged' : None
                        ,'Untagged' : None},
    'MaxSpdDigits' : {'Tagged' : None
                      ,'Untagged' : None },
    'MaxITClusters' : {'Tagged' : None
                       ,'Untagged' : None },
    'MaxVeloTracks' : {'Tagged' : None
                       , 'Untagged' : None },
    }

lb_K3Pi = D02K3PiForXSecConf('k3piForXsec', config_paramsK3Pi)
stream.appendLines( lb_K3Pi.lines() )

config_paramsDstarGeo=  {
'D0_ADAMASS_WIN'      : 250.0
    , 'D0_AMAXDOCA_MAX'     :  10.0
    , 'D0_ADMASS_WIN'       : 125.0
    , 'D0_NU_2PT_MIN'       :  14.0
    , 'D0_BPVVDSIGN_MIN'    :   1.0
    , 'SPi_TRCHI2DOF_MAX'   :   9.0
    , 'Dstar_AMDiff_MAX'    : 160.0
    , 'Dstar_VCHI2VDOF_MAX' : 100.0
    , 'Dstar_MDiff_MAX'     : 155.0
    , 'HltFilter'          : "HLT_PASS_RE('Hlt1.*')"
    , 'PrescaleD02HH'             :   0.1
    , 'PrescaleDstar2D0Pi_D02HH'  :   0.1
    , 'PostscaleD02HH'            :   1.0
    , 'PostscaleDstar2D0Pi_D02HH' :   1.0
}
lb_DstarGeo =StrippingD02KPiGeoForXSecConf('DstarGeoForXsec', config_paramsDstarGeo)
stream.appendLines( lb_DstarGeo.lines() )

from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

# Configure the stripping using the same options as in Reco06-Stripping10
sc = StrippingConf( Streams = [ stream ],
                    MaxCandidates = 10000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents )

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

# Remove the microbias, nobias and beam gas etc events before doing the tagging step. Remove these lines if inapropriate for your analysis
regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!BeamGas)(?!Incident).*Decision')"#add microbias back in (?!MB)(?!NoBias)(?!NZS)(?!Velo)
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
CondDB().IgnoreHeartBeat = True

DaVinci().PrintFreq = 2000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 5000
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
DaVinci().DataType  = "2011"
DaVinci().InputType = "SDST"


importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco09-Stripping13_SDSTs.py")
