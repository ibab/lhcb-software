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
#  Excited charm baryons for Biagio Saitta:
#
#    - Sigma_c(0,++) -> Lambda_c+ pi        ( 2455 & 2520 )
#    - Lambda_c+*    -> Lambda_c+ pi+ pi-   ( 2595, 2625 & 2880 )
#
#  Also one has the combinations for 2xCharm studies
#
#    - Charm +   Charm            ( both Charm/Charm and Charm/anti-Charm )
#    - Charm +   Dimuon
#    - Charm + ( Dimuon + gamma ) ( get for free chi_(c,b) )
#
#  Associative production of  charm & W+ :
#    - Charm     & W+
#    - dimuons   & W+
#    - chi_(c,b) & W+
#
#  Double charmed baryons:
#
#    - Xi_cc+    -> Lambda_c+ K- pi+
#    - Xi_cc++   -> Lambda_c+ K- pi+ pi+
#
#  The cuts more or less correspond to D*+ selection by Alexandr Kozlinzkiy.
#  In addition the PT-cut for the long-lived charmed particle has been applied.
#  Thanks to Marco Gersabeck & Harry Cliff for nice idea.
#
# +--------------------------------+--------+------+-------+
# | StrippingReport INFO Event 276300, Good event 276300   |
# +--------------------------------+--------+------+-------+
# |    Decision name               | Rate,% | Acc. | ms/evt|
# +--------------------------------+--------+------+-------+
# |                 the regular prompt charm               |
# +--------------------------------+--------+------+-------+
# | <>D02HHForPromptCharm          | 2.1260 | 5874 | 5.069 |
# | <>DstarForPromptCharm          | 0.6739 | 1862 | 0.063 |
# | <>DsForPromptCharm             | 0.1795 |  496 | 0.266 |
# | <>DForPromptCharm              | 1.1596 | 3204 | 0.128 |
# | <>LambdaCForPromptCharm        | 0.1364 |  377 | 1.588 |
# +--------------------------------+--------+------+-------+
# | excited prompt charm baryons, no contribution to rate  |
# +--------------------------------+--------+------+-------+
# | <>SigmaCForPromptCharm         | 0.1227 |  339 | 0.267 |
# | <>LambdaCstarForPromptCharm    | 0.0879 |  243 | 0.037 |
# +--------------------------------+--------+------+-------+
# |                 double charm, the rate is tiny         |
# +--------------------------------+--------+------+-------+
# | <>DiCharmForPromptCharm        | 0.0076 |   21 | 1.558 |
# | <>DiMuonAndCharmForPromptCharm | 0.0257 |   71 | 0.284 |
# | <>ChiAndCharmForPromptCharm    | 0.0159 |   44 | 0.046 |
# +--------------------------------+--------+------+-------+
# |               associative charm & W+ production        |
# +--------------------------------+--------+------+-------+
# | <>CharmAndWForPromptCharm      | 0.0007 |    2 | 0.061 |
# | <>DiMuonAndWForPromptCharm     | 0.0119 |   33 | 0.033 |
# | <>ChiAndWForPromptCharm        | 0.0054 |   15 | 0.033 |
# +--------------------------------+--------+------+-------+
# |               other                                    |
# +--------------------------------+--------+------+-------+
# | <>DsPsiForPromptCharm          | 0.0090 |   25 | 0.032 |
# | <>DsLamCForPromptCharm         | 0.0090 |   25 | 0.032 |
# | <>Xicc+ForPromptCharm          | 0.0090 |   25 | 0.032 |
# | <>Xicc++ForPromptCharm         | 0.0090 |   25 | 0.032 |
# +--------------------------------+--------+------+-------+
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

   The attempt for coherent stripping of all stable charm hadrons :
 
     - D*+       -> ( D0 -> K pi , K K , pi pi, pi K ) pi+
     - D0        -> K pi , K K
     - Ds+/D+    -> ( phi -> K K ) pi
     - D+        -> K pi pi
     - Lambda_c+ -> p K pi
 
   Excited charm baryons for Biagio Saitta:
 
     - Sigma_c(0,++) -> Lambda_c+ pi        ( 2455 & 2520 )
     - Lambda_c+*    -> Lambda_c+ pi+ pi-   ( 2595, 2625 & 2880 )
 
   Also one has the combinations for 2xCharm studies
 
     - Charm +   Charm            ( both Charm/Charm and Charm/anti-Charm )
     - Charm +   Dimuon
     - Charm + ( Dimuon + gamma ) ( get for free chi_(c,b) )
 
   Associative production of  charm & W+ :
     - Charm     & W+
     - dimuons   & W+
     - chi_(c,b) & W+


  Usage:


    >>> stream = ...

    >>> from StrippingSelections.StirppingPromptCharm import StrippingPromptCharmConf

    >>> conf = {}

    >>> promptCharm = StrippingPromptCharmConf ( 'PromptCharm' , config = conf )

    >>> stream.appendLines ( promptCharm.lines() )

  +--------------------------------+--------+------+-------+
  | StrippingReport INFO Event 276300, Good event 276300   |
  +--------------------------------+--------+------+-------+
  |    Decision name               | Rate,% | Acc. | ms/evt|
  +--------------------------------+--------+------+-------+
  |                 the regular prompt charm               |
  +--------------------------------+--------+------+-------+
  | <>D02HHForPromptCharm          | 2.1260 | 5874 | 5.069 |
  | <>DstarForPromptCharm          | 0.6739 | 1862 | 0.063 | 
  | <>DsForPromptCharm             | 0.1795 |  496 | 0.266 |
  | <>DForPromptCharm              | 1.1596 | 3204 | 0.128 |
  | <>LambdaCForPromptCharm        | 0.1364 |  377 | 1.588 |
  +--------------------------------+--------+------+-------+
  | excited prompt charm baryons, no contribution to rate  |
  +--------------------------------+--------+------+-------+
  | <>SigmaCForPromptCharm         | 0.1227 |  339 | 0.267 |
  | <>LambdaCstarForPromptCharm    | 0.0879 |  243 | 0.037 |
  +--------------------------------+--------+------+-------+
  |                 double charm, the rate is tiny         |
  +--------------------------------+--------+------+-------+
  | <>DiCharmForPromptCharm        | 0.0076 |   21 | 1.558 |
  | <>DiMuonAndCharmForPromptCharm | 0.0257 |   71 | 0.284 |
  | <>ChiAndCharmForPromptCharm    | 0.0159 |   44 | 0.046 |
  +--------------------------------+--------+------+-------+
  |               associative charm & W+ production        |
  +--------------------------------+--------+------+-------+
  | <>CharmAndWForPromptCharm      | 0.0007 |    2 | 0.061 |
  | <>DiMuonAndWForPromptCharm     | 0.0119 |   33 | 0.033 |
  | <>ChiAndWForPromptCharm        | 0.0054 |   15 | 0.033 |
  +--------------------------------+--------+------+-------+
  | <>DsPsiForPromptCharm          | 0.0090 |   25 | 0.032 |
  +--------------------------------+--------+------+-------+
 

                  $Revision$
Last modification $Date$
               by $Author$

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = '2010-08-03'
__version__ = '$Revision$'
# =============================================================================
__all__ = (
    'StrippingPromptCharmConf',
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
                                     ## for onia, prompt and W
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
    #
    ## PT-cuts
    #
    'pT(D0)'           :  3.0 * GeV ,    ## pt-cut for  prompt   D0
    'pT(D0) for D*+'   :  2.0 * GeV ,    ## pt-cut for  D0 from  D*+
    'pT(D+)'           :  3.0 * GeV ,    ## pt-cut for  prompt   D+
    'pT(Ds+)'          :  3.0 * GeV ,    ## pt-cut for  prompt   Ds+
    'pT(Ds+) for Bc+'  :  1.0 * GeV ,    ## pt-cut for  Ds+ from Bc+
    'pT(Ds+) for Lb'   :  1.0 * GeV ,    ## pt-cut for  Ds+ from Lb0
    'pT(Lc+)'          :  3.0 * GeV ,    ## pt-cut for  prompt   Lc+
    'pT(Lc+) for Xicc' :  1.0 * GeV ,    ## pt-cut for  Lc+ from Xicc
    'pT(Lc+) for Lb'   :  1.0 * GeV ,    ## pt-cut for  Lc+ from Lb0
    'pT(Xicc+)'        :  2.0 * GeV ,    ## pt-cut for  Xicc+ 
    'pT(Xicc++)'       :  2.0 * GeV ,    ## pt-cut for  Xicc++
    #
    # Selection of basic particles
    #
    'TrackCuts'       : """
    ( TRCHI2DOF < 4     ) &
    ( PT > 250 * MeV    ) &
    ( TRGHOSTPROB < 0.5 ) & 
    in_range  ( 2 , ETA , 5 )
    """ ,
    'BasicCuts'       : ' & ( 9 < MIPCHI2DV() ) ' ,
    'KaonCuts'        : ' & ( 2 < PIDK  - PIDpi ) '                           ,
    'PionCuts'        : ' & ( 2 < PIDpi - PIDK  ) '                           ,
    'ProtonCuts'      : ' & ( 2 < PIDp  - PIDpi ) & ( 2 < PIDp - PIDK ) '     ,
    'SlowPionCuts'    : ' TRCHI2DOF < 5 '                                     ,
    'MuonCuts'        : ' ISMUON & ( PT > 650 * MeV ) & ( TRCHI2DOF < 5 ) '   ,
    #
    ## photons from chi_(c,b)
    #
    'GammaChi'        : ' ( PT > 450 * MeV ) & ( CL > 0.05 ) '  ,
    #
    ## W+- selection
    #
    'WCuts'           : " ( 'mu+'== ABSID ) & ( PT > 15 * GeV )" ,
    #
    # Global Event cuts
    #
    'CheckPV'         : True ,
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
    "ctau     = BPVLTIME (   9 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<9
    "ctau_9   = BPVLTIME (   9 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<9
    "ctau_16  = BPVLTIME (  16 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<16
    "ctau_25  = BPVLTIME (  25 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<25
    "ctau_100 = BPVLTIME ( 100 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<100
    "ctau_no  = BPVLTIME (     ) * c_light "  , ## no embedded cut for chi2(lifetimeFit)
    # dimuons:
    "psi           =   ADAMASS ('J/psi(1S)') < 150 * MeV"  ,
    "psi_prime     =   ADAMASS (  'psi(2S)') < 150 * MeV"  ,
    ## good proton
    "good_proton   = ( 'p+' == ABSID ) & HASRICH & in_range ( 8 * GeV , P , 150 * GeV )  " ,
    "good_proton   = good_proton & ( PIDp - PIDpi > 2 ) " ,
    "good_proton   = good_proton & ( PIDp - PIDK  > 2 ) " 
    ] ,
    ## monitoring ?
    'Monitor'     : False ,
    ## pescales
    'D0Prescale'             : 1.0 ,
    'D*Prescale'             : 1.0 ,
    'DsPrescale'             : 1.0 ,
    'D+Prescale'             : 1.0 ,
    'LambdaCPrescale'        : 1.0 ,
    'LambdaC*Prescale'       : 1.0 ,
    'Xicc+Prescale'          : 1.0 ,
    'Xicc++Prescale'         : 1.0 ,
    'SigmaCPrescale'         : 1.0 ,
    'DiCharmPrescale'        : 1.0 ,
    'DiMu&CharmPrescale'     : 1.0 ,
    'Chi&CharmPrescale'      : 1.0 ,
    'Ds&PsiPrescale'         : 1.0 ,
    'Ds&Lc+Prescale'         : 1.0 ,
    'Charm&WPrescale'        : 1.0 ,
    'DiMuon&WPrescale'       : 1.0 ,
    'Chi&WPrescale'          : 1.0 ,
    ## ========================================================================
    }
# =============================================================================
# use a bit faster fitter 
FITTER = 'LoKi::VertexFitter:PUBLIC'
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
                logger.warning ('new configuration: %-16s : %s ' % ( key , _config[key] ) )

        ## cehck for prescales
        for keys in self.keys() :
            if 0 > key.find('Prescale') and 0 > key.find('prescale') :  continue
            if 1 != self[key] : logger.warning ( '%s is %s' % ( key , self[key] ) )

        for line in self._lines_charm() :
            self.registerLine(line)
            logger.debug ( "Register line: %s" %  line.name () )
                
    ## get the selections
    def _selections_private ( self ) :

        sel = self._selections ( 'Selections' )
        if sel : return sel

        sel =  [ self.D02HH          () ,
                 self.Dstar          () ,
                 self.Ds             () ,
                 self.Dplus          () ,
                 self.preLamC        () ,
                 self.LamC           () ,
                 self.LamC4Xicc      () ,
                 self.SigC           () ,
                 self.LamCstar       () ,
                 self.XiccP          () ,
                 self.XiccPP         () ,
                 self.DiMuon         () ,
                 self.DiCharm        () ,
                 self.DiMuonAndCharm () ,
                 self.ChiAndCharm    () ,
                 self.preDs          () ,
                 self.DsPsi          () ,
                 self.DsLamC         () ,
                 self.W              () ,
                 self.CharmAndW      () ,
                 self.DiMuonAndW     () ,
                 self.ChiAndW        () ,
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
            "D02HHFor" + self.name() ,
            prescale = self['D0Prescale'  ]  , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'     ]  ,
            algos    =     [ self.D02HH() ]
            ) ,
            ##
            StrippingLine (
            "DstarFor" + self.name() ,
            prescale = self['D*Prescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'     ] ,
            algos    =     [ self.Dstar() ]
            ) ,
            ##
            StrippingLine (
            "DsFor" + self.name() ,
            prescale = self['DsPrescale']    , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'   ]    ,
            algos    =     [ self.Ds()  ]
            ) ,
            ##
            StrippingLine (
            "DFor" + self.name() ,
            prescale = self['D+Prescale' ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'    ] ,
            algos    =     [ self.Dplus () ]
            ) ,
            ##
            StrippingLine (
            "LambdaCFor" + self.name() ,
            prescale = self['LambdaCPrescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'        ] ,
            algos    =     [ self.LamC () ]
            ) ,
            ## Sigma_c
            StrippingLine (
            "SigmaCFor" + self.name() ,
            prescale = self['SigmaCPrescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'       ] ,
            algos    =     [ self.SigC ()   ]
            ) ,
            ## Lambda_c*
            StrippingLine (
            "LambdaCstarFor" + self.name() ,
            prescale = self['LambdaC*Prescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'         ] ,
            algos    =     [ self.LamCstar () ]
            ) ,
            ## Xicc+ 
            StrippingLine (
            "Xicc+For" + self.name()     ,
            prescale = self['Xicc+Prescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'      ] ,
            algos    =     [ self.XiccP () ]
            ) ,
            ## Xicc++ 
            StrippingLine (
            "Xicc++For" + self.name()         ,
            prescale = self['Xicc++Prescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'       ] ,
            algos    =     [ self.XiccPP () ]
            ) ,
            ## DiCharm
            StrippingLine (
            "DiCharmFor" + self.name() ,
            prescale = self['DiCharmPrescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'        ] ,
            algos    = [ self.DiCharm () ]
            ) ,
            ##
            StrippingLine (
            "DiMuonAndCharmFor" + self.name()     ,
            prescale = self['DiMu&CharmPrescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'           ] ,
            algos    =     [ self.DiMuonAndCharm () ]
            ) ,
            ##
            StrippingLine (
            "ChiAndCharmFor" + self.name() ,
            prescale = self['Chi&CharmPrescale']  , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'          ] ,
            algos    =     [ self.ChiAndCharm () ]
            ) ,
            ##
            StrippingLine (
            "DsLamCFor" + self.name() ,
            prescale   = self['Ds&Lc+Prescale' ] , ## ATTENTION! Prescale here !!
            checkPV    = self['CheckPV'        ] ,
            algos      =     [ self.DsLamC ()  ]
            ) ,
            ##
            StrippingLine (
            "DsPsiFor" + self.name() ,
            prescale   = self['Ds&PsiPrescale' ] , ## ATTENTION! Prescale here !!
            checkPV    = self['CheckPV'        ] ,
            algos      =     [ self.DsPsi () ]
            ) ,
            ##
            StrippingLine (
            "CharmAndWFor" + self.name() ,
            prescale   = self['Charm&WPrescale' ] , ## ATTENTION! Prescale here !!
            checkPV    = self['CheckPV'         ] ,
            algos      =     [ self.CharmAndW () ]
            ) ,
            ##
            StrippingLine (
            "DiMuonAndWFor" + self.name() ,
            prescale   = self['DiMuon&WPrescale' ] , ## ATTENTION! Prescale here !!
            checkPV    = self['CheckPV'          ] ,
            algos      =     [ self.DiMuonAndW ()  ]
            ) ,
            ##
            StrippingLine (
            "ChiAndWFor" + self.name() ,
            prescale   = self['Chi&WPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV    = self['CheckPV'        ] ,
            algos      =     [ self.ChiAndW () ]
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

    def trackCuts    ( self ) : return                    self [ 'TrackCuts'    ]
    def basicCuts    ( self ) : return self.trackCuts() + self [ 'BasicCuts'    ]
    def kaonCuts     ( self ) : return self.basicCuts() + self [ 'KaonCuts'     ]
    def pionCuts     ( self ) : return self.basicCuts() + self [ 'PionCuts'     ]
    def protonCuts   ( self ) : return self.basicCuts() + self [ 'ProtonCuts'   ]
    def slowPionCuts ( self ) : return                    self [ 'SlowPionCuts' ]
    def muonCuts     ( self ) : return                    self [ 'MuonCuts'     ]

    ## get the selection of kaons
    def kaons ( self ) :
        """
        Get the kaons
        """
        sel = self._selection ( 'KaonSelection')
        if sel : return sel

        _KaonFilter = FilterDesktop ( Code = """
        ( 'K+'  == ABSID ) & ( %s )
        """ % self.kaonCuts() )
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

        _PionFilter = FilterDesktop (
            Code = """
            ( 'pi+' == ABSID ) & ( %s )
            """ % self.pionCuts() )
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

        _ProtonFilter = FilterDesktop (
            Code = """
            ( 'p+'  == ABSID ) & ( %s )
            """ % self.protonCuts() )

        sel = Selection (
            'SelProtonsFor' + self.name() ,
            Algorithm          = _ProtonFilter ,
            RequiredSelections = [ StdLooseProtons ]
            )
        #
        return self._add_selection ( 'ProtonSelection' , sel )

    ## get the common preambulo:
    def preambulo ( self ) : return self['Preambulo']

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
            ( APT >  %s  )
            """ %  ( 0.95 * min ( self['pT(D0)'        ] ,
                                  self['pT(D0) for D*+'] ) ) ,
            #
            ## mother cut
            MotherCut      = """
            ( chi2vx < 9 )              &
            ( PT     > %s             ) &
            ( ADMASS('D0') < 75 * MeV ) &
            ( abs ( LV01 ) < 0.9      ) &
            ( ctau > 100 * micrometer )
            """ %           min ( self['pT(D0)'        ] ,
                                  self['pT(D0) for D*+'] ) ,
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } ,
            #
            )
        #
        ## make (pre)selection
        sel = Selection (
            ##
            'PreSelD02HHFor'   + self.name()     ,
            ##
            Algorithm          = _D0PreCombine   ,
            ##
            RequiredSelections = [ self.kaons () ,
                                   self.pions () ]
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
        _fltr = FilterDesktop (
            ##
            Monitor      = self['Monitor']  ,
            HistoProduce = self['Monitor']  ,
            ##
            Preambulo = self.preambulo() + [
            "hKpi = Gaudi.Histo1DDef ( 'mass K pi' , 1800 , 1920 , 60 )" ,
            "hKK  = Gaudi.Histo1DDef ( 'mass K K'  , 1800 , 1920 , 60 )"
            ] ,
            ##
            Code      = """
            ( PT             > %s         ) &
            ( ADMASS( 'D0' ) < 75  * MeV  ) &
            ( kk | kpi )                    &
            ( ctau > 100 * micrometer )
            """ %  self['pT(D0)'] ,
            ##
            PostMonitor  = """
            process ( switch ( kpi , monitor ( M , hKpi , 'mass Kpi' ) , monitor ( M , hKK  , 'mass KK'  ) ) ) >> ~EMPTY
            """
            )

        ## make selection
        sel = Selection (
            'SelD02HHFor'      + self. name()      ,
            Algorithm          = _fltr             ,
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

        cmb = CombineParticles(
            ##
            Monitor      = self['Monitor'] ,
            HistoProduce = self['Monitor'] ,
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
            'D0'  : """
            ( PT > %s ) &
            switch ( pipi , in_range ( -50 * MeV , DMASS('D0') , 75 * MeV ) , ALL )
            """ % self['pT(D0) for D*+']
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
            """ , 
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } ,
            #
            )

        ## convert it to selection
        sel = Selection  (
            ##
            "SelDstarFor"      + self.name() ,
            ##
            Algorithm          = cmb         ,
            ##
            ## ATTENITON: we need slow prompt pions!
            RequiredSelections = [ self.preD02HH()   ,
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

        cmb = CombineParticles(
            ##
            Monitor      = self['Monitor'] ,
            HistoProduce = self['Monitor'] ,
            ##
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
            aphi          &
            ( APT > %s  ) &
            ( admD | admDs  )
            """  % ( 0.95 * min ( self['pT(Ds+)'        ] ,
                                  self['pT(Ds+) for Bc+'] ,
                                  self['pT(Ds+) for Lb' ] ) ) ,
            #
            ## ATTENTION: there is neither c*tau nor pointing cuts here!
            MotherCut      = """
            ( chi2vx  < 25  ) &
            ( PT      > %s  ) &
            ( dmD | dmDs    )
            """  % min ( self['pT(Ds+)'        ] ,
                         self['pT(Ds+) for Bc+'] ) ,
            ##
            MotherMonitor  = """
            process ( monitor ( M , hKKpi , 'mass KKpi' )  ) >> ~EMPTY
            """ ,
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } ,
            #
            )

        ## convert it to selection
        sel = Selection (
            "SelPreDsFor"      + self.name()     ,
            Algorithm          = cmb             ,
            RequiredSelections = [ self.kaons () ,
                                   self.pions () ]
            )

        return self._add_selection ( 'PreDsForPromptCharm_Selection' , sel )

    # =============================================================================
    # Ds+/D+ -> KKpi   selection
    # =============================================================================
    def Ds ( self ) :

        sel = self._selection ( 'DsForPromptCharm_Selection' )
        if sel : return sel

        fltr = FilterDesktop (
            Code = """
            ( PT   > %s               ) &
            ( ctau > 100 * micrometer )
            """  % self['pT(Ds+)']
            )

        ## convert it to selection
        sel = Selection (
            "SelDsFor"         + self.name()     ,
            Algorithm          = fltr            ,
            RequiredSelections = [ self.preDs()  ]
            )

        return self._add_selection ( 'DsForPromptCharm_Selection' , sel )


    # =============================================================================
    # D+ -> Kpipi   selection
    # =============================================================================
    def Dplus ( self ) :

        sel = self._selection ( 'DForPromptCharm_Selection' )
        if sel : return sel

        cmb = CombineParticles(
            ##
            Monitor      = self['Monitor'] ,
            HistoProduce = self['Monitor'] ,
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
            ( APT > %s )
            """ % ( 0.95 * self['pT(D+)' ] ) ,
            ##
            MotherCut      = """
            ( chi2vx  < 25                 ) &
            ( PT      > %s                 ) &
            ( ADMASS  ('D+'  )  < 50 * MeV ) &
            ( ctau    > 100 * micrometer   )
            """ % self['pT(D+)' ] ,
            ##
            MotherMonitor  = """
            process ( monitor ( M , hKpipi , 'mass K pi pi' ) )  >> ~EMPTY
            """ ,
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } ,
            #
            )

        # convert it to selection
        sel = Selection (
            "SelDFor"          + self.name()     ,
            Algorithm          = cmb             ,
            RequiredSelections = [ self.kaons () ,
                                   self.pions () ]
            )

        return self._add_selection ( 'DForPromptCharm_Selection' , sel )

    # =============================================================================
    # Lambda_C -> ( pKpi )  (pre)selection
    # =============================================================================
    def preLamC ( self ) :

        sel = self._selection ( 'PreLambdaCForPromptCharm_Selection' )
        if sel : return sel

        cmb = CombineParticles(
            ##
            Monitor      = self['Monitor'] ,
            HistoProduce = self['Monitor'] ,
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
            ( APT > %s )
            """ % ( 0.95 * self['pT(Lc+)' ] ) ,
            ##
            MotherCut      = """
            ( chi2vx  < 25 )                   &
            ( PT      > %s                   ) &
            ( ADMASS('Lambda_c+') < 50 * MeV ) &
            ( ctau_no > 80 * micrometer      )
            """ %  min ( self [ 'pT(Lc+)'          ] ,
                         self [ 'pT(Lc+) for Xicc' ] ,
                         self [ 'pT(Lc+) for Lb'   ] ) , 
            ##
            MotherMonitor  = """
            process ( monitor ( M , hpKpi , 'mass p K pi ' ) ) >> ~EMPTY
            """ , 
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } ,
            #
            )

        # convert it to selection
        sel = Selection (
            "PreSelLambdaCFor" + self.name()       ,
            Algorithm          = cmb               ,
            RequiredSelections = [ self.kaons   () ,
                                   self.protons () ,
                                   self.pions   () ] 
            )

        return self._add_selection( 'PreLambdaCForPromptCharm_Selection' ,  sel )

    # =============================================================================
    # Lambda_C -> ( pKpi )  (prompt) selection 
    # =============================================================================
    def LamC ( self ) :
        """
        Lambda_C -> ( pKpi )  (prompt) selection 
        """
        sel = self._selection ( 'LambdaCForPromptCharm_Selection' )
        if sel : return sel
        
        fltr = FilterDesktop (
            ##
            Preambulo = self.preambulo() ,
            ##
            Code = """
            ( PT     > %s               ) &
            ( ctau_9 > 100 * micrometer ) 
            """ % self [ 'pT(Lc+)' ] 
            )
        
        # convert it to selection
        sel = Selection (
            "SelLambdaCFor"    + self.name()       ,
            Algorithm          = fltr              ,
            RequiredSelections = [ self.preLamC () ] 
            )
        
        return self._add_selection( 'LambdaCForPromptCharm_Selection' ,  sel )
    

    # =============================================================================
    # Lambda_C -> ( pKpi ) selection for Xicc 
    # =============================================================================
    def LamC4Xicc ( self ) :
        """
        Lambda_C -> ( pKpi ) selection for Xicc
        """
        sel = self._selection ( 'LamC4XiccForPromptCharm_Selection' )
        if sel : return sel
        
        fltr = FilterDesktop (
            ##
            Preambulo = self.preambulo() ,
            ##
            Code = """
            ( PT > %s ) & INTREE ( good_proton ) 
            """ % self [ 'pT(Lc+) for Xicc' ] 
            )
        
        # convert it to selection
        sel = Selection (
            "SelLamC4XiccFor"    + self.name()     ,
            Algorithm          = fltr              ,
            RequiredSelections = [ self.preLamC () ] 
            )
        
        return self._add_selection( 'LamC4XiccForPromptCharm_Selection' ,  sel )
    
    
    # =============================================================================
    # Sigma_C -> Lambda_C pi selection
    # =============================================================================
    def SigC ( self ) :

        sel = self._selection ( 'SigmaCForPromptCharm_Selection' )
        if sel : return sel

        cmb = CombineParticles(
            ##
            Monitor      = self['Monitor'] ,
            HistoProduce = self['Monitor'] ,
            ##
            DecayDescriptors = [
            " [ Sigma_c0  -> Lambda_c+ pi- ]cc" ,
            " [ Sigma_c++ -> Lambda_c+ pi+ ]cc" ,
            ] ,
            ##
            DaughtersCuts = {
            'Lambda_c+' : ' INTREE ( good_proton ) ' , 
            'pi+'       :  self.slowPionCuts()
            } ,
            ##
            Preambulo = self.preambulo() + [
            "hDmSigC = Gaudi.Histo1DDef ( 'dm(Sigma_c)' , 100 , 400 , 150 )"
            ] ,
            ##
            CombinationCut = """
            ( AM - AM1 < 400 * MeV )
            """ ,
            ##
            MotherCut      = """
            ( chi2vx  < 16 )
            """ ,
            ##
            MotherMonitor  = """
            process ( monitor ( M-M1 , hDmSigC , 'dm(Sigma_c)' ) ) >> ~EMPTY
            """ ,
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } ,
            #
            )

        # convert it to selection
        sel = Selection (
            "SelSigmaCFor"     +   self.name()       ,
            Algorithm          =   cmb               ,
            RequiredSelections = [ self.LamC    ()   ,
                                   StdAllNoPIDsPions ] ## slow prompt pion!
            )

        return self._add_selection( 'SigmaCForPromptCharm_Selection' ,  sel )


    # =============================================================================
    # Lambda_C* -> Lambda_C pi pi selection
    # =============================================================================
    def LamCstar ( self ) :

        sel = self._selection ( 'LambdaCstarForPromptCharm_Selection' )
        if sel : return sel

        cmb = CombineParticles(
            ##
            Monitor      = self['Monitor'] ,
            HistoProduce = self['Monitor'] ,
            ##
            DecayDescriptors = [
            " [ Lambda_c(2625)+ -> Lambda_c+ pi+ pi-]cc"
            ] ,
            ##
            DaughtersCuts = {
            'Lambda_c+' : ' INTREE ( good_proton ) ' , 
            'pi+'       :  self.slowPionCuts()
            } ,
            ##
            Preambulo = self.preambulo() + [
            "hDmLamC = Gaudi.Histo1DDef ( 'dm(Lambda_c)' , 200 , 700 , 200 )"
            ] ,
            ##
            CombinationCut = """
            AM - AM1 < 650 * MeV
            """ ,
            MotherCut      = """
            ( chi2vx  < 25 )
            """ ,
            ##
            MotherMonitor  = """
            process ( monitor ( M-M1 , hDmLamC , 'dm(Lambda_c)' ) ) >> ~EMPTY
            """ ,
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } ,
            #
            )

        # convert it to selection
        sel = Selection (
            "SelLambdaCstarFor" + self.name()         ,
            Algorithm           =   cmb               ,
            RequiredSelections  = [ self.LamC    ()   ,
                                    StdAllNoPIDsPions ] ## slow prompt pion!
            )

        return self._add_selection( 'SigmaCstarForPromptCharm_Selection' ,  sel )

    # ===========================================================================
    ## try to reconstruct   Xicc+
    def XiccP  ( self ) :
        """
        Try to reconstruct   Xicc+ -> Lambda_c+ K- pi+ 
        """
        sel = self._selection ( 'Xicc+ForPromptCharm_Selection' )
        if sel : return sel
        
        cmb = CombineParticles(
            ##
            Preambulo = self.preambulo() , 
            ##
            DecayDescriptors = [
            " [ Xi_cc+ -> Lambda_c+ K- pi+]cc" , 
            " [ Xi_cc+ -> Lambda_c+ K+ pi-]cc"
            ] ,
            ##
            DaughtersCuts = {
            'K-'        : self [ 'TrackCuts' ] + ' & HASRICH '                    ,
            'pi+'       : self [ 'TrackCuts' ] + ' & HASRICH ' + self['PionCuts'] 
            } ,
            ##
            CombinationCut = """
            ( AM  < 6 * GeV ) &
            ( APT > %s      ) 
            """ %  ( 0.95 * self[ 'pT(Xicc+)'] ) ,
            ## 
            MotherCut      = """
            ( PT      > %s              ) &
            ( chi2vx  < 16              ) &
            ( ctau_16 > 50 * micrometer ) 
            """ %        self[ 'pT(Xicc+)' ] , 
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } 
            #
            )
        
        # convert it to selection
        sel = Selection (
            "SelXicc+For"       +   self.name()       ,
            Algorithm           =   cmb               ,
            RequiredSelections  = [ self.LamC4Xicc () ,
                                    StdLooseKaons     ,
                                    StdLoosePions     ]
            )
        return self._add_selection( 'Xicc+ForPromptCharm_Selection' ,  sel )


    # ===========================================================================
    ## try to reconstruct   XiccPP
    def XiccPP  ( self ) :
        """
        Try to reconstruct   Xicc++ -> Lambda_c+ K- pi+ pi+
        """
        sel = self._selection ( 'Xicc++ForPromptCharm_Selection' )
        if sel : return sel
        
        cmb = CombineParticles(
            ##
            Preambulo = self.preambulo() , 
            ##
            DecayDescriptors = [
            " [ Xi_cc++ -> Lambda_c+ K- pi+ pi+]cc" , 
            " [ Xi_cc++ -> Lambda_c+ K+ pi- pi-]cc" 
            ] ,
            ##
            DaughtersCuts = {
            'K-'        : self [ 'TrackCuts' ] + ' & HASRICH '                    ,
            'pi+'       : self [ 'TrackCuts' ] + ' & HASRICH ' + self['PionCuts'] 
            } ,
            ##
            CombinationCut = """
            ( AM  < 6 * GeV ) &
            ( APT > %s      ) 
            """ %  ( 0.95 * self[ 'pT(Xicc++)'] ) ,
            MotherCut      = """
            ( PT      > %s              ) &
            ( chi2vx  < 36              ) &
            ( ctau_16 > 50 * micrometer ) 
            """ %        self[ 'pT(Xicc++)'] ,
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } 
            #
            )
        
        # convert it to selection
        sel = Selection (
            "SelXicc++For"      +   self.name()       ,
            Algorithm           =   cmb               ,
            RequiredSelections  = [ self.LamC4Xicc () ,
                                    StdLooseKaons     ,
                                    StdLoosePions     ]
            )
        return self._add_selection( 'Xicc++ForPromptCharm_Selection' ,  sel )
     
    ## helper merged selection of all charmed particles
    def PromptCharm ( self ) :
        """
        Helper merged selection of all charmed particles
        """
        sel = self._selection ( 'PromptCharm_Selection' )
        if sel : return sel

        sel =  MergedSelection (
            ##
            'PromptCharmFor' + self.name ()      ,
            RequiredSelections = [ self.D02HH () ,
                                   self.Dstar () ,
                                   self.Ds    () ,
                                   self.Dplus () ,
                                   self.LamC  () ]
            )

        return self._add_selection ( 'PromptCharm_Selection' , sel )

    # =========================================================================
    ## Lambda_c + Ds selection 
    def DsLamC ( self ) :
        """
        Get Lambda_c + Ds 
        """
        sel = self._selection ( 'DsLamC_Selection' )
        if sel : return sel
        
        cmb = CombineParticles (
            ##
            DecayDescriptors = [
            "[ Lambda_b0 -> Lambda_c+ D_s- ]cc" ,
            "[ Lambda_b0 -> Lambda_c+ D_s+ ]cc" ,
            ] , 
            ##
            DaughtersCuts  = {
            'Lambda_c+'   : " PT > %s " % self['pT(Lc+) for Lb'] , 
            'D_s-'        : " PT > %s " % self['pT(Ds+) for Lb'] 
            } ,
            ##
            CombinationCut = """
            AALL
            """,
            ##
            MotherCut      = """
            ( chi2vx  < 100 )  
            """ ,
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } 
            #
            )
        
        sel = Selection  (
            "SelDsLamCFor"     +   self.name()     ,
            Algorithm          =   cmb             ,
            RequiredSelections = [ self.preLamC () ,
                                   self.preDs   () ]
            )
        
        return self._add_selection( 'DsLamc_Selection' , sel )
    

    ## selection of W+-
    def W ( self )  :
        """
        Get simple  W+-selection
        """

        sel = self._selection ( 'W_Selection' )
        if sel : return sel

        _Filter = FilterDesktop (
            ##
            Preambulo = self.preambulo () + [ 
            "ptCone_ = SUMCONE (   0.25 , PT , '/Event/Phys/StdAllLoosePions/Particles'   )",
            "etCone_ = SUMCONE (   0.25 , PT , '/Event/Phys/StdLooseAllPhotons/Particles' )",
            "ptCone  =   SINFO (  55001 , ptCone_ , True ) " ,
            "etCone  =   SINFO (  55002 , etCone_ , True ) " ,
            ] ,
            ##
            Code = self [ 'WCuts'] + """
            & ( -100 * GeV < ptCone ) 
            & ( -100 * GeV < etCone ) 
            """ 
            )

        sel = Selection  (
            "SelWFor" + self._name  ,
            Algorithm = _Filter ,
            ## ATTENTION! prompt muons are here!
            RequiredSelections = [ StdAllLooseMuons ]
            )

        return self._add_selection ( 'W_Selection' , sel )

    ## get "Di-Charm"-selection
    def DiCharm ( self ) :
        """
        Di-Charm selection
        """
        sel = self._selection ( 'DiCharm_Selection' )
        if sel : return sel

        ## prepare Di-charm
        cmb = CombineParticles (
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
            Preambulo = self.preambulo () ,
            ## combination cut : accept all
            CombinationCut = " AALL " ,
            ##      mother cut : accept all
            MotherCut      = "  ALL " , 
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } 
            #
            )

        sel = Selection (
            ##
            'SelDiCharmFor'    + self.name() ,
            ##
            Algorithm          = cmb    ,
            ##
            RequiredSelections = [ self.PromptCharm () ]
            )

        return self._add_selection ( 'DiCharm_Selection' , sel )

    ## charm & W
    def CharmAndW ( self ) :
        """
        Charm & W+-
        """
        sel = self._selection ( 'CharmW_Selection' )
        if sel : return sel

        ## prepare Di-charm
        cmb = CombineParticles (
            ## the decays to be reconstructed
            DecayDescriptors = [
            #
            # charm-anti-charm
            #
            " [ chi_b0(2P) -> D0        mu+ ]cc " ,
            " [ chi_b0(2P) -> D0        mu- ]cc " ,
            " [ chi_b0(2P) -> D*(2010)+ mu+ ]cc " ,
            " [ chi_b0(2P) -> D*(2010)+ mu- ]cc " ,
            " [ chi_b0(2P) -> D+        mu+ ]cc " ,
            " [ chi_b0(2P) -> D+        mu- ]cc " ,
            " [ chi_b0(2P) -> D_s+      mu+ ]cc " ,
            " [ chi_b0(2P) -> D_s+      mu- ]cc " ,
            " [ chi_b0(2P) -> Lambda_c+ mu+ ]cc " ,
            " [ chi_b0(2P) -> Lambda_c+ mu- ]cc " ,
            ] ,
            ##
            Preambulo = self.preambulo() ,
            ## combination cut : accept all
            CombinationCut = " AALL " ,
            ##      mother cut : accept all
            MotherCut      = "  ALL " , 
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } 
            #
            )

        sel = Selection (
            ##
            'SelCharmWFor'    + self.name() ,
            ##
            Algorithm          = cmb    ,
            ##
            RequiredSelections = [ self . W           () ,
                                   self . PromptCharm () ]
            )

        return self._add_selection ( 'CharmW_Selection' , sel )

    ## get the dimuons
    def DiMuon ( self ) :
        """
        Get the dimuons
        """
        sel = self._selection ( 'DiMuon_Selection' )
        if sel : return sel

        ## prepare Di-muons
        cmb = CombineParticles (
            ## the decays to be reconstructed
            DecayDescriptor = 'J/psi(1S) -> mu+ mu-' ,
            DaughtersCuts   = {
            'mu+' : self.muonCuts()
            } ,
            Preambulo       = self.preambulo() ,
            ## combination cut
            CombinationCut  = " psi | psi_prime | ( AM > 8 * GeV ) " ,
            ##      mother cut
            MotherCut       = " chi2vx < 20 " ,
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } 
            #
            )

        ##
        sel = Selection (
            ##
            'SelDiMuonFor'     + self.name() ,
            ##
            Algorithm          = cmb         ,
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

        cmb = CombineParticles (
            DecayDescriptors = [
            "[ Upsilon(1S) -> J/psi(1S) D0        ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) D*(2010)+ ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) D+        ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) D_s+      ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) Lambda_c+ ]cc"
            ] ,
            Preambulo      = self.preambulo() , 
            CombinationCut = " AALL " ,
            MotherCut      = "  ALL " , 
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } 
            #
            )

        sel = Selection  (
            "SelDiMuonAndCharmFor" +   self.name()         ,
            Algorithm              =   cmb                 ,
            RequiredSelections     = [ self.DiMuon      () ,
                                       self.PromptCharm () ]
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

        cmb = CombineParticles (
            ##
            DecayDescriptors = [
            "[ Upsilon(1S) -> J/psi(1S) D0        gamma ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) D*(2010)+ gamma ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) D+        gamma ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) D_s+      gamma ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) Lambda_c+ gamma ]cc"
            ] ,
            ##
            Preambulo = self.preambulo () ,
            ##
            DaughtersCuts = {
            #                    J/psi                              Upsilon(1S)
            "J/psi(1S)" : " ( M  < 3.21 * GeV ) | in_range ( 9.3 * GeV , M , 9.6 * GeV ) " ,
            "gamma"     : self [ 'GammaChi' ]
            },
            ## require chi_(c,b)
            CombinationCut = """
            AM13 - AM1 < 1.01 * GeV
            """,
            ##
            MotherCut      = "  ALL " , 
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } 
            #
            )
        ##
        sel_ = Selection (
            "ChiAndCharmPreFor" +   self.name()            ,
            Algorithm           =   cmb                    ,
            RequiredSelections  = [ self.DiMuonAndCharm () , ## fake one!
                                    self.DiMuon         () ,
                                    self.PromptCharm    () ,
                                    StdLooseAllPhotons     ]
            )
        ## apply pi0-veto-tagger !
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger
        tag = Pi0Veto__Tagger (
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25010     ## unique !
            )
        ## the final selection
        sel = Selection  (
            "SelChiAndCharmFor" +   self.name()  ,
            Algorithm           =   tag          ,
            RequiredSelections  = [ sel_         ]
            )

        return self._add_selection( 'ChiAndCharm_Selection' , sel )

    ## DiMuon & W
    def DiMuonAndW ( self ) :
        """
        dimuon & W+-
        """
        sel = self._selection ( 'DiMuon&W_Selection' )
        if sel : return sel

        ## prepare Di-charm
        cmb = CombineParticles (
            ## the decays to be reconstructed
            DecayDescriptors = [
            #
            # dimuon and W
            #
            " [ chi_b0(2P) -> J/psi(1S) mu+ ]cc " ,
            ] ,
            ##
            Preambulo = self.preambulo () ,
            ## combination cut : accept all
            CombinationCut = " AALL " ,
            ##      mother cut : accept all
            MotherCut      = "  ALL " , 
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } 
            #
            )

        sel = Selection (
            ##
            'SelDiMuonAndWFor' + self.name() ,
            ##
            Algorithm          = cmb    ,
            ##
            RequiredSelections = [ self . W      () ,
                                   self . DiMuon () ]
            )

        return self._add_selection ( 'DiMuon&W_Selection' , sel )

    ## get the dimuons & charn
    def ChiAndW ( self ) :
        """
        Get chi & W :
        Select events with at leats one charm particle and
        at least one chi
        """
        sel = self._selection ( 'Chi&W_Selection' )
        if sel : return sel

        cmb = CombineParticles (
            ##
            DecayDescriptor = "[ chi_b1(2P) -> J/psi(1S) gamma mu+ ]cc" ,
            ##
            DaughtersCuts = {
            "J/psi(1S)" : " ( M  < 3.21 * GeV ) | in_range ( 8.5 * GeV , M , 12.0 * GeV ) " ,
            "gamma"     : self [ 'GammaChi' ]
            },
            ##
            Preambulo  = self.preambulo() ,
            ## require chi_(c,b)
            CombinationCut = """
            AM12 - AM1 < 1.01 * GeV
            """,
            ##
            MotherCut      = "  ALL " , 
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } 
            #
            )
        ##
        sel_ = Selection (
            "ChiAndWPreFor" +   self.name()                ,
            Algorithm           =   cmb                    ,
            RequiredSelections  = [ self.DiMuonAndW     () , ## fake one!
                                    self.W              () ,
                                    self.DiMuon         () ,
                                    StdLooseAllPhotons     ]
            )
        ## apply pi0-veto-tagger !
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger
        tag = Pi0Veto__Tagger (
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25015     ## unique !
            )
        ## the final selection
        sel = Selection  (
            "SelChiAndWFor    " +   self.name()  ,
            Algorithm           =   tag          ,
            RequiredSelections  = [ sel_         ]
            )

        return self._add_selection( 'Chi&W_Selection' , sel )

    ## get DsPsi
    def DsPsi( self ) :
        """
        Get Ds and J/psi :
        """
        sel = self._selection ( 'DsPsi_Selection' )
        if sel : return sel

        cmb = CombineParticles (
            ##
            DecayDescriptor = "[ B_c+ -> J/psi(1S) D_s+ ]cc" ,
            ##
            Preambulo      = self.preambulo() ,
            DaughtersCuts  = {
            "J/psi(1S)" : "M < 4.0 * GeV " ,
            "D_s+"      : "PT > %s " % self['pT(Ds+) for Bc+'],
            } ,
            ##
            CombinationCut = """
            AM < 7.1 * GeV
            """,
            ##
            MotherCut      = """
            ( chi2vx < 16 )
            """, 
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } 
            #
            )

        sel = Selection  (
            "SelDsPsiFor"      +   self.name()     ,
            Algorithm          =   cmb             ,
            RequiredSelections = [ self.DiMuon  () ,
                                   self.preDs   () ]
            )

        return self._add_selection( 'DsPsi_Selection' , sel )


# =============================================================================
if '__main__' == __name__ :

    print 80*'*'
    print __doc__
    print ' Author :  %s' % __author__
    print ' Date   :  %s' % __date__
    print ' The output locations for default configuration: '
    _conf = StrippingPromptCharmConf ( 'PromptCharm' , config = {}  )
    for l in _conf.lines() :
        print ' \t ', l.outputLocation  () , l
    print 80*'*'

# =============================================================================
# The END
# =============================================================================

