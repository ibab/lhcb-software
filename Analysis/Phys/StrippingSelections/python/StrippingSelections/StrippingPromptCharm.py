#!/usr/bin/env python
# =============================================================================
# $Id: StrippingPromptCharm.py,v 1.9 2010-09-05 09:28:05 ibelyaev Exp $
# =============================================================================
# $URL$
# =============================================================================
## @file
# 
#  The attempt for coherent stripping of all stable charm hadrons : 
#
#    - D*+       -> ( D0 -> K pi , K K , pi pi, pi K ) pi+
#    - D0        -> K pi , K K                 
#    - Ds+/D+    -> ( phi -> K K ) pi          
#    - D+        -> K pi pi                    
#    - Lambda_c+ -> p K pi                     
#
#  The main purpose is the measurement of Gamma(D0->K+K-)-Gamma(D~0->K+K-)
#  in D*+ -> D0 pi+ chain. 
#  
#  Other modes are used as coherent set of control chanels to calibrate
#  the propertime scale and possible biases/systematics.
#  Finally they could be prescaled (if needed)
#
#  As cross-checks one measures:
#
#     - Gamma ( D0       -> KK    ) - Gamma ( D0    -> Kpi   ) , both tagged and untagged 
#     - Gamma ( D0       -> Kpi   ) - Gamma ( D~0   -> Kpi   ) , both tagged and untagged 
#     - Gamma ( D0       -> pipi  ) - Gamma ( D0    -> Kpi   ) , tagged only 
#     - Gamma ( D0       -> pipi  ) - Gamma ( D0    -> KK    ) , tagged only 
#     - Gamma ( Ds+      -> KKpi  ) - Gamma ( D+    -> KKpi  )   
#     - Gamma ( Ds+      -> KKpi  ) - Gamma ( Ds-   -> KKpi  )   
#     - Gamma ( D+       -> Kpipi ) - Gamma ( D-    -> Kpipi )   
#     - Gamma ( /\cbar-  -> pKpi  ) - Gamma ( /\c-  -> pKpi  )   
#     - Gamma ( Ds+      -> KKpi  ) - Gamma ( /\c-  -> pKpi  )   
#     - Gamma ( D+       -> KKpi  ) - Gamma ( /\c-  -> pKpi  )   
#     - Gamma ( D+       -> Kpipi ) - Gamma ( /\c-  -> pKpi  )
#
#
#  The cuts more or less correspond to D*+ selection by Alexandr Kozlinzkiy.
#  In addition the PT-cut for the long-lived charmed particle has been applied.
#  Thanks to Marco Gersabeck & Harry Cliff for nice idea.
#
#  The performance with 100k events from Reco05-Stripping08_SDSTs.py:
#
# +-----------------------------------+----------+----------+-------+--------+
# | Decision name                     |     Rate | Accepted | Mult. | <T>,ms |
# +-----------------------------------+----------+----------+-------+--------+
# | StrippingSequenceStream_Vanya     | 0.006300 |     630  |       | 3.038  |
# | -- StrippingDstarForPromptCharm   | 0.001030 |     103  | 1.417 | 1.323  |
# | -- StrippingD02HHForPromptCharm   | 0.003160 |     316  | 1.016 | 0.032  |
# | -- StrippingDsForPromptCharm      | 0.000530 |      53  | 1.132 | 0.210  |
# | -- StrippingDForPromptCharm       | 0.001600 |     160  | 1.019 | 0.276  |
# | -- StrippingLambdaCForPromptCharm | 0.000860 |      86  | 1.105 | 1.137  |
# +-----------------------------------+----------+----------+-------+--------+
#                                                   Thanks to Anton Poluektov
#
# Usage:
#
# @code
#
#  stream = ...
#
#  from StrippingSelections.StirppingPromptCharm import Lines as PromptCharmLines
#  stream.appendLines ( PromptCharmLines )
#
# @endcode 
# 
# $Revision: 1.9 $
# Last modification $Date: 2010-09-05 09:28:05 $
#                by $Author: ibelyaev $
# =============================================================================
"""

The attempt for coherent stripping of all stable charm hadrons

    - D*+       -> ( D0 -> K pi , K K , pi pi, pi K ) pi+
    - D0        -> K pi , K K                       
    - Ds+/D+    -> ( phi -> K K ) pi                
    - D+        -> K pi pi                          
    - Lambda_c+ -> p K pi                           
    
The main purpose is the measurement of Gamma(D0->K+K-)-Gamma(D~0->K+K-)
in D*+ -> D0 pi+ chain

Other modes are used as coherent set of control chanels to calibrate
the propertime scale and possible biases/systematics.
Finally they could be prescaled (if needed)

  As cross-checks one measures:

     - Gamma ( D0       -> KK    ) - Gamma ( D0    -> Kpi   ) , both tagged and untagged 
     - Gamma ( D0       -> Kpi   ) - Gamma ( D~0   -> Kpi   ) , both tagged and untagged 
     - Gamma ( D0       -> pipi  ) - Gamma ( D0    -> Kpi   ) , tagged only 
     - Gamma ( D0       -> pipi  ) - Gamma ( D0    -> KK    ) , tagged only 
     - Gamma ( Ds+      -> KKpi  ) - Gamma ( D+    -> KKpi  )   
     - Gamma ( Ds+      -> KKpi  ) - Gamma ( Ds-   -> KKpi  )   
     - Gamma ( D+       -> Kpipi ) - Gamma ( D-    -> Kpipi )   
     - Gamma ( /\cbar-  -> pKpi  ) - Gamma ( /\c-  -> pKpi  )   
     - Gamma ( Ds+      -> KKpi  ) - Gamma ( /\c-  -> pKpi  )   
     - Gamma ( D+       -> KKpi  ) - Gamma ( /\c-  -> pKpi  )   
     - Gamma ( D+       -> Kpipi ) - Gamma ( /\c-  -> pKpi  )   

    
The cuts more or less correspond to D*+ selection by Alexandr Kozlinzkiy
In addition the PT-cut for the long-lived charmed particle has been applied
Thanks to Marco Gersabeck & Harry Cliff for nice idea.

The performance with 100k events from Reco05-Stripping08_SDSTs.py:

  +-----------------------------------+----------+----------+-------+--------+
  | Decision name                     |     Rate | Accepted | Mult. | <T>,ms |
  +-----------------------------------+----------+----------+-------+--------+
  | StrippingSequenceStream_Vanya     | 0.006300 |     630  |       | 3.038  |
  | -- StrippingDstarForPromptCharm   | 0.001030 |     103  | 1.417 | 1.323  |
  | -- StrippingD02HHForPromptCharm   | 0.003160 |     316  | 1.016 | 0.032  |
  | -- StrippingDsForPromptCharm      | 0.000530 |      53  | 1.132 | 0.210  |
  | -- StrippingDForPromptCharm       | 0.001600 |     160  | 1.019 | 0.276  |
  | -- StrippingLambdaCForPromptCharm | 0.000860 |      86  | 1.105 | 1.137  |
  +-----------------------------------+----------+----------+-------+--------+
                                                     Thanks to Anton Poluektov

  Usage:
 

    >>> stream = ...

    >>> from StrippingSelections.StirppingPromptCharm import Lines as PromptCharmLines
    
    >>> stream.appendLines ( PromptCharmLines )


$Revision: 1.9 $
Last modification $Date: 2010-09-05 09:28:05 $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__date__    = '2010-08-03'
__version__ = '$Revision: 1.9 $'
# =============================================================================
__all__ = (
    #
    "Selections"                       , ## the selections
    #
    'D02HHForPromptCharm_PreSelection' ,
    'D02HHForPromptCharm_Selection'    ,
    'DstarForPromptCharm_Selection'    ,
    'DsForPromptCharm_Selection'       ,
    'DForPromptCharm_Selection'        ,
    'LambdaCForPromptCharm_Selection'  ,
    #
    "Lines"                            , ## the stripping lines
    #
    'D02HHForPromptCharm_Line'         ,
    'DstarForPromptCharm_Line'         ,
    'DsForPromptCharm_Line'            , 
    'DForPromptCharm_Line'             , 
    'LambdaCForPromptCharm_Line'
    )
# =============================================================================

from PhysSelPython.Wrappers import AutomaticData    , Selection     , MergedSelection 
from Configurables          import CombineParticles , FilterDesktop

_Pions      = AutomaticData ( Location = "Phys/StdLoosePions"   )
_Kaons      = AutomaticData ( Location = "Phys/StdLooseKaons"   )
_Protons    = AutomaticData ( Location = "Phys/StdLooseProtons" )
_NoPIDPions = AutomaticData ( Location = "Phys/StdNoPIDsPions"  )

## the common cuts for all basic particles: 
_trackcuts     = " ( TRCHI2DOF < 10 ) & ( PT > 250 * MeV ) "
_basiccuts     = _trackcuts + " & ( MIPCHI2DV () > 9  ) "
_kaoncuts      = _basiccuts + " & ( 0 < PIDK  - PIDpi ) "
_pioncuts      = _basiccuts + " & ( 0 < PIDpi - PIDK  ) " 
_protoncuts    = _basiccuts + " & ( 0 < PIDp  - PIDpi ) & ( 0 < PIDp - PIDK ) "
# slow pion is the special case 
_slowpioncuts  = "TRCHI2DOF < 10 "

## switch on/off embedded monitoring 
_monitor        = False 
## use intermediate meson-selection
_use_mesons     = True 

# =============================================================================
## common selections of daughter particles:
_KaonFilter     = FilterDesktop (
    "KaonsForPromptCharm" ,
    Code        = " ( 'K+'  == ABSID ) & %s " % _kaoncuts
    )
_PionFilter     = FilterDesktop (
    "PionsForPromptCharm" ,
    Code        = " ( 'pi+' == ABSID ) & %s " % _pioncuts
    )
_ProtonFilter   = FilterDesktop (
    "ProtonsForPromptCharm" ,
    Code        = " ( 'p+'  == ABSID ) & %s " % _protoncuts
    )

KaonSelection = Selection (
    'SelKaonsForPromptCharm' ,
    Algorithm          = _KaonFilter ,
    RequiredSelections = [ _Kaons ]
    )
PionSelection = Selection (
    'SelPionsForPromptCharm' ,
    Algorithm          = _PionFilter ,
    RequiredSelections = [ _Pions ]
    )
MesonsSelection =  MergedSelection (
    ##
    'SelBasicMesonsForPromptCharm' , 
    RequiredSelections = [ PionSelection , KaonSelection ]
    )
ProtonSelection = Selection (
    'SelProtonsForPromptCharm' ,
    Algorithm          = _ProtonFilter ,
    RequiredSelections = [ _Protons ]
    )


_preambulo      = [
    ## the D0 decay channels
    "pipi   = DECTREE ('[D0]cc -> pi- pi+   ') " ,
    "kk     = DECTREE ('[D0]cc -> K-  K+    ') " ,
    "kpi    = DECTREE ('[D0    -> K-  pi+]CC') " ,
    ## number of kaons in final state (as CombinationCuts)
    "ak2      = 2 == ANUM( 'K+' == ABSID ) "        ,
    ## shortcut for chi2 of vertex fit 
    'chi2vx = VFASPF(VCHI2) '                   , 
    ## shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light" , 
    "ctau   = BPVLTIME ( 9 ) * c_light "  ## use the embedded cut for chi2(LifetimeFit)<9 
    ]

## prepare D0 for D0 and D*+ 
_D0PreCombine = CombineParticles (
    "PreCombineD02HHForPromptCharm" ,
    ## the decays to be reconstructed 
    DecayDescriptors = [
    " D0  -> pi+ pi-    " ,
    " D0  -> K+  K-     " ,
    "[D0  -> K-  pi+]cc " 
    ] ,
    ## 
    Preambulo = _preambulo , 
    ## daughter cuts 
    DaughtersCuts = {} if _use_mesons else {
    'K-'  :  _kaoncuts ,
    'pi+' :  _pioncuts 
    } ,
    ## combination cut : wide mass-cut & PT-cut 
    CombinationCut = """
    ( ADAMASS('D0') < 85 * MeV ) &
    ( APT > 1.4 * MeV          )
    """ ,
    ## mother cut
    MotherCut      = """
    ( chi2vx < 9 )              &
    ( PT     > 1.5 * GeV      ) & 
    ( ADMASS('D0') < 75 * MeV ) &
    ( abs ( LV01 ) < 0.9      ) &
    ( ctau > 100 * micrometer )
    """
    )

## make (pre)selection
D02HHForPromptCharm_PreSelection = Selection (
    ##
    'PreSelD02HHForPromptCharm' ,
    ##
    Algorithm          = _D0PreCombine   ,
    ##
    RequiredSelections = [ MesonsSelection ] if _use_mesons else [ _Kaons ,
                                                                   _Pions ]
    )

# =============================================================================
## D0 -> K- pi+/K-K+ selection 
# =============================================================================
_D0Filter = FilterDesktop (
    ##
    "PromptD02HH" ,
    ##
    Monitor      = _monitor  ,
    HistoProduce = _monitor  ,
    ##
    Preambulo = _preambulo + [
    "hKpi = Gaudi.Histo1DDef ( 'mass K pi' , 1800 , 1920 , 60 )" ,
    "hKK  = Gaudi.Histo1DDef ( 'mass K K'  , 1800 , 1920 , 60 )" 
    ] ,
    ##
    Code      = """
    ( PT             > 1.5 * GeV ) & 
    ( ADMASS( 'D0' ) < 50  * MeV ) &
    ( kpi | kk  )                  &
    ( ctau > 100 * micrometer )
    """ ,
    ##
    PostMonitor  = """
    process ( switch ( kpi , monitor ( M , hKpi , 'mass Kpi' ) , monitor ( M , hKK  , 'mass KK'  ) ) ) >> ~EMPTY 
    """
    )

## make selection 
D02HHForPromptCharm_Selection = Selection (
    'SelD02HHForPromptCharm'                ,
    Algorithm          = _D0Filter          ,
    RequiredSelections = [ D02HHForPromptCharm_PreSelection ]
    )

# =============================================================================
## D*+ -> (D0 -> Kpi, piK, KK, pipi) pi+ selection 
# =============================================================================
_DstarCombine = CombineParticles(
    ##
    "CombineDstarForPromptCharm" ,
    ##
    Monitor      = _monitor  ,
    HistoProduce = _monitor  ,
    ##
    DecayDescriptors = [
    " [D*(2010)+ -> D0 pi+]cc" ,
    " [D*(2010)- -> D0 pi-]cc" 
    ] ,
    ##
    Preambulo = _preambulo + [
    "hdm1 = Gaudi.Histo1DDef ( 'delta Mass' , 135 , 160  , 50 )" 
    ] , 
    ##
    DaughtersCuts = {
    'pi+' :  _slowpioncuts ,
    'D0'  : " switch ( pipi , in_range ( -50 * MeV , DMASS('D0') , 75 * MeV ) , ALL ) "
    } ,
    ##
    CombinationCut = """
    ( AM       < 2.5 * GeV ) & 
    ( AM - AM1 < 165 * MeV )
    """ ,
    ## 
    MotherCut      = """
    ( chi2vx < 64        ) &
    ( M - M1 < 155 * MeV )
    """ , 
    ## 
    MotherMonitor  = """
    process ( monitor ( M - M1 , hdm1 , 'Delta Mass' ) )  >> ~EMPTY 
    """
    )

## convert it to selection
DstarForPromptCharm_Selection = Selection (
    ##
    "SelDstarForPromptCharm" ,
    ## 
    Algorithm          = _DstarCombine ,
    ##
    RequiredSelections = [
    D02HHForPromptCharm_PreSelection ,
    _NoPIDPions
    ]
    )

# =============================================================================
## Ds+/D+ -> KKpi   selection 
# =============================================================================
_DsCombine = CombineParticles(
    ## 
    "CombineDsForPromptCharm" ,
    ##
    Monitor      = _monitor  ,
    HistoProduce = _monitor  ,
    ##
    DecayDescriptors = [
    " [D_s+ -> K-  K+  pi+ ]cc" , ## through phi !!!
    ] ,
    ##
    Preambulo      = _preambulo  + [
    "aphi  = AM12              < 1050 * MeV " , ## phi-mass window 
    "admD  = ADAMASS ('D+'  )  <   85 * MeV " , ## D+  mass window for combination cut 
    "admDs = ADAMASS ('D_s+')  <   85 * MeV " , ## Ds+ mass window for combiantion cut 
    "dmD   = ADMASS  ('D+'  )  <   75 * MeV " , ## D+  mass window 
    "dmDs  = ADMASS  ('D_s+')  <   75 * MeV " , ## Ds+ mass window
    ##
    "hKKpi = Gaudi.Histo1DDef ( ' mass K K pi' , 1800 , 2020 , 110 )" ,
    ] ,
    ##
    DaughtersCuts = {} if _use_mesons else {
    'K-'  :  _kaoncuts ,
    'pi+' :  _pioncuts 
    } ,
    ##                                 phi                         
    CombinationCut = """
    aphi                &
    ( APT > 900 * MeV ) & 
    ( admD | admDs  ) 
    """ ,
    ##
    MotherCut      = """
    ( chi2vx  < 25            ) &
    ( PT      > 1.0 * GeV     ) & 
    ( dmD | dmDs              ) &
    ( ctau > 100 * micrometer )
    """ , 
    ##
    MotherMonitor  = """
    process ( monitor ( M , hKKpi , 'mass KKpi' )  ) >> ~EMPTY 
    """
    )

## convert it to selection
DsForPromptCharm_Selection = Selection (
    "SelDsForPromptCharm" ,
    Algorithm          = _DsCombine ,
    RequiredSelections = [ MesonsSelection ] if _use_mesons else [ _Kaons ,
                                                                   _Pions ]
    )

# =============================================================================
## D+ -> Kpipi   selection 
# =============================================================================
_DCombine = CombineParticles(
    ## 
    "CombineDForPromptCharm" ,
    ##
    Monitor      = _monitor  ,
    HistoProduce = _monitor  ,
    ##
    DecayDescriptors = [
    " [D+ -> K-  pi+  pi+ ]cc" , 
    ] ,
    ##
    Preambulo      = _preambulo + [
    "hKpipi = Gaudi.Histo1DDef ( 'mass K pi pi' , 1800 , 1920 , 60 )" 
    ] , 
    ##
    DaughtersCuts = {} if _use_mesons else {
    'K-'  :  _kaoncuts ,
    'pi+' :  _pioncuts 
    } ,
    ##                                 
    CombinationCut = """
    ( ADAMASS('D+') < 65 * MeV ) &
    ( APT > 1 * GeV )
    """ , 
    MotherCut      = """
    ( chi2vx  < 25                   ) &
    ( PT      > 1.5 * GeV            ) & 
    ( ADMASS  ('D+'  )  <   50 * MeV ) & 
    ( ctau    > 100 * micrometer )
    """ , 
    MotherMonitor  = """
    process ( monitor ( M , hKpipi , 'mass K pi pi' ) )  >> ~EMPTY 
    """
    )

## convert it to selection
DForPromptCharm_Selection = Selection (
    "SelDForPromptCharm" ,
    Algorithm          = _DCombine ,
    RequiredSelections = [ MesonsSelection ] if _use_mesons else [ _Kaons ,
                                                                   _Pions ]
    )

# =============================================================================
## Lambda_C -> ( pKpi )  selection 
# =============================================================================
_LambdaC_Combine = CombineParticles(
    ## 
    "CombineLambdaCForPromptCharm" ,
    ##
    Monitor      = _monitor  ,
    HistoProduce = _monitor  ,
    ##
    DecayDescriptors = [
    " [ Lambda_c+ -> p+  K-  pi+ ]cc" ,
    ] ,
    ##
    Preambulo = _preambulo + [ 
    "hpKpi = Gaudi.Histo1DDef ( 'mass p K pi' , 2230 , 2350 , 60 )" 
    ] , 
    ##
    DaughtersCuts = {} if _use_mesons else {    
    'p+'  :  _protoncuts ,
    'K-'  :  _kaoncuts   ,
    'pi+' :  _pioncuts 
    } ,
    ##
    CombinationCut = """
    ( ADAMASS('Lambda_c+') < 65 * MeV ) &
    ( APT > 1.30 * MeV ) 
    """ ,
    ##
    MotherCut      = """
    ( chi2vx  < 25 )                   &
    ( PT      > 1.5 * GeV            ) & 
    ( ADMASS('Lambda_c+') < 50 * MeV ) &
    ( ctau    > 100 * micrometer )
    """ ,
    ##
    MotherMonitor  = """
    process ( monitor ( M , hpKpi , 'mass p K pi ' ) ) >> ~EMPTY 
    """ 
    )

## convert it to selection
LambdaCForPromptCharm_Selection = Selection (
    "SelLambdaCForPromptCharm" ,
    Algorithm          = _LambdaC_Combine ,
    RequiredSelections = [ MesonsSelection ,
                           ProtonSelection ] if _use_mesons else [ _Kaons   ,
                                                                   _Pions   ,
                                                                   _Protons ]
    )


Selections = [
    D02HHForPromptCharm_PreSelection ,
    D02HHForPromptCharm_Selection    ,
    DstarForPromptCharm_Selection    ,
    DsForPromptCharm_Selection       ,
    DForPromptCharm_Selection        ,
    LambdaCForPromptCharm_Selection     
    ]

if _use_mesons :
    Selections += [
        PionSelection   ,
        KaonSelection   ,
        MesonsSelection ,
        ProtonSelection
        ]
    
# =============================================================================
## build the stripping lines 
# =============================================================================

from StrippingConf.StrippingLine import StrippingLine

## require 1,2, or 3 primary vertex 
PrimaryVertices = (1,3) 

D02HHForPromptCharm_Line   = StrippingLine (
    "D02HHForPromptCharm"      ,
    prescale = 1.00            ,                ## ATTENTION! Prescale here !!
    checkPV  = PrimaryVertices ,
    algos    = [ D02HHForPromptCharm_Selection   ]
    )

DstarForPromptCharm_Line   = StrippingLine (
    "DstarForPromptCharm"     ,
    checkPV = PrimaryVertices , 
    algos   = [ DstarForPromptCharm_Selection    ]
    )

DsForPromptCharm_Line       = StrippingLine (
    "DsForPromptCharm"        ,
    checkPV = PrimaryVertices , 
    algos   = [ DsForPromptCharm_Selection       ]
    )

DForPromptCharm_Line       = StrippingLine (
    "DForPromptCharm"         ,
    prescale = 1.00            ,                ## ATTENTION! Prescale here !!
    checkPV = PrimaryVertices , 
    algos   = [ DForPromptCharm_Selection        ]
    )

LambdaCForPromptCharm_Line = StrippingLine (
    "LambdaCForPromptCharm" ,
    prescale = 1.00            ,                ## ATTENTION! Prescale here !!
    checkPV = PrimaryVertices , 
    algos   = [ LambdaCForPromptCharm_Selection  ]
    )

Lines  = [
    DstarForPromptCharm_Line   ,
    D02HHForPromptCharm_Line   ,
    DsForPromptCharm_Line      , 
    DForPromptCharm_Line       , 
    LambdaCForPromptCharm_Line 
    ]

# =============================================================================
if '__main__' == __name__ :

    print 80*'*'
    print __doc__
    print ' Author :  %s' % __author__
    print ' Date   :  %s' % __date__
    print ' The output locations : '
    for l in Lines :
        print ' \t ', l.outputLocation  () 
    print 80*'*'
        
# =============================================================================
# The END 
# =============================================================================
