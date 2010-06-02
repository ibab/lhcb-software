########################################################################
#
# Options for V0 Reconstruction algorithm
#
# @author Mathias Oleg Knecht
# @author Raluca  Anca Muresan 
#
########################################################################
from os import environ
import GaudiKernel.SystemOfUnits as Units
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import CombineParticles
import CommonParticles.StdNoPIDsPions
import CommonParticles.StdNoPIDsProtons
import CommonParticles.StdNoPIDsDownPions
import CommonParticles.StdNoPIDsDownProtons
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from CommonParticles.Utils import *


_Preambulo  = [
    ## define DD-category of V0
    "DD   = CHILDCUT ( ISDOWN , 1 ) & CHILDCUT ( ISDOWN , 2 ) " ,
    ## define LL-category of V0 
    "LL   = CHILDCUT ( ISLONG , 1 ) & CHILDCUT ( ISLONG , 2 ) " ,
    ## define nu_0
    "NU_0 = CHILD ( MIPDV ( PRIMARY ) , 1 ) * CHILD ( MIPDV ( PRIMARY ) , 2 ) / MIPDV ( PRIMARY )  " ,
    ## redefine track chi2/nDoF 
    "TRCHI2DOF  = monitor ( TRCHI2DOF , 'chi2/nDoF' , LoKi.Monitoring.ContextSvc) " ,
    ]

LambdaAllCombineGeo = StrippingMember (
    CombineParticles
    , 'Combine'    
    , InputLocations  = [ "StdNoPIDsPions"   ,
                          "StdNoPIDsProtons" ]
    , DecayDescriptor = "[Lambda0 -> p+ pi-]cc"
    , Preambulo       = _Preambulo 
    , DaughtersCuts   = { '' : " TRCHI2DOF < 15 "}
    , CombinationCut  = "AM < 1.5 * GeV "
    , MotherCut = """
    ( ADMASS ( 'Lambda0' ) < 100*MeV ) & 
    ( VFASPF ( VCHI2     ) < 100     ) &
    ( NU_0 > 7.39 * mm ) &
    ( BPVDIRA > 0 ) &
    ( BPVLTIME() > 0 )
    """
    )

##########################################################
KsAllCombineGeo = StrippingMember(
    CombineParticles
    , 'Combine'    
    , InputLocations  = [ "StdNoPIDsPions" ]
    , DecayDescriptor = " KS0 -> pi+ pi- "
    , DaughtersCuts   = { '' : " TRCHI2DOF < 15 "}
    , Preambulo       = _Preambulo 
    , CombinationCut  = " AM < 1*GeV "                                    
    , MotherCut = """
    ( ADMASS ( 'KS0'  ) < 100 * MeV ) & 
    ( VFASPF (  VCHI2 ) < 100       ) & 
    ( NU_0 > 7.39 * mm ) &
    ( BPVDIRA > 0 ) &
    ( BPVLTIME() > 0 )
    """
    )

##########################################################################
line_lambda_all= StrippingLine(
    'LambdaAllGeoLine'
    , prescale = 0.5
    , HLT = "HLT_PASS_RE('Hlt1MBMicro.*Decision')"
    , algos = [  LambdaAllCombineGeo ])


line_KS_all= StrippingLine(
    'KSAllGeoLine'
    , prescale = 0.5
    , HLT = "HLT_PASS_RE('Hlt1MBMicro.*Decision')"
    , algos = [  KsAllCombineGeo ])

############################################################################
