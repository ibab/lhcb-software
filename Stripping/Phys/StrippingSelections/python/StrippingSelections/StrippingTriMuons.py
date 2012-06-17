#!/usr/bin/env python
# =============================================================================
# $Id$
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
# $Revision$
# Last modification $Date$
#                by $Author$
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


$Revision$
Last modification $Date$
               by $Author$
"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__date__    = '2010-08-08'
__version__ = '$Revision$'
# =============================================================================
from   Gaudi.Configuration                   import *
from   GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from   PhysSelPython.Wrappers                import Selection, VoidEventSelection
from   StrippingConf.StrippingLine           import StrippingLine
from   StrippingUtils.Utils                  import LineBuilder
import GaudiKernel.SystemOfUnits as units
from   Configurables                         import LoKi__VoidFilter
from   StandardParticles                     import StdLooseMuons


__all__ = ('StrippingTriMuonsConf',
           'confdict')

confdict = {
    #
    # Selection of basic particles 
    #
    'GoodMuons'        : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 5  ) & ( BPVIPCHI2 () >  9 ) " ,
    'GoodMuonsForBc' : " ( BPVIPCHI2 () >  9 ) " ,
    'TightMuons'   : " ( PT > 1.9 * GeV ) & ( BPVIPCHI2 () > 25 ) " ,
    #
    # Trigger 
    #
    'HLT'    : None ,  
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

class StrippingTriMuonsConf(LineBuilder) :
    """
    """
    __configuration_keys__ = (
        'GoodMuons',
        'GoodMuonsForBc' ,
        'TightMuons',
        'HLT',
        '3mu-Prescale',
        'Bc-Prescale',
        'Tau-Prescale',
        'Preambulo'
        )

    __confdict__={}    

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.__confdict__= config
        self.name = name
        #
        ## 3 muons
        #
        self.line3mu =   StrippingLine (
            self.name+'ThreeMuons'                  ,
            HLT      = self.__confdict__['HLT']  ,
            prescale = self.__confdict__['3mu-Prescale']  ,              
            algos    = [ self.threeTightMuons() ]
            )
        self.registerLine(self.line3mu)
            
        #
        ##  Bc -> 3mu 
        #
        self.lineBc3mu =  StrippingLine (
            self.name+'Bc2ThreeMu'                  ,
            HLT      = self.__confdict__['HLT']  ,
            prescale = self.__confdict__['Bc-Prescale']  ,              
            algos    = [ self.Bc() ]
            )
        self.registerLine(self.lineBc3mu)
        
        '''
        JA 20110825 remove, now in separate file
        #
        ## tau -> 3mu 
        #
        self.lineTau3mu =    StrippingLine (
            self.name+'Tau2ThreeMu'                  ,
            HLT      = self.__confdict__['HLT']  ,
            prescale = self.__confdict__['Tau-Prescale']  ,              
            algos    = [ self.tau() ]
            )
        self.registerLine(self.lineTau3mu)
        '''
    ## get the selections 
    def selections ( self , name ) :
        
        if hasattr ( self , '_Selections' ) : return self._Selections
        
        self._Selections =  [ self.goodMuons           () ,
                              self.threeGoodMuons      () ,
                              self.goodMuonsForBc      () ,
                              self.tightMuons          () ,
                              self.threeGoodMuonsForBc () ,
                              self.threeTightMuons     () ,
                              self.Bc                  () ,
                              self.tau                 () ]
       
        return self._Selections
 
        
    # =========================================================================
    # get good muons 
    # =========================================================================
    def goodMuons      ( self ) :
        """
        Get selection for good muons 
        """
        if hasattr ( self , 'GoodMuons_Selection' ) :
            return self.GoodMuons_Selection
        
        _LooseMuons = StdLooseMuons
        
        ## helper selection of 'good' muons 
        _GoodMuons = FilterDesktop (
            Code = self.__confdict__['GoodMuons']
            )
        
        ## make (pre)-selection
        self.GoodMuons_Selection = Selection (
            ##
            self.name + "GoodMuonsForTriMuons"                 ,
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
        self.ThreeGoodMuons_EventSelection =VoidEventSelection (
            self.name + "ThreeGoodMuonsEventSelection",
            ##Algorithm = LoKi__VoidFilter (

            ##self.name + "ThreeGoodMuons"  ,
            Code = " CONTAINS('%s') > 2.5 " %  ( _goodmu.outputLocation() ) ,
            ##) ,
            RequiredSelection = _goodmu
            )
        
        return self.ThreeGoodMuons_EventSelection

    # =========================================================================
    # get good muons 
    # =========================================================================
    def goodMuonsForBc ( self ) :
        """
        Get  muons for tau 
        """
        if hasattr ( self , 'GoodMuonsForBc_Selection' ) :
            return self.GoodMuonsForBc_Selection

        ## helper selection of 'tight' muons 
        _MuonsForBc = FilterDesktop (
            Code = self.__confdict__['GoodMuonsForBc']
            )
        
        ## make selection
        self.GoodMuonsForBc_Selection = Selection (
            ##
            self.name + "GoodMuonsForBcMuons"             ,
            ##
            Algorithm          = _MuonsForBc    ,
            ##
            RequiredSelections = [ self.threeGoodMuons() ]
            )

        return self.GoodMuonsForBc_Selection

    # =========================================================================
    # get three good muons 
    # =========================================================================
    def threeGoodMuonsForBc      ( self ) :
        """
        Get three good muons ForTau
        """
        if hasattr ( self , 'ThreeGoodMuonsForBc_EventSelection' ) :
            return self.ThreeGoodMuonsForBc_EventSelection
        
        _goodmu = self.goodMuonsForBc()
        
        ## require at least 3 good muons
        self.ThreeGoodMuonsForBc_EventSelection =VoidEventSelection (
            self.name + "ThreeGoodMuonsForBcEventSelection",
            ##Algorithm = LoKi__VoidFilter (

            ##self.name + "ThreeGoodMuons"  ,
            Code = " CONTAINS('%s') > 2.5 " %  ( _goodmu.outputLocation() ) ,
            ##) ,
            RequiredSelection = _goodmu
            )
        
        return self.ThreeGoodMuonsForBc_EventSelection

 
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
            Code = self.__confdict__['TightMuons']
            )
        
        ## make selection
        self.TightMuons_Selection = Selection (
            ##
            self.name + "TightMuonsForTriMuons"             ,
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
            self.ThreeTightMuons_EventSelection = VoidEventSelection (
                ##
                self.name + "ThreeTightMuonsEventSelection",
                ##Algorithm = LoKi__VoidFilter (
                ##self.name + "ThreeTightMuons"  ,
                Code = " CONTAINS( '%s' ) > 2.5 " %  ( _tightmu.outputLocation() ) ,
                ##),
                ##
                RequiredSelection  = _tightmu
                )
            
        ## helper selection, essentially it is just the cloner of 'tight' muons 
        _ThreeTightMuons = FilterDesktop (
            Code = ' ALL '
            )
        
        ## make the final selection
        self.ThreeTightMuons_Selection = Selection (
            ##
            self.name + "ThreeTightMuonsForTriMuons"             ,
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
            ##self.name + "CombineTau2ThreeMuons" ,
            ##
            DecayDescriptor  = " [ tau+ -> mu+ mu+ mu- ]cc" ,
            ##
            Preambulo        = self.__confdict__['Preambulo'] , 
            ## 
            ## 
            CombinationCut  = """
            ( ADAMASS('tau+') < 400 * MeV ) 
            """ , ## wide mass-combination + PT-cuts 
            ##
            MotherCut       = """
            ( chi2vx < 20 ) &
            ( ctau   > 100 * micrometer ) &
            ( BPVIPCHI2() < 225 )
            """ 
            )
        
        ## make selections
        self.Tau2ThreeMu_Selection  = Selection (
            ##
            self.name + "SelTau2ThreeMu" ,
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
            ##self.name + "CombineBc2ThreeMuons" ,
            ##
            DecayDescriptor = " [B_c+ -> mu+ mu+ mu- ]cc" ,
            ##
            Preambulo        = self.__confdict__['Preambulo'] + [
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
            self.name + "SelBc2ThreeMu" ,
            ##
            Algorithm          = _Bc2ThreeMu  ,
            ##
            RequiredSelections = [ self.threeGoodMuonsForBc() ] 
            )
        
        return self.Bc2ThreeMu_Selection

    

# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author :  %s' % __author__
    print ' Date   :  %s' % __date__
    print 80*'*'
        
# =============================================================================
# The END 
# =============================================================================
