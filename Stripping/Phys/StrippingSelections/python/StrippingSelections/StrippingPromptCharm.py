#!/usr/bin/env python
# =============================================================================
# $Id$
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
#  Also one has the combinations for 2xCharm studies 
#
#    - Charm +   Charm            ( both Charm/Charm and Charm/anti-Charm )
#    - Charm +   Dimuon
#    - Charm + ( Dimuon + gamma ) ( get for free chi_(c,b) )
#
#  Excited charm baryons for Biagio Saitta:
#
#    - Sigma_c(0,++) -> Lambda_c+ pi        ( 2455 & 2520 )
#    - Lambda_c+*    -> Lambda_c+ pi+ pi-   ( 2595, 2625 & 2880 )
#
#  The cuts more or less correspond to D*+ selection by Alexandr Kozlinzkiy.
#  In addition the PT-cut for the long-lived charmed particle has been applied.
#  Thanks to Marco Gersabeck & Harry Cliff for nice idea.
#
# Usage:
#
# @code
#
#   stream = ...
#
#   from StrippingSelections.StirppingPromptCharm import StrippingPromptCharmConf
#
#   conf = {}
#
#   promtCharm = StrippingPromptCharmConf ( 'PromptCharm' , config = conf ) 
#
#   stream.appendLines ( promptCharm.lines() )
#
# @endcode 
#
# @todo Increment PT-threshold for D0,D+,D_s+, Lambda_c+
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
# =============================================================================
"""

The attempt for coherent stripping of all stable charm hadrons

    - D*+       -> ( D0 -> K pi , K K , pi pi, pi K ) pi+
    - D0        -> K pi , K K                       
    - Ds+/D+    -> ( phi -> K K ) pi                
    - D+        -> K pi pi                          
    - Lambda_c+ -> p K pi                           
    
  Also one has combinations:
    - Charm + Charm  (both Charm/Charm and Charm/anti-Charm)
    - Charm + DiMuon
    
   The cuts more or less correspond to D*+ selection by Alexandr Kozlinzkiy
   In addition the PT-cut for the long-lived charmed particle has been applied
   Thanks to Marco Gersabeck & Harry Cliff for nice idea.

  Usage:
 

    >>> stream = ...

    >>> from StrippingSelections.StirppingPromptCharm import StrippingPromptCharmConf

    >>> conf = {}

    >>> promptCharm = StrippingPromptCharmConf ( 'PromptCharm' , config = conf ) 
    
    >>> stream.appendLines ( promptCharm.lines() )


                  $Revision$
Last modification $Date$
               by $Author$

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__date__    = '2010-08-03'
__version__ = '$Revision$'
# =============================================================================
__all__ = (
    'StrippingPromptCharmConf',
    'default_config'
    )
# =============================================================================

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits             import GeV, MeV, mm
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import   Selection, MergedSelection 
from StandardParticles      import ( StdNoPIDsPions     ,
                                     StdLoosePions      ,
                                     StdLooseKaons      ,
                                     StdLooseProtons    , 
                                     ## for onia, prompt
                                     StdAllLooseMuons   ,
                                     ## for soft pion from D*+, Sigma_c, Lambda_c*
                                     StdAllNoPIDsPions  ,
                                     ## for chi_(c,b)
                                     StdLooseAllPhotons )

                                     
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
# =============================================================================
## logging
# =============================================================================
import logging
logger = logging.getLogger(__name__)
if not logger.handlers :
    logging.basicConfig()
logger.setLevel(logging.INFO)
# =============================================================================
## Define the default configuration 
_default_configuration_ = {
    #
    # Selection of basic particles 
    #
    'TrackCuts'       : ' ( TRCHI2DOF < 5 ) & ( PT > 250 * MeV ) '          , 
    'BasicCuts'       : ' & ( 9 < MIPCHI2DV()   ) '                         , 
    'KaonCuts'        : ' & ( 2 < PIDK  - PIDpi ) '                         , 
    'PionCuts'        : ' & ( 2 < PIDpi - PIDK  ) '                         , 
    'ProtonCuts'      : ' & ( 2 < PIDp  - PIDpi ) & ( 2 < PIDp - PIDK ) '   , 
    'SlowPionCuts'    : ' TRCHI2DOF < 5   '                                 ,
    'MuonCuts'        : ' ISMUON & ( PT > 650 * MeV ) & ( TRCHI2DOF < 5 ) ' , 
    #
    # Global Event cuts 
    #
    'PrimaryVertices' : True , 
    #
    # Technicalities:
    #
    'Preambulo'       : [
    # the D0 decay channels
    "pipi   = DECTREE ('[D0]cc -> pi- pi+   ') " ,
    "kk     = DECTREE ('[D0]cc -> K-  K+    ') " ,
    "kpi    = DECTREE ('[D0    -> K-  pi+]CC') " ,
    # number of kaons in final state (as CombinationCuts)
    "ak2    = 2 == ANUM( 'K+' == ABSID ) "       ,
    # shortcut for chi2 of vertex fit 
    'chi2vx = VFASPF(VCHI2) '                    , 
    # shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light" , 
    "ctau   = BPVLTIME ( 9 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<9
    # dimons:
    "psi           =   ADAMASS ('J/psi(1S)') < 150 * MeV"  ,
    "psi_prime     =   ADAMASS (  'psi(2S)') < 150 * MeV"  ,
    ] ,
    ## monitoring ?
    'Monitor'     : False ,
    ## pescales 
    'D0Prescale'             : 1.0 ,
    'DstarPrescale'          : 1.0 ,
    'DsPrescale'             : 1.0 ,
    'DplusPrescale'          : 1.0 ,
    'LamCPrescale'           : 1.0 ,
    'LamCstarPrescale'       : 1.0 ,
    'SigCPrescale'           : 1.0 ,
    'DiCharmPrescale'        : 1.0 , 
    'DiMuonAndCharmPrescale' : 1.0 , 
    'ChiAndCharmPrescale'    : 1.0 , 
    'DsPsiPrescale'          : 1.0 ,
    }

## ============================================================================
## @class  StrippingPromptCharmConf
#  Helper class required by Tom & Greig 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-09-26
class StrippingPromptCharmConf(LineBuilder) :
    """
    Helper class to confiugure 'PromptCharm'-lines
    """
    __configuration_keys__ = tuple ( _default_configuration_.keys() )
    
    
    ## get the default configuration 
    @staticmethod
    def defaultConfiguration( key = None ) :
        """
        Get the defualt configurtaion

        >>> conf = StrippingPromptCharmConf.defaultConfiguration()


        Get the elements of default configurtaion:
        
        >>> d0prescale = StrippingPromptCharmConf.defaultConfiguration( 'D0Prescale' )
        """
        from copy import deepcopy
        _config = deepcopy ( _default_configuration_ )
        if key : return _config[ key ]
        return _config
    
    ## constructor
    def __init__ ( self , name , config ) :
        """
        Constructor
        """
        # check the names 
        if not name : name = 'PromptCharm'
        # check the names 
        if 'PromptCharm' != name :
            logger.warning ( 'The non-default name is specified "%s"' % name  ) 

        from copy import deepcopy
        _config = deepcopy ( _default_configuration_ )

        if isinstance ( config , dict ):
            _config.update ( config )
            LineBuilder.__init__( self , name , _config )
        else :
            LineBuilder.__init__( self , name ,  config )

        ## private set of selections 
        self.__selections_ = {}

        LineBuilder.__init__( self , name , _config )

        if not self.__selections_.has_key ( self.name() ) :
            self.__selections_[ self.name() ] = {}
            
        self.__selections_[ self.name() ]['CONFIG'] = deepcopy ( _config ) 
        
        keys = _config.keys()
        for key in keys :
            
            if not key in _default_configuration_ :
                raise KeyError("Invalid key is specified: '%s'" % key )
            
            val = _config[key]
            if val != _default_configuration_ [ key ] : 
                logger.warning ('StrippingPromptCharm: new configuration: %-16s : %s ' % ( key , _config[key] ) )

        self._name         = name

        self._trackcuts    =                   _config.pop ( 'TrackCuts'    , _default_configuration_ [ 'TrackCuts'    ] )
        self._basiccuts    = self._trackcuts + _config.pop ( 'BasicCuts'    , _default_configuration_ [ 'BasicCuts'    ] )
        self._kaoncuts     = self._basiccuts + _config.pop ( 'KaonCuts'     , _default_configuration_ [ 'KaonCuts'     ] )    
        self._pioncuts     = self._basiccuts + _config.pop ( 'PionCuts'     , _default_configuration_ [ 'PionCuts'     ] )
        self._protoncuts   = self._basiccuts + _config.pop ( 'ProtonCuts'   , _default_configuration_ [ 'ProtonCuts'   ] )
        self._slowpioncuts =                   _config.pop ( 'SlowPionCuts' , _default_configuration_ [ 'SlowPionCuts' ] )
        self._muoncuts     =                   _config.pop ( 'MuonCuts'     , _default_configuration_ [ 'MuonCuts'   ] )
        
        self._checkPV      = _config.pop ( 'PrimaryVertices' , _default_configuration_ [ 'PrimaryVertices' ] )

        self.D0Prescale             = _config.pop ( 'D0Prescale'             , _default_configuration_ [ 'D0Prescale'             ] ) 
        self.DstarPrescale          = _config.pop ( 'DstarPrescale'          , _default_configuration_ [ 'DstarPrescale'          ] )  
        self.DsPrescale             = _config.pop ( 'DsPrescale'             , _default_configuration_ [ 'DsPrescale'             ] ) 
        self.DplusPrescale          = _config.pop ( 'DplusPrescale'          , _default_configuration_ [ 'DplusPrescale'          ] ) 
        self.LamCPrescale           = _config.pop ( 'LamCPrescale'           , _default_configuration_ [ 'LamCPrescale'           ] ) 
        self.LamCstarPrescale       = _config.pop ( 'LamCstarPrescale'       , _default_configuration_ [ 'LamCstarPrescale'       ] ) 
        self.SigCPrescale           = _config.pop ( 'SigCPrescale'           , _default_configuration_ [ 'SigCPrescale'           ] ) 
        self.DiCharmPrescale        = _config.pop ( 'DiCharmPrescale'        , _default_configuration_ [ 'DiCharmPrescale'        ] ) 
        self.DiMuonAndCharmPrescale = _config.pop ( 'DiMuonAndCharmPrescale' , _default_configuration_ [ 'DiMuonAndCharmPrescale' ] )
        self.ChiAndCharmPrescale    = _config.pop ( 'ChiAndCharmPrescale'    , _default_configuration_ [ 'ChiAndCharmPrescale'    ] )
        self.DsPsiPrescale          = _config.pop ( 'DsPsiPrescale'          , _default_configuration_ [ 'DsPsiPrescale'          ] )

            
        self._Preambulo    = _config.pop ( 'Preambulo'       , _default_configuration_ [ 'Preambulo'       ] )
        self._monitor      = _config.pop ( 'Monitor'         , _default_configuration_ [ 'Monitor'         ] )
        
        if _config :
            raise KeyError ( 'Invalid keys are specified for configuration: %s ' % _config.keys() )
        
        if 1.0 != self.D0Prescale             :
            logger.warning ( 'D0              prescale is %s' % self.D0Prescale      )
        if 1.0 != self.DstarPrescale          :
            logger.warning ( 'D*+             prescale is %s' % self.DstarPrescale   )
        if 1.0 != self.DplusPrescale          :
            logger.warning ( 'D+              prescale is %s' % self.DplusPrescale   )
        if 1.0 != self.DsPrescale             :
            logger.warning ( 'D_s+            prescale is %s' % self.DsPrescale      )
        if 1.0 != self.LamCPrescale           :
            logger.warning ( 'Lambda_c+       prescale is %s' % self.LamCPrescale    )
        if 1.0 != self.SigCPrescale           :
            logger.warning ( 'Sigma_c+        prescale is %s' % self.SigCPrescale    )
        if 1.0 != self.LamCstarPrescale       :
            logger.warning ( 'Lambda_c*+      prescale is %s' % self.LamCstarPrescale)
        if 1.0 != self.DiCharmPrescale        :
            logger.warning ( '2xCharm         prescale is %s' % self.DiCharmPrescale )
        if 1.0 != self.DiMuonAndCharmPrescale :
            logger.warning ( 'DiMuon&Charm    prescale is %s' % self.DiMuonAndCharmPrescale )
        if 1.0 != self.ChiAndCharmPrescale    :
            logger.warning ( 'Chi_(c,b)&Charm prescale is %s' % self.ChiAndCharmPrescale    )
        if 1.0 != self.DsPsiPrescale          :
            logger.warning ( 'D_s+&psi        prescale is %s' % self.DsPsiPrescale    )
            
        for line in self._lines_charm() :
            self.registerLine(line)
            logger.info ( "Register line: %s" %  line.name () ) 
            
    ## get the selections
    def _selections_private ( self ) :
                
        sel = self._selections ( 'Selections' )
        if sel : return sel
        
        sel =  [ self.D02HH          () ,
                 self.Dstar          () ,
                 self.Ds             () ,
                 self.Dplus          () ,
                 self.LamC           () ,
                 self.SigC           () ,
                 self.LamCstar       () ,
                 self.DiMuon         () ,
                 self.DiCharm        () ,
                 self.DiMuonAndCharm () , 
                 self.ChiAndCharm    () ,
                 self.preDs          () ,
                 self.DsPsi          () 
                 ]
        
        return self._add_selection ( 'Selections' , sel )  

    ## get the selection, associated with some nickname name 
    def _selection ( self , nick ) :
        """
        Get the selection, associated with some nickname name
        """
        
        if not self.__selections_.has_key ( self.name() ) :
            self.__selections_[ self.name() ] = {} 
            
        return self.__selections_[ self.name() ].get( nick , None ) 
    
    ## add the selection, associated with some nickname name 
    def _add_selection ( self , nick , sel ) :
        """
        add the selection, associated with some nickname name
        """
        if not self.__selections_.has_key ( self.name() ) : self.__selections_[ self.name() ] = {} 
        
        if self.__selections_[ self.name()].has_key( nick ) :
            raise AttributeError , "Selection '%s'already exists " % nick
        
        self.__selections_[ self.name() ][ nick ] = sel
        
        return sel 
    

    ## get all single charm lines 
    def _lines_charm   ( self ) :
        """
        Get all (single) charm lines 
        """
        sel = self._selection ( 'CharmLines' )
        if sel : return sel
        #
        sel = [
            ##
            StrippingLine (
            "D02HHFor" + self._name ,
            prescale = self.D0Prescale    , ## ATTENTION! Prescale here !!
            checkPV  = self._checkPV      ,
            algos    = [ self.D02HH() ]
            ) ,
            ## 
            StrippingLine (
            "DstarFor" + self._name ,
            prescale = self.DstarPrescale , ## ATTENTION! Prescale here !!
            checkPV  = self._checkPV      ,
            algos    = [ self.Dstar() ]
            ) ,
            ##
            StrippingLine (
            "DsFor" + self._name ,
            prescale = self.DsPrescale    , ## ATTENTION! Prescale here !!
            checkPV  = self._checkPV      ,
            algos    = [ self.Ds()    ]
            ) ,
            ##
            StrippingLine (
            "DFor" + self._name ,
            prescale = self.DplusPrescale , ## ATTENTION! Prescale here !!
            checkPV  = self._checkPV      ,
            algos    = [ self.Dplus () ]
            ) ,
            ##
            StrippingLine (
            "LambdaCFor" + self._name ,
            prescale = self.LamCPrescale  , ## ATTENTION! Prescale here !!
            checkPV  = self._checkPV      ,
            algos    = [ self.LamC () ]
            ) ,
            ## Sigma_c 
            StrippingLine (
            "SigmaCFor" + self._name ,
            prescale = self.SigCPrescale  , ## ATTENTION! Prescale here !!
            checkPV  = self._checkPV      ,
            algos    = [ self.SigC () ]
            ) ,
            ## Lambda_c* 
            StrippingLine (
            "LambdaCstarFor" + self._name ,
            prescale = self.LamCstarPrescale  , ## ATTENTION! Prescale here !!
            checkPV  = self._checkPV      ,
            algos    = [ self.LamCstar () ]
            ) ,
            ##
            StrippingLine (
            "DiCharmFor" + self._name ,
            prescale = self.DiCharmPrescale  , ## ATTENTION! Prescale here !!              
            checkPV  = self._checkPV         ,
            algos    = [ self.DiCharm () ]            
            ) ,
            ## 
            StrippingLine (
            "DiMuonAndCharmFor" + self._name ,
            prescale = self.DiMuonAndCharmPrescale  , ## ATTENTION! Prescale here !!              
            checkPV  = self._checkPV         ,
            algos    = [ self.DiMuonAndCharm () ]            
            ) ,
            ##
            StrippingLine (
            "ChiAndCharmFor" + self._name ,
            prescale = self.ChiAndCharmPrescale  , ## ATTENTION! Prescale here !!              
            checkPV  = self._checkPV         ,
            algos    = [ self.ChiAndCharm () ]            
            ) ,
            ## 
            StrippingLine (
            "DsPsiFor" + self._name ,
            prescale = self.DsPsiPrescale  , ## ATTENTION! Prescale here !!              
            checkPV  = self._checkPV         ,
            algos    = [ self.DsPsi () ]            
            ) ,
            ## 
            ]
        #
        return self._add_selection ( 'CharmLines' , sel ) 
    
    
    ## get all stripping lines 
    def _lines_private ( self ) :
        """
        get Charm stripping lines
        """
        return self._lines_charm () 
            
    def trackCuts    ( self ) : return self._trackcuts 
    def basicCuts    ( self ) : return self._basiccuts
    def kaonCuts     ( self ) : return self._kaoncuts 
    def pionCuts     ( self ) : return self._pioncuts 
    def protonCuts   ( self ) : return self._protoncuts 
    def slowPionCuts ( self ) : return self._slowpioncuts 
    def muonCuts     ( self ) : return self._muoncuts 

    ## get the selection of kaons 
    def kaons ( self ) :
        """
        Get the kaons 
        """
        sel = self._selection ( 'KaonSelection')
        if sel : return sel
        
        _KaonFilter = FilterDesktop ( Code = " ( 'K+'  == ABSID ) & ( %s ) " % self.kaonCuts() )
        #
        sel = Selection (
            'SelKaonsFor'      + self.name() ,
            Algorithm          = _KaonFilter ,
            RequiredSelections = [ StdLooseKaons ]
            )
        
        return self._add_selection ( 'KaonSelection' , sel )
    
    ## get the pion selection
    def pions ( self ) :
        """
        Get the pions 
        """
        sel = self._selection ( 'PionSelection')
        if sel : return sel
        
        _PionFilter = FilterDesktop ( Code = " ( 'pi+' == ABSID ) & ( %s ) " % self.pionCuts() )
        #
        sel = Selection (
            'SelPionsFor'      + self.name() ,
            Algorithm          = _PionFilter ,
            RequiredSelections = [ StdLoosePions ]
            )
        #
        return self._add_selection ( 'PionSelection' , sel )
    
    
    ## get the proton selection
    def protons ( self ) :
        """
        Get the proton selection
        """
        sel = self._selection ( 'ProtonSelection')
        if sel : return sel
        
        _ProtonFilter = FilterDesktop ( Code = " ( 'p+'  == ABSID ) & ( %s ) " % self.protonCuts() )
        
        sel = Selection (
            'SelProtonsFor' + self._name ,
            Algorithm          = _ProtonFilter ,
            RequiredSelections = [ StdLooseProtons ]
            )
        #
        return self._add_selection ( 'ProtonSelection' , sel )
    
    ## get the meson selection 
    def mesons ( self ) :
        """
        Get the meson ( kaon + pion ) selection
        """
        sel = self._selection ( 'MesonSelection')
        if sel : return sel
        
        #
        sel = MergedSelection (
            'SelBasicMesonsFor' + self._name , 
            RequiredSelections = [ self.pions() , self.kaons() ]
            )
        #
        return self._add_selection ( 'MesonSelection' , sel )
    
    ## get the common preambulo: 
    def preambulo ( self ) : return self._Preambulo
    
    ## get the preselection of D0->hh
    def preD02HH ( self ) :
        """
        Get the preselection of D0->hh
        """
        sel = self._selection ( 'D02HHForPromptCharm_PreSelection' ) 
        if sel : return sel
        
        ## prepare D0 for D0 and D*+ 
        _D0PreCombine = CombineParticles (
            ## the decays to be reconstructed 
            DecayDescriptors = [
            " D0  -> pi+ pi-    " ,
            " D0  -> K+  K-     " ,
            "[D0  -> K-  pi+]cc " 
            ] ,
            ## 
            Preambulo      = self.preambulo() , 
            ## combination cut : wide mass-cut & PT-cut 
            CombinationCut = """
            ( ADAMASS('D0') < 85 * MeV ) &
            ( APT > 1750 * MeV         )
            """ , 
            ## mother cut
            MotherCut      = """ 
            ( chi2vx < 9 )              &
            ( PT     > 2.0 * GeV      ) & 
            ( ADMASS('D0') < 75 * MeV ) &
            ( abs ( LV01 ) < 0.9      ) &
            ( ctau > 100 * micrometer )
            """
            )
        #
        ## make (pre)selection
        sel = Selection (
            ##
            'PreSelD02HHFor' + self._name ,
            ##
            Algorithm          = _D0PreCombine   ,
            ##
            RequiredSelections = [ self.mesons() ] 
            )
        
        return self._add_selection ( 'D02HHForPromptCharm_PreSelection' , sel )
    
    # =============================================================================
    # D0 -> Kpi, KK selection
    # =============================================================================
    def D02HH ( self ) :
        """
        Get the selection of D0 -> HH 
        """
        sel = self._selection ( 'D02HHForPromptCharm_Selection' ) 
        if sel : return sel

        ##
        _D0Filter = FilterDesktop (
            ##
            Monitor      = self._monitor  ,
            HistoProduce = self._monitor  ,
            ##
            Preambulo = self.preambulo() + [
            "hKpi = Gaudi.Histo1DDef ( 'mass K pi' , 1800 , 1920 , 60 )" ,
            "hKK  = Gaudi.Histo1DDef ( 'mass K K'  , 1800 , 1920 , 60 )" 
            ] ,
            ##
            Code      = """
            ( PT             > 2.0 * GeV  ) & 
            ( ADMASS( 'D0' ) < 75  * MeV  ) &
            ( kk | kpi )                    &
            ( ctau > 100 * micrometer )
            """ ,
            ##
            PostMonitor  = """
            process ( switch ( kpi , monitor ( M , hKpi , 'mass Kpi' ) , monitor ( M , hKK  , 'mass KK'  ) ) ) >> ~EMPTY 
            """
            )
        
        ## make selection 
        sel = Selection (
            'SelD02HHFor' + self._name ,
            Algorithm          = _D0Filter          ,
            RequiredSelections = [ self.preD02HH() ]
            )
        
        return self._add_selection ( 'D02HHForPromptCharm_Selection' , sel )
    
    
    # =============================================================================
    # D*+ -> (D0 -> Kpi, piK, KK, pipi) pi+ selection 
    # =============================================================================
    def Dstar ( self ) :
        """
        D*+ -> (D0 -> Kpi, piK, KK, pipi) pi+ selection
        """
        
        sel = self._selection ( 'DstarForPromptCharm_Selection' )
        if sel : return sel 
        
        _DstarCombine = CombineParticles(
            ##
            Monitor      = self._monitor  ,
            HistoProduce = self._monitor  ,
            ##
            DecayDescriptors = [
            " [D*(2010)+ -> D0 pi+]cc" ,
            " [D*(2010)- -> D0 pi-]cc" 
            ] ,
            ##
            Preambulo = self.preambulo() + [
            "hdm1 = Gaudi.Histo1DDef ( 'delta Mass' , 135 , 160  , 50 )" 
            ] , 
            ##
            DaughtersCuts = {
            'pi+' :  self.slowPionCuts() ,
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
        sel = Selection  (
            ##
            "SelDstarFor" + self._name ,
            ## 
            Algorithm          = _DstarCombine ,
            ##
            ## ATTENITON: we need slow prompt pions!
            RequiredSelections = [ self.preD02HH() ,
                                   StdAllNoPIDsPions ] ## slow prompt pion!
            )
        
        return self._add_selection ( 'DstarForPromptCharm_Selection' , sel ) 
    
    
    # =============================================================================
    # Ds+/D+ -> KKpi   selection  (all!!! without ctau & IP cuts!)
    # =============================================================================
    def preDs ( self ) :
        """
        ``Preselection'' for Ds+
        """
        sel = self._selection ( 'PreDsForPromptCharm_Selection' )
        if sel : return sel 
        
        _DsCombine = CombineParticles(
            ## 
            Monitor      = self._monitor  ,
            HistoProduce = self._monitor  ,
            ##
            DecayDescriptors = [
            " [D_s+ -> K-  K+  pi+ ]cc" , ## through phi !!!
            ] ,
            ##
            Preambulo      = self.preambulo()  + [
            "aphi  = AM12              < 1050 * MeV " , ## phi-mass window 
            "admD  = ADAMASS ('D+'  )  <   85 * MeV " , ## D+  mass window for combination cut 
            "admDs = ADAMASS ('D_s+')  <   85 * MeV " , ## Ds+ mass window for combination cut 
            "dmD   = ADMASS  ('D+'  )  <   75 * MeV " , ## D+  mass window 
            "dmDs  = ADMASS  ('D_s+')  <   75 * MeV " , ## Ds+ mass window
            ##
            "hKKpi = Gaudi.Histo1DDef ( ' mass K K pi' , 1800 , 2020 , 110 )" ,
            ] ,
            ##                                 phi                         
            CombinationCut = """
            aphi                 &
            ( APT > 950 * MeV ) & 
            ( admD | admDs  ) 
            """ ,
            ##
            # ATTENTION: there is neither c*tau nor pointing cuts here!
            MotherCut      = """
            ( chi2vx  < 25            ) &
            ( PT      > 1.0 * GeV     ) & 
            ( dmD | dmDs              ) 
            """ , 
            ##
            MotherMonitor  = """
            process ( monitor ( M , hKKpi , 'mass KKpi' )  ) >> ~EMPTY 
            """
            )
        
        ## convert it to selection
        sel = Selection (
            "SelPreDsFor" + self._name ,
            Algorithm          = _DsCombine ,
            RequiredSelections = [ self.mesons() ]
            )
        
        return self._add_selection ( 'PreDsForPromptCharm_Selection' , sel ) 
    
    # =============================================================================
    # Ds+/D+ -> KKpi   selection 
    # =============================================================================
    def Ds ( self ) :
        
        sel = self._selection ( 'DsForPromptCharm_Selection' )
        if sel : return sel 
        
        _DsFilter = FilterDesktop ( 
            Code = """
            ( PT   > 2.0 * GeV        ) & 
            ( ctau > 100 * micrometer )  
            """ 
            )
        
        ## convert it to selection
        sel = Selection (
            "SelDsFor" + self._name ,
            Algorithm          =    _DsFilter ,
            RequiredSelections = [ self.preDs()  ]
            )
        
        return self._add_selection ( 'DsForPromptCharm_Selection' , sel ) 
    
    
    # =============================================================================
    # D+ -> Kpipi   selection 
    # =============================================================================
    def Dplus ( self ) :
        
        sel = self._selection ( 'DForPromptCharm_Selection' )
        if sel : return sel 
        
        _DCombine = CombineParticles(
            ## 
            Monitor      = self._monitor  ,
            HistoProduce = self._monitor  ,
            ##
            DecayDescriptors = [
            " [D+ -> K-  pi+  pi+ ]cc" , 
            ] ,
            ##
            Preambulo      = self.preambulo() + [
            "hKpipi = Gaudi.Histo1DDef ( 'mass K pi pi' , 1800 , 1920 , 60 )" 
            ] , 
            ##                                 
            CombinationCut = """
            ( ADAMASS('D+') < 65 * MeV ) &
            ( APT > 1.75 * GeV )
            """ , 
            MotherCut      = """
            ( chi2vx  < 25                   ) &
            ( PT      > 2.0 * GeV            ) & 
            ( ADMASS  ('D+'  )  <   50 * MeV ) & 
            ( ctau    > 100 * micrometer )
            """ , 
            MotherMonitor  = """
            process ( monitor ( M , hKpipi , 'mass K pi pi' ) )  >> ~EMPTY 
            """
            )
        
        # convert it to selection
        sel = Selection (
            "SelDFor" + self._name ,
            Algorithm          = _DCombine ,
            RequiredSelections = [ self.mesons() ]
            )
        
        return self._add_selection ( 'DForPromptCharm_Selection' , sel ) 
        
    
    # =============================================================================
    # Lambda_C -> ( pKpi )  selection 
    # =============================================================================
    def LamC ( self ) :
        
        sel = self._selection ( 'LambdaCForPromptCharm_Selection' )
        if sel : return sel 
        
        _LambdaC_Combine = CombineParticles(
            ## 
            Monitor      = self._monitor  ,
            HistoProduce = self._monitor  ,
            ##
            DecayDescriptors = [
            " [ Lambda_c+ -> p+  K-  pi+ ]cc" ,
            ] ,
            ##
            Preambulo = self.preambulo() + [ 
            "hpKpi = Gaudi.Histo1DDef ( 'mass p K pi' , 2230 , 2350 , 60 )" 
            ] , 
            ##
            CombinationCut = """
            ( ADAMASS('Lambda_c+') < 65 * MeV ) &
            ( APT > 1.75 * MeV ) 
            """ ,
            ##
            MotherCut      = """
            ( chi2vx  < 25 )                   &
            ( PT      > 2.0 * GeV            ) & 
            ( ADMASS('Lambda_c+') < 50 * MeV ) &
            ( ctau    > 100 * micrometer )
            """ ,
            ##
            MotherMonitor  = """
            process ( monitor ( M , hpKpi , 'mass p K pi ' ) ) >> ~EMPTY 
            """ 
            )
        
        # convert it to selection
        sel = Selection (
            "SelLambdaCFor" + self._name ,
            Algorithm          = _LambdaC_Combine ,
            RequiredSelections = [ self.mesons  () ,
                                   self.protons () ]  
            )
        
        return self._add_selection( 'LambdaCForPromptCharm_Selection' ,  sel )

    
    # =============================================================================
    # Sigma_C -> Lambda_C pi selection 
    # =============================================================================
    def SigC ( self ) :
        
        sel = self._selection ( 'SigmaCForPromptCharm_Selection' )
        if sel : return sel 
        
        _SigmaC_Combine = CombineParticles(
            ## 
            Monitor      = self._monitor  ,
            HistoProduce = self._monitor  ,
            ##
            DecayDescriptors = [
            " [ Sigma_c0  -> Lambda_c+ pi- ]cc" ,
            " [ Sigma_c++ -> Lambda_c+ pi+ ]cc" ,
            ] ,
            ##
            DaughtersCuts = {
            'pi+' :  self.slowPionCuts() 
            } ,
            ##
            Preambulo = self.preambulo() + [ 
            "hDmSigC = Gaudi.Histo1DDef ( 'dm(Sigma_c)' , 100 , 400 , 150 )" 
            ] , 
            ##
            CombinationCut = """
            ( AM - AM1 < 400 * MeV ) & ( APT > 2.85 * MeV ) 
            """ ,
            ##
            MotherCut      = """
            ( chi2vx  < 16 ) &  ( PT > 3.0 * GeV )  
            """ ,
            ##
            MotherMonitor  = """
            process ( monitor ( M-M1 , hDmSigC , 'dm(Sigma_c)' ) ) >> ~EMPTY 
            """ 
            )
        
        # convert it to selection
        sel = Selection (
            "SelSigmaCFor" + self._name ,
            Algorithm          = _SigmaC_Combine ,
            RequiredSelections = [ self.LamC    () ,
                                   StdAllNoPIDsPions ] ## slow prompt pion!
            )
        
        return self._add_selection( 'SigmaCForPromptCharm_Selection' ,  sel )
    
    
    # =============================================================================
    # Lambda_C* -> Lambda_C pi pi selection 
    # =============================================================================
    def LamCstar ( self ) :
        
        sel = self._selection ( 'LambdaCstarForPromptCharm_Selection' )
        if sel : return sel 
        
        _LambdaCstar_Combine = CombineParticles(
            ## 
            Monitor      = self._monitor  ,
            HistoProduce = self._monitor  ,
            ##
            DecayDescriptors = [
            " [ Lambda_c(2625)+ -> Lambda_c+ pi+ pi-]cc"
            ] ,
            ##
            DaughtersCuts = {
            'pi+' :  self.slowPionCuts() 
            } ,
            ##
            Preambulo = self.preambulo() + [ 
            "hDmLamC = Gaudi.Histo1DDef ( 'dm(Lambda_c)' , 200 , 700 , 200 )" 
            ] , 
            ##
            CombinationCut = """
            ( APT > 2.85 * MeV ) & ( AM - AM1 < 650 * MeV) 
            """ ,
            ## ( ( AM - AM1 < 400 * MeV ) | in_range( 550 * MeV , AM - AM1 , 650 * MeV ) ) 
            ##
            MotherCut      = """
            ( chi2vx  < 25 ) &  ( PT > 3.0 * GeV )  
            """ ,
            ##
            MotherMonitor  = """
            process ( monitor ( M-M1 , hDmLamC , 'dm(Lambda_c)' ) ) >> ~EMPTY 
            """ 
            )
        
        # convert it to selection
        sel = Selection (
            "SelLambdaCstarFor" + self._name ,
            Algorithm          = _LambdaCstar_Combine ,
            RequiredSelections = [ self.LamC    () ,
                                   StdAllNoPIDsPions ] ## slow prompt pion!
            )
        
        return self._add_selection( 'SigmaCstarForPromptCharm_Selection' ,  sel )
    

    ## helper merged selection of all charmed particles 
    def PromptCharm ( self ) :
        """
        Helper merged selection of all charmed particles 
        """        
        sel = self._selection ( 'PromptCharm_Selection' )
        if sel : return sel 
        
        sel =  MergedSelection (
            ##
            'PromptCharmFor' + self._name , 
            RequiredSelections = [ self.D02HH () ,
                                   self.Dstar () ,
                                   self.Ds    () ,
                                   self.Dplus () ,
                                   self.LamC  () ]
            )
        
        return self._add_selection ( 'PromptCharm_Selection' , sel ) 
    
    ## get "Di-Charm"-selection
    def DiCharm ( self ) :
        """
        Di-Charm selection
        """
        sel = self._selection ( 'DiCharm_Selection' )
        if sel : return sel
        
        ## prepare Di-charm
        _Combine = CombineParticles (
            ## the decays to be reconstructed 
            DecayDescriptors = [
            #
            # charm-anti-charm
            #
            "   psi(3770) -> D0        D~0            "  ,
            " [ psi(3770) -> D0        D*(2010)-  ]cc "  ,
            " [ psi(3770) -> D0        D-         ]cc "  ,
            " [ psi(3770) -> D0        D_s-       ]cc "  ,
            " [ psi(3770) -> D0        Lambda_c~- ]cc "  ,
            #
            "   psi(3770) -> D*(2010)+ D*(2010)-      "  ,
            " [ psi(3770) -> D*(2010)+ D-         ]cc "  ,
            " [ psi(3770) -> D*(2010)+ D_s-       ]cc "  ,
            " [ psi(3770) -> D*(2010)+ Lambda_c~- ]cc "  ,
            #
            "   psi(3770) -> D+        D-             "  ,
            " [ psi(3770) -> D+        D_s-       ]cc "  ,
            " [ psi(3770) -> D+        Lambda_c~- ]cc "  ,
            #
            "   psi(3770) -> D_s+      D_s-           "  ,
            " [ psi(3770) -> D_s+      Lambda_c~- ]cc "  ,
            #
            "   psi(3770) -> Lambda_c+ Lambda_c~-     "  ,
            #
            # double charm
            #
            " [ psi(3770) -> D0        D0         ]cc"  ,
            " [ psi(3770) -> D0        D*(2010)+  ]cc "  ,
            " [ psi(3770) -> D0        D+         ]cc "  ,
            " [ psi(3770) -> D0        D_s+       ]cc "  ,
            " [ psi(3770) -> D0        Lambda_c+  ]cc "  ,
            #
            " [ psi(3770) -> D*(2010)+ D*(2010)+  ]cc "  ,
            " [ psi(3770) -> D*(2010)+ D+         ]cc "  ,
            " [ psi(3770) -> D*(2010)+ D_s+       ]cc "  ,
            " [ psi(3770) -> D*(2010)+ Lambda_c+  ]cc "  ,
            #
            " [ psi(3770) -> D+        D+         ]cc "  ,
            " [ psi(3770) -> D+        D_s+       ]cc "  ,
            " [ psi(3770) -> D+        Lambda_c+  ]cc "  ,
            #
            " [ psi(3770) -> D_s+      D_s+       ]cc "  ,
            " [ psi(3770) -> D_s+      Lambda_c+  ]cc "  ,
            #
            " [ psi(3770) -> Lambda_c+ Lambda_c+  ]cc "
            
            ] ,
            ## combination cut : accept all 
            CombinationCut = " AALL " ,
            ##      mother cut : accept all 
            MotherCut      = "  ALL " 
            )
        
        sel = Selection (
            ##
            'SelDiCharmFor' + self._name ,
            ##
            Algorithm          = _Combine   ,
            ##
            RequiredSelections = [ self.PromptCharm () ]
            )
        
        return self._add_selection ( 'DiCharm_Selection' , sel ) 
    
    ## get the dimuons 
    def DiMuon ( self ) :
        """
        Get the dimuons  
        """
        sel = self._selection ( 'DiMuon_Selection' )
        if sel : return sel 
        
        ## prepare Di-muons
        _Combine = CombineParticles (
            ## the decays to be reconstructed 
            DecayDescriptor = 'J/psi(1S) -> mu+ mu-' ,
            DaughtersCuts   = {
            'mu+' : self.muonCuts() 
            } , 
            Preambulo       = self.preambulo() , 
            ## combination cut 
            CombinationCut  = " psi | psi_prime | ( AM > 4.9 * GeV ) " ,
            ##      mother cut 
            MotherCut       = " chi2vx < 20 " 
            )
        
        ##
        sel = Selection (
            ##
            'SelDiMuonFor' + self._name ,
            ##
            Algorithm          = _Combine   ,
            #
            ## ATTENTION: use 'AllLooseMuons' - we need prompt Onia!
            RequiredSelections = [ StdAllLooseMuons ]
            )
        
        return self._add_selection ( 'DiMuon_Selection' , sel ) 
    
    ## get the dimuons & charn 
    def DiMuonAndCharm ( self ) :
        """
        Get charm & dimuon :
        Select events with at leats one charm particle and
        at least one dimuon 
        """
        sel = self._selection ( 'DiMuonAndCharm_Selection' )
        if sel : return sel 
        
        _Filter = CombineParticles (
            DecayDescriptors = [
            "[ Upsilon(1S) -> J/psi(1S) D0        ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) D*(2010)+ ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) D+        ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) D_s+      ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) Lambda_c+ ]cc" 
            ] ,
            CombinationCut = " AALL " ,
            MotherCut      = "  ALL " 
            )
        
        sel = Selection  (
            "SelDiMuonAndCharmFor" + self._name  ,
            Algorithm = _Filter ,
            RequiredSelections = [
            self.DiMuon      () ,
            self.PromptCharm ()
            ]
            )
        
        return self._add_selection( 'DiMuonAndCharm_Selection' , sel ) 

    ## get the dimuons & charn 
    def ChiAndCharm ( self ) :
        """
        Get charm & chi:
        Select events with at leats one charm particle and
        at least one chi  
        """
        sel = self._selection ( 'ChiAndCharm_Selection' )
        if sel : return sel 
        
        _alg = CombineParticles (
            ##
            DecayDescriptors = [
            "[ Upsilon(1S) -> J/psi(1S) D0        gamma ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) D*(2010)+ gamma ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) D+        gamma ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) D_s+      gamma ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) Lambda_c+ gamma ]cc" 
            ] ,
            ##
            DaughtersCuts = {
            #                    J/psi                              Upsilon(1S) 
            "J/psi(1S)" : " ( M  < 3.21 * GeV ) | in_range ( 9.3 * GeV , M , 9.6 * GeV ) " ,  
            "gamma"     : " ( PT > 500  * MeV ) & ( CL > 0.05 ) " 
            },
            ## require chi_(c,b) 
            CombinationCut = """
            AM13 - AM1 < 1.01 * GeV 
            """,
            ## 
            MotherCut      = "  ALL " 
            )
        ##
        _sel = Selection (
            "ChiAndCharmPre",
            Algorithm          =   _alg ,
            RequiredSelections = [ self.DiMuonAndCharm () , ## fake one!
                                   self.DiMuon         () ,
                                   self.PromptCharm    () ,
                                   StdLooseAllPhotons     ] 
            )
        ## apply pi0-veto-tagger ! 
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger 
        _tag = Pi0Veto__Tagger (
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25010     ## unique ! 
            )
        ## the final selection 
        sel = Selection  (
            "SelChiAndCharmFor" + self._name  ,
            Algorithm          =   _tag  ,
            RequiredSelections = [ _sel ]
            )
        
        return self._add_selection( 'ChiAndCharm_Selection' , sel ) 

    ## get DsPsi 
    def DsPsi( self ) :
        """
        Get Ds and J/psi :
        """
        sel = self._selection ( 'DsPsi_Selection' )
        if sel : return sel 
        
        _DsPsiCombine = CombineParticles (
            ## 
            DecayDescriptor = "[ B_c+ -> J/psi(1S) D_s+ ]cc" ,
            ##
            DaughtersCuts  = {
            "J/psi(1S)" : "M < 4.0 * GeV " ,
            } ,
            ## 
            CombinationCut = """
            AM < 7.1 * GeV 
            """,
            ##
            MotherCut      = """
            ( chi2vx < 16 ) 
            """
            ## MotherCut      = """
            ## ( chi2vx < 16 ) &
            ## ( ctau   > 0  ) 
            ## """
            )
        
        sel = Selection  (
            "SelDsPsiFor" + self._name  ,
            Algorithm = _DsPsiCombine ,
            RequiredSelections = [
            self.DiMuon  () ,
            self.preDs   () 
            ]
            )
        
        return self._add_selection( 'DsPsi_Selection' , sel ) 
    
        
    
default_config = {
    'D0Prescale'             : 1.00 ,
    'DstarPrescale'          : 1.00 ,
    'DsPrescale'             : 1.00 ,
    'DplusPrescale'          : 1.00 ,
    'LamCPrescale'           : 1.00 ,
    'DiCharmPrescale'        : 1.00 ,
    'DiMuonAndCharmPrescale' : 1.00 ,
    }

# =============================================================================
if '__main__' == __name__ :

    print 80*'*'
    print __doc__
    print ' Author :  %s' % __author__
    print ' Date   :  %s' % __date__
    print ' The output locations for default configuration: '
    _conf = StrippingPromptCharmConf ( 'PromptCharm' , config = default_config  ) 
    for l in _conf.lines() :
        print ' \t ', l.outputLocation  () , l 
    print 80*'*'
        
# =============================================================================
# The END 
# =============================================================================

