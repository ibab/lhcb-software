#!/usr/bin/env python
# =============================================================================
# $Id: StrippingPromptCharm.py,v 1.6 2010-08-24 11:05:43 ibelyaev Exp $
# =============================================================================
# $URL$
# =============================================================================
## @file
# 
#  The attempt for coherent stripping of all stable charm hadrons : 
#
#    - D0        -> K pi , K K                 PRESCALED: 0.10 
#    - D*+       -> ( D0 -> K pi , K K , pi pi, pi K ) pi+
#    - Ds+/D+    -> ( phi -> K K ) pi          
#    - D+        -> K pi pi                    PRESCALED: 0.25 
#    - Lambda_c+ -> p K pi                     PRESCALED: 0.25 
#
#  The cuts more or less correspond to D*+ selection by Alexandr Kozlinzkiy
#
#  The performance with 100k events from Reco05-Stripping08_SDSTs.py:
# 
#    + ----------------------------------+----------+----------+--------+--------+
#    | Decision name                     |     Rate | Accepted |  Mult. | <T>,ms |
#    + ----------------------------------+----------+----------+--------+--------+
#    | StrippingGlobal                   | 0.009630 |      963 |        |  3.387 |
#    + ----------------------------------+----------+----------+--------+--------+
#    | StrippingSequenceStreamTest       | 0.009630 |      963 |        |  3.381 |
#    | -- StrippingD02HHForPromptCharm   | 0.001060 |      106 |  1.009 |  0.192 |
#    | -- StrippingDstarForPromptCharm   | 0.002660 |      266 |  1.541 |  1.457 | 
#    | -- StrippingDsForPromptCharm      | 0.002580 |      258 |  1.190 |  0.779 |
#    | -- StrippingDForPromptCharm       | 0.001460 |      146 |  1.233 |  0.301 |
#    | -- StrippingLambdaCForPromptCharm | 0.002500 |      250 |  1.224 |  0.578 | 
#    + ----------------------------------+----------+----------+--------+--------+
#
# $Revision: 1.6 $
# Last modification $Date: 2010-08-24 11:05:43 $
#                by $Author: ibelyaev $
# =============================================================================
"""

The attempt for coherent stripping of all stable charm hadrons

    - D0        -> K pi , K K                       PRESCALED: 0.10 
    - D*+       -> ( D0 -> K pi , K K , pi pi, pi K ) pi+
    - Ds+/D+    -> ( phi -> K K ) pi                
    - D+        -> K pi pi                          PRESCALED: 0.25 
    - Lambda_c+ -> p K pi                           PRESCALED: 0.25 

The cuts more or less correspond to D*+ selection by Alexandr Kozlinzkiy

The performance with 100k events from Reco05-Stripping08_SDSTs.py:

    + ----------------------------------+----------+----------+--------+--------+
    | Decision name                     |     Rate | Accepted |  Mult. | <T>,ms |
    + ----------------------------------+----------+----------+--------+--------+
    | StrippingGlobal                   | 0.009630 |      963 |        |  3.387 |
    + ----------------------------------+----------+----------+--------+--------+
    | StrippingSequenceStreamTest       | 0.009630 |      963 |        |  3.381 |
    | -- StrippingD02HHForPromptCharm   | 0.001060 |      106 |  1.009 |  0.192 |
    | -- StrippingDstarForPromptCharm   | 0.002660 |      266 |  1.541 |  1.457 | 
    | -- StrippingDsForPromptCharm      | 0.002580 |      258 |  1.190 |  0.779 |
    | -- StrippingDForPromptCharm       | 0.001460 |      146 |  1.233 |  0.301 |
    | -- StrippingLambdaCForPromptCharm | 0.002500 |      250 |  1.224 |  0.578 | 
    + ----------------------------------+----------+----------+--------+--------+

$Revision: 1.6 $
Last modification $Date: 2010-08-24 11:05:43 $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__date__    = '2010-08-03'
__version__ = '$Revision: 1.6 $'
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

from PhysSelPython.Wrappers import AutomaticData, Selection 
from Configurables          import CombineParticles,FilterDesktop

_Pions      = AutomaticData ( Location = "Phys/StdLoosePions"   )
_Kaons      = AutomaticData ( Location = "Phys/StdLooseKaons"   )
_Protons    = AutomaticData ( Location = "Phys/StdLooseProtons" )
_NoPIDPions = AutomaticData ( Location = "Phys/StdNoPIDsPions"  )

_kaoncuts       = "(TRCHI2DOF<10) & ( 0 < PIDK  - PIDpi )                       & ( MIPCHI2DV() > 9 ) " 
_pioncuts       = "(TRCHI2DOF<10) & ( 0 < PIDpi - PIDK  )                       & ( MIPCHI2DV() > 9 ) " 
_protoncuts     = "(TRCHI2DOF<10) & ( 0 < PIDp  - PIDpi ) & ( 0 < PIDp - PIDK ) & ( MIPCHI2DV() > 9 ) " 
_slowpioncuts   = " TRCHI2DOF<10 "


## switch on-off embedded monitoring 
_monitor        = True 

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
    DaughtersCuts = {
    'K-'  :  _kaoncuts ,
    'pi+' :  _pioncuts 
    } ,
    ## combination cut
    CombinationCut = " ADAMASS('D0') < 80 * MeV " , ## wide mass-cut for combination 
    ## mother cut
    MotherCut      = """
    ( chi2vx < 9 )              &
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
    RequiredSelections = [ _Pions , _Kaons ]
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
    ( ADMASS( 'D0' ) < 50 * MeV ) &
    ( kpi | kk  )                 &
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
    'D0'  : " switch ( pipi , in_range ( -30 * MeV , DMASS('D0') , 50 * MeV ) , ALL ) "
    } ,
    ##
    CombinationCut = "AM - AM1 < 160 * MeV"  ,
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
    "CombineDdForPromptCharm" ,
    ##
    Monitor      = _monitor  ,
    HistoProduce = _monitor  ,
    ##
    DecayDescriptors = [
    " [D_s+ -> K-  K+  pi+ ]cc" , ## through phi !!!
    ] ,
    ##
    Preambulo      = _preambulo  + [
    "aphi  = AM12              < 1040 * MeV " , ## phi-mass window 
    "admD  = ADAMASS ('D+'  )  <   55 * MeV " , ## D+  mass window for combination cut 
    "admD1 = ADAMASS ('D+'  )  <   45 * MeV " , ## D+  mass window for combination cut 
    "admDs = ADAMASS ('D_s+')  <   55 * MeV " , ## Ds+ mass window for combiantion cut 
    "dmD   = ADMASS  ('D+'  )  <   50 * MeV " , ## D+  mass window 
    "dmDs  = ADMASS  ('D_s+')  <   50 * MeV " , ## Ds+ mass window
    ##
    "hKKpi = Gaudi.Histo1DDef ( ' mass K K pi' , 1800 , 2020 , 110 )" ,
    ] ,
    ##
    DaughtersCuts = {
    'K-'  :  _kaoncuts ,
    'pi+' :  _pioncuts 
    } ,
    ##                                 phi                         
    CombinationCut = " aphi & ( admD | admDs ) " , 
    MotherCut      = """
    ( chi2vx  < 25 ) &
    ( dmD | dmDs   ) &
    ( ctau > 100 * micrometer )
    """ ,
    MotherMonitor  = """
    process ( monitor ( M , hKKpi , 'mass KKpi' )  ) >> ~EMPTY 
    """
    )

## convert it to selection
DsForPromptCharm_Selection = Selection (
    "SelDsForPromptCharm" ,
    Algorithm          = _DsCombine ,
    RequiredSelections = [ _Kaons , _Pions ]
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
    DaughtersCuts = {
    'K-'  :  _kaoncuts ,
    'pi+' :  _pioncuts 
    } ,
    ##                                 
    CombinationCut = " ADAMASS('D+') < 55 * MeV  " , 
    MotherCut      = """
    ( chi2vx  < 25 ) &
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
    RequiredSelections = [ _Kaons , _Pions ]
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
    ##    ##
    DaughtersCuts = {    
    'p+'  :  _protoncuts ,
    'K-'  :  _kaoncuts   ,
    'pi+' :  _pioncuts 
    } ,
    ##
    CombinationCut = " ADAMASS('Lambda_c+') < 55 * MeV  " , 
    MotherCut      = """
    ( chi2vx  < 25 )                   &
    ( ADMASS('Lambda_c+') < 50 * MeV ) &
    ( ctau    > 100 * micrometer )
    """ ,
    MotherMonitor  = """
    process ( monitor ( M , hpKpi , 'mass p K pi ' ) ) >> ~EMPTY 
    """ 
    )

## convert it to selection
LambdaCForPromptCharm_Selection = Selection (
    "SelLambdaCForPromptCharm" ,
    Algorithm          = _LambdaC_Combine ,
    RequiredSelections = [ _Protons, _Kaons , _Pions ]
    )


Selections = [
    D02HHForPromptCharm_PreSelection ,
    D02HHForPromptCharm_Selection    ,
    DstarForPromptCharm_Selection    ,
    DForPromptCharm_Selection        ,
    LambdaCForPromptCharm_Selection     
    ]

# =============================================================================
## build the stripping lines 
# =============================================================================

from StrippingConf.StrippingLine import StrippingLine

## require 1,2, or 3 primary vertex 
PrimaryVertices = (1,3) 

D02HHForPromptCharm_Line   = StrippingLine (
    "D02HHForPromptCharm"      ,
    prescale = 0.10            ,                ## ATTENTION! Prescale here !!
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
    prescale = 0.25            ,                ## ATTENTION! Prescale here !!
    checkPV = PrimaryVertices , 
    algos   = [ DForPromptCharm_Selection        ]
    )

LambdaCForPromptCharm_Line = StrippingLine (
    "LambdaCForPromptCharm" ,
    prescale = 0.25            ,                ## ATTENTION! Prescale here !!
    checkPV = PrimaryVertices , 
    algos   = [ LambdaCForPromptCharm_Selection  ]
    )

Lines  = [
    D02HHForPromptCharm_Line   ,
    DstarForPromptCharm_Line   ,
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
