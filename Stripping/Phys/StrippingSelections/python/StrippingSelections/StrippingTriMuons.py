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
#  Test wuth RUN_79647_RealData+Reco06-Stripping10_90000000_SDST.py
#  100k input events events, 63350 ``physics''-triggers
# +----------------------+----------------+----------+-------------------------------------+
# | Decision name        |    Rate [%]    | Accepted | Mult | ms/evt | Errs | Incds | Slow |
# +----------------------+----------------+----------+------+--------+---------------------+
# | StrippingThreeMuons  | 0.017 +- 0.005 |    11    | 3.273|  3.582 |   0  |    0  |  207 |
# | StrippingBc2ThreeMu  | 0.033 +- 0.007 |    21    | 1.238|  0.061 |   0  |    0  |    0 |
# | StrippingTau2ThreeMu | 0.032 +- 0.007 |    20    | 1.450|  0.056 |   0  |    0  |    0 |
# +----------------------+----------------+----------+------+--------+---------------------+
#
#  Usage:
#
#  @code
#
#    from StrippinSelections.StrippingTriMuons import  StrippingTriMuonsConf
# 
#    stream.addLined ( StrippingTriMuonsConf ( {} ).lines() )
#
#  @endcode
#
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


    Test wuth RUN_79647_RealData+Reco06-Stripping10_90000000_SDST.py
    100k input events events, 63350 ``physics''-triggers
   +----------------------+----------------+----------+-------------------------------------+
   | Decision name        |    Rate [%]    | Accepted | Mult | ms/evt | Errs | Incds | Slow |
   +----------------------+----------------+----------+------+--------+---------------------+
   | StrippingThreeMuons  | 0.017 +- 0.005 |    11    | 3.273|  3.582 |   0  |    0  |  207 |
   | StrippingBc2ThreeMu  | 0.033 +- 0.007 |    21    | 1.238|  0.061 |   0  |    0  |    0 |
   | StrippingTau2ThreeMu | 0.032 +- 0.007 |    20    | 1.450|  0.056 |   0  |    0  |    0 |
   +----------------------+----------------+----------+------+--------+---------------------+

 Usage:

     from StrippinSelections.StrippingTriMuons import  StrippingTriMuonsConf
     
     stream.addLined ( StrippingTriMuonsConf ( {} ).lines() )


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
    ##
    "StrippingTriMuonsConf"      ## asked by Tom & Greig
    ##
    )
# =============================================================================
from PhysSelPython.Wrappers import AutomaticData, Selection, EventSelection 
from Configurables          import CombineParticles,FilterDesktop, LoKi__VoidFilter 

import logging
log = logging.getLogger('StrippingTriMuons')


# =============================================================================
## Define the default configuration 
_default_configuration_ = {
    #
    # Selection of basic particles 
    #
    'GoodMuons'    : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 5  ) & ( BPVIPCHI2 () >  4 ) " ,
    'TightMuons'   : " ( PT > 1.5 * GeV )                      & ( BPVIPCHI2 () > 16 ) " ,
    #
    # Trigger 
    #
    'HLT'          : " HLT_PASS_RE('Hlt.*(MicroBias|Muon|MuMu|DiMu).*Decision') " , 
    #
    # Prescale 
    #
    '3mu-Prescale' : 1.00 , 
    'Bc-Prescale'  : 1.00 , 
    'Tau-Prescale' : 1.00 , 
    #
    # Technicalities:
    #
    'Preambulo'    : [
    ## shortcut for chi2 of vertex fit 
    'chi2vx = VFASPF(VCHI2) '                               ,
    ## shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light"     ,
    "ctau    =                    BPVLTIME ( ) * c_light "  ,
    "ctauBc  = PDGM('B_c+') / M * BPVLTIME ( ) * c_light "  
    ]
    #
    }

# =============================================================================
## @class  StrippingTriMuonsConf
#  Helper class required by Tom & Greig 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-09-26
class StrippingTriMuonsConf(object) :
    """
    Helper class to confugiure ``Tri-muon''-lines 
    """
    __configuration_keys__ = _default_configuration_ 

    ## get the default configuration 
    @staticmethod
    def defaultConfiguration( key = None ) :
        """
        Get the defualt configurtaion
        
        >>> conf = StrippingTriMuonsConf.defaultConfiguration()
        
        
        Get the elements of default configurtaion:
        
        >>> prescale = StrippingTriMuonsConf.defaultConfiguration( '3mu-Prescale' )
        """
        from copy import deepcopy
        _config = deepcopy ( _default_configuration_ )
        if key : return _config[ key ]
        return _config

    ## constructor:
    def __init__   ( self , config = {} ) :
        """
        constructor
        """
        from copy import deepcopy
        _config   = deepcopy ( config )
        
        keys = _config.keys()
        for key in keys :
            
            if not key in _default_configuration_ :
                raise KeyError("Invalid key is specified: '%s'" % key )
            
            val = _config[key]
            if val != _default_configuration_ [ key ] : 
                log.warning ('StrippingTriMuons: new configuration: %-16s : %s ' % ( key , _config[key] ) )
                
        self._goodMuonCuts  = _config.pop ( 'GoodMuons'       , _default_configuration_ [ 'GoodMuons'    ] )
        self._tightMuonCuts = _config.pop ( 'TightMuons'      , _default_configuration_ [ 'TightMuons'   ] )
        self._hlt           = _config.pop ( 'HLT'             , _default_configuration_ [ 'HLT'          ] )
        
        self._3mu_prescale  = _config.pop ( '3mu-Prescale'    , _default_configuration_ [ '3mu-Prescale' ] )
        self._Bc_prescale   = _config.pop ( 'Bc-Prescale'     , _default_configuration_ [ 'Bc-Prescale'  ] )
        self._tau_prescale  = _config.pop ( 'Tau-Prescale'    , _default_configuration_ [ 'Tau-Prescale' ] )

        self._preambulo     = _config.pop ( 'Preambulo'       , _default_configuration_ [ 'Preambulo'    ] )

        if _config :
            raise KeyError('Invalid keys are specified for configuration: %s ' % _config.keys() )


    # =========================================================================
    ## get all striping lines 
    def lines ( self ) :
        """
        Get all stripping lines 
        """
        
        if hasattr ( self , '_Lines' ) : return self._Lines

        from StrippingConf.StrippingLine import StrippingLine
        
        self._Lines = [
            #
            ## 3 muons
            #
            StrippingLine (
            'ThreeMuons'                  ,
            HLT      = self._hlt          ,
            prescale = self._3mu_prescale ,              
            algos    = [ self.threeTightMuons() ]
            ) ,
            #
            ## Bc -> 3mu 
            #
            StrippingLine (
            "Bc2ThreeMu"                  ,
            HLT      = self._hlt          ,
            prescale = self._Bc_prescale  ,
            algos    = [ self.Bc () ]
            ) ,            
            #
            ## tau -> 3mu 
            #
            StrippingLine (
            "Tau2ThreeMu"                 ,
            HLT      = self._hlt          ,
            prescale = self._tau_prescale ,
            algos    = [ self.tau () ]
            )            
            ]
        
        return self._Lines 
    
    ## get the selections 
    def selections ( self ) :
        
        if hasattr ( self , '_Selections' ) : return self._Selections
        
        self._Selections =  [ self.goodMuons       () ,
                              self.tightMuons      () ,
                              self.threeGoodMuons  () ,
                              self.threeTightMuons () ,
                              self.Bc              () ,
                              self.tau             () ]
        
        return self._Selections
    
    
    def goodMuonCuts   ( self ) : return self._goodMuonCuts
    def tightMuonCuts  ( self ) : return self._tightMuonCuts
    def hlt            ( self ) : return self._hlt
    def preambulo      ( self ) : return self._preambulo 
    
    
    # =========================================================================
    # get good muons 
    # =========================================================================
    def goodMuons      ( self ) :
        """
        Get selection for good muons 
        """
        if hasattr ( self , 'GoodMuons_Selection' ) :
            return self.GoodMuons_Selection
        
        _LooseMuons      = AutomaticData ( Location = "Phys/StdLooseMuons" )
        
        ## helper selection of 'good' muons 
        _GoodMuons = FilterDesktop (
            ##
            "GoodMuForTriMuons"  ,
            ##
            Code = self.goodMuonCuts() 
            )
        
        ## make (pre)-selection
        self.GoodMuons_Selection = Selection (
            ##
            "GoodMuonsForTriMuons"                 ,
            ##
            Algorithm          = _GoodMuons    ,
            ##
            RequiredSelections = [ _LooseMuons ]
            )
        
        return self.GoodMuons_Selection

    # =========================================================================
    # get three good muons 
    # =========================================================================
    def threeGoodMuons      ( self ) :
        """
        Get three good muons 
        """
        if hasattr ( self , 'ThreeGoodMuons_EventSelection' ) :
            return self.ThreeGoodMuons_EventSelection
        
        _goodmu = self.goodMuons() 
        ## require at least 3 good muons
        self.ThreeGoodMuons_EventSelection = EventSelection (
            ##
            LoKi__VoidFilter (
            "ThreeGoodMuons"  ,
            Code = " CONTAINS('%s') > 2.5 " %  ( _goodmu.outputLocation() ) 
            ) ,
            ##
            RequiredSelection  = _goodmu
            )
        
        return self.ThreeGoodMuons_EventSelection
    

    # =========================================================================
    # get good muons 
    # =========================================================================
    def tightMuons ( self ) :
        """
        Get tight  muons 
        """
        if hasattr ( self , 'TightMuons_Selection' ) :
            return self.TightMuons_Selection
        
        ## helper selection of 'tight' muons 
        _TightMuons = FilterDesktop (
            ##
            "TightMuForTriMuons"  ,
            ##
            Code = self.tightMuonCuts() 
            )
        
        ## make selection
        self.TightMuons_Selection = Selection (
            ##
            "TightMuonsForTriMuons"             ,
            ##
            Algorithm          = _TightMuons    ,
            ##
            ## RequiredSelections = [ self.goodMuons() ]
            RequiredSelections = [ self.threeGoodMuons() ]
            )

        return self.TightMuons_Selection
    
    # =========================================================================
    # get three tight muons 
    # =========================================================================
    def threeTightMuons      ( self ) :
        """
        Get three tight muons 
        """
        if hasattr ( self , 'ThreeTightMuons_Selection' ) :
            return self.ThreeTightMuons_Selection
        
        _tightmu = self.tightMuons() 
        ## require at least 3 good muons
        if not hasattr ( self , 'ThreeTightMuons_EventSelection' ) :
            self.ThreeTightMuons_EventSelection = EventSelection (
                ##
                LoKi__VoidFilter (
                "ThreeTightMuons"  ,
                Code = " CONTAINS( '%s' ) > 2.5 " %  ( _tightmu.outputLocation() ) 
                ) ,
                ##
                RequiredSelection  = _tightmu
                )
            
        ## helper selection, essentially it is just the cloner of 'tight' muons 
        _ThreeTightMuons = FilterDesktop (
            ##
            "ThreeTightMuForTriMuons"  ,
            ##
            Code = ' ALL '
            )
        
        ## make the final selection
        self.ThreeTightMuons_Selection = Selection (
            ##
            "ThreeTightMuonsForTriMuons"             ,
            ##
            Algorithm          = _ThreeTightMuons    ,
            ##
            RequiredSelections = [ self.ThreeTightMuons_EventSelection ]
            )
        return self.ThreeTightMuons_Selection
        ## return self.ThreeTightMuons_EventSelection

    
    # =========================================================================
    # get tau -> 3mu 
    # =========================================================================
    def tau ( self ) :
        """
        get tau -> 3mu 
        """
        
        if hasattr ( self , 'Tau2ThreeMu_Selection' ) : return self.Tau2ThreeMu_Selection
        
        _Tau2ThreeMu = CombineParticles (
            ##
            "CombineTau2ThreeMuons" ,
            ##
            DecayDescriptor  = " [ tau+ -> mu+ mu+ mu- ]cc" ,
            ##
            Preambulo        = self.preambulo() , 
            ## 
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
        self.Tau2ThreeMu_Selection  = Selection (
            ##
            "SelTau2ThreeMu" ,
            ##
            Algorithm          = _Tau2ThreeMu  ,
            ##
            RequiredSelections = [ self.threeGoodMuons() ] 
            )
        ##
        return self.Tau2ThreeMu_Selection

    # =========================================================================
    # get Bc -> ( ``psi'' -> mu mu) mu nu 
    # =========================================================================
    def Bc ( self ) :
        """
        Get Bc -> ( ``psi'' -> mu mu) mu nu 
        """

        if hasattr ( self , 'Bc2ThreeMu_Selection' ) : return self.Bc2ThreeMu_Selection 
        
        _Bc2ThreeMu = CombineParticles (
            ##
            "CombineBc2ThreeMuons" ,
            ##
            DecayDescriptor = " [B_c+ -> mu+ mu+ mu- ]cc" ,
            ##
            Preambulo        = self.preambulo() + [
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
            CombinationCut  = """
            ( l1S < AM ) & AHASCHILD( PT > 1.0 * GeV ) & ( dm1 | dm2 )
            """,
            ##
            MotherCut       = " ( chi2vx < 25 ) & ( ctauBc > 20 * micrometer ) " 
            )
        
        ## make selections
        self.Bc2ThreeMu_Selection  = Selection (
            ##
            "SelBc2ThreeMu" ,
            ##
            Algorithm          = _Bc2ThreeMu  ,
            ##
            RequiredSelections = [ self.threeGoodMuons() ] 
            )
        
        return self.Bc2ThreeMu_Selection

    
# =============================================================================
if '__main__' == __name__ :

    print 80*'*'
    print __doc__
    print ' Author :  %s' % __author__
    print ' Date   :  %s' % __date__
    
    print 80*'*'
    print ' The default configuration : '
    for key in _default_configuration_ :
        print '  %s : %s ' % ( key ,_default_configuration_ [ key ] )  
    print 80*'*'
    conf = StrippingTriMuonsConf( config = {} ) 
    print ' The output locations : '
    for l in conf.lines()      : 
        print ' \t ', l.outputLocation  ()         
    for l in conf.selections() : 
        print ' \t ', None if not hasattr ( l , 'outputLocation') else l.outputLocation ()         
    print 80*'*'

# =============================================================================
# The END 
# =============================================================================
