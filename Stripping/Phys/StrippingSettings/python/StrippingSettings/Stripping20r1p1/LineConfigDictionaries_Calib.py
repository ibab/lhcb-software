# Config dictionaries for calibration lines (2011 restripping)
"""
Line config dictionaries for the PID and Calibration streams
(all WGs, 2011 incremental restripping).

Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingNoPIDDstarWithD02RSKPi linebuilder should be added as:

      StrippingNoPIDDstarWithD02RSKPi = { ... }
"""

from GaudiKernel.SystemOfUnits import MeV, GeV, cm, mm

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
