#!/usr/bin/env python
# =============================================================================
# $Id: StrippingPromptCharm.py 122310 2011-04-20 18:57:46Z ibelyaev $
# =============================================================================
# $URL: svn+ssh://svn.cern.ch/reps/lhcb/Stripping/tags/Phys/StrippingSelections/v3r4p1/python/StrippingSelections/StrippingPromptCharm.py $
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
#     - Gamma ( /\cbar-  -> pKpi  ) - Gamma ( /\c+  -> pKpi  )   
#     - Gamma ( Ds+      -> KKpi  ) - Gamma ( /\c+  -> pKpi  )   
#     - Gamma ( D+       -> KKpi  ) - Gamma ( /\c+  -> pKpi  )   
#     - Gamma ( D+       -> Kpipi ) - Gamma ( /\c+  -> pKpi  )
#
#  The cuts more or less correspond to D*+ selection by Alexandr Kozlinzkiy.
#  In addition the PT-cut for the long-lived charmed particle has been applied.
#  Thanks to Marco Gersabeck & Harry Cliff for nice idea.
#
# The performance with 100k events from Reco05-Stripping09_SDSTs.py:
#
# For default configuration *NO* prescaling is defined, and the performance is: 
# +-------------------------------------+----------+----------+-------+--------+
# | Decision name                       |     Rate | Accepted | Mult. | <T>,ms |
# +-------------------------------------+----------+----------+-------+--------+
# | StrippingSequenceStreamPromptCharm  | 0.003300 |   330    |       | 4.567  |
# +-------------------------------------+----------+----------+-------+--------+
# |    StrippingD02HHForPromptCharm     | 0.001970 |   197    | 1.020 |  3.119 |
# |    StrippingDstarForPromptCharm     | 0.000480 |    48    | 1.208 |  0.372 |
# |    StrippingDsForPromptCharm        | 0.000170 |    17    | 1.000 |  0.165 |
# |    StrippingDForPromptCharm         | 0.000930 |    93    | 1.000 |  0.170 |
# |    StrippingLambdaCForPromptCharm   | 0.000220 |    22    | 1.000 |  0.646 |
# +-----------------------------------------------------------+-------+--------+
#
# +-------------------------------------+----------+----------+-------+--------+
# | Decision name                       |     Rate | Accepted | Mult. | <T>,ms |
# +-------------------------------------+----------+----------+-------+--------+
# | StrippingSequenceStreamPromptCharm  | 0.001590 |   159    |       | 4.097  |
# +-------------------------------------+----------+----------+-------+--------+
# |    StrippingDstarForPromptCharm     | 0.000490 |    49    | 1.204 |  3.093 |
# |  * StrippingD02HHForPromptCharm     | 0.000530 |    53    | 1.000 |  0.040 |
# |    StrippingDsForPromptCharm        | 0.000170 |    17    | 1.000 |  0.151 |
# |  * StrippingDForPromptCharm         | 0.000250 |    25    | 1.000 |  0.064 |
# |    StrippingLambdaCForPromptCharm   | 0.000230 |    23    | 1.000 |  0.675 |
# +-----------------------------------------------------------+-------+--------+
#                                                      Thanks to Anton Poluektov
#    * denotes prescaled lines: prescale = 0.25 
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
#   promtCharm = StrippingPromptCharmConf ( config = conf ) 
#
#   stream.appendLines ( promptCharm.lines() )
#
# @endcode 
# 
# $Revision: 122310 $
# Last modification $Date: 2011-04-20 20:57:46 +0200 (Wed, 20 Apr 2011) $
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
     - Gamma ( /\cbar-  -> pKpi  ) - Gamma ( /\c+  -> pKpi  )   
     - Gamma ( Ds+      -> KKpi  ) - Gamma ( /\c+  -> pKpi  )   
     - Gamma ( D+       -> KKpi  ) - Gamma ( /\c+  -> pKpi  )   
     - Gamma ( D+       -> Kpipi ) - Gamma ( /\c+  -> pKpi  )   

    
The cuts more or less correspond to D*+ selection by Alexandr Kozlinzkiy
In addition the PT-cut for the long-lived charmed particle has been applied
Thanks to Marco Gersabeck & Harry Cliff for nice idea.

  The performance with 100k events from Reco05-Stripping09_SDSTs.py:
 
  For default configuration *NO* prescaling is defined, and the performance is: 
  +-------------------------------------+----------+----------+-------+--------+
  | Decision name                       |     Rate | Accepted | Mult. | <T>,ms |
  +-------------------------------------+----------+----------+-------+--------+
  | StrippingSequenceStreamPromptCharm  | 0.003300 |   330    |       | 4.567  |
  +-------------------------------------+----------+----------+-------+--------+
  |    StrippingD02HHForPromptCharm     | 0.1970 |   197    | 1.020 |  3.119 |
  |    StrippingDstarForPromptCharm     | 0.0480 |    48    | 1.208 |  0.372 |
  |    StrippingDsForPromptCharm        | 0.0170 |    17    | 1.000 |  0.165 |
  |    StrippingDForPromptCharm         | 0.0930 |    93    | 1.000 |  0.170 |
  |    StrippingLambdaCForPromptCharm   | 0.0220 |    22    | 1.000 |  0.646 |
  +-----------------------------------------------------------+-------+--------+
 
  +-------------------------------------+----------+----------+-------+--------+
  | Decision name                       |     Rate | Accepted | Mult. | <T>,ms |
  +-------------------------------------+----------+----------+-------+--------+
  | StrippingSequenceStreamPromptCharm  | 0.001590 |   159    |       | 4.097  |
  +-------------------------------------+----------+----------+-------+--------+
  |    StrippingDstarForPromptCharm     | 0.000490 |    49    | 1.204 |  3.093 |
  |  * StrippingD02HHForPromptCharm     | 0.000530 |    53    | 1.000 |  0.040 |
  |    StrippingDsForPromptCharm        | 0.000170 |    17    | 1.000 |  0.151 |
  |  * StrippingDForPromptCharm         | 0.000250 |    25    | 1.000 |  0.064 |
  |    StrippingLambdaCForPromptCharm   | 0.000230 |    23    | 1.000 |  0.675 |
  +-----------------------------------------------------------+-------+--------+
                                                       Thanks to Anton Poluektov
     * denotes prescaled lines: prescale = 0.25 

  Usage:
 

    >>> stream = ...

    >>> from StrippingSelections.StirppingPromptCharm import StrippingPromptCharmConf

    >>> conf = {}

    >>> promtCharm = StrippingPromptCharmConf ( config = conf ) 
    
    >>> stream.appendLines ( promptCharm.lines() )


Data sample: $STRIPPINGSELECTIONSROOT/tests/data/RUN_81430_RealData+Reco08-Stripping12_90000000_SDST.py
Note: no prescales

StrippingReport                                                INFO Event 50000, Good event 46049
 |                                    *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*| *Errs*|*Incds*| *Slow*|
 |_StrippingGlobal_                                  |  1.1531|       531|       |   5.498|       |       |       |
 |_StrippingSequenceStreamTest_                      |  1.1531|       531|       |   5.493|       |       |       |
 |!StrippingD02HHForPromptCharm                      |  0.7275|       335|  0.000|   4.354|      0|      0|     65|
 |!StrippingDstarForPromptCharm                      |  0.1716|        79|  0.000|   0.064|      0|      0|      0|
 |!StrippingDsForPromptCharm                         |  0.0586|        27|  0.000|   0.086|      0|      0|      0|
 |!StrippingDForPromptCharm                          |  0.3496|       161|  0.000|   0.094|      0|      0|      0|
 |!StrippingLambdaCForPromptCharm                    |  0.0217|        10|  0.000|   0.436|      0|      0|      0|

#=============================================================================================================
                               AlgorithmCorrelationsAlg.AlgorithmCorrelations
#=============================================================================================================
    Algorithm                         Eff.       1        2        3        4        5        6        7   
-------------------------------------------------------------------------------------------------------------
  1 StrippingGlobal                   1.153% |  ####### 100.000% 100.000% 100.000% 100.000% 100.000% 100.000%
  2 StrippingSequenceStreamTest       1.153% | 100.000%  ####### 100.000% 100.000% 100.000% 100.000% 100.000%
  3 StrippingD02HHForPromptCharm      0.727% |  63.089%  63.089%  #######  97.468%   3.704%   0.621%   0.000%
  4 StrippingDstarForPromptCharm      0.172% |  14.878%  14.878%  22.985%  #######   0.000%   0.000%   0.000%
  5 StrippingDsForPromptCharm         0.059% |   5.085%   5.085%   0.299%   0.000%  #######   0.000%   0.000%
  6 StrippingDForPromptCharm          0.350% |  30.320%  30.320%   0.299%   0.000%   0.000%  #######  20.000%
  7 StrippingLambdaCForPromptCharm    0.022% |   1.883%   1.883%   0.000%   0.000%   0.000%   1.242%  #######
#=============================================================================================================

$Revision: 122310 $
Last modification $Date: 2011-04-20 20:57:46 +0200 (Wed, 20 Apr 2011) $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__date__    = '2010-08-03'
__version__ = '$Revision: 122310 $'
# =============================================================================
__all__ = (
    'StrippingPromptCharmConf',
    'default_config'
    )
# =============================================================================

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import   Selection, MergedSelection 
from StandardParticles      import ( StdNoPIDsPions  ,
                                     StdLoosePions   ,
                                     StdLooseKaons   ,
                                     StdLooseProtons , 
                                     StdLooseMuons   ) 

from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

import logging
log = logging.getLogger('StrippingPromptCharm')

# =============================================================================
## Define the default configuration 
_default_configuration_ = {
    #
    # Selection of basic particles 
    #
    'TrackCuts'       : ' ( TRCHI2DOF < 5 ) & ( PT > 250 * MeV ) '        , 
    'BasicCuts'       : ' & ( 9 < MIPCHI2DV()   ) '                       , 
    'KaonCuts'        : ' & ( 2 < PIDK  - PIDpi ) '                       , 
    'PionCuts'        : ' & ( 2 < PIDpi - PIDK  ) '                       , 
    'ProtonCuts'      : ' & ( 2 < PIDp  - PIDpi ) & ( 2 < PIDp - PIDK ) ' , 
    'SlowPionCuts'    : ' TRCHI2DOF < 5 '                                 ,
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
    # dimuons:
    "psi       = ADAMASS ('J/psi(1S)') < 150 * MeV" ,
    "psi_prime = ADAMASS (  'psi(2S)') < 150 * MeV" ,
    ] ,
    ## monitoring ?
    'Monitor'     : False ,
    ## pescales 
    'D0Prescale'             : 1.0 ,
    'DstarPrescale'          : 1.0 ,
    'DsPrescale'             : 1.0 ,
    'DplusPrescale'          : 1.0 ,
    'LamCPrescale'           : 1.0 ,
    'DiCharmPrescale'        : 1.0 , 
    'CharmAndDiMuonPrescale' : 1.0 
    }

# =============================================================================
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
    def __init__(self, name, config ) :
        """
        Constructor
        """

        from copy import deepcopy
        _config = deepcopy ( _default_configuration_ )
        _config.update ( config ) 
        
        LineBuilder.__init__( self , name , _config )
        
        keys = _config.keys()
        for key in keys :
            
            if not key in _default_configuration_ :
                raise KeyError("Invalid key is specified: '%s'" % key )
            
            val = _config[key]
            if val != _default_configuration_ [ key ] : 
                log.warning ('StrippingPromptCharm: new configuration: %-16s : %s ' % ( key , _config[key] ) )

        self._name         = name

        self._trackcuts    =                   _config.pop ( 'TrackCuts'    , _default_configuration_ [ 'TrackCuts'    ] )
        self._basiccuts    = self._trackcuts + _config.pop ( 'BasicCuts'    , _default_configuration_ [ 'BasicCuts'    ] )
        self._kaoncuts     = self._basiccuts + _config.pop ( 'KaonCuts'     , _default_configuration_ [ 'KaonCuts'     ] )    
        self._pioncuts     = self._basiccuts + _config.pop ( 'PionCuts'     , _default_configuration_ [ 'PionCuts'     ] )
        self._protoncuts   = self._basiccuts + _config.pop ( 'ProtonCuts'   , _default_configuration_ [ 'ProtonCuts'   ] )
        self._slowpioncuts =                   _config.pop ( 'SlowPionCuts' , _default_configuration_ [ 'SlowPionCuts' ] )

        self._checkPV      = _config.pop ( 'PrimaryVertices' , _default_configuration_ [ 'PrimaryVertices' ] )

        self.D0Prescale             = _config.pop ( 'D0Prescale'             , _default_configuration_ [ 'D0Prescale'             ] ) 
        self.DstarPrescale          = _config.pop ( 'DstarPrescale'          , _default_configuration_ [ 'DstarPrescale'          ] )  
        self.DsPrescale             = _config.pop ( 'DsPrescale'             , _default_configuration_ [ 'DsPrescale'             ] ) 
        self.DplusPrescale          = _config.pop ( 'DplusPrescale'          , _default_configuration_ [ 'DplusPrescale'          ] ) 
        self.LamCPrescale           = _config.pop ( 'LamCPrescale'           , _default_configuration_ [ 'LamCPrescale'           ] ) 
        self.DiCharmPrescale        = _config.pop ( 'DiCharmPrescale'        , _default_configuration_ [ 'DiCharmPrescale'        ] ) 
        self.CharmAndDiMuonPrescale = _config.pop ( 'CharmAndDiMuonPrescale' , _default_configuration_ [ 'CharmAndDiMuonPrescale' ] )
        
        self._Preambulo    = _config.pop ( 'Preambulo'       , _default_configuration_ [ 'Preambulo'       ] )
        self._monitor      = _config.pop ( 'Monitor'         , _default_configuration_ [ 'Monitor'         ] )
        
        if _config :
            raise KeyError ( 'Invalid keys are specified for configuration: %s ' % _config.keys() )
        
        for line in self._lines_private() :
          self.registerLine(line)

    ## get the selections
    def _selections_private ( self ) :
        
        if hasattr ( self , '_Selections' ) : return self._Selections
        
        self._Selections =  [ self.D02HH () ,
                              self.Dstar () ,
                              self.Ds    () ,
                              self.Dplus () ,
                              self.LamC  () ]
        
        return self._Selections

    ## get all stripping lines 
    def _lines_private ( self ) :
        
        if hasattr ( self , '_Lines' ) : return self._Lines
        
        self._Lines = [
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
            ##
            StrippingLine (
            "DiCharmFor" + self._name ,
            prescale = self.DiCharmPrescale  , ## ATTENTION! Prescale here !!              
            checkPV  = self._checkPV         ,
            algos    = [ self.DiCharm () ]            
            ) ,
            ## 
            StrippingLine (
            "CharmAndDiMuonFor" + self._name ,
            prescale = self.CharmAndDiMuonPrescale  , ## ATTENTION! Prescale here !!              
            checkPV  = self._checkPV         ,
            algos    = [ self.CharmAndDiMuon () ]            
            ) 
            ]
        #
        return self._Lines
            
    def trackCuts    ( self ) : return self._trackcuts 
    def basicCuts    ( self ) : return self._basiccuts
    def kaonCuts     ( self ) : return self._kaoncuts 
    def pionCuts     ( self ) : return self._pioncuts 
    def protonCuts   ( self ) : return self._protoncuts 
    def slowPionCuts ( self ) : return self._slowpioncuts 

    ## get the selection of kaons 
    def kaons ( self ) :
        """
        Get the kaons 
        """

        if hasattr ( self , 'KaonSelection' ) : return self.KaonSelection

        _KaonFilter = FilterDesktop ( Code = " ( 'K+'  == ABSID ) & ( %s ) " % self.kaonCuts() )
        #
        self.KaonSelection   = Selection (
            'SelKaons' + self._name ,
            Algorithm          = _KaonFilter ,
            RequiredSelections = [ StdLooseKaons ]
            )
        #
        return self.KaonSelection
    
    ## get the pion selection
    def pions ( self ) :
        """
        Get the pions 
        """
        
        if hasattr ( self , 'PionSelection' ) : return self.PionSelection

        _PionFilter = FilterDesktop ( Code = " ( 'pi+' == ABSID ) & ( %s ) " % self.pionCuts() )
        #
        self.PionSelection = Selection (
            'SelPionsFor' + self._name ,
            Algorithm          = _PionFilter ,
            RequiredSelections = [ StdLoosePions ]
            )
        #
        return self.PionSelection
    
    ## get the proton selection
    def protons ( self ) :
        """
        Get the proton selection
        """
        
        if hasattr ( self , 'ProtonSelection' ) : return self.ProtonSelection

        _ProtonFilter = FilterDesktop ( Code = " ( 'p+'  == ABSID ) & ( %s ) " % self.protonCuts() )
        
        self.ProtonSelection = Selection (
            'SelProtonsFor' + self._name ,
            Algorithm          = _ProtonFilter ,
            RequiredSelections = [ StdLooseProtons ]
            )
        #
        return self.ProtonSelection
    
    ## get the meson selection 
    def mesons ( self ) :
        """
        Get the meson ( kaon + pion ) selection
        """
        
        if hasattr ( self , 'MesonSelection' ) : return self.MesonSelection
        #
        self.MesonSelection =  MergedSelection (
            ##
            'SelBasicMesonsFor' + self._name , 
            RequiredSelections = [ self.pions() , self.kaons() ]
            )
        #
        return self.MesonSelection

    ## get the common preambulo: 
    def preambulo ( self ) : return self._Preambulo
    
    ## get the preselection of D0->hh
    def preD02HH ( self ) :
        """
        Get the preselection of D0->hh
        """
        if hasattr ( self , 'D02HHForPromptCharm_PreSelection' ) :
            return self.D02HHForPromptCharm_PreSelection
        #
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
        self.D02HHForPromptCharm_PreSelection = Selection (
            ##
            'PreSelD02HHFor' + self._name ,
            ##
            Algorithm          = _D0PreCombine   ,
            ##
            RequiredSelections = [ self.mesons() ] 
            )
        
        return self.D02HHForPromptCharm_PreSelection
    
    # =============================================================================
    # D0 -> Kpi, KK selection
    # =============================================================================
    def D02HH ( self ) :
        """
        Get the selection of D0 -> HH 
        """
        if hasattr ( self , 'D02HHForPromptCharm_Selection' ) : 
            return self.D02HHForPromptCharm_Selection
        
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
        self.D02HHForPromptCharm_Selection = Selection (
            'SelD02HHFor' + self._name ,
            Algorithm          = _D0Filter          ,
            RequiredSelections = [ self.preD02HH() ]
            )
        
        return self.D02HHForPromptCharm_Selection
    
    
    # =============================================================================
    # D*+ -> (D0 -> Kpi, piK, KK, pipi) pi+ selection 
    # =============================================================================
    def Dstar ( self ) :
        
        if hasattr ( self , 'DstarForPromptCharm_Selection' ) : 
            return self.DstarForPromptCharm_Selection
        
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
        self.DstarForPromptCharm_Selection = Selection  (
            ##
            "SelDstarFor" + self._name ,
            ## 
            Algorithm          = _DstarCombine ,
            ##
            RequiredSelections = [ self.preD02HH() , StdNoPIDsPions ]
            )
        
        return self.DstarForPromptCharm_Selection
    
    
    # =============================================================================
    # Ds+/D+ -> KKpi   selection 
    # =============================================================================
    def Ds ( self ) :
        
        if hasattr  ( self , 'DsForPromptCharm_Selection' ) : 
            return self.DsForPromptCharm_Selection 
        
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
            "admDs = ADAMASS ('D_s+')  <   85 * MeV " , ## Ds+ mass window for combiantion cut 
            "dmD   = ADMASS  ('D+'  )  <   75 * MeV " , ## D+  mass window 
            "dmDs  = ADMASS  ('D_s+')  <   75 * MeV " , ## Ds+ mass window
            ##
            "hKKpi = Gaudi.Histo1DDef ( ' mass K K pi' , 1800 , 2020 , 110 )" ,
            ] ,
            ##                                 phi                         
            CombinationCut = """
            aphi                 &
            ( APT > 1750 * MeV ) & 
            ( admD | admDs  ) 
            """ ,
            ##
            MotherCut      = """
            ( chi2vx  < 25            ) &
            ( PT      > 2.0 * GeV     ) & 
            ( dmD | dmDs              ) &
            ( ctau > 100 * micrometer )
            """ , 
            ##
            MotherMonitor  = """
            process ( monitor ( M , hKKpi , 'mass KKpi' )  ) >> ~EMPTY 
            """
            )
        
        ## convert it to selection
        self.DsForPromptCharm_Selection = Selection (
            "SelDsFor" + self._name ,
            Algorithm          = _DsCombine ,
            RequiredSelections = [ self.mesons() ]
            )
        
        return self.DsForPromptCharm_Selection


    
    # =============================================================================
    # D+ -> Kpipi   selection 
    # =============================================================================
    def Dplus ( self ) :
        
        if hasattr ( self , 'DForPromptCharm_Selection' ) :
            return self.DForPromptCharm_Selection
        
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
        self.DForPromptCharm_Selection = Selection (
            "SelDFor" + self._name ,
            Algorithm          = _DCombine ,
            RequiredSelections = [ self.mesons() ]
            )

        return self.DForPromptCharm_Selection
        
    
    # =============================================================================
    # Lambda_C -> ( pKpi )  selection 
    # =============================================================================
    def LamC ( self ) :
        
        if hasattr ( self , 'LambdaCForPromptCharm_Selection' ) :
            return self.LambdaCForPromptCharm_Selection
                    
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
        self.LambdaCForPromptCharm_Selection = Selection (
            "SelLambdaCFor" + self._name ,
            Algorithm          = _LambdaC_Combine ,
            RequiredSelections = [ self.mesons  () ,
                                   self.protons () ]  
            )
        
        return self.LambdaCForPromptCharm_Selection
    
    ## helper merged selection of all charmed particles 
    def PromptCharm ( self ) :
        """
        Helper merged selection of all charmed particles 
        """        
        if hasattr ( self , 'PromptCharm_Selection' ) :
            return self.PromptCharm_Selection
        
        self.PromptCharm_Selection =  MergedSelection (
            ##
            'PromptCharmFor' + self._name , 
            RequiredSelections = [ self.D02HH () ,
                                   self.Dstar () ,
                                   self.Ds    () ,
                                   self.Dplus () ,
                                   self.LamC  () ]
            )
        
        return self.PromptCharm_Selection
    
    ## get "Di-Charm"-selection
    def DiCharm ( self ) :
        """
        Di-Charm selection
        """
        if hasattr ( self , 'DiCharm_Selection' ) :
            return self.DiCharm_Selection
        
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
            MotherCut      = " ALL " 
            )
        
        self.DiCharm_Selection = Selection (
            ##
            'SelDiCharmFor' + self._name ,
            ##
            Algorithm          = _Combine   ,
            ##
            RequiredSelections = [ self.PromptCharm () ]
            )
        
        return self.DiCharm_Selection
    
    ## get the dimuons 
    def DiMuon ( self ) :
        """
        Get the dimuons  
        """
        if hasattr ( self , 'DiMuon_Selection' ) :
            return self.DiMuon_Selection
        
        ## prepare Di-muons
        _Combine = CombineParticles (
            ## the decays to be reconstructed 
            DecayDescriptor = 'J/psi(1S) -> mu+ mu-' ,
            DaughtersCuts = {
            'mu+' : ' TRCHI2DOF < 5 '
            } , 
            ## combination cut 
            CombinationCut  = " psi | psi_prime | ( AM > 4.5 * GeV ) " ,
            ##      mother cut 
            MotherCut       = " chi2vx < 10 " 
            )
        
        ##
        self.DiMuon_Selection = Selection (
            ##
            'SelDiMuonFor' + self._name ,
            ##
            Algorithm          = _Combine   ,
            ##
            RequiredSelections = [ StdLooseMuons ]
            )
        
        return self.DiMuon_Selection
    
    ## get the dimuons & charn 
    def CharmAndDiMuon ( self ) :
        """
        get charm & dimuon :
        Select event with at elats one charm particle and
        at least one dimuon 
        """
        if hasattr ( self , 'CharmAndDiMuon_Selection' ) :
            return self.CharmAndDiMuon_Selection
        
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
        
        self.CharmAndDiMuon_Selection = Selection  (
            "SelCharmAndDiMuonFor" + self._name  ,
            Algorithm = _Filter ,
            RequiredSelections = [
            self.DiMuon      () ,
            self.PromptCharm ()
            ]
            )
        
        return self.CharmAndDiMuon_Selection
        

default_config = {
    'D0Prescale'             : 1.00 ,
    'DstarPrescale'          : 1.00 ,
    'DsPrescale'             : 1.00 ,
    'DplusPrescale'          : 1.00 ,
    'LamCPrescale'           : 1.00 ,
    'DiCharmPrescale'        : 1.00 ,
    'CharmAndDiMuonPrescale' : 1.00 
    }

# =============================================================================
if '__main__' == __name__ :

    print 80*'*'
    print __doc__
    print ' Author :  %s' % __author__
    print ' Date   :  %s' % __date__
    print ' The output locations for default configuration: '
    _conf = StrippingPromptCharmConf( "PromptCharm", config = default_config  ) 
    for l in _conf.lines() :
        print ' \t ', l.outputLocation  () , l 
    print 80*'*'
        
# =============================================================================
# The END 
# =============================================================================

