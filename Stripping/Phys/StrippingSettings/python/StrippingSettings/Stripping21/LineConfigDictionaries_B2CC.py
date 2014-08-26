################################################################################
##                          S T R I P P I N G  2 1                            ##
##                                                                            ##
##  Configuration for B2CC WG                                                 ##
##  Contact person: Carlos Vazquez Sierra (carlos.vazquez@cern.ch)            ##
################################################################################

from GaudiKernel.SystemOfUnits import *

######################################################################
## StrippingBetaSBs2JpsieePhiDetachedLine (MicroDST)
## StrippingBetaSBs2JpsieePhiFromTracksLine (MicroDST)
## StrippingBetaSBs2JpsieePhiLine (MicroDST)
## -------------------------------------------------------------------
## Lines defined in StrippingBs2JpsieePhi.py
## Authors: Artur Ukleja, Jibo He, Konrad Klimaszewski
## Last changes made by Konrad Klimaszewski
## -------------------------------------------------------------------
## Stripping reports (Revision 176907):
##
## StrippingReport (2012)                                        INFO Event 100000, Good event 100000
## |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
## |_StrippingGlobal_                                            |  0.0530|        53|       |  37.926|
## |_StrippingSequenceStreamTest_Bs2JpsieePhi_                   |  0.0530|        53|       |  37.919|
## |!StrippingBetaSBs2JpsieePhiDetachedLine                      |  0.0430|        43|  1.721|  23.977|
## |!StrippingBetaSBs2JpsieePhiFromTracksLine                    |  0.0290|        29|  1.897|   4.405|
## |!StrippingBetaSBs2JpsieePhiLine                              |  0.0180|        18|  2.000|   0.481|
##
## StrippingReport (2011, magnet down)                           INFO Event 100000, Good event 100000
## |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
## |_StrippingGlobal_                                            |  0.0430|        43|       |  30.893|
## |_StrippingSequenceStreamTest_Bs2JpsieePhi_                   |  0.0430|        43|       |  30.886|
## |!StrippingBetaSBs2JpsieePhiDetachedLine                      |  0.0340|        34|  1.588|  17.630| (x 0.1 if DiElectron is open before)
## |!StrippingBetaSBs2JpsieePhiFromTracksLine                    |  0.0290|        29|  1.483|   3.653|
## |!StrippingBetaSBs2JpsieePhiLine                              |  0.0090|         9|  1.778|   0.711|
##
## StrippingReport (2011, magnet up)                             INFO Event 100000, Good event 100000
## |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
## |_StrippingGlobal_                                            |  0.0460|        46|       |  26.687|
## |_StrippingSequenceStreamTest_Bs2JpsieePhi_                   |  0.0460|        46|       |  26.680|
## |!StrippingBetaSBs2JpsieePhiDetachedLine                      |  0.0340|        34|  1.294|  15.391|
## |!StrippingBetaSBs2JpsieePhiFromTracksLine                    |  0.0310|        31|  1.484|   3.041|
## |!StrippingBetaSBs2JpsieePhiLine                              |  0.0140|        14|  1.214|   0.206|
##
######################################################################

BetaSBs2JpsieePhi = {
    'BUILDERTYPE' : 'Bs2JpsieePhiConf',
    'CONFIG'      : { 'ElectronPTLoose'            :   500.    # MeV
                    , 'ElectronPIDLoose'           :     0.    # adimensional
                    , 'ElectronTrackCHI2pDOFLoose' :     5.    # adimensional
                    , 'JpsiVertexCHI2pDOFLoose'    :    15.    # adimensional
                    , 'JpsiMassMinLoose'           :  2500.    # MeV
                    , 'JpsiMassMaxLoose'           :  3300.    # MeV
                    , 'KaonTrackCHI2pDOFLoose'     :     5.    # adimensional
                    , 'PhiPTLoose'                 :  1000.    # MeV
                    , 'PhiVertexCHI2pDOFLoose'     :    15.    # adimensional
                    , 'PhiMassMinLoose'            :   990.    # MeV
                    , 'PhiMassMaxLoose'            :  1050.    # MeV
                    , 'BsVertexCHI2pDOFLoose'      :    10.    # adimensional
                    , 'BsMassMinLoose'             :  4600.    # MeV
                    , 'BsMassMaxLoose'             :  6000.    # MeV
                    , 'LifetimeCut'                : " & (BPVLTIME()>0.3*ps)"
                    , 'PrescaleLoose'              :     1.    # adimensional
                    , 'ElectronPT'                 :   500.    # MeV
                    , 'ElectronPID'                :     0.    # adimensional
                    , 'ElectronTrackCHI2pDOF'      :     5.    # adimensional
                    , 'JpsiVertexCHI2pDOF'         :    15.    # adimensional
                    , 'JpsiMassMin'                :  2500.    # MeV
                    , 'JpsiMassMax'                :  3300.    # MeV
                    , 'KaonTrackCHI2pDOF'          :     5.    # adimensional
                    , 'KaonPID'                    :    -2.    # adimensional
                    , 'PhiPT'                      :  1000.    # MeV
                    , 'PhiVertexCHI2pDOF'          :    15.    # adimensional
                    , 'PhiMassMin'                 :   990.    # MeV
                    , 'PhiMassMax'                 :  1050.    # MeV
                    , 'BsVertexCHI2pDOF'           :    10.    # adimensional
                    , 'BsMassMin'                  :  4600.    # MeV
                    , 'BsMassMax'                  :  6000.    # MeV
                    , 'BsDIRA'                     :     0.99  # adimensional
                    , 'Prescale'                   :     0.1   # adimensional
                    },
    'STREAMS'     : { 'Radiative': ['StrippingBetaSBs2JpsieePhiDetachedLine',
                                    'StrippingBetaSBs2JpsieePhiFromTracksLine',
                                    'StrippingBetaSBs2JpsieePhiLine'] },
    'WGs'         : [ 'B2CC' ]
    }

######################################################################
## StrippingBetaSPsi2S_InclPsi2SToJpsiPiPiLine (MicroDST)
## StrippingBetaSPsi2S_Bs2Psi2SPhiJpsiPiPiLine (MicroDST)
## StrippingBetaSPsi2S_Bu2Psi2SKJpsiPiPiLine (MicroDST)
## StrippingBetaSPsi2S_Bd2Psi2SKstarJpsiPiPiLine (MicroDST)
## StrippingBetaSPsi2S_Bd2Psi2SKsJpsiPiPiLine (MicroDST)
## -------------------------------------------------------------------
## Lines defined in StrippingB2Psi2SX.py
## Authors: Fred Blanc, Neus Lopez March (left LHCb!)
## -------------------------------------------------------------------
## Stripping reports (Revision 176850):
##
## StrippingReport (2012)                                        INFO Event 100000, Good event 100000
## |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
## |_StrippingGlobal_                                            |  0.0300|        30|       |  18.212|
## |_StrippingSequenceStreamTest_B2Psi2SX_                       |  0.0300|        30|       |  18.206|
## |!StrippingBetaSPsi2S_InclPsi2SToJpsiPiPiLine                 |  0.0060|         6|  1.000|   0.874|
## |!StrippingBetaSPsi2S_Bs2Psi2SPhiJpsiPiPiLine                 |  0.0010|         1|  1.000|   7.671|
## |!StrippingBetaSPsi2S_Bu2Psi2SKJpsiPiPiLine                   |  0.0210|        21|  2.000|   0.475|
## |!StrippingBetaSPsi2S_Bd2Psi2SKstarJpsiPiPiLine               |  0.0080|         8|  1.625|   0.119|
## |!StrippingBetaSPsi2S_Bd2Psi2SKsJpsiPiPiLine                  |  0.0030|         3|  1.000|   0.068|
##
## StrippingReport (2011, magnet down)                           INFO Event 100000, Good event 100000
## |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
## |_StrippingGlobal_                                            |  0.0390|        39|       |  17.262|
## |_StrippingSequenceStreamTest_B2Psi2SX_                       |  0.0390|        39|       |  17.254|
## |!StrippingBetaSPsi2S_InclPsi2SToJpsiPiPiLine                 |  0.0130|        13|  1.000|   0.819|
## |!StrippingBetaSPsi2S_Bs2Psi2SPhiJpsiPiPiLine                 |  0.0020|         2|  1.500|   6.946|
## |!StrippingBetaSPsi2S_Bu2Psi2SKJpsiPiPiLine                   |  0.0180|        18|  2.833|   0.465|
## |!StrippingBetaSPsi2S_Bd2Psi2SKstarJpsiPiPiLine               |  0.0080|         8|  2.125|   0.138|
## |!StrippingBetaSPsi2S_Bd2Psi2SKsJpsiPiPiLine                  |  0.0060|         6|  1.000|   0.069|
##
## StrippingReport (2011, magnet up)                             INFO Event 100000, Good event 100000
## |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
## |_StrippingGlobal_                                            |  0.0300|        30|       |  15.881|
## |_StrippingSequenceStreamTest_B2Psi2SX_                       |  0.0300|        30|       |  15.874|
## |!StrippingBetaSPsi2S_InclPsi2SToJpsiPiPiLine                 |  0.0170|        17|  1.412|   1.181|
## |!StrippingBetaSPsi2S_Bs2Psi2SPhiJpsiPiPiLine                 |  0.0010|         1|  1.000|   6.437|
## |!StrippingBetaSPsi2S_Bu2Psi2SKJpsiPiPiLine                   |  0.0090|         9|  1.444|   0.065|
## |!StrippingBetaSPsi2S_Bd2Psi2SKstarJpsiPiPiLine               |  0.0040|         4|  2.000|   0.117|
## |!StrippingBetaSPsi2S_Bd2Psi2SKsJpsiPiPiLine                  |  0.0020|         2|  1.000|   0.068|
##
######################################################################

BetaSPsi2S = {
    'BUILDERTYPE' : 'B2Psi2SXConf',
    'CONFIG'      : { 'PionsTRCHI2DOF'       : 5
                    , 'Psi2SPiMIPCHI2DV'     : 9         # before 4
                    , 'Psi2SAM23down'        : 400
                    , 'Psi2SAM23up'          : 800
                    , 'Psi2SAPT'             : 500
                    , 'Psi2SADAMASS'         : 30
                    , 'Psi2SVFASPF'          : 16
                    , 'PIDpi'                : 10        # new 10
                    , 'ChKTRCHI2DOF'         : 5
                    , 'ChKPID'               : 0         # before -2
                    , 'PhiWin'               : 20
                    , 'PhiPT'                : 1000      # before 500, now 1000
                    , 'PhiVFASPF'            : 25        # new change it was 16 before
                    , 'PhiMAXTRCHI2DOF'      : 5
                    , 'PhiMINTRCHI2DOF'      : -2
                    , 'KstMassDown'          : 826
                    , 'KstMassUp'            : 966
                    , 'KstAPT'               : 1500      # before 500
                    , 'KstVFASPF'            : 16
                    , 'KstTRCHI2DOF'         : 4
                    , 'KstPIDK'              : 0         # before -2
                    , 'KstPIDpi'             : 10        # new 10
                    , 'KsVFASPF'             : 20
                    , 'KsBPVDLS'             : 5
                    , 'incl_LinePrescale'    : 0.1       # now 0.1, before 0.5 & 0.2
                    , 'incl_LinePostscale'   : 1
                    , 'BPVLTIME_detached'    : 0.15
                    , 'BsMassCutDownPre'     : 5000      # before 5000
                    , 'BsMassCutUpPre'       : 5650      # before 5650
                    , 'BsMassCutDownPost'    : 5150      # before 5100
                    , 'BsMassCutUpPost'      : 5550      # before 5550
                    , 'BsVCHI2PDOF'          : 20        # new change it was 10 before
                    , 'sig_LinePrescale'     : 1
                    , 'sig_LinePostscale'    : 1
                    , 'ChKPT'                : 500
                    , 'K_LinePrescale'       : 1         # 0.5
                    , 'K_LinePostscale'      : 1
                    , 'Kstar_LinePrescale'   : 1         # before 0.5
                    , 'Kstar_LinePostscale'  : 1
                    , 'MINTREEPT2'           : 1000
                    , 'BKsVCHI2PDOF'         : 10
                    , 'Ks_LinePrescale'      : 1
                    , 'Ks_LinePostscale'     : 1
                    },
    'STREAMS'     : { 'Leptonic': ['StrippingBetaSPsi2S_InclPsi2SToJpsiPiPiLine',
                                   'StrippingBetaSPsi2S_Bs2Psi2SPhiJpsiPiPiLine',
                                   'StrippingBetaSPsi2S_Bu2Psi2SKJpsiPiPiLine',
                                   'StrippingBetaSPsi2S_Bd2Psi2SKstarJpsiPiPiLine',
                                   'StrippingBetaSPsi2S_Bd2Psi2SKsJpsiPiPiLine'] },
    'WGs'         : [ 'B2CC' ]
    }

######################################################################
## StrippingBetaSPsi2SMuMu_InclPsi2SToMuMuLine (MicroDST)
## StrippingBetaSPsi2SMuMu_InclPsi2SToMuMuDetachedLine (MicroDST)
## StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuPrescaledLine (MicroDST)
## StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuDetachedLine (MicroDST)
## StrippingBetaSPsi2SMuMu_Bu2Psi2SKMuMuDetachedLine (MicroDST)
## StrippingBetaSPsi2SMuMu_Bu2Psi2SKMuMuPrescaledLine (MicroDST)
## StrippingBetaSPsi2SMuMu_Bd2Psi2SKstarMuMuDetachedLine (MicroDST)
## StrippingBetaSPsi2SMuMu_Bd2Psi2SKstarMuMuPrescaledLine (MicroDST)
## StrippingBetaSPsi2SMuMu_Bd2Psi2SKsMuMuDetachedLine (MicroDST)
## StrippingBetaSPsi2SMuMu_Bd2Psi2SKsMuMuPrescaledLine (MicroDST)
## -------------------------------------------------------------------
## Lines defined in StrippingB2Psi2SXMuMu.py
## Authors: Neus Lopez March (left LHCb!)
## -------------------------------------------------------------------
## Stripping reports (Revision 176850):
##
## StrippingReport (2012)                                        INFO Event 100000, Good event 100000
## |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
## |_StrippingGlobal_                                            |  0.1100|       110|       |  18.789|
## |_StrippingSequenceStreamTest_B2Psi2SMuMu_                    |  0.1100|       110|       |  18.782|
## |!StrippingBetaSPsi2SMuMu_InclPsi2SToMuMuLine                 |  0.0020|         2|  1.000|   0.929|
## |!StrippingBetaSPsi2SMuMu_InclPsi2SToMuMuDetachedLine         |  0.0610|        61|  1.000|   7.690|
## |!StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuPrescaledLine        |  0.0020|         2|  1.500|   0.076|
## |!StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuDetachedLine         |  0.0020|         2|  1.000|   0.072|
## |!StrippingBetaSPsi2SMuMu_Bu2Psi2SKMuMuDetachedLine           |  0.0350|        35|  1.286|   0.477|
## |!StrippingBetaSPsi2SMuMu_Bu2Psi2SKMuMuPrescaledLine          |  0.0170|        17|  2.118|   0.054|
## |!StrippingBetaSPsi2SMuMu_Bd2Psi2SKstarMuMuDetachedLine       |  0.0100|        10|  1.900|   0.187|
## |!StrippingBetaSPsi2SMuMu_Bd2Psi2SKstarMuMuPrescaledLine      |  0.0090|         9|  1.889|   0.054|
## |!StrippingBetaSPsi2SMuMu_Bd2Psi2SKsMuMuDetachedLine          |  0.0070|         7|  1.000|   0.077|
## |!StrippingBetaSPsi2SMuMu_Bd2Psi2SKsMuMuPrescaledLine         |  0.0150|        15|  1.133|   0.058|
##
## StrippingReport (2011, magnet down)                           INFO Event 100000, Good event 100000
## |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
## |_StrippingGlobal_                                            |  0.1630|       163|       |  17.794|
## |_StrippingSequenceStreamTest_B2Psi2SMuMu_                    |  0.1630|       163|       |  17.787|
## |!StrippingBetaSPsi2SMuMu_InclPsi2SToMuMuLine                 |  0.0470|        47|  1.000|   1.204|
## |!StrippingBetaSPsi2SMuMu_InclPsi2SToMuMuDetachedLine         |  0.0380|        38|  1.000|   6.867|
## |!StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuPrescaledLine        |  0.0270|        27|  1.222|   0.085|
## |!StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuDetachedLine         |  0.0020|         2|  1.000|   0.079|
## |!StrippingBetaSPsi2SMuMu_Bu2Psi2SKMuMuDetachedLine           |  0.0220|        22|  1.182|   0.062|
## |!StrippingBetaSPsi2SMuMu_Bu2Psi2SKMuMuPrescaledLine          |  0.0300|        30|  1.467|   0.054|
## |!StrippingBetaSPsi2SMuMu_Bd2Psi2SKstarMuMuDetachedLine       |  0.0150|        15|  1.533|   0.250|
## |!StrippingBetaSPsi2SMuMu_Bd2Psi2SKstarMuMuPrescaledLine      |  0.0160|        16|  2.562|   0.054|
## |!StrippingBetaSPsi2SMuMu_Bd2Psi2SKsMuMuDetachedLine          |  0.0010|         1|  1.000|   0.087|
## |!StrippingBetaSPsi2SMuMu_Bd2Psi2SKsMuMuPrescaledLine         |  0.0120|        12|  1.000|   0.058|
##
## StrippingReport (2011, magnet up)                             INFO Event 100000, Good event 100000
## |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
## |_StrippingGlobal_                                            |  0.1730|       173|       |  16.014|
## |_StrippingSequenceStreamTest_B2Psi2SMuMu_                    |  0.1730|       173|       |  16.006|
## |!StrippingBetaSPsi2SMuMu_InclPsi2SToMuMuLine                 |  0.0570|        57|  1.000|   1.167|
## |!StrippingBetaSPsi2SMuMu_InclPsi2SToMuMuDetachedLine         |  0.0420|        42|  1.000|   6.175|
## |!StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuPrescaledLine        |  0.0120|        12|  1.250|   0.076|
## |!StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuDetachedLine         |  0.0020|         2|  1.000|   0.079|
## |!StrippingBetaSPsi2SMuMu_Bu2Psi2SKMuMuDetachedLine           |  0.0270|        27|  1.185|   0.061|
## |!StrippingBetaSPsi2SMuMu_Bu2Psi2SKMuMuPrescaledLine          |  0.0290|        29|  1.931|   0.053|
## |!StrippingBetaSPsi2SMuMu_Bd2Psi2SKstarMuMuDetachedLine       |  0.0220|        22|  1.818|   0.205|
## |!StrippingBetaSPsi2SMuMu_Bd2Psi2SKstarMuMuPrescaledLine      |  0.0080|         8|  2.000|   0.054|
## |!StrippingBetaSPsi2SMuMu_Bd2Psi2SKsMuMuDetachedLine          |  0.0020|         2|  1.000|   0.081|
## |!StrippingBetaSPsi2SMuMu_Bd2Psi2SKsMuMuPrescaledLine         |  0.0230|        23|  1.000|   0.058|
##
######################################################################

BetaSPsi2SMuMu = {
    'BUILDERTYPE' : 'Bs2Psi2SPhiMuMuConf',
    'CONFIG'      : { 'muPID'                       : 0.
                    , 'Psi2SMassWin'                : 60.
                    , 'Psi2SADOCACHI2CUT'           : 30.
                    , 'Psi2SVFASPF'                 : 16
                    , 'ChKTRCHI2DOF'                : 5
                    , 'ChKPID'                      : 0          # before -2
                    , 'PhiWin'                      : 20
                    , 'PhiPT'                       : 1000       # before 500
                    , 'PhiVFASPF'                   : 25         # new change, it was 16 before
                    , 'PhiMAXTRCHI2DOF'             : 5
                    , 'PhiMINTRCHI2DOF'             : -2
                    , 'KstMassDown'                 : 826
                    , 'KstMassUp'                   : 966
                    , 'KstAPT'                      : 1200       # before 500
                    , 'KstVFASPF'                   : 16
                    , 'KstTRCHI2DOF'                : 4
                    , 'KstPIDK'                     : 0          # before -2
                    , 'KstPIDpi'                    : 10         # new
                    , 'KsVFASPF'                    : 20
                    , 'KsBPVDLS'                    : 5
                    , 'incl_LinePrescale'           : 0.1        # before 0.1
                    , 'incl_LinePostscale'          : 1
                    , 'incl_LineTisTosHlt1'         : { "Hlt1DiMuonHighMassDecision%TOS"      : 0 }
                    , 'incl_LineTisTosHlt2'         : { "Hlt2DiMuonPsi2SDecision%TOS"         : 0 }
                    , 'BPVLTIME_detached'           : 0.2        # before 0.15
                    , 'incl_DetachedLinePrescale'   : 1          # before 0.5
                    , 'incl_DetachedLinePostscale'  : 1
                    , 'incl_DetachedLineTisTosHlt1' : { "Hlt1DiMuonHighMassDecision%TOS"      : 0
                                                      , "Hlt1TrackMuonDecision%TOS"           : 0
                                                      , "Hlt1TrackAllL0Decision%TOS"          : 0 
                                                      }
                    , 'incl_DetachedLineTisTosHlt2' : { "Hlt2DiMuonDetachedHeavyDecision%TOS" : 0
                                                      , "Hlt2DiMuonDetachedPsi2SDecision%TOS" : 0
                                                      }
                    , 'incl_DetachedCut'            : "BPVDLS > 3"
                    , 'BsMassCutDownPre'            : 5000       # before 5000
                    , 'BsMassCutUpPre'              : 5650       # before 5650
                    , 'BsMassCutDownPost'           : 5150       # before 5100
                    , 'BsMassCutUpPost'             : 5550       # before 5550
                    , 'BsVCHI2PDOF'                 : 20         # new change, it was 10 before
                    , 'sig_PrescaledLinePrescale'   : 0.5        # before 1.0
                    , 'sig_PrescaledLinePostscale'  : 1
                    , 'sig_DetachedLinePrescale'    : 1
                    , 'sig_DetachedLinePostscale'   : 1
                    , 'BPVLTIME_unbiased'           : None
                    , 'MINTREEPT'                   : 1000       # before 1000
                    , 'MINTREEPT2'                  : 1500       # before 1000
                    , 'ChKPT'                       : 500
                    , 'K_PrescaledLinePrescale'     : 0.1        # before 0.3 & 0.2
                    , 'K_PrescaledLinePostscale'    : 1
                    , 'K_DetachedLinePrescale'      : 1          # before 0.1
                    , 'K_DetachedLinePostscale'     : 1
                    , 'KstarPT'                     : 2
                    , 'Kstar_PrescaledLinePrescale' : 0.07       # before 0.1
                    , 'Kstar_PrescaledLinePostscale': 1
                    , 'Kstar_DetachedLinePrescale'  : 1          # before 1
                    , 'Kstar_DetachedLinePostscale' : 1
                    , 'BKsVCHI2PDOF'                : 10
                    , 'Ks_PrescaledLinePrescale'    : 1
                    , 'Ks_PrescaledLinePostscale'   : 1
                    , 'Ks_DetachedLinePrescale'     : 1
                    , 'Ks_DetachedLinePostscale'    : 1
                    },
    'STREAMS'     : { 'Leptonic': ['StrippingBetaSPsi2SMuMu_InclPsi2SToMuMuLine',
                                   'StrippingBetaSPsi2SMuMu_InclPsi2SToMuMuDetachedLine',
                                   'StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuPrescaledLine',
                                   'StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuDetachedLine',
                                   'StrippingBetaSPsi2SMuMu_Bu2Psi2SKMuMuDetachedLine',
                                   'StrippingBetaSPsi2SMuMu_Bu2Psi2SKMuMuPrescaledLine',
                                   'StrippingBetaSPsi2SMuMu_Bd2Psi2SKstarMuMuDetachedLine',
                                   'StrippingBetaSPsi2SMuMu_Bd2Psi2SKstarMuMuPrescaledLine',
                                   'StrippingBetaSPsi2SMuMu_Bd2Psi2SKsMuMuDetachedLine',
                                   'StrippingBetaSPsi2SMuMu_Bd2Psi2SKsMuMuPrescaledLine'] },
    'WGs'         : [ 'B2CC' ]
    }

