##!/usr/bin/env python
# =============================================================================
# $Id:$
# =============================================================================
## @file
# 
#  The attempt for coherent stripping for B -> psi(') X0 modes,
#  where  X0 :  eta   -> gamma gamma ,
#               eta   -> pi+ pi- pi0 ,
#               eta'  -> rho0 gamma 
#               eta'  -> pi+ pi- ( eta -> gamma gamma )
#               omega -> pi+ pi- pi0
#
# The cuts correspond to B->J/psiX0 selection by Dasha Savrina LHCb-ANA-2011-096
#
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2012-02-19
#
# Attention: the ``wide'' versions of pi0->2gg  and eta->gg a used
#           to cure the potential problems with bad ECAL calibration
#
#                   $Revision: 135537 $
# Last modification $Date: 2012-02-19 15:18:08 +0100 (Sun, 19 Feb 2012) $
#                by $Author: ibelyaev $
# =============================================================================
""" The attempt for coherent stripping for B -> psi(') X0 modes, where X0 :

- eta   -> gamma gamma ,
- eta   -> pi+ pi- pi0 ,
- eta'  -> rho0 gamma 
- eta'  -> pi+ pi- ( eta -> gamma gamma )
- omega -> pi+ pi- pi0

The cuts correspond to J/psiX0 selection by Dasha Savrina LHCb-ANA-2011-096

Attention: the ``wide'' versions of pi0->2gg  and eta->gg a used
           to cure the potential problems with bad ECAL calibration

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = '2012-02-19'
__version__ = '$Revision$'
# =============================================================================
__all__ = (
    'PsiX0Conf'      ,
    'default_config' , 
    )
# =============================================================================
from GaudiKernel.SystemOfUnits             import GeV, MeV, mm, micrometer 
from StrippingUtils.Utils                  import LineBuilder
# =============================================================================
## logging
# =============================================================================
import logging
logger = logging.getLogger(__name__)
if not logger.handlers : logging.basicConfig()
logger.setLevel(logging.INFO)
# =============================================================================
## Define the default configuration 
_default_configuration_ = {
    #
    'NOPIDHADRONS'   : False ,  ## USE FOR SUIMULATION 
    ## use for B&Q wg production
    'DIMUONLINES'    : []    ,  ## USE FOR B&Q WG-selection 
    #
    ## PV-requiremens
    #
    'CheckPV'   : True ,
    #
    ## the transverse momentum of X0
    'X0PT'      : 1.2  * GeV        ,
    ## the transverse momentum of X0, when accompanied with K(K) 
    'X0PTK'     : 0.7  * GeV        ,
    #
    ## c*tau cut for B-candidates
    #
    'CTAU'      : 100  * micrometer ,
    'CTAU_Kst'  : 150  * micrometer , ## for B->J/psi K*+ line 
    #
    ## photon selection for eta' -> rho gamma
    #
    'GammaCut'  : ' ( PT > 250 * MeV ) ' ,
    #
    ## pi0 selection for  eta/omega -> pi+ pi- pi0 ,
    #
    'Pi0Cut'    : """
    ( 250    * MeV < MINTREE ( 'gamma' == ID , PT ) ) 
    """ ,
    #
    ## eta -> gamma gamma selection for eta' -> pi+ pi- eta
    #
    'EtaCut'    :"""
    ( 250    * MeV < MINTREE ( 'gamma' == ID , PT ) ) 
    """ ,
    #
    ## muon selection for  psi(') -> mu+ mu-
    #
    'MuonCut'   : """
    ISMUON &
    ( PT            >  550 * MeV ) &
    ( PIDmu - PIDpi >    0       ) &
    ( CLONEDIST     > 5000       )     
    """ , 
    #
    ## pions and kaons
    # 
    'PionCut'   : """
    ( PT          > 200 * MeV ) & 
    ( CLONEDIST   > 5000      ) & 
    ( TRGHOSTPROB < 0.5       ) &
    ( TRCHI2DOF   < 4         ) & 
    in_range ( 2          , ETA , 5         ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                     &
    ( MIPCHI2DV()  > 4        )
    """ ,
    #
    'KaonCut'   : """
    ( PT          > 200 * MeV ) & 
    ( CLONEDIST   > 5000      ) & 
    ( TRGHOSTPROB < 0.5       ) &
    ( TRCHI2DOF   < 4         ) & 
    in_range ( 2          , ETA , 5         ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                     &
    ( MIPCHI2DV()  > 4        )
    """ ,
    #
    ##
    'ProtonCut'   : """
    ( PT           > 200 * MeV ) & 
    ( CLONEDIST    > 5000      ) & 
    ( TRCHI2DOF    < 4         ) & 
    ( TRGHOSTPROB  < 0.5       ) & 
    in_range ( 2         , ETA , 5         ) &
    in_range ( 10 * GeV  , P   , 150 * GeV ) &
    HASRICH                     &
    ( MIPCHI2DV()  > 4        ) 
    """ ,
    #
    ## PID-cuts for hadrons 
    #
    'PionPIDCut'   : " PROBNNpi > 0.1 " ,
    'KaonPIDCut'   : " PROBNNk  > 0.1 " ,
    'ProtonPIDCut' : " PROBNNp  > 0.1 " ,
    #
    ## useful shortcuts:
    #
    'Preambulo' : [
    ## shortcut for chi2 of vertex fit 
    'chi2vx    = VFASPF(VCHI2)     '                           , 
    'chi2vxNDF = VFASPF(VCHI2PDOF) '                           , 
    'chi2vxndf = chi2vxNDF         '                           ,
    ## shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light"        , 
    ## use the embedded cut for chi2(LifetimeFit)<25
    "ctau      = BPVLTIME ( 25 ) * c_light "                   ,
    "ctau_9    = BPVLTIME (  9 ) * c_light "                   ,
    "ctau_16   = BPVLTIME ( 16 ) * c_light "                   ,
    "APT23     = LoKi.AParticles.TransverseMomentum ( 2 , 3 )" ,
    ## Combination mass-cut for beauty particles 
    "mb0_acut   = in_range ( 4.50 * GeV , AM , 6.00 * GeV ) "  ,
    "mbu_acut   = in_range ( 4.50 * GeV , AM , 6.00 * GeV ) "  ,
    "mbl_acut   = in_range ( 5.00 * GeV , AM , 6.50 * GeV ) "  ,
    "mbc_acut   = in_range ( 4.50 * GeV , AM , 6.75 * GeV ) "  ,
    ## mass-cut for beauty particles 
    "mb0_cut    = in_range ( 4.55 * GeV ,  M , 5.95 * GeV ) "  ,
    "mbu_cut    = in_range ( 4.55 * GeV ,  M , 5.95 * GeV ) "  ,
    "mbl_cut    = in_range ( 5.05 * GeV ,  M , 6.45 * GeV ) "  ,
    "mbc_cut    = in_range ( 4.55 * GeV ,  M , 6.70 * GeV ) "  ,
    ] ,
    # =========================================================================
    ## Prescales 
    # =========================================================================
    'B2EtaPrescale'             : 1.0 ,
    'B2EtaPrimePrescale'        : 1.0 ,
    'B2OmegaPrescale'           : 1.0 ,
    #
    'B2KEtaPrescale'            : 1.0 ,
    'B2KEtaPrimePrescale'       : 1.0 ,
    'B2KOmegaPrescale'          : 1.0 ,
    #
    'B2PiEtaPrescale'           : 1.0 ,
    'B2PiEtaPrimePrescale'      : 1.0 ,
    'B2PiOmegaPrescale'         : 1.0 ,
    #
    'B2KKEtaPrescale'           : 1.0 ,
    'B2KKEtaPrimePrescale'      : 1.0 ,
    'B2KKOmegaPrescale'         : 1.0 ,
    #
    'B2KPiEtaPrescale'          : 1.0 ,
    'B2KPiEtaPrimePrescale'     : 1.0 ,
    'B2KPiOmegaPrescale'        : 1.0 ,
    #
    'B2PiPiEtaPrescale'         : 1.0 ,
    'B2PiPiEtaPrimePrescale'    : 1.0 ,
    'B2PiPiOmegaPrescale'       : 1.0 ,
    #
    'Bu2PsiKstarPrescale'       : 1.0 ,
    'Bu2PsiKstarMPrescale'      : 1.0 ,
    'Bc2PsiRhoPrescale'         : 1.0 ,
    #
    'B2ChicKPrescale'           : 1.0 ,
    'B2ChicKKPrescale'          : 1.0 ,
    'B2ChicKPiPrescale'         : 1.0 ,
    'B2ChicKPiPiPrescale'       : 1.0 ,
    'B2ChicPiPiPrescale'        : 1.0 ,
    ## Bc * Lb  
    'Bc2ChicPiPrescale'         : 1.0 ,
    'Lb2ChicPKPrescale'         : 1.0 ,
    # =========================================================================
    }
## ============================================================================
## the mandatory element for stripping framework 
default_config = {
    #
    'NAME'        :   'PsiX0'                ,
    'WGs'         : [ 'BandQ' ]              ,
    'CONFIG'      : _default_configuration_  , 
    'BUILDERTYPE' :   'PsiX0Conf'            ,
    'STREAMS'     : { 'Bhadron'    : [ 'StrippingB2PsiEtaForPsiX0'          ,
                                       'StrippingB2PsiEtaPrimeForPsiX0'     ,
                                       'StrippingB2PsiOmegaForPsiX0'        ,
                                       'StrippingB2PsiKEtaForPsiX0'         ,
                                       'StrippingB2PsiPiEtaForPsiX0'        ,
                                       'StrippingB2PsiKKEtaForPsiX0'        , 
                                       'StrippingB2PsiKPiEtaForPsiX0'       ,
                                       'StrippingB2PsiPiPiEtaForPsiX0'      , 
                                       'StrippingB2PsiKEtaPrimeForPsiX0'    ,
                                       'StrippingB2PsiPiEtaPrimeForPsiX0'   , 
                                       'StrippingB2PsiKKEtaPrimeForPsiX0'   , 
                                       'StrippingB2PsiKPiEtaPrimeForPsiX0'  ,
                                       'StrippingB2PsiPiPiEtaPrimeForPsiX0' ,
                                       'StrippingB2PsiKOmegaForPsiX0'       ,
                                       'StrippingB2PsiPiOmegaForPsiX0'      ,
                                       'StrippingB2PsiKKOmegaForPsiX0'      ,
                                       'StrippingB2PsiKPiOmegaForPsiX0'     ,
                                       'StrippingB2PsiPiPiOmegaForPsiX0'    ,
                                       'StrippingBu2PsiKstarForPsiX0'       , ## CALIB ? 
                                       'StrippingBu2PsiKstarMergedForPsiX0' , ## CALIB ?
                                       'StrippingBc2PsiRhoForPsiX0'         ,
                                       'StrippingB2ChicKForPsiX0'           ,
                                       'StrippingB2ChicKKForPsiX0'          ,
                                       'StrippingB2ChicKPiForPsiX0'         ,
                                       'StrippingB2ChicKPiPiForPsiX0'       ,
                                       'StrippingB2ChicPiPiForPsiX0'        ,
                                       'StrippingBc2ChicPiForPsiX0'         ,
                                       'StrippingLb2ChicPKForPsiX0'         ] }
    }
# =============================================================================
## @class  PsiX0Conf
#  B-> psi(') X0 configuration file 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2012-02-19
class PsiX0Conf(LineBuilder) :
    """
    Helper class to configure 'PsiX0'-lines
    """
    __configuration_keys__ = tuple ( _default_configuration_.keys() )
    
    ## get the default configuration 
    @staticmethod
    def defaultConfiguration( key = None ) :
        """
        Get the default/recommended configurtaion
        
        >>> conf = PsiX0.defaultConfiguration()
        
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
        if 'PsiX0' != name :
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


        if not self.__selections_.has_key ( self.name() ) :
            self.__selections_[ self.name() ] = {}
            
        self.__selections_[ self.name() ]['CONFIG'] = deepcopy ( _config ) 
        
        keys = _config.keys()
        for key in keys :
            
            if not key in _default_configuration_ :
                raise KeyError("Invalid key is specified: '%s'" % key )
            
            val = _config[key]
            if val != _default_configuration_ [ key ] : 
                logger.debug ('new configuration: %-16s : %s ' % ( key , _config[key] ) )
                
        self._name         = name
                
        for line in self._lines_psiX0 () :
            self.registerLine(line)
            logger.debug ( "Register line: %s" %  line.name () ) 
            
    ## get the selection, associated with some nickname name 
    def _selection ( self, nick ) :
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
        if not self.__selections_.has_key ( self.name() ) :
            self.__selections_[ self.name() ] = {} 
        
        if self.__selections_[ self.name()].has_key( nick ) :
            raise AttributeError , "Selection '%s'already exists " % nick
        
        self.__selections_[ self.name() ][ nick ] = sel
        
        return sel 
    
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

        if not kwargs.has_key( 'ParticleCombiners' ) :
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
        
        sel =  [
            #
            ## basic
            #
            self.gamma            () ,
            self.pi0              () ,
            self.eta_             () ,
            self.muons            () ,
            self.pions            () ,
            self.kaons            () ,
            self.protons          () ,
            ##
            self.eta0             () ,
            self.eta_prime        () ,
            #
            ## local 
            self.psi              () ,
            self.eta2gg           () ,
            self.eta23pi          () ,
            self.etap2rhog        () ,
            self.etap2pipieta     () ,
            self.omega            () ,
            self.chi_c            () , 
            #
            ## beauty
            #
            self.b2eta            () ,
            self.b2etap           () ,
            self.b2omega          () , 
            #
            ## 
            # 
            self.b2Keta           () ,
            self.b2KKeta          () ,
            self.b2Kpieta         () ,
            self.b2pieta          () ,
            self.b2pipieta        () ,
            ## 
            self.b2Ketap          () ,
            self.b2KKetap         () ,
            self.b2Kpietap        () ,
            self.b2pietap         () ,
            self.b2pipietap       () ,
            ## 
            self.b2Komega         () ,
            self.b2KKomega        () ,
            self.b2Kpiomega       () ,
            self.b2piomega        () ,
            self.b2pipiomega      () ,
            ## 
            self.bc2rho           () ,
            self.bu2Kstar         () , ## for extraction of photon efficiency
            self.bu2KstarM        () ,  ## for merged pi0s
            #
            ## b -> chi_c
            self.b2chicK          () ,
            self.b2chicKK         () ,
            self.b2chicKpi        () ,
            self.b2chicKpipi      () ,
            self.b2chicpipi       () ,
            # 
            self.bc2chicpi        () ,
            ]
        
        return self._add_selection ( 'Selections' , sel )


    # =========================================================================
    ## get all lines  
    # =========================================================================
    def _lines_psiX0   ( self ) :
        """
        Get all psiX0 lines 
        """
        sel = self._selection ( 'PsiX0Lines' )
        if sel : return sel
        #
        from StrippingConf.StrippingLine  import StrippingLine
        sel = [
            #
            ## B -> psi(') X0  
            #
            StrippingLine (
            "B2PsiEtaFor" + self.name()       ,
            prescale = self['B2EtaPrescale' ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV']        ,
            algos    = [ self.b2eta ()      ]
            ) ,
            ## 
            StrippingLine (
            "B2PsiEtaPrimeFor" + self.name()     ,
            prescale = self['B2EtaPrimePrescale' ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'            ] ,
            algos    = [ self.b2etap ()          ]
            ) ,
            ## 
            StrippingLine (
            "B2PsiOmegaFor" + self.name ()       ,
            prescale = self['B2OmegaPrescale' ]  , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'         ]  ,
            algos    = [ self.b2omega ()      ]
            ) ,
            #
            ## B -> psi(') [...] eta 
            #
            StrippingLine (
            "B2PsiKEtaFor" + self.name()        ,
            prescale = self['B2KEtaPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'         ] ,
            algos    = [ self.b2Keta ()       ]
            ) ,
            ## 
            StrippingLine (
            "B2PsiPiEtaFor" + self.name()        ,
            prescale = self['B2PiEtaPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'          ] ,
            algos    = [ self.b2pieta ()       ]
            ) ,
            ##
            StrippingLine (
            "B2PsiKKEtaFor" + self.name()        ,
            prescale = self['B2KKEtaPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'          ] ,
            algos    = [ self.b2KKeta ()       ]
            ) ,
            StrippingLine (
            "B2PsiKPiEtaFor" + self.name()        ,
            prescale = self['B2KPiEtaPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'           ] ,
            algos    = [ self.b2Kpieta ()       ]
            ) ,
            ##
            ##
            StrippingLine (
            "B2PsiPiPiEtaFor" + self.name()        ,
            prescale = self['B2PiPiEtaPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'            ] ,
            algos    = [ self.b2pipieta ()       ]
            ) ,
            #
            ## B -> psi(') [...] eta' 
            #
            StrippingLine (
            "B2PsiKEtaPrimeFor" + self.name()        ,
            prescale = self['B2KEtaPrimePrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'              ] ,
            algos    = [ self.b2Ketap ()           ]
            ) ,
            ## 
            StrippingLine (
            "B2PsiPiEtaPrimeFor" + self.name()        ,
            prescale = self['B2PiEtaPrimePrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'               ] ,
            algos    = [ self.b2pietap ()           ]
            ) ,
            ##
            StrippingLine (
            "B2PsiKKEtaPrimeFor" + self.name()        ,
            prescale = self['B2KKEtaPrimePrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'               ] ,
            algos    = [ self.b2KKetap ()           ]
            ) ,
            StrippingLine (
            "B2PsiKPiEtaPrimeFor" + self.name()        ,
            prescale = self['B2KPiEtaPrimePrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'                ] ,
            algos    = [ self.b2Kpietap ()           ]
            ) ,
            ##
            StrippingLine (
            "B2PsiPiPiEtaPrimeFor" + self.name()        ,
            prescale = self['B2PiPiEtaPrimePrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'                 ] ,
            algos    = [ self.b2pipietap ()           ]
            ) ,
            #
            ## B -> psi(') [...] omega
            #
            StrippingLine (
            "B2PsiKOmegaFor" + self.name()           ,
            prescale = self['B2KOmegaPrescale'     ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'              ] ,
            algos    = [ self.b2Komega ()          ]
            ) ,
            ## 
            StrippingLine (
            "B2PsiPiOmegaFor" + self.name()        ,
            prescale = self['B2PiOmegaPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'            ] ,
            algos    = [ self.b2omega ()         ]
            ) ,
            ##
            StrippingLine (
            "B2PsiKKOmegaFor" + self.name()          ,
            prescale = self['B2KKOmegaPrescale'    ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'              ] ,
            algos    = [ self.b2KKomega ()         ]
            ) ,
            StrippingLine (
            "B2PsiKPiOmegaFor" + self.name()          ,
            prescale = self['B2KPiOmegaPrescale'    ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'               ] ,
            algos    = [ self.b2Kpiomega ()         ]
            ) ,
            ##
            StrippingLine (
            "B2PsiPiPiOmegaFor" + self.name()        ,
            prescale = self['B2PiPiOmegaPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'              ] ,
            algos    = [ self.b2pipiomega ()       ]
            ) ,
            ##            
            StrippingLine (
            "Bu2PsiKstarFor" + self.name()              ,
            prescale = self['Bu2PsiKstarPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'              ] ,
            algos    = [ self.bu2Kstar ()          ]
            ) ,
            ## 
            StrippingLine (
            "Bu2PsiKstarMergedFor" + self.name()     ,
            prescale = self['Bu2PsiKstarMPrescale' ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'              ] ,
            algos    = [ self.bu2KstarM ()         ]
            ) ,
            ##            
            StrippingLine (
            "Bc2PsiRhoFor" + self.name()             ,
            prescale = self['Bc2PsiRhoPrescale'    ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'              ] ,
            algos    = [ self.bc2rho ()            ]
            ) ,
            ##
            ## b -> chi_c
            ##
            StrippingLine (
            "B2ChicKFor" + self.name()               ,
            prescale = self['B2ChicKPrescale'      ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'              ] ,
            algos    = [ self.b2chicK ()           ]
            ) ,
            ##
            StrippingLine (
            "B2ChicKKFor" + self.name()              ,
            prescale = self['B2ChicKKPrescale'     ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'              ] ,
            algos    = [ self.b2chicKK ()          ]
            ) ,
            ##
            StrippingLine (
            "B2ChicKPiFor" + self.name()             ,
            prescale = self['B2ChicKPiPrescale'    ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'              ] ,
            algos    = [ self.b2chicKpi ()         ]
            ) ,
            ##
            StrippingLine (
            "B2ChicKPiPiFor" + self.name()           ,
            prescale = self['B2ChicKPiPiPrescale'  ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'              ] ,
            algos    = [ self.b2chicKpipi ()       ]
            ) ,
            ##
            StrippingLine (
            "B2ChicPiPiFor" + self.name()            ,
            prescale = self['B2ChicPiPiPrescale'   ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'              ] ,
            algos    = [ self.b2chicpipi ()        ]
            ) ,
            ##
            StrippingLine (
            "Bc2ChicPiFor" + self.name()             ,
            prescale = self['Bc2ChicPiPrescale'    ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'              ] ,
            algos    = [ self.bc2chicpi ()         ]
            ) ,
            ##
            StrippingLine (
            "Lb2ChicPKFor" + self.name()             ,
            prescale = self['Lb2ChicPKPrescale'    ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'              ] ,
            algos    = [ self.lb2chicpK ()         ]
            ) ,
            #
            ]
        #
        return self._add_selection ( 'PsiX0Lines' , sel ) 
    
    # ========================================================================
    ## pions :
    # ========================================================================
    def pions    ( self ) :
        """
        Pions for   B -> psi X lines 
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        ##
        if self['NOPIDHADRONS'] :
            from StandardParticles import   StdAllNoPIDsPions as inpts
            pioncut = self['PionCut']
        else                    :
            from StandardParticles import StdAllLooseANNPions as inpts
            pioncut = "(%s)&(%s)" % ( self['PionCut'] , self['PionPIDCut'] ) 
        ##
        return self.make_selection (
            'Pion'                 ,
            FilterDesktop          ,
            [ inpts ]              ,
            Code = pioncut         ,
            )
    
    # ========================================================================
    ## kaons :
    # ========================================================================
    def kaons     ( self ) :
        """
        Kaons for   B -> psi X lines 
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        ## 
        if self['NOPIDHADRONS'] :
            from StandardParticles import   StdAllNoPIDsKaons as inpts
            kaoncut = self['KaonCut']
        else                    :
            from StandardParticles import StdAllLooseANNKaons as inpts 
            kaoncut = "(%s)&(%s)" % ( self['KaonCut'] , self['KaonPIDCut'] ) 
        #
        ##
        return self.make_selection (
            'Kaon'                 ,
            FilterDesktop          ,
            [ inpts ]              ,
            Code = kaoncut         ,
            )

    # ========================================================================
    ## protons
    # ========================================================================
    def protons    ( self ) :
        """
        protons for   B -> psi X lines 
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        if self['NOPIDHADRONS'] :
            from StandardParticles import   StdAllNoPIDsProtons as inpts
            protoncut = self['ProtonCut']
        else                    :
            from StandardParticles import StdAllLooseANNProtons as inpts 
            protoncut = "(%s)&(%s)" % ( self['ProtonCut'] , self['ProtonPIDCut'] ) 
        ##
        return self.make_selection (
            'Proton'                 ,
            FilterDesktop            ,
            [ inpts ]                ,
            Code = protoncut         ,
            )

    # =========================================================================
    ## muons 
    # =========================================================================
    def muons  ( self ) :
        from   StandardParticles  import StdLooseMuons as muons_
        return muons_

    # =========================================================================
    ## gamma 
    # =========================================================================
    def gamma  ( self ) :
        from   StandardParticles  import StdLooseAllPhotons as photons_
        return photons_ 
    
    # =========================================================================
    ## define proper pi0 
    # =========================================================================
    def pi0  ( self ) :
        """
        Define proper pi0
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from StandardParticles                     import StdLoosePi02gg as inpts 
        ##
        return self.make_selection (
            'Pi0'                  ,
            FilterDesktop          ,
            [ inpts ]              ,
            Code = self['Pi0Cut']  ,
            )
    
    # =========================================================================
    ## define proper eta
    # =========================================================================
    def eta_  ( self ) :
        """
        Define proper eta -> gamma gamma 
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from StandardParticles                     import StdLooseEta2gg as inpts 
        ##
        return self.make_selection (
            'EtaStd'               ,
            FilterDesktop          ,
            [ inpts ]              ,
            Code = self['EtaCut']  ,
            )
    
    
    # =========================================================================
    ## selection for  eta -> gamma gamma 
    # =========================================================================
    def eta2gg ( self ) :
        """
        Selection for Eta -> gamma gamma
        """
        ##
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        pre_eta = self.make_selection (
            'PreEta2gg'            ,
            FilterDesktop          ,
            [ self.eta_ () ]       ,
            Code = """
            ( ADMASS ('eta') < 100 * MeV ) &
            ( PT > %s ) 
            """ % min ( self['X0PT'] , self['X0PTK'] ) 
            )
        ##
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger2g
        return self.make_selection (
            'Eta2gg'                   ,
            Pi0Veto__Tagger2g          ,
            [ pre_eta  ]               ,
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25015     ## unique ! 
            )
    
    # =========================================================================
    ## (selection for  eta -> pipipi
    # =========================================================================
    def eta23pi ( self ) :
        """
        Selection for Eta -> pipipi
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays
        ## 
        pre_eta = self.make_selection (
            ## the unique tag 
            'PreEta3Pi'                   ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.pions () , self.pi0 ()  ] ,
            ## algorithm properties 
            DecayDescriptor = " eta -> pi+ pi- pi0" ,
            ## 
            Combination12Cut  = """  ( AM < 700 * MeV  ) &
            ( ACHI2DOCA(1,2) < 12 ) 
            """ , 
            ##
            CombinationCut  = """
            ( APT > %s ) & ( ADAMASS ( 'eta' ) < 100 * MeV )
            """ % ( 0.9 * min ( self['X0PT'] , self['X0PTK'] ) ) ,
            ##
            MotherCut       = """
            ( PT     > %s ) &
            ( chi2vx <  9 ) 
            """ % min ( self['X0PT'] , self['X0PTK'] )
            )
        ##
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger2g
        ## 
        return self.make_selection (
            'Eta23pi'                  ,
            Pi0Veto__Tagger2g          ,
            [ pre_eta ]                ,
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25016     ## unique ! 
            )

    # =========================================================================
    ## all etas 
    # =========================================================================
    def eta0 ( self ) :
        """
        All etas 
        """
        tag      = 'Eta0_selection'
        sel      = self._selection ( tag )
        if sel : return sel
        #
        from PhysSelPython.Wrappers import MergedSelection
        sel = MergedSelection (
            'SelEta0AllFor' + self.name() ,
            RequiredSelections = [ self.eta2gg  () ,
                                   self.eta23pi () ]
            )
        
        return self._add_selection ( tag , sel ) 
    
    
    # ===========================================================================
    ## selection for  etap -> rho0 gamma 
    # ===========================================================================
    def etap2rhog ( self ) :
        """
        Selection for Eta' -> rho gamma        
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        pre_etap = self.make_selection (
            ## the unique tag 
            'PreEtapRhoG'                 ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.pions () , self.gamma ()  ] ,
            ##
            DecayDescriptor = " eta_prime -> pi+ pi- gamma" ,
            ##
            DaughtersCuts   = { 'gamma' :  self['GammaCut'] } ,
            ## 
            Combination12Cut  = """  ( AM < 950 * MeV  ) &
            ( ACHI2DOCA(1,2) < 12 ) 
            """ , 
            CombinationCut  = """
            ( APT > %s ) & 
            in_range  ( 500 * MeV , AM12 , 950 * MeV ) & 
            ( ADAMASS ( 'eta_prime' ) <    100 * MeV ) 
            """ % ( 0.9 * min ( self['X0PT'] , self['X0PTK'] ) ),
            ##
            MotherCut       = """
            ( PT     > %s ) &
            ( chi2vx <  9 )
            """ % min ( self['X0PT'] , self['X0PTK'] )
            )
        ##         
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger
        ## 
        return self.make_selection (
            'Etap2rhogamma'               ,
            Pi0Veto__Tagger               ,
            [ pre_etap ]                  ,
            MassWindow     = 25 * MeV     ,
            MassChi2       = -1           ,
            ExtraInfoIndex = 25017     ## unique ! 
            )
    
    # ============================================================================
    ## selection for  etap -> pi+ pi- eta
    # ============================================================================ 
    def etap2pipieta ( self ) :
        """
        Selection for Eta' -> pi+ pi- eta         
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        ##
        pre_etap = self.make_selection (
            ## the unique tag 
            'PreEtapPiPiEta'               ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.pions () , self.eta_ ()  ] ,
            ##
            DecayDescriptor = " eta_prime -> pi+ pi- eta" ,
            ## 
            Combination12Cut  = """  ( AM < 600 * MeV  ) &
            ( ACHI2DOCA(1,2) < 12 ) 
            """ , 
            CombinationCut  = """
            ( APT > %s ) & 
            ( ADAMASS ( 'eta_prime' ) <    100 * MeV ) 
            """ % ( 0.9 * min ( self['X0PT'] , self['X0PTK'] ) ),
            ##
            MotherCut       = """
            ( PT     > %s ) &
            ( chi2vx <  9 )
            """ % min ( self['X0PT'] , self['X0PTK'] )
            )
        
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger2g
        ## 
        return self.make_selection (
            'Etap2pipieta'                ,
            Pi0Veto__Tagger2g             ,
            [ pre_etap ]                  ,
            MassWindow     = 25 * MeV     ,
            MassChi2       = -1           ,
            ExtraInfoIndex = 25018     ## unique ! 
            )
    
    # =========================================================================
    ## all etas 
    # =========================================================================
    def eta_prime ( self ) :
        """
        All eta's 
        """
        tag      = 'EtaPrime_selection'
        sel      = self._selection ( tag )
        if sel : return sel
        #
        from PhysSelPython.Wrappers import MergedSelection
        sel = MergedSelection (
            'SelEtaPrimAllFor' + self.name() ,
            RequiredSelections = [ self.etap2rhog    () ,
                                   self.etap2pipieta () ]
            )
        
        return self._add_selection ( tag , sel ) 
    
    
    # =========================================================================
    ## omega -> 3 pi 
    # =========================================================================
    def omega ( self ) :
        """
        omega -> 3 pi selection 
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        ##
        pre_omega = self.make_selection (
            ## the unique tag 
            'PreOmega'                ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.pions () , self.pi0 ()  ] ,
            ##
            DecayDescriptor = " omega(782) -> pi+ pi- pi0" ,
            ## 
            Combination12Cut  = """  ( AM < 1 * GeV  ) &
            ( ACHI2DOCA(1,2) < 12 ) 
            """ ,
            ## 
            CombinationCut  = """
            ( APT > %s ) & ( ADAMASS ( 'omega(782)' ) < 100 * MeV )
            """ % ( 0.9 * min ( self['X0PT'] , self['X0PTK'] ) ),
            ##
            MotherCut       = """
            ( PT     > %s ) &
            ( chi2vx <  9 )
            """ % min ( self['X0PT'] , self['X0PTK'] ) 
            )
        
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger2g
        ## 
        return self.make_selection (
            'Omega'                       ,
            Pi0Veto__Tagger2g             ,
            [ pre_omega ]                 ,
            MassWindow     = 25 * MeV     ,
            MassChi2       = -1           ,
            ExtraInfoIndex = 25019     ## unique ! 
            )
    
    # =========================================================================
    ## psi(') -> mu+ mu-
    # =========================================================================
    def psi ( self ) :
        """
        psi(') -> mu+ mu- 
        """
        psi_name = 'Psi'
        sel_name = 'Sel%sFor%s' % ( psi_name , self.name() )
        sel_tag  = '%s_Selection'
        sel      = self._selection( sel_tag ) 
        if sel  : return sel 
        
        dimu_lines = self['DIMUONLINES']
        if not dimu_lines :
            ## 
            from GaudiConfUtils.ConfigurableGenerators import CombineParticles
            ##
            return self.make_selection (
                psi_name         ,
                CombineParticles ,
                [ self.muons() ] ,
                ## 
                DecayDescriptor = " J/psi(1S) -> mu+ mu- " ,
                ##
                DaughtersCuts   = {
                'mu+'   :  self [ 'MuonCut' ] 
                } ,
                ## 
                CombinationCut  = """
                ( ADAMASS  ( 'J/psi(1S)' ) < 120 * MeV ) | 
                ( ADAMASS  (   'psi(2S)' ) < 120 * MeV ) 
                """ ,
                ##
                MotherCut       = """
                chi2vx < 20
                """ 
                )

        ## list of lines 
        from PhysSelPython.Wrappers import AutomaticData 
        if 1 < len( dimu_lines ) :
            lines = [ AutomaticData ( l ) for l in dimu_lines ] 
            from PhysSelPython.Wrappers import MergedSelection
            sel = MergedSelection ( sel_name , RequiredSelections = lines ) 
            return self._add_selection ( sel_tag , sel ) 

        ## single line  
        dimu_line = dimu_lines[0]
        sel = AutomaticData ( dimu_line ) 
        return self._add_selection ( sel_tag , sel ) 
        
    # =========================================================================
    ## chi_c -> J/psi gamma 
    # =========================================================================
    def chi_c ( self ) :
        """
        chi_c -> J/psi gamma 
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        ##
        pre_chic = self.make_selection (
            'PreChic' ,
            CombineParticles ,
            [ self.psi() , self.gamma()  ] ,
            ## 
            DecayDescriptor = " chi_c1(1P) -> J/psi(1S) gamma " ,
            ##
            DaughtersCuts   = {
            'J/psi(1S)'   :  " M  < ( 3.100 + 0.120 ) * GeV " ,
            'gamma'       :  " PT > 400 * MeV "
            } ,
            ## 
            CombinationCut  = """
            ( AM - AM1 ) < 650 * MeV
            """ ,
            ##
            MotherCut       = " PALL "
            )
        
        ##         
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger
        ## 
        return self.make_selection (
            'Chic'                        ,
            Pi0Veto__Tagger               ,
            [ pre_chic ]                  ,
            MassWindow     = 25 * MeV     ,
            MassChi2       = -1           ,
            ExtraInfoIndex = 25030     ## unique ! 
            )
    
    # ============================================================================
    ## Beauty -> psi(') X0
    # ============================================================================
    
    # ============================================================================
    # B -> psi(') eta 
    # ============================================================================
    def b2eta ( self ) :
        """
        B -> psi(') eta
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        ##
        return self.make_selection (
            'B2PsiEta2gg'  ,
            CombineParticles ,
            [ self.psi() , self.eta0()  ] ,
            ## 
            DecayDescriptor = "B_s0 -> J/psi(1S) eta" ,
            ##
            Preambulo       = self['Preambulo'] ,
            DaughtersCuts   = { 'eta' : ' PT > %s ' % self['X0PT'] } ,
            ##
            CombinationCut  = "mb0_acut" ,
            ##
            MotherCut       = """
            mb0_cut     & 
            ( ctau > %s ) 
            """ % self['CTAU']
            ##
            )
    
    # =========================================================================
    # B -> psi(')  eta'
    # =========================================================================
    def b2etap ( self ) :
        """
        B -> psi(') eta'
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        ##
        return self.make_selection (
            'B2PsiEtaPrime'  ,
            CombineParticles  ,
            [ self.psi()      , self.eta_prime () ] ,
            ## 
            DecayDescriptor = "B_s0 -> J/psi(1S) eta_prime" ,
            ##
            Preambulo       = self['Preambulo'] ,
            DaughtersCuts   = { 'eta_prime' : ' PT > %s ' % self['X0PT'] } ,
            ##
            CombinationCut  = "mb0_acut" ,
            ##
            MotherCut       = """
            mb0_cut            & 
            ( chi2vxndf < 10 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU']
            ##
            )
    
    # =========================================================================
    # B -> psi(') (omega -> 3pi  ) 
    # =========================================================================
    def b2omega ( self ) :
        """
        B -> psi(') (omega -> 3pi  ) 
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        ##
        return self.make_selection (
            'B2PsiOmega'  ,
            CombineParticles  ,
            [ self.psi()      , self.omega () ] ,
            ## 
            DecayDescriptor = "B0 -> J/psi(1S) omega(782)" ,
            ##
            DaughtersCuts   = { 'omega(782)' : ' PT > %s ' % self['X0PT'] } ,
            ##
            CombinationCut  = "mb0_acut" ,
            ##
            MotherCut       = """
            mb0_cut            &
            ( chi2vxndf < 10 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU']
            ##
            )


    # ============================================================================
    ## Beauty -> chi_c  X 
    # ============================================================================

    # ============================================================================
    # B -> chi_c K 
    # ============================================================================
    def b2chicK ( self ) :
        """
        B -> chic K 
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2ChicK'                   ,
            ## algorithm type to be used
            CombineParticles            ,
            ## input selections 
            [ self.chi_c () , self.kaons()  ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B+ -> chi_c1(1P) K+ ]cc" ,
            ##
            CombinationCut = """ mbu_acut &
            ( ACHI2DOCA(1,2) < 20 )
            """ , 
            ## 
            MotherCut        = """
            mbu_cut                 &
            ( chi2vxNDF < 10      ) &  
            ( ctau      > %s      ) 
            """ % self['CTAU'],
            )


    # ============================================================================
    # Bs -> chi_c K K 
    # ============================================================================
    def b2chicKK ( self ) :
        """
        B -> chic K K
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2ChicKK'                    ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.chi_c () , self.kaons()  ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "B_s0 -> chi_c1(1P) K- K+" ,
            ##
            Combination12Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 ) 
            """  ,
            ## 
            CombinationCut   = """ mb0_acut &
            ( ACHI2DOCA(1,3) < 20 ) & 
            ( ACHI2DOCA(2,3) < 20 ) 
            """ , 
            ## 
            MotherCut        = """
            mbu_cut                 &
            ( chi2vxNDF < 10      ) &  
            ( ctau      > %s      ) 
            """ % self['CTAU'],
            )
    
    # ============================================================================
    # B0 -> chi_c K- pi+ 
    # ============================================================================
    def b2chicKpi ( self ) :
        """
        B -> chic K- pi+
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2ChicKPi'                   ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.chi_c () , self.kaons() , self.pions ()  ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B0 -> chi_c1(1P) K- pi+]cc" ,
            ##
            Combination12Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 ) 
            """  ,
            ## 
            CombinationCut   = """ mb0_acut &
            ( ACHI2DOCA(1,3) < 20 ) & 
            ( ACHI2DOCA(2,3) < 20 ) 
            """ , 
            ## 
            MotherCut        = """
            mbu_cut                 &
            ( chi2vxNDF < 10      ) &  
            ( ctau      > %s      ) 
            """ % self['CTAU'],
            )


    # ============================================================================
    # B- -> chi_c K+ pi+ pi-  
    # ============================================================================
    def b2chicKpipi ( self ) :
        """
        B -> chic K+ pi+ pi-
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2ChicKPiPi'                   ,
            ## algorithm type to be used
            DaVinci__N4BodyDecays         ,
            ## input selections 
            [ self.chi_c () , self.kaons() , self.pions ()  ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B+ -> chi_c1(1P) K+ pi+ pi-]cc" ,
            ##
            Combination12Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 ) 
            """  ,
            ## 
            Combination123Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,3) < 20 ) & 
            ( ACHI2DOCA(2,3) < 20 ) 
            """  ,
            ## 
            CombinationCut   = """ mb0_acut &
            ( ACHI2DOCA(1,4) < 20 ) & 
            ( ACHI2DOCA(2,4) < 20 ) &
            ( ACHI2DOCA(3,4) < 20 ) 
            """ , 
            ## 
            MotherCut        = """
            mbu_cut                 &
            ( chi2vxNDF < 10      ) &  
            ( ctau      > %s      ) 
            """ % self['CTAU'],
            )


    # ============================================================================
    # Bs -> chi_c pi pi 
    # ============================================================================
    def b2chicpipi ( self ) :
        """
        B -> chic pi pi 
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2ChicPiPi'                  ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.chi_c () , self.pions()  ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "B_s0 -> chi_c1(1P) pi- pi+" ,
            ##
            Combination12Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 ) 
            """  ,
            ## 
            CombinationCut   = """ mb0_acut &
            ( ACHI2DOCA(1,3) < 20 ) & 
            ( ACHI2DOCA(2,3) < 20 ) 
            """ , 
            ## 
            MotherCut        = """
            mbu_cut                 &
            ( chi2vxNDF < 10      ) &  
            ( ctau      > %s      ) 
            """ % self['CTAU'],
            )
    
    # ============================================================================
    # Bc -> chi_c pi
    # ============================================================================
    def bc2chicpi ( self ) :
        """
        Bc -> chic pi+ 
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
        ## 
        return self.make_selection (
            ## the unique tag 
            'Bc2ChicPi'                 ,
            ## algorithm type to be used
            CombineParticles            ,
            ## input selections 
            [ self.chi_c () , self.pions()  ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B_c+ -> chi_c1(1P) pi+ ]cc" ,
            ##
            CombinationCut = """ mbc_acut &
            ( ACHI2DOCA(1,2) < 20 )
            """ , 
            ## 
            MotherCut        = """
            mbc_cut                 &
            ( chi2vxNDF < 10      ) &  
            in_range ( 75   * um  , ctau ,  5 * mm     )
            """
            )

    # ============================================================================
    # Lb -> chi_c p K 
    # ============================================================================
    def lb2chicpK ( self ) :
        """
        Lambda_b -> chic p K 
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays
        ## 
        return self.make_selection (
            ## the unique tag 
            'Lb2ChicPK'                   ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.chi_c () , self.protons() , self.kaons () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[Lambda_b0 -> chi_c1(1P) p+ K-]cc" ,
            ##
            Combination12Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 )
            """  ,
            ## 
            CombinationCut   = """ mbl_acut &
            ( ACHI2DOCA(1,3) < 20 ) & 
            ( ACHI2DOCA(2,3) < 20 ) 
            """ , 
            ## 
            MotherCut        = """
            mbl_cut                 &
            ( chi2vxNDF < 10      ) &  
            ( ctau      > %s      ) 
            """ % self['CTAU'],
            )
    
    # ============================================================================
    # B -> psi(') K eta
    # ============================================================================
    def b2Keta ( self ) :
        """
        B -> psi(') K eta 
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2PsiKEta'                   ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons()   , self.eta0 () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B+ -> J/psi(1S) K+ eta]cc" ,
            ##
            DaughtersCuts   = { 'eta' : ' PT > %s ' % self['X0PTK'] } ,
            ##
            Combination12Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 )  
            """  ,
            ## 
            CombinationCut   = " mbu_acut " , 
            ## 
            MotherCut        = """
            mbu_cut                 &
            ( PT        > 1 * GeV ) &
            ( chi2vxNDF < 10      ) &  
            ( ctau      > %s      ) 
            """ % self['CTAU'],
            )
    
    # ============================================================================
    # B -> psi(') pi  eta
    # ============================================================================
    def b2pieta ( self ) :
        """
        B -> psi(') pi eta 
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2PsiPiEta'                ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.psi() , self.pions() , self.eta0 () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B+ -> J/psi(1S) pi+ eta]cc" ,
            ## 
            DaughtersCuts   = { 'eta' : ' PT > %s ' % self['X0PTK'] } ,
            ##
            Combination12Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 )  
            """  ,
            ## 
            CombinationCut   = " mbu_acut " , 
            ## 
            MotherCut        = """
            mbu_cut                 &
            ( PT        > 1 * GeV ) &
            ( chi2vxNDF < 10      ) &  
            ( ctau      > %s      ) 
            """ % self['CTAU'],
            )
        
    # ============================================================================
    # B -> psi(') K K eta
    # ============================================================================
    def b2KKeta ( self ) :
        """
        B -> psi(') K K eta 
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays 
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2PsiKKEta'                   ,
            ## algorithm type to be used
            DaVinci__N4BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons()   , self.eta0 () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "B_s0 -> J/psi(1S) K+ K- eta" ,
            ##
            DaughtersCuts   = { 'eta' : ' PT > %s ' % self['X0PTK'] } ,
            ##
            Combination12Cut  = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 )  
            """  ,
            Combination123Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,3) < 20 )  &
            ( ACHI2DOCA(2,3) < 20 )  
            """  ,
            ## 
            CombinationCut   = " mb0_acut " , 
            ## 
            MotherCut        = """
            mb0_cut            &
            ( chi2vxNDF < 10 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'],
            )
    
    # ============================================================================
    # B -> psi(') pi pi  eta
    # ============================================================================
    def b2pipieta ( self ) :
        """
        B -> psi(') pi pi eta 
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays 
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2PsiPiPiEta'                ,
            ## algorithm type to be used
            DaVinci__N4BodyDecays         ,
            ## input selections 
            [ self.psi() , self.pions() , self.eta0 () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "B_s0 -> J/psi(1S) pi+ pi- eta" ,
            ## 
            DaughtersCuts   = { 'eta' : ' PT > %s ' % self['X0PTK'] } ,
            ##
            Combination12Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 )  
            """  ,
            ## 
            Combination123Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,3) < 20 ) & 
            ( ACHI2DOCA(2,3) < 20 )  
            """  ,
            ## 
            CombinationCut   = " mb0_acut " , 
            ## 
            MotherCut        = """
            mb0_cut            &
            ( chi2vxNDF < 10 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'],
            )
    
    # ============================================================================
    # B -> psi(') K pi eta
    # ============================================================================
    def b2Kpieta ( self ) :
        """
        B -> psi(') K pi eta 
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays 
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2PsiKPiEta'                 ,
            ## algorithm type to be used
            DaVinci__N4BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons() , self.pions() , self.eta0 () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B0 -> J/psi(1S) K+ pi- eta]cc" ,
            ##
            DaughtersCuts   = { 'eta' : ' PT > %s ' % self['X0PTK'] } ,
            ##
            Combination12Cut  = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 )  
            """  ,
            Combination123Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,3) < 20 ) & 
            ( ACHI2DOCA(2,3) < 20 )  
            """  ,
            ## 
            CombinationCut   = " mb0_acut " , 
            ## 
            MotherCut        = """
            mb0_cut            &
            ( chi2vxNDF < 10 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'],
            )
    
    

    # ============================================================================
    # B -> psi(') K eta'
    # ============================================================================
    def b2Ketap ( self ) :
        """
        B -> psi(') K eta'
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2PsiKEtaPrime'               ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons()   , self.eta_prime () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B+ -> J/psi(1S) K+ eta_prime]cc" ,
            ##
            DaughtersCuts   = { 'eta_prime' : ' PT > %s ' % self['X0PTK'] } ,
            ##
            Combination12Cut = """ ( AM < 7 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 )  
            """  ,
            ## 
            CombinationCut   = " mbc_acut " , 
            ## 
            MotherCut        = """
            mbc_cut            &
            ( chi2vxNDF < 10 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'],
            )

    # ============================================================================
    # B -> psi(') pi  eta'
    # ============================================================================
    def b2pietap ( self ) :
        """
        B -> psi(') pi eta'
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2PsiPiEtaPrime'                ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.psi() , self.pions() , self.eta_prime () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B+ -> J/psi(1S) pi+ eta_prime]cc" ,
            ## 
            DaughtersCuts   = { 'eta_prime' : ' PT > %s ' % self['X0PTK'] } ,
            ##
            Combination12Cut = """ ( AM < 7 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 )  
            """  ,
            ## 
            CombinationCut   = " mbc_acut " , 
            ## 
            MotherCut        = """
            mbc_cut            &
            ( chi2vxNDF < 10 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'],
            )
        
    # ============================================================================
    # B -> psi(') K K eta'
    # ============================================================================
    def b2KKetap ( self ) :
        """
        B -> psi(') K K eta' 
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays 
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2PsiKKEtaPrime'             ,
            ## algorithm type to be used
            DaVinci__N4BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons()   , self.eta_prime () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "B_s0 -> J/psi(1S) K+ K- eta_prime" ,
            ##
            DaughtersCuts   = { 'eta_prime' : ' PT > %s ' % self['X0PTK'] } ,
            ##
            Combination12Cut  = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 )  
            """  ,
            Combination123Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,3) < 20 ) & 
            ( ACHI2DOCA(2,3) < 20 )  
            """  ,
            ## 
            CombinationCut   = " mb0_acut " , 
            ## 
            MotherCut        = """
            mb0_cut            &
            ( chi2vxNDF < 10 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'],
            )
    
    # ============================================================================
    # B -> psi(') pi pi  eta'
    # ============================================================================
    def b2pipietap ( self ) :
        """
        B -> psi(') pi pi eta'
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays 
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2PsiPiPiEtaPrime'           ,
            ## algorithm type to be used
            DaVinci__N4BodyDecays         ,
            ## input selections 
            [ self.psi() , self.pions() , self.eta_prime () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "B_s0 -> J/psi(1S) pi+ pi- eta_prime " ,
            ## 
            DaughtersCuts   = { 'eta_prime' : ' PT > %s ' % self['X0PTK'] } ,
            ##
            Combination12Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 )  
            """  ,
            ## 
            Combination123Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,3) < 20 ) & 
            ( ACHI2DOCA(2,3) < 20 )  
            """  ,
            ## 
            CombinationCut   = " mb0_acut " , 
            ## 
            MotherCut        = """
            mb0_cut            &
            ( chi2vxNDF < 10 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'],
            )
    
    # ============================================================================
    # B -> psi(') K pi eta'
    # ============================================================================
    def b2Kpietap ( self ) :
        """
        B -> psi(') K pi eta' 
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays 
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2PsiKPiEtaPrime'            ,
            ## algorithm type to be used
            DaVinci__N4BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons() , self.pions() , self.eta_prime () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B0 -> J/psi(1S) K+ pi- eta_prime]cc" ,
            ##
            DaughtersCuts   = { 'eta_prime' : ' PT > %s ' % self['X0PTK'] } ,
            ##
            Combination12Cut  = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 )  
            """  ,
            Combination123Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,3) < 20 )  & 
            ( ACHI2DOCA(2,3) < 20 )  
            """  ,
            ## 
            CombinationCut   = " mb0_acut " , 
            ## 
            MotherCut        = """
            mb0_cut            &
            ( chi2vxNDF < 10 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'],
            )
        

    # ============================================================================
    # B -> psi(') K omega
    # ============================================================================
    def b2Komega ( self ) :
        """
        B -> psi(') K omega
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2PsiKOmega'               ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons()   , self.omega () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B+ -> J/psi(1S) K+ omega(782) ]cc" ,
            ##
            DaughtersCuts   = { 'omega(782)' : ' PT > %s ' % self['X0PTK'] } ,
            ##
            Combination12Cut = """ ( AM < 7 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 )  
            """  ,
            ## 
            CombinationCut   = " mbc_acut " , 
            ## 
            MotherCut        = """
            mbc_cut            &
            ( chi2vxNDF < 10 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'],
            )

    # ============================================================================
    # B -> psi(') pi  omega
    # ============================================================================
    def b2piomega ( self ) :
        """
        B -> psi(') pi omega
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2PsiPiOmega'                ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.psi() , self.pions() , self.omega () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B+ -> J/psi(1S) pi+ omega(782) ]cc" ,
            ## 
            DaughtersCuts   = { 'omega(782)' : ' PT > %s ' % self['X0PTK'] } ,
            ##
            Combination12Cut = """ ( AM < 7 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 )  
            """  ,
            ## 
            CombinationCut   = " mbc_acut " , 
            ## 
            MotherCut        = """
            mbc_cut            &
            ( chi2vxNDF < 10 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'],
            )
    
    # ============================================================================
    # B -> psi(') K K omega 
    # ============================================================================
    def b2KKomega ( self ) :
        """
        B -> psi(') K K omega
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays 
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2PsiKKOmega'                ,
            ## algorithm type to be used
            DaVinci__N4BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons()   , self.omega () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "B_s0 -> J/psi(1S) K+ K- omega(782)" ,
            ##
            DaughtersCuts   = { 'omega(782)' : ' PT > %s ' % self['X0PTK'] } ,
            ##
            Combination12Cut  = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 )  
            """  ,
            Combination123Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,3) < 20 ) &
            ( ACHI2DOCA(2,3) < 20 )  
            """  ,
            ## 
            CombinationCut   = " mb0_acut " , 
            ## 
            MotherCut        = """
            mb0_cut            &
            ( chi2vxNDF < 10 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'],
            )
    
    # ============================================================================
    # B -> psi(') pi pi  omega
    # ============================================================================
    def b2pipiomega ( self ) :
        """
        B -> psi(') pi pi omega
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays 
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2PsiPiPiOmega'           ,
            ## algorithm type to be used
            DaVinci__N4BodyDecays         ,
            ## input selections 
            [ self.psi() , self.pions() , self.omega () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "B_s0 -> J/psi(1S) pi+ pi- omega(782)" ,
            ## 
            DaughtersCuts   = { 'omega(782)' : ' PT > %s ' % self['X0PTK'] } ,
            ##
            Combination12Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 )  
            """  ,
            ## 
            Combination123Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,3) < 20 )  &
            ( ACHI2DOCA(2,3) < 20 )  
            """  ,
            ## 
            CombinationCut   = " mb0_acut " , 
            ## 
            MotherCut        = """
            mb0_cut            &
            ( chi2vxNDF < 10 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'],
            )
    
    # ============================================================================
    # B -> psi(') K pi omega 
    # ============================================================================
    def b2Kpiomega ( self ) :
        """
        B -> psi(') K pi omega
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays 
        ## 
        return self.make_selection (
            ## the unique tag 
            'B2PsiKPiOmega'               ,
            ## algorithm type to be used
            DaVinci__N4BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons() , self.pions() , self.omega () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B0 -> J/psi(1S) K+ pi- omega(782)]cc" ,
            ##
            DaughtersCuts   = { 'omega(782)' : ' PT > %s ' % self['X0PTK'] } ,
            ##
            Combination12Cut  = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,2) < 20 )  
            """  ,
            Combination123Cut = """ ( AM < 6 * GeV  ) &
            ( ACHI2DOCA(1,3) < 20 ) & 
            ( ACHI2DOCA(2,3) < 20 )  
            """  ,
            ## 
            CombinationCut   = " mb0_acut " , 
            ## 
            MotherCut        = """
            mb0_cut            &
            ( chi2vxNDF < 10 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU']
            )
    
    # =========================================================================
    # B+ -> psi ( K*+ -> K+ pi0)
    # =========================================================================
    def bu2Kstar ( self ) :
        """
        This is just a control line to study
        the reconstruction efficiency for pi0 and gamma
        see LHCb-INT-2012-001 
        
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays
        pre_bkst = self.make_selection (
            ## the unique tag 
            'PreBu2PsiKstar'              ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## inputs 
            [ self.psi   () , self.kaons () , self.pi0  () ] ,
            ## configuration:
            DecayDescriptor = "[B+ -> J/psi(1S) K+ pi0 ]cc" ,
            ## keep only J/psi 
            DaughtersCuts = { 'J/psi(1S)' : " M < 3.3 * GeV " } ,
            ## 
            Combination12Cut = """  ( AM < 5.75 * GeV ) &
            ( ACHI2DOCA (1,2) < 20 )
            """ ,             
            CombinationCut = """
            in_range ( 4.90 * GeV , AM   , 5.80 * GeV ) &
            in_range ( 750  * MeV , AM23 , 1250 * MeV ) &
            ( APT23 > 1 * GeV ) 
            """ , 
            ## 
            MotherCut = """
            in_range ( 4.95 * GeV , M , 5.75 * GeV ) & 
            ( chi2vxndf < 10 ) &
            ( ctau_9    > %s ) 
            """ % self['CTAU_Kst'] 
            )
        ##
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger2g
        ## 
        return self.make_selection (
            'Bu2PsiKstar'              ,
            Pi0Veto__Tagger2g          ,
            [ pre_bkst ]               ,
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25020     ## unique ! 
            )

    # =========================================================================
    # B+ -> psi ( K* -> K+ pi0-merged)
    # =========================================================================
    def bu2KstarM ( self ) :
        """
        B+ -> psi ( K* -> K+ pi0-merged )        
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays
        from StandardParticles                     import StdLooseMergedPi0 as pi0_merged
        return self.make_selection (
            ## the unique tag 
            'Bu2PsiKstarMerged'           ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## inputs 
            [ self.psi   () , self.kaons () , pi0_merged ] ,
            ## configuration:
            DecayDescriptor = "[B+ -> J/psi(1S) K+ pi0 ]cc" ,
            ## keep only J/psi 
            DaughtersCuts = { 'J/psi(1S)' : " M < 3.3 * GeV " } ,
            ## 
            Combination12Cut = """ ( AM < 7.1 * GeV ) &
            ( ACHI2DOCA (1,2) < 20 ) 
            """ ,             
            CombinationCut = """
            in_range ( 4.40 * GeV , AM , 7.10 * GeV ) &
            ( AM23 < 2.5 * GeV ) 
            """ , 
            ##
            MotherCut = """
            in_range ( 4.45 * GeV , M , 7.05 * GeV ) & 
            ( chi2vxndf < 10 ) 
            """
            )
    ## ( ctau_9 > %s ) 
    ## """ % self['CTAU_Kst'] 

    # =========================================================================
    # B(c) -> psi ( rho+ -> pi+ pi0 )
    # =========================================================================
    def bc2rho ( self ) :
        """
        Bc+ -> psi ( rho+ -> pi+ pi0 ) in very wide window 
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays
        ##
        pre_rho = self.make_selection (
            ## the unique tag 
            'PreBc2PsiRho'                ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## inputs 
            [ self.psi   () , self.pions() , self.pi0  () ] ,
            ## configuration:
            DecayDescriptor  = "[B_c+ -> J/psi(1S) pi+ pi0 ]cc" ,
            ## 
            DaughtersCuts    = {
            'J/psi(1S)' :   "    M   < 3.3 * GeV " , ## keep only J/psi 
            'pi+'       :   "   PT   > 500 * MeV " ,
            'pi0'       : """
            (             PT   > 500 * MeV ) &
            ( CHILD ( 1 , PT ) > 300 * MeV ) &
            ( CHILD ( 2 , PT ) > 300 * MeV ) 
            """
            } ,
            ## 
            Combination12Cut = """
            ( AM < 7.6 * GeV    )  & 
            ( ACHI2DOCA ( 1 , 2 )  < 10 )  
            """ ,
            ##
            CombinationCut   = """
            in_range ( 4.4  * GeV , AM , 7.6 * GeV ) &
            ( AM23 < 2.5 * GeV ) 
            """ ,
            ##
            MotherCut        = """
            ( chi2vx < 16  ) & 
            in_range ( 4.45 * GeV , M      ,  7.55 * GeV ) & 
            in_range ( 90   * um  , ctau_9 ,  5 * mm     )
            """
            )
        ##
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger2g
        ## 
        return self.make_selection (
            'Bc2PsiRho'                ,
            Pi0Veto__Tagger2g          ,
            [ pre_rho ]                ,
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25025     ## unique ! 
            )
    
# =============================================================================
if '__main__' == __name__ :
    
    
    logger.info ( 80*'*'  ) 
    logger.info (  __doc__ ) 
    logger.info ( ' Author :  %s' % __author__ ) 
    logger.info ( ' Date   :  %s' % __date__   )
    ##
    clines = set() 
    logger.info ( ' Lines declared in default_config["STREAMS"] are' )
    for stream in default_config['STREAMS'] :
        lines = default_config['STREAMS'][stream] 
        for l in lines :
            logger.info ( ' %-15s : %-50s ' % ( stream , l ) )
            clines.add ( l )
    ##
    logger.info ( ' The output locations for the default configuration: ' )
    ##
    _conf = PsiX0Conf ( 'PsiX0' , 
                        config = default_config['CONFIG']  )
    ##
    _ln   = ' ' + 61*'-' + '+' + 30*'-'
    logger.info ( _ln ) 
    logger.info ( '  %-60s| %-30s  ' % ( 'Output location', 'Stripping line name' ) ) 
    logger.info ( _ln )
    for l in _conf.lines() :
        lout  = l.outputLocation()
        lname = l.name() 
        logger.info ( '  %-60s| %-30s  ' % ( lout, lname ) )
        if not lname in clines :
            raise AttributeError ('Unknown Line %s' % lname )
        clines.remove ( lname )
    logger.info ( _ln ) 
    logger.info ( 80*'*'  ) 
    if clines :
        raise AttributeError('Undeclared lines: %s' % clines )

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

