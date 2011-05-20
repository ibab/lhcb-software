# $Id: $
# Test your line(s) of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")

config_default ={'PrescalepipiBox'     : 0.5
                 , 'PrescalemumuBox'     : 1.
                 , 'PrescaleKpiBox'    : 1.
                 , 'PrescaleemuBox'    : 1.
                 , 'PrescaleeKBox'    : 1.
                 , 'PrescaleepiBox'    : 1.
                 , 'PrescalepimuBox'    : 1.
                 , 'PrescaleKmuBox'    : 1.
                 , 'Prescalepipi_untagged_Box'     : 1.
                 , 'Prescalemumu_untagged_Box'     : 1.
                 , 'PrescaleKpi_untagged_Box'    :  1.
                 , 'Prescalepimu_untagged_Box'    : 1.
                 , 'PrescaleKmu_untagged_Box'    : 1.
                 , 'PrescaleKpi_untagged_BoxMB' : 1
                 , 'Prescalepipi_untagged_BoxMB':1
                 ,'DMassWin'           : 70.       # MeV
                 ,'DMassWinMuMu'       : 300       #MeV
                 ,'DMassWinEMu'        : 300
                 ,'doca'               : 0.1        # mm
                 ,'XminPT'             : 750.       # MeV
                 ,'XmaxPT'             : 1100.      # MeV
                 ,'XminP'              : 4000.      # MeV
                 ,'XTrackChi2'         : 5.        # adimensional
                 ,'XTrackChi2Pi'         : 7.        # adimensional
                 ,'XminIPChi2'         : 3        # adimensional
                 ,'XmaxIPChi2'         : 8        # adimensional
                 ,'DMinFlightChi2'    :  20.
                 ,'DDira'              : 0.9997     # adimensional
                 ,'D0MinPT'            : 1800.      # MeV
                 ,'DMaxIPChi2'        :15.
                 ,'DVChi2'            :10. 
                 ,'PiMinPT'            : 110.       # MeV
                 ,'PiMaxIPCHI2'        : 10.         # adimensional
                 ,'DstMassWin'         : 110.       # MeV
                 ,'DstD0DMWin'         : 10.        # MeV
                 ,'DstD0DMWinMuMu'      : 30.        # MeV  
                 ,'RequireHlt'         : 1          # 
                 ,'prefix'         : '' 
                 }


from StrippingSelections.StrippingDstarD02xx import StrippingDstarD02xxConf as builder
#from StrippingSelections.StrippingDstarD02xx import config_default as config_params


lb = builder('Test',config_default)

stream.appendLines( lb.lines() )


#from StrippingSelections.StrippingD2HHLTUnbiased import StrippingD2HHLTUnbiasedConf as builderhh
#from StrippingSelections.StrippingD2HHLTUnbiased import default_config as config_paramshh


#lbhh = builderhh('Pippo',config_paramshh)

#stream.appendLines( lbhh.lines() )








#from StrippingSelections.StrippingB2SameChargeMuon import StrippingB2SameChargeMuonConf as builder

#config_params =  {
#    'LinePrescale'        : 1.0
#    , 'LinePostscale'            : 1.0
#}
#lb = builder('B2SameChargeMuon',config_params)

#stream.appendLines( lb.lines() )

from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

# Configure the stripping using the same options as in Reco06-Stripping10
sc = StrippingConf( Streams = [ stream ],
                    MaxCandidates = 50000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents )

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

# Remove the microbias, nobias and beam gas etc events before doing the tagging step. Remove these lines if inapropriate for your analysis
#regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NoBias)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
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
DaVinci().EvtMax = 10000
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
DaVinci().DataType  = "2011"
DaVinci().InputType = "SDST"


importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco09-Stripping13_SDSTs.py")
