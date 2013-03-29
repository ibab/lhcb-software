# Config dictionaries for calibration lines (2012 restripping)
"""
Line config dictionaries for the PID and Calibration streams
(all WGs, 2012 incremental restripping).

Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingNoPIDDstarWithD02RSKPi linebuilder should be added as:

      StrippingNoPIDDstarWithD02RSKPi = { ... }
"""

from GaudiKernel.SystemOfUnits import MeV, GeV, cm, mm

## D* tagged D0->Ksh+h-
## Used to measure the downstream tracking efficiency
## Bugfix from Stripping20, where the line took no data
##
## Responsible: Patrick Owen
## Test: Yes
DstarD02KShh_ForTrackingEff = {
    'BUILDERTYPE' : 'DstarD02KShh_ForTrackingEffBuilder',
    'CONFIG' : {  'LongTrackGEC'            :    150
                  , 'Trk_PT_MIN'            :  600.0
                  , 'Trk_P_MIN'             :10000.0
                  , 'Trk_GHOST_MAX'         :    0.4
                  , 'Pair_AMINDOCA_MAX'     :    0.1
                  , 'Pair_BPVVD_MIN'        :    0.0
                  , 'Pair_SumAPT_MIN'       : 2500.0
                  , 'Pair_BPVCORRM_MAX'     : 3000.0
                  , 'TrkPt_SlowPion'        :    0.0
                  , 'TrkP_SlowPion'         : 3000.0
                  , 'TrkChi2_SlowPion'      :    2.25
                  , 'TrkChi2_MAX_Child_MAX' :    2.25
                  , 'KAON_PIDK_MIN'         :    3.0
                  , 'PION_PIDK_MAX'         :    0.0
                  , 'IPCHI2_MAX_Child_MIN'  :   16.0
                  , 'IPCHI2_PiSlow_MAX'     :    9.0
                  , 'PairMaxDoca_Dstar'     :  100.0
                  , 'PT_Dstar_MIN'          : 2500.0
                  , 'DIRA_D0_MIN'           :    0.999
                  , 'FDCHI2_D0_MIN'         :   80.0
                  , 'VCHI2_D0_MAX'          :    4.0
                  , 'M_MAX'                 : 1800.0
                  , 'DeltaM_MIN'            :    0.0
                  , 'DeltaM_MAX'            :  250.0
                  , 'HLTFILTER'             : "(HLT_PASS_RE('Hlt2CharmHadD02HHXDst.*Decision'))"
                  , 'Hlt2TisTosSpec'        : { 'Hlt2CharmHadD02HHXDst.*Decision%TOS' : 0 }
                  , 'KKprescale'            :    0.5
                  , 'PiPiprescale'          :    0.1
                  , 'KPlusPiMinusprescale'  :    0.1
                  , 'KMinusPiPlusprescale'  :    0.1
                  , 'postscale'             :    1.0
    },
    'STREAMS' : [  'Calibration' ],
    'WGs' : [ 'ALL' ]
}

## Sigma_c ->Lambda_c(->pKpi)pi
## New stripping line for proton PID calibration
## To be used in conjuction with the Lambda PID
## lines to give additional kinematic coverage
##
## Responsible: Laurence Carson
## Test: Yes
SigmacForPID = {
  'BUILDERTYPE' : 'SigmacForPIDConf',
  'CONFIG' :  {
     'pT(Lc+)'         :  3.0 * GeV , ## pt-cut for  prompt   Lc+
     #
     # Selection of basic particles
     #
     'TrackCuts'       : """
( TRCHI2DOF < 4     ) &
( PT > 250 * MeV    ) &
( TRGHOSTPROB < 0.5 ) & 
in_range  ( 2 , ETA , 5 )
""" ,
     'BasicCuts'       : ' & ( 9 < MIPCHI2DV() ) ' ,
     'KaonCuts'        : ' & ( 2 < PIDK  - PIDpi ) & ( -5 < PIDK  - PIDp ) ',
     'PionCuts'        : ' & ( 2 < PIDpi - PIDK  ) & ( -5 < PIDpi - PIDp  ) ',
     'SlowPionCuts'    : ' TRCHI2DOF < 5 ',
     #
     # Global Event cuts
     #
     'CheckPV'         : True ,
     #
     # Technicalities:
     #
     'Preambulo'       : [
        # shortcut for chi2 of vertex fit
        'chi2vx = VFASPF(VCHI2) ',
        # shortcut for the c*tau
        "from GaudiKernel.PhysicalConstants import c_light" ,
        ## use the embedded cut for chi2(LifetimeFit)<9
        "ctau_9   = BPVLTIME (   9 ) * c_light ", 
     ],
     ## monitoring ?
     'Monitor'         : False ,
     ## prescale
     'SigmaCPrescale'  : 1.0 ,
  },
  'STREAMS' : [ 'PID' ],
  'WGs' : [ 'ALL' ]
}

## Muon tagged Lambda_c -> pKpi
## New stripping line for proton PID calibration
## To be used in conjuction with the Lambda PID
## lines to give additional kinematic coverage
##
## Responsible: Mika Vesterinen
## Test: Yes
CharmFromBSemiForProtonPID = {
  'BUILDERTYPE' : 'CharmFromBSemiForProtonPIDAllLinesConf',
  'CONFIG' : {
      "prescale"              : 1 # adimensional
      ,"GEC_nLongTrk"         : 250 # adimensional
      ,"MAXGHOSTPROB"         : 0.35 # adimensional
      ,"Mu_PT"                : 1000 # MeV
      ,"Pi_PIDKMax"           : -2 # adimensional
      ,"K_PIDKMin"            : 8 # adimensional
      ,"H_PT"                 : 300 # MeV
      ,"LambdaC_MASSWIN"      : 80 # MeV
      ,"LambdaC_PTSUM"        : 2000 # MeV
      ,"LambdaC_DOCACHI2_MAX" : 6 # adimensional
      ,"LambdaC_VCHI2NDF"     : 3 # adimensional
      ,"LambdaC_FDCHI2"       : 20 # adimensional
      ,"LambdaC_DIRA"         : 0.99 # adimensional
      ,"LambdaC_DZ"           : 0.5 # mm
      ,"LambdaB_MASS_MIN"     : 3000 # MeV
      ,"LambdaB_MASS_MAX"     : 5000 # MeV
      ,"LambdaB_DOCACHI2_MAX" : 6 # adimensional
      ,"LambdaB_DIRA"         : 0.999 # adimensional
      ,"LambdaB_VCHI2NDF"     : 3 # adimentional
  },
  'STREAMS' : [ 'PID' ],
  'WGs' : [ 'ALL' ]
}
