#!/usr/bin/env python
# =============================================================================
# $Id: StrippingPromptCharm.py 177629 2014-09-11 14:03:00Z ibelyaev $
# =============================================================================
## @file
#
#  The attempt for coherent stripping of all stable charm hadrons :
#
#    - D0        -> K pi ,
#    - D+        -> K pi pi
#    - Ds+/D+    -> ( phi -> K K ) pi
#    - D*+       -> ( D0 -> K pi ) pi+
#    
#    - D0        -> KK, pipi                 ## for Eva 
#    - D*+       ->  ( D0 -> KK , pipi) pi+  ## for Eva 
#    
#    - Lambda_c+/Xi_c -> p K pi              ## for Lesha Dziuba  
#    - Lambda_c+/Xi_c -> p K K               ## for Lesha Dziuba 
#    - Xic0           -> Lc+ pi-             ## for Lesha Dziuba 
#    - Xi_c0 -> p K K pi                     ## for Marco Pappagallo
#    - Omega_c0 -> p K K pi                  ## for Marco Pappagallo  
#    - Xi'_c+ -> Xi_c+ gamma                 ## for Marco Pappagallo 
#    - Xi_c+* -> Xi_c0 pi+                   ## for Marco Pappagallo
# 
#  Excited charm baryons 
#
#    - Sigma_c(0,++) -> Lambda_c+ pi        ( 2455 & 2520 )
#    - Xi_c0*        -> Xi_c+ pi                            
#    - Lambda_c+*    -> Lambda_c+ pi+ pi-   ( 2595, 2625 & 2880 )
#    - Omega_c0*     -> Lambda_c+/Xic+ K-    ## for Marco Pappagallo 
#    - Omega_c0*     -> Xic0 pi+ K-          ## for Marco Pappagallo
#    - Omega_c0*     -> Xic'+ K-             ## for Marco Pappagallo
#    - Omega_cc+     -> Xic+ pi+ K-          ## for Marco Pappagallo
#    - Omega_cc+     -> Xic+ K+ K-           ## for Marco Pappagallo
#
#  Also one has the combinations for 2xCharm studies
#
#    - Charm +   Charm            ( both Charm/Charm and Charm/anti-Charm )
#    - Charm +   Dimuon
#    - Charm + ( Dimuon + gamma ) ( get for free chi_(c,b) )
#
#  Associative production of  charm & W+ :
#
#    - Charm     & W+
#    - dimuons   & W+
#    - chi_(c,b) & W+
#
#  The cuts more or less correspond to D*+ selection by Alexandr Kozlinzkiy.
#  In addition the PT-cut for the long-lived charmed particle has been applied.
#  Thanks to Marco Gersabeck & Harry Cliff for nice idea.
#
#
# Usage:
#
# @code
#
#   stream = ...
#
#   from StrippingSelections.StirppingPromptCharm import StrippingPromptCharmConf
#   from StrippingSelections.StirppingPromptCharm import default_config 
#
#   conf = default_config['CONFIG']
#   name = default_config['NAME'  ]
#   
#   builder = StrippingPromptCharmConf ( name , config = conf )
#
#   stream.appendLines ( builder.lines() )
#
# @endcode
#
# @todo Increment PT-threshold for D0,D+,D_s+, Lambda_c+
#
#                   $Revision: 177629 $
# Last modification $Date: 2013-03-26 18:58:29 +0100
#                by $Author: ibelyaev $
# =============================================================================
"""The attempt for coherent stripping of all stable charm hadrons :
   
    - D0        -> K pi ,
    - D+        -> K pi pi
    - Ds+/D+    -> ( phi -> K K ) pi
    - D*+       -> ( D0 -> K pi ) pi+
    
    - D0        -> KK, pipi                 ## for Eva 
    - D*+       ->  ( D0 -> KK , pipi) pi+  ## for Eva 
    
    - Lambda_c+/Xi_c -> p K pi              ## for Lesha Dziuba  
    - Lambda_c+/Xi_c -> p K K               ## for Lesha Dziuba 

    Excited charm baryons 

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

  stream = ...
  
  from StrippingSelections.StirppingPromptCharm import StrippingPromptCharmConf
  from StrippingSelections.StirppingPromptCharm import default_config 
  
  conf = default_config['CONFIG']
  name = default_config['NAME'  ]
  
  builder = StrippingPromptCharmConf ( name , config = conf )
  
  stream.appendLines ( builder.lines() )

  
                  $Revision: 177629 $
Last modification $Date: 2013-03-26 18:58:29 +0100
               by $Author: ibelyaev $

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = '2010-08-03'
__version__ = '$Revision: 177629 $'
# =============================================================================
__all__ = (
    'StrippingPromptCharmConf',
    'default_config' 
    )
# =============================================================================
from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits             import GeV, MeV, mm
from StrippingUtils.Utils                  import LineBuilder
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
    # use for simulation:
    'NOPIDHADRONS'   : False   , 
    #
    ## PT-cuts
    ## attention: with 1GeV pt-cut prescale is needed for D0,D+,D*+ and Ds
    #
    'pT(D0)'     :  1.0 * GeV ,    ## pt-cut for  prompt   D0
    'pT(D+)'     :  1.0 * GeV ,    ## pt-cut for  prompt   D+
    'pT(Ds+)'    :  1.0 * GeV ,    ## pt-cut for  prompt   Ds+
    'pT(Lc+)'    :  1.0 * GeV ,    ## pt-cut for  prompt   Lc+
    'pT(Xic0)'   :  1.0 * GeV ,    ## pt-cut for  prompt   Xic0/Omegac0
    'pT(Omgcc)'  :  1.0 * GeV ,    ## pt-cut for  prompt   Omegacc
    #
    'pT(D0->HH)' :  1.0 * GeV ,    ## pt-cut for  prompt   D0->KK,pipi models 
    #
    # Selection of basic particles
    #
    'TrackCut' : """
    ( CLONEDIST   > 5000      ) &
    ( TRGHOSTPROB < 0.5       ) &
    in_range ( 2  , ETA , 4.9 ) &
    HASRICH
    """ ,
    # 
    'PionCut'   : """
    ( PT          > 250 * MeV ) & 
    ( CLONEDIST   > 5000      ) & 
    ( TRGHOSTPROB < 0.5       ) &
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                     &
    ( MIPCHI2DV()  > 9        )
    """ ,
    #
    'KaonCut'   : """
    ( PT          > 250 * MeV ) & 
    ( CLONEDIST   > 5000      ) & 
    ( TRGHOSTPROB < 0.5       ) &
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                     &
    ( MIPCHI2DV()  > 9        )
    """ ,
    #
    'ProtonCut'   : """
    ( PT           > 250 * MeV ) & 
    ( CLONEDIST    > 5000      ) & 
    ( TRGHOSTPROB  < 0.5       ) & 
    in_range ( 2         , ETA , 4.9       ) &
    in_range ( 10 * GeV  , P   , 150 * GeV ) &
    HASRICH                      &
    ( MIPCHI2DV()  > 9         ) 
    """ ,
    ##
    'MuonCut'   : """
    ISMUON &
    in_range ( 2 , ETA , 4.9     ) &
    ( PT            >  550 * MeV ) &
    ( PIDmu - PIDpi >    0       ) &
    ( CLONEDIST     > 5000       )     
    """ ,
    ## cust for prompt kaon
    'PromptKaonCut'   : """
    ( CLONEDIST   > 5000         ) & 
    ( TRGHOSTPROB < 0.5          ) &
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                     
    """ ,
    #
    ## PID-cuts for hadrons 
    #
    'PionPIDCut'   : " PROBNNpi > 0.1 " ,
    'KaonPIDCut'   : " PROBNNk  > 0.1 " ,
    'ProtonPIDCut' : " PROBNNp  > 0.1 " ,
    'PhotonCLCut'  : 0.05,
    ##
    #
    ## photons from chi_(c,b)
    #
    'GammaChi'        : " ( PT > 400 * MeV ) & ( CL > 0.05 ) " ,
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
    "ctau_400 = BPVLTIME ( 400 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<400
    "ctau_no  = BPVLTIME (     ) * c_light "  , ## no embedded cut for chi2(lifetimeFit)
    # dimuons:
    "psi           =   ADAMASS ('J/psi(1S)') < 150 * MeV"  ,
    "psi_prime     =   ADAMASS (  'psi(2S)') < 150 * MeV"  ,
    ] ,
    # Q Values:
    'QvalueXiCK'     :  500 * MeV ,
    'QvalueXiCprime' :  250 * MeV ,
    'QvalueXiCstar'  :  150 * MeV ,
    'QvalueXiCPiK'   :  500 * MeV ,
    'QvalueXiCprimeK':  500 * MeV ,
    'QvalueOmegaCC'  : 4500 * MeV ,
    ## monitoring ?
    'Monitor'     : False ,
    ## pescales
    'D0Prescale'               : 1.0 ,
    'D*Prescale'               : 1.0 ,
    'DsPrescale'               : 1.0 ,
    'D+Prescale'               : 1.0 ,
    'LambdaCPrescale'          : 1.0 ,
    'XiC0Prescale'             : 1.0 ,
    'OmegaC0Prescale'          : 1.0 ,
    'LambdaCpKKPrescale'       : 1.0 ,
    'LambdaC*Prescale'         : 1.0 ,
    'OmegaC*Prescale'          : 1.0 ,
    'XiCprimePrescale'         : 1.0 ,
    'XiC*Prescale'             : 1.0 ,
    'OmegaC*2XiCPiKPrescale'   : 1.0 ,
    'OmegaC*2XiCprimeKPrescale': 1.0 ,
    'OmegaCCPrescale'          : 1.0 ,
    'SigmaCPrescale'           : 1.0 ,
    'Xic02LcPiPrescale'        : 1.0 ,
    #
    'OmegaCCKpiPrescale'       : 1.0 ,
    'OmegaCCKKPrescale'        : 1.0 ,
    ##
    'D02KKPrescale'            : 1.0 ,
    'D02pipiPrescale'          : 1.0 ,
    'D*CPPrescale'             : 1.0 ,
    ##
    'DiCharmPrescale'          : 1.0 ,
    'DiMu&CharmPrescale'       : 1.0 ,
    'DoubleDiMuPrescale'       : 1.0 ,
    'Chi&CharmPrescale'        : 1.0 ,
    'Charm&WPrescale'          : 1.0 ,
    'DiMuon&WPrescale'         : 1.0 ,
    'Chi&WPrescale'            : 1.0 ,
    ## ========================================================================
    }
# =============================================================================
## the mandatory element for stripping framework 
default_config = {
    #
    'NAME'        :   'PromptCharm'       ,
    'WGs'         : [ 'BandQ' , 'Charm' ] ,
    'CONFIG'      : _default_configuration_  , 
    'BUILDERTYPE' :   'StrippingPromptCharmConf'            ,
    'STREAMS'     : { 'Charm'    : [ 'StrippingD02KpiForPromptCharm'              , 
                                     'StrippingDstarForPromptCharm'               , 
                                     'StrippingDForPromptCharm'                   , 
                                     'StrippingDsForPromptCharm'                  ,
                                     'StrippingLambdaCForPromptCharm'             ,
                                     'StrippingXiC0ForPromptCharm'                ,
#                                     'StrippingOmegaC0ForPromptCharm'             ,
                                     'StrippingLambdaC2pKKForPromptCharm'         ,
                                     'StrippingSigmaCForPromptCharm'              ,
                                     'StrippingLambdaCstarForPromptCharm'         ,
                                     'StrippingOmegaCstarForPromptCharm'          ,
                                     'StrippingXiCprimeForPromptCharm'            ,
                                     'StrippingXiCstarForPromptCharm'             ,
                                     'StrippingOmegaCstar2XiCPiKForPromptCharm'   ,
                                     'StrippingOmegaCstar2XiCprimeKForPromptCharm',
                                     'StrippingOmegaCCKpiForPromptCharm'          ,
                                     'StrippingOmegaCCKKForPromptCharm'           ,
                                     'StrippingXic02LcPiForPromptCharm'           ,
                                     'StrippingDiCharmForPromptCharm'             , ## ? 
                                     'StrippingChiAndCharmForPromptCharm'         ,
                                     'StrippingCharmAndWForPromptCharm'           ,
                                     'StrippingDiMuonAndCharmForPromptCharm'      ,
                                     ## for Eva
                                     'StrippingD02KKForPromptCharm'               ,   ## prescale ?
                                     'StrippingD02pipiForPromptCharm'             ,   ## prescale ?
                                     'StrippingDstarCPForPromptCharm'           ] , 
                      ##
                      'CharmCompleteEvent' : [ 'StrippingOmegaC0ForPromptCharm'],
                      'Leptonic' : [ 'StrippingDoubleDiMuonForPromptCharm'   , ## Full DST ?
                                     'StrippingDiMuonAndWForPromptCharm'     , ## Full DST ? 
                                     'StrippingChiAndWForPromptCharm'        ] }
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
                raise KeyError ("Invalid key is specified: '%s'" % key )

            val = _config[key]
            if val != _default_configuration_ [ key ] :
                logger.debug ('new configuration: %-16s : %s ' % ( key , _config[key] ) )

        ## cehck for prescales
        for keys in self.keys() :
            if 0 > key.find('Prescale') and 0 > key.find('prescale') :  continue
            if 1 != self[key] : logger.warning ( '%s is %s' % ( key , self[key] ) )

        for line in self._lines_charm() :
            self.registerLine(line)
            logger.debug ( "Register line: %s" %  line.name () )

    # =========================================================================
    ## pure technical method for creation of selections
    # =========================================================================
    def make_selection ( self      ,
                         tag       , 
                         algotype  ,
                         inputs    , 
                         *args     ,
                         **kwargs  ) :
        """
        Technical method for creation of 1-step selections 
        """
        sel_tag  = '%s_Selection' % tag
        sel_name = 'Sel%sFor%s'   % ( tag , self.name() )
        #
        ## check existing selection
        #
        sel      = self._selection ( sel_tag )
        if sel : return sel 
        
        #
        ## adjust a bit the arguments
        if not kwargs.has_key('Preambulo')           :
            kwargs ['Preambulo'        ] = self['Preambulo']

        if not kwargs.has_key ( 'ParticleCombiners' ) :
            kwargs ['ParticleCombiners'] = { '' : 'LoKi::VertexFitter:PUBLIC' }

        #
        ## use "simple-selection"
        #
        from PhysSelPython.Wrappers import SimpleSelection
        sel = SimpleSelection (
            sel_name ,
            algotype ,
            inputs   , 
            *args    ,
            **kwargs )
        # 
        return self._add_selection( sel_tag , sel ) 
                           
    ## get the selections
    def _selections_private ( self ) :

        sel = self._selection ( 'Selections' )
        if sel : return sel

        sel =  [ self.D02Kpi         () ,
                 self.Dstar          () ,
                 self.Ds             () ,
                 self.Dplus          () ,
                 self.preLamC        () ,
                 self.LamC           () ,
                 self.preXiC0        () ,
                 self.XiC0           () ,
                 self.OmegaC0        () ,
                 self.LamC2pKK       () ,
                 self.SigC           () ,
                 self.LamCstar       () ,
                 self.OmgCstar         () ,
                 self.XiCprime         () ,
                 self.XiCstar          () ,
                 self.OmgCstar2XiCPiK  () ,
                 self.OmgCstar2XiCprimeK  () ,
                 self.OmgCCKpi () ,
                 self.OmgCCKK  () ,
                 self.Xic02LcPi      () ,
                 self.DiMuon         () ,
                 self.DiCharm        () ,
                 self.DoubleDiMuon   () ,
                 self.ChiAndCharm    () ,
                 self.W              () ,
                 self.CharmAndW      () ,
                 self.DiMuonAndW     () ,
                 self.ChiAndW        () ,
                 ## for Eva
                 self.D02KK          () ,
                 self.D02pipi        () ,
                 self.D02CP          () ,
                 self.DstarCP        () ,
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
        from StrippingConf.StrippingLine           import StrippingLine
        #
        sel = [
            ##
            StrippingLine (
            "D02KpiFor" + self.name()         ,
            prescale = self['D0Prescale'   ]  , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'      ]  ,
            algos    =     [ self.D02Kpi() ]  ,
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
            "DFor" + self.name() ,
            prescale = self['D+Prescale'   ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'      ] ,
            algos    =     [ self.Dplus () ]
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
            "LambdaCFor" + self.name() ,
            prescale = self['LambdaCPrescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'        ] ,
            algos    =     [ self.LamC ()    ]
            ) ,
            ## Xi_c0
            StrippingLine (
            "XiC0For" + self.name() ,
            prescale = self['XiC0Prescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'        ] ,
            algos    =     [ self.XiC0 ()    ]
            ) ,
            ## Omega_c0
            StrippingLine (
            "OmegaC0For" + self.name() ,
            prescale = self['OmegaC0Prescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'        ] ,
            algos    =     [ self.OmegaC0 () ]
            ) ,
            ##
            StrippingLine (
            "LambdaC2pKKFor" + self.name() ,
            prescale = self['LambdaCpKKPrescale' ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'            ] ,
            algos    =     [ self.LamC2pKK ()    ]
            ) ,
            ## Sigma_c
            StrippingLine (
            "SigmaCFor" + self.name() ,
            prescale = self['SigmaCPrescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'       ] ,
            algos    =     [ self.SigC ()   ]
            ) ,
            ## Omega_c* -> Xic+ K-
            StrippingLine (
            "OmegaCstarFor" + self.name() ,
            prescale = self['OmegaC*Prescale' ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'         ] ,
            algos    =     [ self.OmgCstar () ]
            ) ,
            ## Xi'_c+
            StrippingLine (
            "XiCprimeFor" + self.name() ,
            prescale = self['XiCprimePrescale' ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'         ] ,
            algos    =     [ self.XiCprime () ]
            ) ,
            ## Xi*_c+
            StrippingLine (
            "XiCstarFor" + self.name() ,
            prescale = self['XiC*Prescale' ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'         ] ,
            algos    =     [ self.XiCstar () ]
            ) ,
            ## Omega_c* -> Xic0 pi+ K-
            StrippingLine (
            "OmegaCstar2XiCPiKFor" + self.name() ,
            prescale = self['OmegaC*2XiCPiKPrescale' ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'         ] ,
            algos    =     [ self.OmgCstar2XiCPiK () ]
            ) ,
            ## Omega_c* -> Xic+' K-
            StrippingLine (
            "OmegaCstar2XiCprimeKFor" + self.name() ,
            prescale = self['OmegaC*2XiCprimeKPrescale' ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'         ] ,
            algos    =     [ self.OmgCstar2XiCprimeK () ]
            ) ,
            ## Omega_cc -> Xic+  K- pi+
            StrippingLine (
            "OmegaCCKpiFor" + self.name() ,
            prescale = self['OmegaCCKpiPrescale' ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'            ] ,
            algos    =     [ self.OmgCCKpi ()    ]
            ) ,
            ## Omega_cc -> Xic+  K- K+
            StrippingLine (
            "OmegaCCKKFor" + self.name() ,
            prescale = self['OmegaCCKKPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'            ] ,
            algos    =     [ self.OmgCCKK ()     ]
            ) ,
            ## Lambda_c*
            StrippingLine (
            "LambdaCstarFor" + self.name() ,
            prescale = self['LambdaC*Prescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'         ] ,
            algos    =     [ self.LamCstar () ]
            ) ,
            ## Xic0 -> Lc pi 
            StrippingLine (
            "Xic02LcPiFor" + self.name() ,
            prescale = self['Xic02LcPiPrescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'          ] ,
            algos    =     [ self.Xic02LcPi () ]
            ) ,
            ## DiCharm
            StrippingLine (
            "DiCharmFor" + self.name() ,
            prescale = self['DiCharmPrescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'        ] ,
            algos    =     [ self.DiCharm () ] , 
            MDSTFlag = True                          ## keep event in MDST.DST 
            ) ,
            ##
            StrippingLine (
            "DiMuonAndCharmFor" + self.name()         ,
            prescale = self['DiMu&CharmPrescale'    ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'               ] ,
            algos    =     [ self.DiMuonAndCharm () ] ,
            MDSTFlag = True                             ## keep event in MDST.DST 
            ) ,
            ##
            StrippingLine (
            "DoubleDiMuonFor" + self.name()         ,
            prescale = self['DoubleDiMuPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'             ] ,
            algos    =     [ self.DoubleDiMuon () ] , 
            MDSTFlag = True                          ## keep event in MDST.DST 
            ) ,
            ##
            StrippingLine (
            "ChiAndCharmFor" + self.name() ,
            prescale = self['Chi&CharmPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'            ] ,
            algos    =     [ self.ChiAndCharm () ] , 
            MDSTFlag = True                          ## keep event in MDST.DST 
            ) ,
            ##
            StrippingLine (
            "CharmAndWFor" + self.name() ,
            prescale   = self['Charm&WPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV    = self['CheckPV'          ] ,
            algos      =     [ self.CharmAndW () ] , 
            MDSTFlag   = True                        ## keep event in MDST.DST 
            ) ,
            ##
            StrippingLine (
            "DiMuonAndWFor" + self.name() ,
            prescale   = self['DiMuon&WPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV    = self['CheckPV'           ] ,
            algos      =     [ self.DiMuonAndW () ] ,
            MDSTFlag   = True                         ## keep event in MDST.DST 
            ) ,
            ##
            StrippingLine (
            "ChiAndWFor" + self.name() ,
            prescale   = self['Chi&WPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV    = self['CheckPV'        ] ,
            algos      =     [ self.ChiAndW () ] , 
            MDSTFlag   = True                       ## keep event in MDST.DST 
            ) ,
            #
            ## For Eva
            #
            ## D0 -> K- K+ 
            StrippingLine (
            "D02KKFor" + self.name() ,
            prescale = self['D02KKPrescale'   ] ,  ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'         ] ,
            algos    =     [ self.D02KK ()    ]
            ) ,
            ## D0 -> pi- pi+ 
            StrippingLine (
            "D02pipiFor" + self.name() ,
            prescale = self['D02pipiPrescale' ] ,  ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'         ] ,
            algos    =     [ self.D02pipi ()  ]
            ) ,
            ##
            ## D*+ -> ( D0 -> K- K+ , pi- pi+ ) pi+
            StrippingLine (
            "DstarCPFor" + self.name() ,
            prescale = self['D*CPPrescale'    ] ,  ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'         ] ,
            algos    =     [ self.DstarCP ()  ]
            ) ,
            ]
        #
        return self._add_selection ( 'CharmLines' , sel )


    ## get all stripping lines
    def _lines_private ( self ) :
        """
        get Charm stripping lines
        """
        return self._lines_charm ()
    
    def muonCuts     ( self ) : return self [ 'MuonCut' ]
    
    # ========================================================================
    ## muons:
    # ========================================================================
    def muons    ( self ) :
        """
        Muons 
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from StandardParticles                     import StdAllLooseMuons as inpts 
        ##
        return self.make_selection (
            'Muon'                 ,
            FilterDesktop          ,
            [ inpts ]              ,
            Code = self['MuonCut'] ,
            )

    # ========================================================================
    ## pions :
    # ========================================================================
    def pions    ( self ) :
        """
        Pions
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        ##
        if self['NOPIDHADRONS'] :
            from StandardParticles import   StdNoPIDsPions as inpts
            pioncut = self['PionCut']
        else                    :
            from StandardParticles import StdLooseANNPions as inpts
            pioncut = "(%s)&(%s)" % ( self['PionCut'] , self['PionPIDCut'] ) 
        ##
        return self.make_selection (
            'Pion'                 ,
            FilterDesktop          ,
            [ inpts ]              ,
            Code = pioncut         ,
            )
    
    # ========================================================================
    ## kaons 
    # ========================================================================
    def kaons     ( self ) :
        """
        Kaons for   B -> psi X lines 
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        #
        if self['NOPIDHADRONS'] :
            from StandardParticles import   StdNoPIDsKaons as inpts
            kaoncut = self['KaonCut']
        else                    :
            from StandardParticles import StdLooseANNKaons as inpts 
            kaoncut = "(%s)&(%s)" % ( self['KaonCut'] , self['KaonPIDCut'] ) 
        #
        return self.make_selection (
            'Kaon'                 ,
            FilterDesktop          ,
            [ inpts ]              ,
            Code = kaoncut         ,
            )

    # ========================================================================
    ## protons :
    # ========================================================================
    def protons    ( self ) :
        """
        Protons for   b -> psi X lines 
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        ##
        if self['NOPIDHADRONS'] :
            from StandardParticles import   StdNoPIDsProtons as inpts
            protoncut = self['ProtonCut']
        else                    :
            from StandardParticles import StdLooseANNProtons as inpts 
            protoncut = "(%s)&(%s)" % ( self['ProtonCut'] , self['ProtonPIDCut'] ) 
        ##
        return self.make_selection (
            'Proton'                 ,
            FilterDesktop            ,
            [ inpts ]                ,
            Code = protoncut         ,
            )
    
    # =========================================================================
    ## Prompt slow pions for D*+, Sigma_c, Lambda_c**: essentially no cuts at all
    # =========================================================================
    def slowPions ( self ) :
        """
        Prompt slow pions for D*+, Sigma_c, Lamda_c**, ...
        - essentially no cuts at all
        """
        ##
        if self['NOPIDHADRONS'] : from StandardParticles import StdAllNoPIDsPions   as slow_pions 
        else                    : from StandardParticles import StdAllLooseANNPions as slow_pions  
        ##
        return slow_pions 
    

    # =========================================================================
    ## prompt kaons fro Omega_c*
    # =========================================================================
    def promptKaons ( self ) :
        """
        Prompt kaons for e.g. Omega_c* 
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        #
        if self['NOPIDHADRONS'] :
            from StandardParticles import   StdAllNoPIDsKaons as inpts
            kaoncut = self['PromptKaonCut']
        else                    :
            from StandardParticles import StdAllLooseANNKaons as inpts 
            kaoncut = "(%s)&(%s)" % ( self['PromptKaonCut'] , self['KaonPIDCut'] ) 
            #
        return self.make_selection (
            'PromptKaon'           ,
            FilterDesktop          ,
            [ inpts ]              ,
            Code = kaoncut         ,
            )
    
        
    ## get the common preambulo:
    def preambulo ( self ) : return self['Preambulo']
    
    
    # =========================================================================
    # D0 -> Kpi
    # =============================================================================
    def D02Kpi ( self ) :
        """
        Get D0->K-pi+
        """
        #
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
        #
        ## D0 -> K- pi+ 
        return self.make_selection (
            'D02Kpi'       ,
            ## the algorithm type
            CombineParticles  ,
            ## required selections
            [ self.kaons() , self.pions() ]  ,
            #
            ## algorithm configuration
            DecayDescriptor = "[D0  -> K-  pi+]cc",
            ## combination cut : wide mass-cut & PT-cut
            CombinationCut  = """
            ( ADAMASS('D0') < 85 * MeV ) &
            ( APT >  %s  )
            """ %  ( 0.95 * self['pT(D0)'] ) ,
            ## mother cut
            MotherCut      = """
            ( chi2vx < 9 )              &
            ( PT     > %s             ) &
            ( ADMASS('D0') < 75 * MeV ) &
            ( ctau > 0.1 * mm )
            """ %  self['pT(D0)'] 
            )           
    
    # =============================================================================
    # D0 -> KK
    # =============================================================================
    def D02KK ( self ) :
        """
        Get D0->K-K+
        """
        #
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
        #
        ## D0 -> K- K+ 
        return self.make_selection (
            'D02KK'           ,
            ## the algorithm type
            CombineParticles  ,
            ## required selections
            [ self.kaons() ]  ,
            #
            ## algorithm configuration
            DecayDescriptor = "D0  -> K- K+",
            ## combination cut : wide mass-cut & PT-cut
            CombinationCut  = """
            ( ADAMASS('D0') < 85 * MeV ) &
            ( APT >  %s  )
            """ %  ( 0.95 * self['pT(D0->HH)'] ) ,
            ## mother cut
            MotherCut      = """
            ( chi2vx < 9 )              &
            ( PT     > %s             ) &
            ( ADMASS('D0') < 75 * MeV ) &
            ( ctau > 0.1 * mm )
            """ %  self['pT(D0->HH)'] 
            )           

    # =============================================================================
    # D0 -> pipi
    # =============================================================================
    def D02pipi ( self ) :
        """
        Get D0->pi-pi+
        """
        #
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
        #
        ## D0 -> pi- pi+ 
        return self.make_selection (
            'D02pipi'           ,
            ## the algorithm type
            CombineParticles  ,
            ## required selections
            [ self.pions() ]  ,
            #
            ## algorithm configuration
            DecayDescriptor = "D0  -> pi- pi+",
            ## combination cut : wide mass-cut & PT-cut
            CombinationCut  = """
            ( ADAMASS('D0') < 85 * MeV ) &
            ( APT >  %s  )
            """ %  ( 0.95 * self['pT(D0->HH)'] ) ,
            ## mother cut
            MotherCut      = """
            ( chi2vx < 9 )              &
            ( PT     > %s             ) &
            ( ADMASS('D0') < 75 * MeV ) &
            ( ctau > 0.1 * mm )
            """ %  self['pT(D0->HH)'] 
            )           
    
    # =============================================================================
    # Merged selection of D0 -> KK and D0 -> pi pi
    # =============================================================================
    def D02CP ( self ) :
        """
        Merged selection of D0 -> KK and D0 -> pi pi
        """
        tag      = "D02CP"
        ##
        sel_tag  = '%s_Selection' % tag
        sel_name = 'Sel%sFor%s'   % ( tag , self.name() )
        #
        ## check existing selection
        #
        sel      = self._selection ( sel_tag )
        if sel : return sel
        #
        ## make merged selections 
        from PhysSelPython.Wrappers import MergedSelection
        sel = MergedSelection (
            sel_name , RequiredSelections = [ self.D02KK() , self.D02pipi() ] 
            )
        # 
        return self._add_selection( sel_tag , sel ) 
    
    # =============================================================================
    # D*+ -> (D0 -> Kpi) pi+ selection
    # =============================================================================
    def Dstar ( self ) :
        """
        D*+ -> (D0 -> K- pi+) pi+ selection
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        ##
        return self.make_selection (
            'Dstar'                   ,
            CombineParticles          ,
            [ self.D02Kpi() , self.slowPions() ] ,
            #
            ## algorithm properties 
            #
            DecayDescriptor = " [D*(2010)+ -> D0 pi+]cc" ,
            ##
            CombinationCut = """
            ( AM       < 2.5 * GeV ) &
            ( AM - AM1 < 165 * MeV )
            """ ,
            ##
            MotherCut      = """
            ( chi2vx < 64        ) &
            ( M - M1 < 155 * MeV )
            """ 
            )
    
    # =============================================================================
    # D*+ -> (D0 -> KK, pipi) pi+ selection
    # =============================================================================
    def DstarCP ( self ) :
        """
        D*+ -> (D0 -> K- K+ , pi- p+ ) pi+ selection
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        ##
        return self.make_selection (
            'DstarCP'                ,
            CombineParticles         ,
            [ self.D02CP() , self.slowPions() ] ,
            #
            ## algorithm properties 
            #
            DecayDescriptors = [
            " D*(2010)+ -> D0 pi+ " , 
            " D*(2010)- -> D0 pi- " , 
            ] ,
            ##
            CombinationCut = """
            ( AM       < 2.5 * GeV ) &
            ( AM - AM1 < 165 * MeV )
            """ ,
            ##
            MotherCut      = """
            ( chi2vx < 64        ) &
            ( M - M1 < 155 * MeV )
            """ 
            )
    
    # =============================================================================
    # Ds+/D+ -> KKpi   selection
    # =============================================================================
    def Ds ( self ) :
        """
        ``Preselection'' for Ds+
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        ## 
        return self.make_selection (
            'Ds' ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays           ,
            ## required selections 
            [ self.kaons() , self.pions() ] ,
            #
            ## algorithm properties
            # 
            DecayDescriptor = " [D_s+ -> K-  K+  pi+ ]cc" , ## through phi !!!
            ##
            Preambulo      = self.preambulo()  + [
            "admD  = ADAMASS ('D+'  )  <   65 * MeV " , ## D+  mass window for combination cut
            "admDs = ADAMASS ('D_s+')  <   65 * MeV " , ## Ds+ mass window for combination cut
            "dmD   = ADMASS  ('D+'  )  <   55 * MeV " , ## D+  mass window
            "dmDs  = ADMASS  ('D_s+')  <   55 * MeV " , ## Ds+ mass window
            ] ,
            ## # require phi
            ## Combination12Cut = """
            ## ( AM < 1050 * MeV      ) &
            ## ( ACHI2DOCA(1,2)  < 16 ) 
            ## """ ,
            # do not require phi anymore
            Combination12Cut = """
            ( AM < 2000 * MeV      ) &
            ( ACHI2DOCA(1,2)  < 16 ) 
            """ ,
            ## # require phi
            ## CombinationCut = """
            ## ( AM12 < 1050 * MeV   ) &
            ## ( APT  > %s           ) &
            ## ( admD | admDs        ) & 
            ## ( ACHI2DOCA(1,3) < 16 ) &
            ## ( ACHI2DOCA(2,3) < 16 ) 
            ## """  %  ( 0.95 * self['pT(Ds+)'] ) ,
            ## #
            # do not require phi anymore
            CombinationCut = """
            ( APT  > %s           ) &
            ( admD | admDs        ) & 
            ( ACHI2DOCA(1,3) < 16 ) &
            ( ACHI2DOCA(2,3) < 16 ) 
            """  %  ( 0.95 * self['pT(Ds+)'] ) ,
            #
            ## 
            MotherCut      = """
            ( chi2vx  < 25  ) &
            ( PT      > %s  ) & 
            ( dmD  | dmDs   ) &
            ( ctau    > 0.1 * mm )
            """  % self['pT(Ds+)' ] 
            )

    # =============================================================================
    # D+ -> Kpipi   selection
    # =============================================================================
    def Dplus ( self ) :
        """
        D+ -> Kpipi   selection
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        return self.make_selection (
            'D' ,
            DaVinci__N3BodyDecays ,
            ## inputs 
            [ self.kaons() , self.pions()] ,
            #
            ## algorithm properties 
            #
            DecayDescriptor = " [D+ -> K-  pi+  pi+ ]cc" ,
            ##
            Combination12Cut = """
            ( AM < 1.9 * GeV      ) &
            ( ACHI2DOCA(1,2) < 16 )  
            """ ,
            CombinationCut   = """
            ( ADAMASS('D+')  < 65 * MeV ) &
            ( APT > %s                  ) & 
            ( ACHI2DOCA(1,3) < 16       ) &
            ( ACHI2DOCA(2,3) < 16       ) 
            """ % ( 0.95 * self['pT(D+)' ] ) ,
            ##
            MotherCut      = """
            ( chi2vx  < 25                 ) &
            ( PT      > %s                 ) &
            ( ADMASS  ('D+'  )  < 55 * MeV ) &
            ( ctau    > 0.1 * mm   )
            """ % self['pT(D+)' ] ,
            )
    
    # =============================================================================
    # preselection of Lambda_c,Xi_c -> ( pKpi )   NO POINTING HERE! 
    # =============================================================================
    def preLamC ( self ) :
        """
        Lambda_c,Xi_c -> ( pKpi )  pre-selection
        """ 
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays
        #
        return self.make_selection (
            'preLambdaC' ,
            DaVinci__N3BodyDecays ,
            ## inputs 
            [ self.protons() , self.kaons() , self.pions()] ,
            ##
            DecayDescriptor = " [ Lambda_c+ -> p+  K-  pi+ ]cc" ,
            ##
            Combination12Cut  = """
            ( AM < 2.6 * GeV      ) &
            ( ACHI2DOCA(1,2) < 16 ) 
            """ ,
            ## 
            CombinationCut = """
            ( ( ADAMASS ( 'Lambda_c+' ) < 65 * MeV ) 
            | ( ADAMASS ( 'Xi_c+'     ) < 65 * MeV ) ) &
            ( APT            > %s ) & 
            ( ACHI2DOCA(1,3) < 16 ) &
            ( ACHI2DOCA(2,3) < 16 ) 
            """ % ( 0.85 * self[ 'pT(Lc+)' ] ) ,
            ##
            MotherCut      = """
            ( chi2vx  < 25 )                          &
            ( PT      > %s                          ) &
            ( ( ADMASS ( 'Lambda_c+' ) < 55 * MeV ) 
            | ( ADMASS ( 'Xi_c+'     ) < 55 * MeV ) ) 
            """ %  self [ 'pT(Lc+)'] 
            )
    
    # =============================================================================
    # Lambda_c,Xi_c -> ( pKpi ) 
    # =============================================================================
    ## selection of "prompt" Lambda_c with very loose IP-cut
    #  @attention: very loose chi2(IP) cuts here:  chi2(IP)<400
    def LamC ( self ) :
        """
        Lambda_c,Xi_c -> ( pKpi )  selection
        Attention: very loose chi2(IP) cuts here:  chi2(IP)<400
        """ 
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        #
        return self.make_selection (
            'LambdaC'     ,
            FilterDesktop ,
            ## inputs 
            [ self.preLamC() ] ,
            ##
            Code = """
            ( PT       > %s       ) &
            ( ctau_400 > 0.1 * mm )
            """ % self['pT(Lc+)']
            )
    
    # =============================================================================
    # preselection of Xi_c0/Omegac -> ( pKKpi )   NO POINTING HERE! Marco Pappagallo
    # =============================================================================
    def preXiC0 ( self ) :
        """
        Xi_c0/Omega_c0 -> ( pKKpi )  pre-selection
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays
        #
        return self.make_selection (
            'preXiC0' ,
            DaVinci__N4BodyDecays ,
            ## inputs
            [ self.protons() , self.kaons() , self.pions()] ,
            ##
            DecayDescriptor = " [ Xi_c0 -> p+  K-  K- pi+ ]cc" ,
            ##
            Combination12Cut  = """
            ( ACHI2DOCA(1,2) < 16 )
            """ ,
            Combination123Cut  = """
            ( ACHI2DOCA(1,3) < 16 ) &
            ( ACHI2DOCA(2,3) < 16 )
            """ ,
            ##
            CombinationCut = """
            ( ( ADAMASS ( 'Omega_c0' ) < 65 * MeV )
            | ( ADAMASS ( 'Xi_c0'     ) < 65 * MeV ) ) &
            ( APT            > %s ) &
            ( ACHI2DOCA(1,4) < 16 ) &
            ( ACHI2DOCA(2,4) < 16 ) &
            ( ACHI2DOCA(3,4) < 16 )
            """ % ( 0.85 * self[ 'pT(Xic0)' ] ) ,
            ##
            MotherCut      = """
            ( chi2vx  < 25 )                          &
            ( PT      > %s                          ) &
            ( ( ADMASS ( 'Omega_c0' ) < 55 * MeV )
            | ( ADMASS ( 'Xi_c0'    ) < 55 * MeV ) )
            """ %  self [ 'pT(Xic0)']
            )
                                    
    # =============================================================================
    # Xi_c0 -> ( pKKpi ) Marco Pappagallo
    # =============================================================================
    def XiC0 ( self ) :
        """
        Xi_c0 -> ( pKKpi )  selection
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        #
        return self.make_selection (
            'XiC0'     ,
            FilterDesktop ,
            ## inputs
            [ self.preXiC0() ] ,
            ##
            Code = """
            ( ADMASS ( 'Xi_c0' ) < 55 * MeV ) &
            ( PT > %s ) & ( ctau > 0.1 * mm )
            """ % self['pT(Xic0)']
            )    
    # =============================================================================
    # Omega_c0 -> ( pKKpi ) Marco Pappagallo
    # =============================================================================
    def OmegaC0 ( self ) :
        """
        Omega_c0 -> ( pKKpi )  selection
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        #
        return self.make_selection (
            'OmegaC0'     ,
            FilterDesktop ,
            ## inputs
            [ self.preXiC0() ] ,
            ##
            Code = """
            ( ADMASS ( 'Omega_c0' ) < 55 * MeV ) &
            ( PT > %s ) & ( ctau > 0.1 * mm )
            """ % self['pT(Xic0)']
            )
    
    # =============================================================================
    #  Xi_c0 -> Lc+ pi-
    # ============================================================================= 
    ## new decay mode \f$ \Xi_c^0 \rightarrow \Lambda_c^+ \pi^-\f$
    #  the evidence for this decay is obtaine by Belle in Phys.Rev. D89 (2014) 9, 091102 
    #  @see http://dx.doi.org/10.1103/PhysRevD.89.091102
    #  Also Lesha Dziuba sees the hint for this peak in his chamr baryon studies 
    def Xic02LcPi ( self ) :
        """
        Xi_c0 -> Lc+ pi-
        The peak seen by Belle in Phys.Rev. D89 (2014) 9, 091102 
        There is a hint in LHCb data 
        """ 
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        ## 
        from StandardParticles  import StdAllLoosePions as inpts 
        #
        return self.make_selection (
            'Xic02LcPi'      ,
            CombineParticles ,
            ## inputs 
            [ self.preLamC() , inpts ] ,
            ##
            DecayDescriptor = " [ Xi_c0 -> Lambda_c+ pi-]cc" ,
            DaughtersCuts   = {
            ## use only Lambda_c+ here, skip Xi_c+
            'Lambda_c+' : "ADMASS ( 'Lambda_c+' ) < 60 * MeV"
            }, 
            ##
            CombinationCut  = """
            ( APT > %s ) & ( ( AM - AM1 ) < 240 * MeV ) 
            """  % ( 0.95 * self['pT(Lc+)'] ) ,
            ##
            MotherCut      = """
            ( chi2vx  < 25 )              &
            ( PT      > %s              ) &
            ( ctau    > 0.05 * mm )  
            """ % self [ 'pT(Lc+)']
            )
    
    # =============================================================================
    # Lambda_c,Xi_c -> ( pKK ) 
    # =============================================================================
    def LamC2pKK ( self ) :
        """
        Lambda_c,Xi_c -> ( pKK )  selection
        """ 
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays
        #
        return self.make_selection (
            'LambdaCpKK' ,
            DaVinci__N3BodyDecays ,
            ## inputs 
            [ self.protons() , self.kaons() ] ,
            ##
            DecayDescriptor = " [ Lambda_c+ -> p+ K- K+ ]cc" ,
            ##
            Combination12Cut  = """
            ( AM < 2.5 * GeV      ) &
            ( ACHI2DOCA(1,2) < 16 ) 
            """ ,
            ## 
            CombinationCut = """
            ( ( ADAMASS ( 'Lambda_c+' ) < 65 * MeV ) 
            | ( ADAMASS ( 'Xi_c+'     ) < 65 * MeV ) ) &
            ( APT            > %s ) & 
            ( ACHI2DOCA(1,3) < 16 ) &
            ( ACHI2DOCA(2,3) < 16 ) 
            """ % ( 0.95 * self[ 'pT(Lc+)' ] ) ,
            ##
            MotherCut      = """
            ( chi2vx  < 25 )                          &
            ( PT      > %s                          ) &
            ( ( ADMASS ( 'Lambda_c+' ) < 55 * MeV ) 
            | ( ADMASS ( 'Xi_c+'     ) < 55 * MeV ) ) &
            ( ctau_400  > 0.1 * mm                  )  
            """ % self [ 'pT(Lc+)']
            )

    # =============================================================================
    # Sigma_C -> Lambda_C pi selection
    # =============================================================================
    def SigC ( self ) :
        """
        Sigma_C -> Lambda_C pi selection
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        return self.make_selection (
            'SigmaC'         ,
            CombineParticles ,
            [ self.LamC()    , self.slowPions() ] , 
            #
            ## algorithm properties 
            DecayDescriptors = [
            " [ Sigma_c0  -> Lambda_c+ pi- ]cc" ,
            " [ Sigma_c++ -> Lambda_c+ pi+ ]cc" ,
            ] ,
            ##
            CombinationCut = """
            ( AM - AM1 < ( 140 * MeV + 100 * MeV ) ) 
            """ ,
            ##
            MotherCut      = """
            ( chi2vx  < 16 )
            """ ,
            )

    # =============================================================================
    # Lambda_C* -> Lambda_C pi pi selection
    # =============================================================================
    def LamCstar ( self ) :
        """
        Lambda_C* -> Lambda_C pi pi selection
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        ## 
        return self.make_selection (
            ## the unique tag 
            'LambdaCstar'                       ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays               ,
            [ self.LamC() , self.slowPions() ]  ,
            #
            ## algorithm properties 
            # 
            DecayDescriptor = " [ Lambda_c(2625)+ -> Lambda_c+ pi+ pi-]cc", 
            ##
            Combination12Cut = """
            ( AM < 3 * GeV         ) &
            ( ACHI2DOCA(1,2) < 16  ) 
            """ , 
            CombinationCut = """
            ( AM - AM1 <  ( 2 * 140 * MeV + 100 * MeV ) ) & 
            ( ACHI2DOCA(1,3) < 16  ) &
            ( ACHI2DOCA(2,3) < 16  ) 
            """ ,
            MotherCut      = """
            ( chi2vx  < 25 )
            """ ,
            ##
            )

    # =============================================================================
    # Omega_C*0 -> Xi_c+ K- selection for Marco Pappagallo
    # =============================================================================
    def OmgCstar ( self ) :
        """
        Omega_C* -> Xi_c+ K- selection for Marco Pappagallo
        The line also includes   Lambda_C+ K- 
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        ##
        return self.make_selection (
            'OmegaCstar'         ,
            CombineParticles     ,
            [ self.LamC() , self.promptKaons() ] , 
            #
            ## algorithm properties 
            DecayDescriptors  = [
            "[ Omega_c*0 -> Lambda_c+ K- ]cc",
            "[ Omega_c*0 -> Lambda_c+ K+ ]cc"
            ] ,
            ##
            CombinationCut   = " AM - AM1 < ( 500 * MeV + %s ) " %  self['QvalueXiCK'],
            ##
            MotherCut        = " chi2vx  < 16 "
            )

    # =============================================================================
    # Xi'_c+ -> Xi_c+ gamma selection for Marco Pappagallo
    # =============================================================================
    def XiCprime ( self ) :
        """
        Xi'_c+ -> Xi_c+ gamma selection for Marco Pappagallo
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from StandardParticles import StdLooseAllPhotons as     inpts
        ##
        photoncut = "CL > %s " % self['PhotonCLCut']                
        ##
        return self.make_selection (
            'XiCprime'         ,
            CombineParticles   ,
            [ self.LamC() , inpts ] ,
            #
            ## algorithm properties
            DecayDescriptor  = " [ Xi'_c+ -> Lambda_c+ gamma ]cc" ,
            ## refine kaon selection
            DaughtersCuts    = {
            'Lambda_c+' : "M > 2400 * MeV",
            'gamma'     : photoncut
            } ,
            ##
            CombinationCut   = " AM - AM1 < %s  " %  self['QvalueXiCprime'],
            ##
            MotherCut        = " ALL "
            )

    # =============================================================================
    # Xi*_c+ -> Xi_C0 pi+ selection for Marco Pappagallo
    # =============================================================================
    def XiCstar ( self ) :
        """
        Xi*_c+ -> Xi_C0 pi+ selection for Marco Pappagallo
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        ##
        pioncut = """
        ( CLONEDIST   > 5000      ) &
        ( TRGHOSTPROB < 0.5       ) &
        in_range ( 2  , ETA , 4.9 ) &
        HASRICH
        """
        ##
        return self.make_selection (
            'XiCstar'         ,
            CombineParticles  ,
            [ self.XiC0()     , self.slowPions()  ] ,
            #
            ## algorithm properties
            DecayDescriptor = " [ Xi_c*+ -> Xi_c0 pi+ ]cc" ,
            ##
            DaughtersCuts    = {'pi+' : pioncut} ,
            ##
            CombinationCut   = " AM - AM1 < ( 140 * MeV + %s ) " %  self['QvalueXiCstar'],
            ##
            MotherCut      = """
            ( chi2vx  < 16 )
            """ ,
            )

    # =============================================================================
    # Omega_c*0 -> Xi_C0 pi+ K- selection for Marco Pappagallo
    # =============================================================================
    def OmgCstar2XiCPiK ( self ) :
        """
        Omega_c*0 -> Xi_C0 pi+ K- selection for Marco Pappagallo
        """
        
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays        
        ##
        pioncut = """
        ( CLONEDIST   > 5000      ) &
        ( TRGHOSTPROB < 0.5       ) &
        in_range ( 2  , ETA , 4.9 ) &
        HASRICH                     
        """
        return self.make_selection (
            'OmegaCstar2XiCPiK'         ,
            DaVinci__N3BodyDecays       ,
            [ self.XiC0() , self.promptKaons() , self.slowPions()  ] ,
            #
            ## algorithm properties
            DecayDescriptors = [
            "[ Omega_c*0 -> Xi_c0 K- pi+ ]cc",
            "[ Omega_c*0 -> Xi_c0 K+ pi+ ]cc"
            ],
            ##
            DaughtersCuts    = {
            'pi+' : self['TrackCut']
            } ,
            ##
            Combination12Cut = """
            ( ACHI2DOCA(1,2)  < 16 ) & ( AM < 6 * GeV ) 
            """,
            ##
            CombinationCut   = """
            ( AM - AM1 < ( 500 * MeV + %s ) ) &
            ( ACHI2DOCA(1,3)  < 16 ) & 
            ( ACHI2DOCA(2,3)  < 16 ) 
            """ %  self['QvalueXiCPiK'] ,
            ##
            MotherCut      = """
            ( chi2vx  < 16 )
            """ ,
            )

    # =============================================================================
    # Omega_c*0 -> Xi'_C+ K- selection for Marco Pappagallo
    # =============================================================================
    def OmgCstar2XiCprimeK ( self ) :
        """
        Omega_c*0 -> Xi'_C+ K- selection for Marco Pappagallo
        """
        ##
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        ##
        return self.make_selection (
            'OmegaCstar2XiCprimeK'         ,
            CombineParticles  ,
            [ self.XiCprime() , self.promptKaons() ] ,
            #
            ## algorithm properties
            DecayDescriptors = [
            "[ Omega_c*0 -> Xi'_c+ K- ]cc",
            "[ Omega_c*0 -> Xi'_c+ K+ ]cc"
            ] ,
            ##
            CombinationCut   = " AM - AM1 < ( 500 * MeV + %s ) " %  self['QvalueXiCprimeK'],
            ##
            MotherCut      = """
            ( chi2vx  < 16 )
            """ ,
            )

    # =============================================================================
    # Omega_cc+ -> Xi_C+ K- pi+ selection for Marco Pappagallo
    # =============================================================================
    def OmgCCKpi( self ) :
        """
        Omega_cc+ -> Xi_C+ K- pi+ selection for Marco Pappagallo
        """
        
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays
        ##
        return self.make_selection (
            'OmegaCC2XiCKpi'      ,
            DaVinci__N3BodyDecays ,
            [ self.preLamC() , self.promptKaons () , self.slowPions() ] ,
            #
            ## algorithm properties
            DecayDescriptor = " [ Omega_cc+ -> Lambda_c+ K- pi+ ]cc" ,
            ##
            DaughtersCuts    = {
            'Lambda_c+' : "M > 2400 * MeV",
            'pi+' : self['TrackCut']
            } ,
            ##
            Combination12Cut = """
            ( ACHI2DOCA(1,2)  < 16 )  & ( AM < 6 * GeV ) 
            """,
            ##
            CombinationCut   = """
            ( AM <  %s )             &
            ( ACHI2DOCA(1,3)  < 16 ) & 
            ( ACHI2DOCA(2,3)  < 16 )              
            """ %  self['QvalueOmegaCC'],
            ##
            MotherCut      = """
            ( chi2vx  < 16 ) & ( PT > %s )
            """ % self['pT(Omgcc)'] ,
            )

    # =============================================================================
    # Omega_cc+ -> Xi_C+ K- K+ selection for Marco Pappagallo
    # =============================================================================
    def OmgCCKK ( self ) :
        """
        Omega_cc+ -> Xi_C+ K- K+ selection for Marco Pappagallo
        """
        
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays
        ##
        return self.make_selection (
            'OmegaCC2XiCKK'       ,
            DaVinci__N3BodyDecays ,
            [ self.preLamC() , self.promptKaons () ] ,
            #
            ## algorithm properties
            DecayDescriptor = " [ Omega_cc+ -> Lambda_c+ K- K+  ]cc" , 
            ##
            DaughtersCuts    = {
            'Lambda_c+' : "M > 2400 * MeV",
            } ,
            ##
            Combination12Cut = """
            ( ACHI2DOCA(1,2)  < 16 )  & ( AM < 6 * GeV ) 
            """,
            ##
            CombinationCut   = """
            ( AM <  %s )             &
            ( ACHI2DOCA(1,3)  < 16 ) & 
            ( ACHI2DOCA(2,3)  < 16 )              
            """ %  self['QvalueOmegaCC'],
            ##
            MotherCut      = """
            ( chi2vx  < 16 ) & ( PT > %s )
            """ % self['pT(Omgcc)'],
            )

    # =============================================================================    
    ## helper merged selection of all charmed particles
    def PromptCharm ( self ) :
        """
        Helper merged selection of all charmed particles
        """
        sel = self._selection ( 'PromptCharm_Selection' )
        if sel : return sel

        from PhysSelPython.Wrappers import MergedSelection
        sel =  MergedSelection (
            ##
            'PromptCharmFor' + self.name ()       ,
            RequiredSelections = [ self.D02Kpi () ,
                                   self.Dstar  () ,
                                   self.Ds     () ,
                                   self.Dplus  () ,
                                   self.LamC   () ,
                                   self.SigC   ()  ## added for pentaquark searches 
                                   ] 
            )
        
        return self._add_selection ( 'PromptCharm_Selection' , sel )

    
    ## selection of W+-
    def W ( self )  :
        """
        Get simple  W+-selection
        """
        from GaudiConfUtils.ConfigurableGenerators      import FilterDesktop 
        from StandardParticles  import StdAllLooseMuons as     inpts 
        #
        return self.make_selection (
            'W'           ,
            FilterDesktop ,
            [ inpts ]     , 
            #
            ## algorithm properties
            #
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
    
    ## get "Di-Charm"-selection
    def DiCharm ( self ) :
        """
        Di-Charm selection
        """
        from GaudiConfUtils.ConfigurableGenerators      import CombineParticles 
        return self.make_selection (
            'DiCharm'        ,
            CombineParticles ,
            [ self.PromptCharm() ] , 
            #
            ## algorithm properties
            # 
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
            " [ psi(3770) -> D0        Sigma_c~0  ]cc "  , ## addded 2015-07-21
            " [ psi(3770) -> D0        Sigma_c~-- ]cc "  , ## addded 2015-07-21
            #
            "   psi(3770) -> D*(2010)+ D*(2010)-      "  ,
            " [ psi(3770) -> D*(2010)+ D-         ]cc "  ,
            " [ psi(3770) -> D*(2010)+ D_s-       ]cc "  ,
            " [ psi(3770) -> D*(2010)+ Lambda_c~- ]cc "  ,
            " [ psi(3770) -> D*(2010)+ Sigma_c~0  ]cc "  , ## addded 2015-07-21
            " [ psi(3770) -> D*(2010)+ Sigma_c~-- ]cc "  , ## addded 2015-07-21
            #
            "   psi(3770) -> D+        D-             "  ,
            " [ psi(3770) -> D+        D_s-       ]cc "  ,
            " [ psi(3770) -> D+        Lambda_c~- ]cc "  ,
            " [ psi(3770) -> D+        Sigma_c~0  ]cc "  , ## addded 2015-07-21
            " [ psi(3770) -> D+        Sigma_c~-- ]cc "  , ## addded 2015-07-21
            #
            "   psi(3770) -> D_s+      D_s-           "  ,
            " [ psi(3770) -> D_s+      Lambda_c~- ]cc "  ,
            " [ psi(3770) -> D_s+      Sigma_c~0  ]cc "  , ## addded 2015-07-21
            " [ psi(3770) -> D_s+      Sigma_c~-- ]cc "  , ## addded 2015-07-21
            #
            "   psi(3770) -> Lambda_c+ Lambda_c~-     "  ,
            " [ psi(3770) -> Lambda_c+ Sigma_c~0  ]cc "  , ## addded 2015-07-21
            " [ psi(3770) -> Lambda_c+ Sigma_c~-- ]cc "  , ## addded 2015-07-21
            #
            "   psi(3770) -> Sigma_c0  Sigma_c~0      "  , ## addded 2015-07-21
            " [ psi(3770) -> Sigma_c0  Sigma_c~-- ]cc "  , ## addded 2015-07-21
            #
            "   psi(3770) -> Sigma_c++ Sigma_c~--     "  , ## addded 2015-07-21
            #
            # double charm
            #
            " [ psi(3770) -> D0        D0         ]cc"  ,
            " [ psi(3770) -> D0        D*(2010)+  ]cc "  ,
            " [ psi(3770) -> D0        D+         ]cc "  ,
            " [ psi(3770) -> D0        D_s+       ]cc "  ,
            " [ psi(3770) -> D0        Lambda_c+  ]cc "  ,
            " [ psi(3770) -> D0        Sigma_c0   ]cc "  , ## addded 2015-07-21
            " [ psi(3770) -> D0        Sigma_c++  ]cc "  , ## addded 2015-07-21
            #
            " [ psi(3770) -> D*(2010)+ D*(2010)+  ]cc "  ,
            " [ psi(3770) -> D*(2010)+ D+         ]cc "  ,
            " [ psi(3770) -> D*(2010)+ D_s+       ]cc "  ,
            " [ psi(3770) -> D*(2010)+ Lambda_c+  ]cc "  ,
            " [ psi(3770) -> D*(2010)+ Sigma_c0   ]cc "  , ## addded 2015-07-21
            " [ psi(3770) -> D*(2010)+ Sigma_c++  ]cc "  , ## addded 2015-07-21
            #
            " [ psi(3770) -> D+        D+         ]cc "  ,
            " [ psi(3770) -> D+        D_s+       ]cc "  ,
            " [ psi(3770) -> D+        Lambda_c+  ]cc "  ,
            " [ psi(3770) -> D+        Sigma_c0   ]cc "  , ## addded 2015-07-21
            " [ psi(3770) -> D+        Sigma_c++  ]cc "  , ## addded 2015-07-21
            #
            " [ psi(3770) -> D_s+      D_s+       ]cc "  ,
            " [ psi(3770) -> D_s+      Lambda_c+  ]cc "  ,
            " [ psi(3770) -> D_s+      Sigma_c0   ]cc "  , ## addded 2015-07-21
            " [ psi(3770) -> D_s+      Sigma_c++  ]cc "  , ## addded 2015-07-21
            #
            " [ psi(3770) -> Lambda_c+ Lambda_c+  ]cc "  ,
            " [ psi(3770) -> Lambda_c+ Sigma_c0   ]cc "  , ## addded 2015-07-21
            " [ psi(3770) -> Lambda_c+ Sigma_c++  ]cc "  , ## addded 2015-07-21
            #
            " [ psi(3770) -> Sigma_c0  Sigma_c0   ]cc "  , ## addded 2015-07-21
            " [ psi(3770) -> Sigma_c0  Sigma_c++  ]cc "  , ## addded 2015-07-21
            #
            " [ psi(3770) -> Sigma_c++ Sigma_c++  ]cc "  , ## addded 2015-07-21
            #
            ] ,
            ## combination cut : accept all
            CombinationCut = " AALL " ,
            ##      mother cut : accept all
            MotherCut      = "  ALL " , 
            #
            )

    ## charm & W
    def CharmAndW ( self ) :
        """
        Charm & W+-
        """
        from GaudiConfUtils.ConfigurableGenerators      import CombineParticles 
        return self.make_selection (
            'CharmW'        ,
            CombineParticles ,
            [ self.W() , self.PromptCharm() ] , 
            #
            ## algorithm properties
            # 
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
            ## combination cut : accept all
            CombinationCut = " AALL " ,
            ##      mother cut : accept all
            MotherCut      = "  ALL " , 
            #
            )
    
    ## get the dimuons
    def DiMuon ( self ) :
        """
        Get the dimuons
        """
        from GaudiConfUtils.ConfigurableGenerators      import CombineParticles 
        return self.make_selection (
            'DiMuon'         ,
            CombineParticles ,
            [ self.muons() ] , 
            #
            ## algorihtm properties
            # 
            ## the decays to be reconstructed
            DecayDescriptor = 'J/psi(1S) -> mu+ mu-' ,
            ## combination cut
            CombinationCut  = " psi | psi_prime | ( AM > 8 * GeV ) " ,
            ##      mother cut
            MotherCut       = " chi2vx < 20 " ,
            )
    
    ## get the 2xdimuons
    def DoubleDiMuon ( self ) :
        """
        Get the 2xdimuons
        """
        from GaudiConfUtils.ConfigurableGenerators      import CombineParticles 
        return self.make_selection (
            'DoubleDiMuon'         ,
            CombineParticles ,
            [ self.DiMuon() ] , 
            #
            ## algorihtm properties
            # 
            ## the decays to be reconstructed
            DecayDescriptor = 'Upsilon(1S) -> J/psi(1S) J/psi(1S)' ,
            ## combination cut
            CombinationCut  = "AALL" ,
            ##      mother cut
            MotherCut       = " ALL" 
            )
    
    ## get the dimuons & charn
    def DiMuonAndCharm ( self ) :
        """
        Get charm & dimuon :
        Select events with at leats one charm particle and
        at least one dimuon
        """
        from GaudiConfUtils.ConfigurableGenerators      import CombineParticles 
        return self.make_selection (
            'DiMuonandCharm' ,
            CombineParticles ,
            [ self.DiMuon()  , self.PromptCharm() ] , 
            #
            ## algorithm properties
            # 
            DecayDescriptors = [
            "[ Upsilon(1S) -> J/psi(1S) D0        ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) D*(2010)+ ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) D+        ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) D_s+      ]cc" ,
            "[ Upsilon(1S) -> J/psi(1S) Lambda_c+ ]cc"
            ] ,
            CombinationCut = " AALL " ,
            MotherCut      = "  ALL " , 
            #
            )
    
    ## get the dimuons & charn
    def ChiAndCharm ( self ) :
        """
        Get charm & chi:
        Select events with at leats one charm particle and
        at least one chi
        """
        from GaudiConfUtils.ConfigurableGenerators       import DaVinci__N3BodyDecays
        from StandardParticles import StdLooseAllPhotons as     inpts 
        #
        pre_chi = self.make_selection (
            'PreChiAndCharm'      ,
            DaVinci__N3BodyDecays ,
            [ self.DiMuonAndCharm () , self.DiMuon() , self.PromptCharm() , inpts ] ,
            #
            ## algorithm proeprties
            # 
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
            "gamma"     : self [ 'GammaChi' ]
            } ,
            ##
            Combination12Cut = """
            ACHI2DOCA(1,2) < 100 
            """ ,
            ## require chi_(c,b)
            CombinationCut = """
            AM13 - AM1 < 1.01 * GeV
            """,
            ##
            MotherCut      = "  ALL " , 
            )
        
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger
        ## 
        return self.make_selection (
            'ChiAndCharm'                 ,
            Pi0Veto__Tagger               ,
            [ pre_chi ]                   ,
            MassWindow     = 25 * MeV     ,
            MassChi2       = -1           ,
            ExtraInfoIndex = 25010     ## unique ! 
            )
    
    ## DiMuon & W
    def DiMuonAndW ( self ) :
        """
        dimuon & W+-
        """
        from GaudiConfUtils.ConfigurableGenerators   import CombineParticles 
        ##
        return self.make_selection (
            'DiMuonAndW'                 ,
            CombineParticles             ,
            [ self.W() , self.DiMuon() ] ,
            #
            ## algorithm properties
            # 
            ## the decays to be reconstructed
            DecayDescriptors = [
            " [ chi_b0(2P) -> J/psi(1S) mu+ ]cc " ,
            ] ,
            ##
            ## combination cut : accept all
            CombinationCut = " AALL " ,
            ##      mother cut : accept all
            MotherCut      = "  ALL " , 
            )
    
    ## get the dimuons & charn
    def ChiAndW ( self ) :
        """
        Get chi & W :
        Select events with at leats one charm particle and
        at least one chi
        """
        from GaudiConfUtils.ConfigurableGenerators       import DaVinci__N3BodyDecays
        from StandardParticles import StdLooseAllPhotons as     inpts 
        #
        pre_chiw = self.make_selection (
            'PreChiAndW'          ,
            DaVinci__N3BodyDecays ,
            [ self.DiMuonAndW () , self.W() , self.DiMuon() , inpts ] ,
            #
            ## algorihtm properties
            # 
            ##
            DecayDescriptor = "[ chi_b1(2P) -> J/psi(1S) mu+ gamma ]cc" ,
            ##
            DaughtersCuts = {
            "J/psi(1S)" : " ( M  < 3.21 * GeV ) | in_range ( 8.5 * GeV , M , 12.0 * GeV ) " ,
            "gamma"     : self [ 'GammaChi' ]
            },
            ##
            Combination12Cut = """
            ACHI2DOCA(1,2) < 100 
            """ , 
            ## require chi_(c,b)
            CombinationCut = """
            AM13 - AM1 < 1.01 * GeV
            """,
            ##
            MotherCut      = "  ALL " , 
            #
            )
        ##
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger
        ## 
        return self.make_selection (
            'ChiAndW'                     ,
            Pi0Veto__Tagger               ,
            [ pre_chiw ]                  ,
            MassWindow     = 25 * MeV     ,
            MassChi2       = -1           ,
            ExtraInfoIndex = 25015     ## unique ! 
            )
    
# =============================================================================
if '__main__' == __name__ :

    logger.info ( 80*'*'  ) 
    logger.info (  __doc__ ) 
    logger.info ( ' Author :  %s' % __author__ ) 
    logger.info ( ' Date   :  %s' % __date__   )
    logger.info ( 80 * '*' ) 
    ##
    clines = set() 
    logger.info ( 70 * '-' ) 
    logger.info ( ' %-15s | %-40s ' % ( 'STREAM' , 'LINE' ) )
    logger.info ( 70 * '-' ) 
    for stream in default_config['STREAMS'] :
        lines = default_config['STREAMS'][stream]
        for l in lines :
            logger.info ( ' %-15s | %-40s ' % ( stream , l ) )
            clines.add ( l )
    logger.info ( 80 * '*' ) 
    ##
    logger.info ( ' The output locations for the default configuration: ' )
    ##
    _conf = StrippingPromptCharmConf ( 'PromptCharm' , 
                                       config = default_config['CONFIG']  )
    ##
    _ln   = ' ' + 61*'-' + '+' + 55*'-'
    logger.info ( _ln ) 
    logger.info ( '  %-60s| %-40s | %s ' % ( 'Output location'     ,
                                             'Stripping line name' ,
                                             'MDST.DST'            ) ) 
    logger.info ( _ln )
    for l in _conf.lines() :
        lout  = l.outputLocation()
        lname = l.name()
        flag  = l.MDSTFlag
        logger.info ( '  %-60s| %-40s | %s ' % ( lout, lname , flag ) )
        if not lname in clines :
            raise AttributeError ('Unknown Line %s' % lname )
        clines.remove ( lname )
        
    logger.info ( _ln ) 
    logger.info ( 80*'*'  ) 
    if clines :
        raise AttributeError('Undeclared lines: %s' % clines )

    keys = default_config['CONFIG'].keys()
    keys.sort()
    prescale = [ i for i in keys if 0 <= i.find('Prescale') ]
    other    = [ i for i in keys if not i in prescale       ] 
    logger.info ( 'Configuration keys are: %s' % other    ) 
    logger.info ( 'Prescale      keys are: %s' % prescale ) 
    logger.info ( 80*'*' ) 

    ## make dot-graphs 
    try:    
        selections = _conf._selections_private() 
        for s in selections :
            from SelPy.graph import graph
            o = graph ( s , format = 'png' )
            if o : logger.info  ( "Generate DOT-graph: %s"          % o        )
            else : logger.error ( "Can't produce DOT=-graph for %s" % s.name() )
            
    except : pass
        
        
# =============================================================================
# The END
# =============================================================================

