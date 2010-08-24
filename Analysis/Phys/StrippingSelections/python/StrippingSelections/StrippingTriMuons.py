#!/usr/bin/env python
# =============================================================================
# $Id: StrippingTriMuons.py,v 1.3 2010-08-24 11:05:43 ibelyaev Exp $
# =============================================================================
# $URL$
# =============================================================================
## @file
# 
#  Strip "three-muon events" : 
#
#     - tau+ -> mu+ mu+ mu-                candidates
#     - Bc+  ->  ( psi -> mu+ mu-) mu+ nu  candidates
#     - search for interesting multimuon (>2muons) events 
#
# The selection criteria are rather natural & straighforward:
#
#
#   tau+ :
#   
#      - three muons chi2(ip)>4, pt > 250 MeV 
#      - at least one muon with PT > 1 GeV 
#      - +-250 MeV mass-window
#      - c*tau > 40 um  
#
#   B_c+ :
#   
#      - three muons chi2(ip)>4, pt > 250 MeV 
#      - at least one muon with pt > 1 GeV
#      - at least one mu+mu- combination in wide +-200 MeV
#                mass window around J/psi or psi'
#      - mass is in excess of 'm(J/psi) - 200 MeV'
#      - c*tau' > 20 um, where c*tau' is corrected c*tau :
#                        c*tau' = c*tau * pdg_mass(B_c) / mass(3mu)  
#
#   ThreeMuons :
#   
#      - at least three muons chi2(ip)>9 and pt > 1.2 GeV 
#
#  
#   The Global Event Selection:
#
#    - each line requres at least one primary vertex
#    - the Hlt-requirements: MicroBias or Muon-trigger
#         HLT_PASS_RE('Hlt.*(MicroBias|Muon).*Decision')
#
#  Test with 100k events Reco05-Stripping08_SDSTs.py : 
# 
#   +-----------------------------+-----------+-----------+---------+--------+
#   | Decision name               |     Rate  |  Accepted |  Mult.  | <T>,ms |
#   +-----------------------------+-----------+-----------+---------+--------+
#   | StrippingGlobal             | 0.000930  |      93   |         | 1.035  |
#   +-----------------------------+-----------+-----------+---------+--------+
#   | StrippingSequenceStreamTest | 0.000930  |      93   |         |  1.029 | 
#   | -- StrippingThreeMuons      | 0.000510  |      51   | 226.000 |  0.860 | 
#   | -- StrippingBc2ThreeMu      | 0.000180  |      18   | 1.556   |  0.057 | 
#   | -- StrippingTau2ThreeMu     | 0.000240  |      24   | 1.500   |  0.051 | 
#   +-----------------------------+-----------+---------------------+--------+
#
# @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
# @date   2010-08-10
# 
# $Revision: 1.3 $
# Last modification $Date: 2010-08-24 11:05:43 $
#                by $Author: ibelyaev $
# =============================================================================
"""
Strip three-muon events: 

     - tau+ -> mu+ mu+ mu-                candidates
     - Bc+  ->  ( psi -> mu+ mu-) mu+ nu  candidates
     - search for interesting multimuon (>2muons) events 

 The selection criteria are rather natural & straighforward:

   tau+ :
   
      - three muons chi2(ip)>4, pt>250 MeV
      - at least one muon with pt>1 GeV 
      - +-250 MeV mass-window
      - c*tau > 40 um  

   B_c+ :
   
      - three muons chi2(ip)>4, pt>250 MeV 
      - at least one muon with PT>1 GeV
      - at least one mu+mu- combination in wide +-200 MeV
                   mass window around J/psi or psi'
      - mass is in excess of 'm(J/psi)-200 MeV'
      - c*tau' > 20 um, where c*tau' is corrected c*tau :
                        c*tau' = c*tau * pdg_mass(B_c) / mass(3mu)  

   ThreeMuons :
   
      - at least three muons chi2(ip)>9 and pt>1.2 GeV 

  
   The Global Event Selection:

    - each line requres at least one primary vertex
    - the Hlt-requirements: MicroBias or Muon-trigger
         HLT_PASS_RE('Hlt.*(MicroBias|Muon).*Decision')


   Test with 100k events Reco05-Stripping08_SDSTs.py : 
 
    +-----------------------------+-----------+-----------+---------+--------+
    | Decision name               |     Rate  |  Accepted |  Mult.  | <T>,ms |
    +-----------------------------+-----------+-----------+---------+--------+
    | StrippingGlobal             | 0.000930  |      93   |         | 1.035  |
    +-----------------------------+-----------+-----------+---------+--------+
    | StrippingSequenceStreamTest | 0.000930  |      93   |         |  1.029 | 
    | -- StrippingThreeMuons      | 0.000510  |      51   | 226.000 |  0.860 | 
    | -- StrippingBc2ThreeMu      | 0.000180  |      18   | 1.556   |  0.057 | 
    | -- StrippingTau2ThreeMu     | 0.000240  |      24   | 1.500   |  0.051 | 
    +-----------------------------+-----------+---------------------+--------+

 
$Revision: 1.3 $
Last modification $Date: 2010-08-24 11:05:43 $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__date__    = '2010-08-08'
__version__ = '$Revision: 1.3 $'
# =============================================================================
__all__ = (
    #
    "Selections"                , ## the list of selections
    #
    "GoodMuons_Selection"       ,
    "ThreeGoodMuons_Selection"  ,
    "TightMuons_Selection"      ,
    "ThreeTightMuons_Selection" ,
    "Tau2ThreeMu_Selection"     , 
    "Bc2ThreeMu_Selection"      ,
    #
    "Lines"                     , ## the list of stripping lines
    #
    "ThreeMuons_Line"           ,
    "Tau2ThreeMu_Line"          , 
    "Bc2ThreeMu_Line"           
    )
# =============================================================================
from PhysSelPython.Wrappers import AutomaticData, Selection, EventSelection 
from Configurables          import CombineParticles,FilterDesktop, LoKi__VoidFilter 

_LooseMuons      = AutomaticData ( Location = "Phys/StdLooseMuons" )
##_LooseMuons      = AutomaticData ( Location = "Phys/StdNoPIDsMuons" )

_Preambulo  = [
    ## shortcut for chi2 of vertex fit 
    'chi2vx = VFASPF(VCHI2) '                   ,
    ## shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light"      ,
    "ctau    =                  BPVLTIME ( ) * c_light "  ,
    "ctauBc  = PDGM('B_c+')/M * BPVLTIME ( ) * c_light "  
    ]

muoncuts      = " ( PT > 250 * MeV ) & ( TRCHI2DOF < 10  ) & ( BPVIPCHI2 () > 4 ) "
tightmuoncuts = " ( PT > 1.2 * GeV )                       & ( BPVIPCHI2 () > 9 ) "

## helper selection of 'good' muons 
_GoodMuons = FilterDesktop (
    ##
    "GoodMuForTriMuons"  ,
    ##
    Code = muoncuts     
    )

## make (pre)-selection
GoodMuons_Selection = Selection (
    ##
    "GoodMuonsForTriMuons"                 ,
    ##
    Algorithm          = _GoodMuons    ,
    ##
    RequiredSelections = [ _LooseMuons ]
    )


## require at least 3 good muons
ThreeGoodMuons_Selection = EventSelection (
    ##
    LoKi__VoidFilter (
    "ThreeGoodMuons"  ,
    Code = " CONTAINS('%s') > 2.5 " %  ( GoodMuons_Selection.outputLocation() + "/Particles" ) 
    ) ,
    ##
    RequiredSelection  = GoodMuons_Selection 
    )


## helper selection of 'tight' muons 
_TightMuons = FilterDesktop (
    ##
    "TightMuForTriMuons"  ,
    ##
    Code = tightmuoncuts 
    )

## make (pre)-selection
TightMuons_Selection = Selection (
    ##
    "TightMuonsForTriMuons"             ,
    ##
    Algorithm          = _TightMuons    ,
    ##
    RequiredSelections = [ GoodMuons_Selection ]
    )

## require at least 3 good muons
ThreeTightMuons_Selection = EventSelection (
    ##
    LoKi__VoidFilter (
    "ThreeTightMuons"  ,
    Code = " CONTAINS('%s') > 2.5 " %  ( TightMuons_Selection.outputLocation() + "/Particles" ) 
    ) ,
    ##
    RequiredSelection  = TightMuons_Selection 
    )

_Tau2ThreeMu = CombineParticles (
    ##
    "CombineTau2ThreeMuons" ,
    ##
    DecayDescriptor  = " [ tau+ -> mu+ mu+ mu- ]cc" ,
    ##
    Preambulo        = _Preambulo , 
    ## 
    DaughtersCuts    = {
    'mu+'  : muoncuts 
    } ,
    ## 
    CombinationCut  = """
    ( ADAMASS('tau+') < 250 * MeV ) & AHASCHILD ( PT > 1 * GeV )  
    """ , ## wide mass-combination + PT-cuts 
    ##
    MotherCut       = """
    ( chi2vx < 25 ) &
    ( ctau   > 40 * micrometer )
    """ 
    )

## make selections
Tau2ThreeMu_Selection  = Selection (
    ##
    "SelTau2ThreeMu" ,
    ##
    Algorithm          = _Tau2ThreeMu  ,
    ##
    RequiredSelections = [ ThreeGoodMuons_Selection ] 
    )

_Bc2ThreeMu = CombineParticles (
    ##
    "CombineBc2ThreeMuons" ,
    ##
    DecayDescriptor = " [B_c+ -> mu+ mu+ mu- ]cc" ,
    ##
    Preambulo        = _Preambulo + [
    ## mass-windows :
    "mPsi1S = PDGM('J/psi(1S)' ) " , 
    "mPsi2S = PDGM(  'psi(2S)' ) " ,
    "l1S    = mPsi1S - 200 * MeV " , 
    "h1S    = mPsi1S + 200 * MeV " , 
    "l2S    = mPsi2S - 200 * MeV " , 
    "h2S    = mPsi2S + 200 * MeV " , 
    "dm1    = in_range ( l1S , AM13 , h1S ) | in_range ( l2S , AM13 , h2S ) " , 
    "dm2    = in_range ( l1S , AM23 , h1S ) | in_range ( l2S , AM23 , h2S ) " 
    ] ,
    ##
    DaughtersCuts    = {
    'mu+'  : muoncuts 
    } ,
    ## 
    CombinationCut  = """
    ( l1S < AM ) & AHASCHILD( PT > 1.0 * GeV ) & ( dm1 | dm2 )
    """,
    ##
    MotherCut       = " ( chi2vx < 25 ) & ( ctauBc > 20 * micrometer ) " 
    )

## make selections
Bc2ThreeMu_Selection  = Selection (
    ##
    "SelBc2ThreeMu" ,
    ##
    Algorithm          = _Bc2ThreeMu  ,
    ##
    RequiredSelections = [ ThreeGoodMuons_Selection ]
    )

Selections = [
    GoodMuons_Selection        ,
    ThreeGoodMuons_Selection   ,
    TightMuons_Selection       ,
    ThreeTightMuons_Selection  ,
    Tau2ThreeMu_Selection      , 
    Bc2ThreeMu_Selection    
    ]

# =============================================================================
## build the stripping lines 
# =============================================================================

from StrippingConf.StrippingLine import StrippingLine

hlt = " HLT_PASS_RE('Hlt.*(MicroBias|Muon).*Decision') "

Tau2ThreeMu_Line = StrippingLine (
    "Tau2ThreeMu" ,
    HLT     = hlt ,
    algos   = [ Tau2ThreeMu_Selection     ]
    )

Bc2ThreeMu_Line  = StrippingLine (
    "Bc2ThreeMu"  ,
    HLT     = hlt ,
    algos   = [ Bc2ThreeMu_Selection      ]
    )

ThreeMuons_Line  = StrippingLine (
    "ThreeMuons"  ,
    HLT     = hlt ,
    algos   = [ ThreeTightMuons_Selection ]
    )

Lines = [
    ThreeMuons_Line  ,
    Bc2ThreeMu_Line  ,
    Tau2ThreeMu_Line 
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
