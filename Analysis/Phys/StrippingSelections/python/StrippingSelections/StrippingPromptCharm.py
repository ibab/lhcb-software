#!/usr/bin/env python
# $Id: StrippingPromptCharm.py,v 1.1 2010-08-03 11:54:25 ibelyaev Exp $
# =============================================================================
# $URL
# =============================================================================
## @file
# 
#  The attempt for coherent stripping of all stable charm hadrons
#
#    - D0        -> K pi , K K
#    - D*+       -> ( D0 -> K pi , K K , pi pi, pi K ) pi+
#    - D+/Ds+    -> K pi pi , K K pi
#    - Lambda_c+ -> p K pi
#
#  The cuts more or less corresponds to D*+ selection by Alexandr Kozlinzkiy
#
#  @todo fix a problem with CHILD 
#  @todo fix a problem with DECTREE
# 
# $Revision: 1.1 $
# Last modification $Date: 2010-08-03 11:54:25 $ by $Author: ibelyaev $
# =============================================================================
"""

The attempt for coherent stripping of all stable charm hadrons

    - D0        -> K pi , K K
    - D*+       -> ( D0 -> K pi , K K , pi pi, pi K ) pi+
    - D+/Ds+    -> K pi pi , K K pi
    - Lambda_c+ -> p K pi

The cuts more or less  corresponds to D*+ selection by Alexandr Kozlinzkiy

The performance with 100k events from Reco04-Stripping07-SDSTs.py:

 +---------------+-----------------+----------------------------------+
 |               |   Retention[%]  |   CPU [s]                        |
 +---------------+-----------------+----------------------------------+     
 |  D0           |  0.61 +- 0.02   |   0.04  / 998     ~ 0            |
 |     D0-presel |    1.00 +- 0.03 |   47    / 87k     = 0.5 ms/event | helper intermedite selection
 |  D*+          |  0.19 +- 0.01   |   0.23  / 998     ~ 0            |
 |  D+/Ds+       |  0.49 +- 0.02   |   63    / 87k     = 0.7 ms/event |
 |  Lambda_c+    |  0.62 +- 0.02   |   81    / 86k     = 0.9 ms/event |
 +---------------+-----------------+----------------------------------+
 | Total         |  1.66 +- 0.04   |   320/DaVinciMain = 3.2 ms/event |
 +---------------+-----------------+----------------------------------+
   
$Revision: 1.1 $
Last modification $Date: 2010-08-03 11:54:25 $ by $Author: ibelyaev $
"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__date__    = '2010-08-03'
__version__ = '$Revision: 1.1 $'
# =============================================================================
__all__ = (
    #
    "Selections"                       , ## the selection
    #
    'D02HHForPromptCharm_PreSelection' ,
    'D02HHForPromptCharm_Selection'    ,
    'DstarForPromptCharm_Selection'    ,
    'DForPromptCharm_Selection'        ,
    'LambdaCForPromptChamrSelection'   ,
    #
    "Lines"                      ,  ## the stripping lines
    #
    'D02HHForPromptCharm_Line'   ,
    'DstarForPromptCharm_Line'   ,
    'DForPromptCharm_Line'       , 
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

_preambulo      = [
    ## the D0 decay channels
    ## "DECTREE = LoKi.Particles.DecTree         " ,
    ##"pipi   = DECTREE('[D0]cc -> pi- pi+   ') " ,
    ##"kk     = DECTREE('[D0]cc -> K-  K+    ') " ,
    ##"kpi    = DECTREE('[D0    -> K-  pi+]CC') " ,
    ## temporary, till DECTREE fix 
    "pipi     =   2 == NINTREE ( 'pi+' == ABSID ) " ,
    "kk       =   2 == NINTREE ( 'K+'  == ABSID ) " ,
    "kpi      = ( 1 == NINTREE ( 'K+'  == ABSID ) ) & ( 1 == NINTREE ( 'pi+' == ABSID ) ) " ,
    ## number of kaons in final state (as CombinationCuts)
    "ak2      = 2 == ANUM( 'K+' == ABSID ) "        ,
    ## shortcut for chi2 of vertex fit 
    'chi2vx = VFASPF(VCHI2) '                   , 
    ## shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light" , 
    "ctau   = BPVLTIME() * c_light "            
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
    CombinationCut = " ADAMASS('D0') < 80 * MeV " ,
    ## mother cut
    MotherCut      = "( chi2vx < 9 ) & ( ADMASS('D0') < 75 * MeV) & (BPVLTFITCHI2() < 9 ) & ( ctau > 90 * micrometer ) "
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
    Preambulo = _preambulo ,
    ##
    Code      = " ( ADMASS('D0') < 50 * MeV ) & ( kpi | kk ) "
    )

## make selection 
D02HHForPromptCharm_Selection = Selection (
    'SelD02HHForPromptCharm'                ,
    Algorithm          = _D0Filter           ,
    RequiredSelections = [ D02HHForPromptCharm_PreSelection ]
    )

# =============================================================================
## D*+ -> (D0 -> Kpi, piK, KK, pipi) pi+ selection 
# =============================================================================
_DstarCombine = CombineParticles(
    ##
    "CombineDstarForPromptCharm" ,
    ##
    DecayDescriptors = [
    " [D*(2010)+ -> D0 pi+]cc" ,
    " [D*(2010)- -> D0 pi-]cc" 
    ] ,
    ##
    Preambulo = _preambulo ,
    ##
    DaughtersCuts = {
    'pi+' :  _slowpioncuts ,
    'D0'  : " switch ( pipi , in_range( -30 * MeV , DMASS('D0') , 50 * MeV ) , ALL ) "
    } ,
    ##
    CombinationCut = "AM - AM1 < 160 * MeV"  ,
    ## 
    MotherCut      = " ( chi2vx < 64 ) & ( (M - M1) < 155 * MeV )"
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
## D+/Ds+ -> ( Kpipi & KKpi )  selection 
# =============================================================================
_DCombine = CombineParticles(
    ## 
    "CombineDForPromptCharm" ,
    ##
    DecayDescriptors = [
    " [D+ -> K-  K+  pi+ ]cc" , ## through phi !!!
    " [D+ -> K-  pi+ pi+ ]cc" 
    ] ,
    ##
    Preambulo      = _preambulo  + [  "aphi  = AM12             < 1040 * MeV " ,
                                      "admD  = ADAMASS('D+'  )  <   70 * MeV " ,
                                      "admDs = ADAMASS('D_s+')  <   70 * MeV " ,
                                      "dmD   = ADMASS ('D+'  )  <   60 * MeV " ,
                                      "dmDs  = ADMASS ('D_s+')  <   60 * MeV " ] ,
    ##
    DaughtersCuts = {
    'K-'  :  _kaoncuts ,
    'pi+' :  _pioncuts 
    } ,
    ##                                 phi                         
    CombinationCut = " switch ( ak2 , aphi & ( admD | admDs ) , admD )" , 
    MotherCut      = "( chi2vx  < 25 ) & ( dmD | dmDs ) & (BPVLTFITCHI2() < 9 ) & ( ctau > 90 * micrometer )"
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
    DecayDescriptors = [
    " [ Lambda_c+ -> p+  K-  pi+ ]cc" ,
    ] ,
    ##
    Preambulo = _preambulo ,
    ##
    DaughtersCuts = {    
    'p+'  :  _protoncuts ,
    'K-'  :  _kaoncuts   ,
    'pi+' :  _pioncuts 
    } ,
    ##
    CombinationCut = " ADAMASS('Lambda_c+') < 80 * MeV  " , 
    MotherCut      = "( chi2vx  < 25 ) & ( ADMASS('Lambda_c+') < 75 * MeV ) & (BPVLTFITCHI2() < 9 ) & ( ctau > 60 * micrometer )"
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

from Configurables               import LoKi__VoidFilter as Filter 
fltr = Filter (
    "PVMultiplicityFilter" ,
    Preambulo = [ "from LoKiCore.functions import in_range" ] ,
    Code = " in_range ( 0.5 , CONTAINS('Rec/Vertex/Primary') , 3.5 ) " 
    )

D02HHForPromptCharm_Line   = StrippingLine (
    "D02HHForPromptCharm" ,
    algos = [ fltr , D02HHForPromptCharm_Selection   ] )

DstarForPromptCharm_Line   = StrippingLine (
    "DstarForPromptCharm" ,
    algos = [ fltr , DstarForPromptCharm_Selection   ] )

DForPromptCharm_Line       = StrippingLine (
    "DForPromptCharm" ,
    algos = [ fltr , DForPromptCharm_Selection       ] )

LambdaCForPromptCharm_Line = StrippingLine (
    "LambdaCForPromptCharm" ,
    algos = [ fltr , LambdaCForPromptCharm_Selection ] )

Lines  = [
    D02HHForPromptCharm_Line   ,
    DstarForPromptCharm_Line   ,
    DForPromptCharm_Line       , 
    LambdaCForPromptCharm_Line 
    ]

# =============================================================================
if '__main__' == __name__ :

    print 80*'*'
    print __doc__
    print ' Author :  %s' % __author__
    print ' Date   :  %s' % __date__
    print ' The output location: '
    for l in Lines :
        print ' \t ', l.outputLocation  () 
    print 80*'*'
        
# =============================================================================
# The END 
# =============================================================================
