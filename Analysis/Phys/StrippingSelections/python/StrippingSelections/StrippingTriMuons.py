#!/usr/bin/env python
# =============================================================================
# $Id: StrippingTriMuons.py,v 1.1 2010-08-08 17:30:18 ibelyaev Exp $
# =============================================================================
# $URL$
# =============================================================================
## @file
# 
#  Strip three-muon events: 
#
#     - tau+ -> mu+mu+mu-   candidates
#     - Bc+  -> mu+mu+mu-)  candidates
#     - search for interesting multimuon events
#
# $Revision: 1.1 $
# Last modification $Date: 2010-08-08 17:30:18 $
#                by $Author: ibelyaev $
# =============================================================================
"""
Strip three-muon events: 

    - tau+ -> mu+mu-mu-         candidates
    - Bc+  -> mu+nu  ( mu-mu )  candidates 
    - search for interesting multimuon events 

NOT YET ADDED TO ANY STREAM

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__date__    = '2010-08-08'
__version__ = '$Revision: 1.1 $'
# =============================================================================
__all__ = (
    #
    "Selections"                       , ## the selections
    #
    "Lines"                            , ## the stripping lines
    #
    )
# =============================================================================
from PhysSelPython.Wrappers import AutomaticData, Selection 
from Configurables          import CombineParticles,FilterDesktop

_LooseMuons      = AutomaticData ( Location = "Phys/StdLooseMuons"   )

_Preambulo  = [
    ## shortcut for chi2 of vertex fit 
    'chi2vx = VFASPF(VCHI2) '                   ,
    ## shortcut for the c*tau
    ## use the embedded cut for chi2(LifetimeFit)<9 
    "from GaudiKernel.PhysicalConstants import c_light" ,
    "ctau   = BPVLTIME ( 9 ) * c_light "
    ]

muoncuts = " ( TRCHI2DOF< 10 ) & ( MIPCHI2DV() > 4 ) "

## helper selection of 'good' muons 
_GoodMuons = FilterDesktop (
    "GoodMuForTriMuons"  ,
    Code = muoncuts     
    )

## make (pre)-selection
GoodMuons_Selection = Selection (
    ##
    "MuonsForTriMuons"                 ,
    ##
    Algorithm          = _GoodMuons    ,
    ##
    RequiredSelections = [ _LooseMuons ]
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
    CombinationCut  = " ADAMASS('tau+') < 500 * MeV " , ## wide mass-combination
    ##
    MotherCut       = " ( chi2vx < 25 ) & ( ctau > 50 * micrometer ) " 
    )

## make selections
Tau2ThreeMu_Selection  = Selection (
    ##
    "SelTau2ThreeMu" ,
    ##
    Algorithm          = _Tau2ThreeMu  ,
    ##
    RequiredSelections = [ GoodMuons_Selection ] 
    )


_Bc2ThreeMu = CombineParticles (
    ##
    "CombineBc2ThreeMuons" ,
    ##
    DecayDescriptor = " [B_c+ -> mu+ mu+ mu- ]cc" ,
    ##
    Preambulo        = _Preambulo + [
    ## mass-windows
    "dm13 =  abs( AM13 - PDGM('J/psi(1S)') ) < 100 * MeV " ,
    "dm23 =  abs( AM23 - PDGM('J/psi(1S)') ) < 100 * MeV " ,
    "dm1S =  lambda s :  abs ( s - PDGM('J/psi(1S)') ) < 100 * MeV " ,
    "dm2S =  lambda s :  abs ( s - PDGM('psi(2S)'  ) ) < 100 * MeV " 
    ] ,
    ##
    DaughtersCuts    = {
    'mu+'  : muoncuts  + ' & ( PT > 1 * GeV ) '
    } ,
    ## 
    CombinationCut  = """
    ( 2.8 * GeV < AM ) & ( dm13 | dm23 ) 
    """ ,
    ##
    MotherCut       = " ( chi2vx < 25 ) & ( ctau > 50 * micrometer ) " 
    )


## make selections
Bc2ThreeMu_Selection  = Selection (
    ##
    "SelBc2ThreeMu" ,
    ##
    Algorithm          = _Bc2ThreeMu  ,
    ##
    RequiredSelections = [ GoodMuons_Selection ] 
    )


Selections = [
    GoodMuons_Selection   ,
    Tau2ThreeMu_Selection , 
    Bc2ThreeMu_Selection    
    ]


# =============================================================================
## build the stripping lines 
# =============================================================================

from StrippingConf.StrippingLine import StrippingLine


Tau2ThreeMu_Line = StrippingLine (
    "Tau2ThreeMu" ,
    algos   = [ Tau2ThreeMu_Selection ]
    )
Bc2ThreeMu_Line  = StrippingLine (
    "Bc2ThreeMu" ,
    algos   = [ Bc2ThreeMu_Selection  ]
    )


Lines = [
    Tau2ThreeMu_Line ,
    Bc2ThreeMu_Line  
    ]


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
