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
"""
The attempt for coherent stripping for B -> psi(') X0 modes,
where X0 :

- eta   -> gamma gamma ,
- eta   -> pi+ pi- pi0 ,
- eta'  -> rho0 gamma 
- eta'  -> pi+ pi- ( eta -> gamma gamma )
- omega -> pi+ pi- pi0

The cuts correspond to J/psiX0 selection by Dasha Savrina LHCb-ANA-2011-096

Attention: the ``wid'' versions of pi0->2gg  and eta->gg a used
           to cure the potential problems with bad ECAL calibration

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = '2012-02-19'
__version__ = '$Revision$'
# =============================================================================
__all__ = (
    'PsiX0Conf'      ,
    )
# =============================================================================
from GaudiKernel.SystemOfUnits             import GeV, MeV, mm, micrometer 
from StrippingConf.StrippingLine           import StrippingLine
from StrippingUtils.Utils                  import LineBuilder
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger 
from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger2g 
from PhysSelPython.Wrappers                import Selection
# =============================================================================
# Standard Particles 
# =============================================================================
from StandardParticles      import ( StdLoosePions       ,
                                     StdLooseKaons       ,
                                     StdLooseMuons       ,
                                     StdLooseAllPhotons  , 
                                     StdLoosePi02gg      , ## ``wide''-version
                                     StdLooseEta2gg      ) ## ``wide''-version 
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
    ## PV-requiremens
    #
    'CheckPV'   : True ,
    #
    ## the transverse momentum of X0
    'X0PT'      : 2.0  * GeV        ,
    ## the transverse momentum of X0, when accompanied with K(K) 
    'X0PTK'     : 1.0  * GeV        ,
    #
    ## c*tau cut for B-candidates
    #
    'CTAU'      : 125  * micrometer ,
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
    ( PT  > 550 * MeV   ) &
    ( PIDmu - PIDpi > 0 ) &
    ( CLONEDIST     > 0 )     
    """ , 
    #
    ## pion cuts
    #
    'PionCut'   : """
    ( CLONEDIST   > 5000   ) & 
    ( TRCHI2DOF   < 5      ) &
    ( TRGHOSTPROB < 0.5    ) & 
    in_range ( 2 , ETA , 5 ) &
    HASRICH                  & 
    ( MIPCHI2DV() > 9      ) 
    """ , 
    #
    ## kaon cuts
    #
    'KaonCut'   : """
    ( CLONEDIST    > 5000   ) & 
    ( TRCHI2DOF    < 5      ) & 
    ( TRGHOSTPROB  < 0.5    ) &
    in_range ( 2 , ETA , 5  ) &
    HASRICH                   & 
    ( MIPCHI2DV()  >  9     ) &
    ( PIDK - PIDpi >  0     ) 
    """ ,
    #
    ## useful shortcuts:
    #
    'Preambulo' : [
    ## shortcut for chi2 of vertex fit 
    'chi2vx    = VFASPF(VCHI2)     '                          , 
    'chi2vxNDF = VFASPF(VCHI2PDOF) '                          , 
    ## shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light"       , 
    ## use the embedded cut for chi2(LifetimeFit)<25
    "ctau      = BPVLTIME ( 25 ) * c_light "                  ,
    ## Combination mass-cut for beauty particles 
    "mb_acut   = in_range ( 4.50 * GeV , AM , 6.00 * GeV ) "  ,
    ## mass-cut for beauty particles 
    "mb_cut    = in_range ( 4.60 * GeV , M  , 5.90 * GeV ) "  ,
    ] ,
    # =========================================================================
    ## Prescales 
    # =========================================================================
    'Eta2ggPrescale'        : 1.0 ,
    'Eta23piPrescale'       : 1.0 ,
    'Etap2rhogPrescale'     : 1.0 ,
    'Etap2pipietaPrescale'  : 1.0 ,
    'OmegaPrescale'         : 1.0 ,
    #
    'KEta2ggPrescale'       : 1.0 ,
    'KEta23piPrescale'      : 1.0 ,
    'KEtap2rhogPrescale'    : 1.0 ,
    'KEtap2pipietaPrescale' : 1.0 ,
    'KOmegaPrescale'        : 1.0 ,    
    # ========================================================================= 
    }
## ============================================================================
## @class  PsiX0Conf
#  B-> psi(') X0 configuration file 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2012-02-19
class PsiX0Conf(LineBuilder) :
    """
    Helper class to configure 'PsiX0'-lines
    """
    __configuration_keys__ = tuple ( _default_configuration_.keys() )
    
    def __getitem__ ( self , item ) :
        return self.configurationParameter ( item )
    
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
                logger.warning ('new configuration: %-16s : %s ' % ( key , _config[key] ) )
                
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

    ## get the selections
    def _selections_private ( self ) :
        
        sel = self._selections ( 'Selections' )
        if sel : return sel
        
        sel =  [
            #
            ## basic
            #
            self.gamma          () ,
            self.pi0            () ,
            self.eta            () ,
            self.muons          () ,
            self.pions          () ,
            self.kaons          () ,
            #
            ## local 
            self.psi            () ,
            self.eta2gg         () ,
            self.eta23pi        () ,
            self.etap2rhog      () ,
            self.etap2pipieta   () ,
            self.omega          () ,
            #
            ## beauty
            #
            self.b2eta2gg        () ,
            self.b2eta23pi       () ,
            self.b2etap2rhog     () ,
            self.b2etap2pipieta  () ,
            self.b2omega         () , 
            ##
            self.b2Keta2gg       () ,
            self.b2Keta23pi      () ,
            self.b2Ketap2rhog    () ,
            self.b2Ketap2pipieta () ,
            self.b2Komega        ()
            ##
            ]
        
        return self._add_selection ( 'Selections' , sel )
    
    ## get all single charm lines 
    def _lines_psiX0   ( self ) :
        """
        Get all psiX0 lines 
        """
        sel = self._selection ( 'PsiX0Lines' )
        if sel : return sel
        #
        sel = [
            #
            ## B -> psi(') X0  
            #
            StrippingLine (
            "B2PsiEta2ggFor" + self.name()     ,
            prescale = self['Eta2ggPrescale' ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'] ,
            algos    = [ self.b2eta2gg ()      ]
            ) ,
            ## 
            StrippingLine (
            "B2PsiEta23piFor" + self.name()     ,
            prescale = self['Eta23piPrescale']  , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'        ]  ,
            algos    = [ self.b2eta23pi ()      ]
            ) ,
            ## 
            StrippingLine (
            "B2PsiEtap2rhogFor" + self.name()    ,
            prescale = self['Etap2rhogPrescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'          ] ,
            algos    = [ self.b2etap2rhog ()     ]
            ) ,
            ## 
            StrippingLine (
            "B2PsiEtap2pipietaFor" + self.name()    ,
            prescale = self['Etap2pipietaPrescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'             ] ,
            algos    = [ self.b2etap2pipieta ()     ]
            ) ,
            ## 
            StrippingLine (
            "B2PsiOmegaFor" + self.name ()    ,
            prescale = self['OmegaPrescale']  , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'      ]  ,
            algos    = [ self.b2omega ()      ]
            ) ,
            #
            ## B -> psi(') X0 K(K)
            #
            StrippingLine (
            "B2PsiKEta2ggFor" + self.name()     ,
            prescale = self['KEta2ggPrescale' ] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'         ] ,
            algos    = [ self.b2Keta2gg ()      ]
            ) ,
            ## 
            StrippingLine (
            "B2PsiKEta23piFor" + self.name()     ,
            prescale = self['KEta23piPrescale']  , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'         ]  ,
            algos    = [ self.b2Keta23pi ()      ]
            ) ,
            ## 
            StrippingLine (
            "B2PsiKEtap2rhogFor" + self.name()    ,
            prescale = self['KEtap2rhogPrescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'           ] ,
            algos    = [ self.b2Ketap2rhog ()     ]
            ) ,
            ## 
            StrippingLine (
            "B2PsiKEtap2pipietaFor" + self.name()    ,
            prescale = self['KEtap2pipietaPrescale'] , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'              ] ,
            algos    = [ self.b2Ketap2pipieta ()     ]
            ) ,
            ## 
            StrippingLine (
            "B2PsiKOmegaFor" + self.name ()    ,
            prescale = self['KOmegaPrescale']  , ## ATTENTION! Prescale here !!
            checkPV  = self['CheckPV'       ]  ,
            algos    = [ self.b2Komega ()      ]
            ) ,
            #
            ]
        #
        return self._add_selection ( 'PsiX0Lines' , sel ) 
    
    ## pions :
    def pions  ( self ) :
        """
        Pions for   B -> psi X0 lines 
        """        
        sel = self._selection ( 'Pion_Selection')
        if sel : return sel
        
        alg  = FilterDesktop (
            ##
            Preambulo = self['Preambulo'] ,
            ##
            Code = self['PionCut'] ,
            ##
            )
        
        sel  = Selection (
            "SelPiFor"         + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ StdLoosePions ]  
            )
        ##
        return self._add_selection( 'Pion_Selection' , sel ) 

    ## kaons :
    def kaons  ( self ) :
        """
        Kaons for   B -> psi X0 K(K)lines 
        """        
        sel = self._selection ( 'Kaon_Selection')
        if sel : return sel
        
        alg  = FilterDesktop (
            ##
            Preambulo = self['Preambulo'] ,
            ##
            Code = self['KaonCut'] ,
            ##
            )
        
        sel  = Selection (
            "SelKFor"         + self.name()      ,
            Algorithm          =   alg           ,
            RequiredSelections = [ StdLooseKaons ]  
            )
        ##
        return self._add_selection( 'Kaon_Selection' , sel )
    
    ## muons 
    def muons  ( self ) : return StdLooseMuons 
    ## gamma 
    def gamma  ( self ) : return StdLooseAllPhotons
    
    ## define proper pi0 
    def pi0  ( self ) :
        """
        Define proper pi0
        """
        sel = self._selection ( 'Pi0_Selection')
        if sel : return sel
        
        ## 
        _alg  = FilterDesktop  ( Code =  self['Pi0Cut'] )
        ## 
        sel  = Selection (
            "SelPi0For"        + self.name()  ,
            Algorithm          =   _alg       ,
            RequiredSelections = [ StdLoosePi02gg ] 
            )
        ##
        return self._add_selection( 'Pi0_Selection' , sel ) 
    
    ## define proper eta
    def eta  ( self ) :
        """
        Define proper eta -> gamma gamma 
        """
        sel = self._selection ( 'Eta_Selection')
        if sel : return sel
        
        ## 
        _alg  = FilterDesktop  ( Code = self['EtaCut'] ) 
        ## 
        sel  = Selection (
            "SelEtaFor"        + self.name() ,
            Algorithm          =   _alg      ,
            RequiredSelections = [ StdLooseEta2gg ] 
            )
        ##
        return self._add_selection( 'Eta_Selection' , sel ) 
    
    ## selection for  eta -> gamma gamma 
    def eta2gg ( self ) :
        """
        Preselection for Eta -> gamma gamma
        
        """
        sel = self._selection ( 'Eta2gg_Selection')
        if sel : return sel
        
        ## 
        _alg  = FilterDesktop  (
            Code = """
            ( ADMASS ('eta') < 100 * MeV ) &
            ( PT > %s ) 
            """ % min ( self['X0PT'] , self['X0PTK'] ) 
            )
        ## 
        _sel  = Selection (
            "SelPreEta2ggFor"  + self.name()  ,
            Algorithm          =   _alg       ,
            RequiredSelections = [  self.eta () ] 
            )
        ##
        _tag = Pi0Veto__Tagger2g (
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25015     ## unique ! 
            )
        ## 
        sel  = Selection (
            "SelEta2ggFor"     + self.name() ,
            Algorithm          =   _tag   ,
            RequiredSelections = [ _sel   ] 
            )
        ##
        return self._add_selection( 'Eta2gg_Selection' , sel ) 
    
    ## selection for  eta -> pipipi
    def eta23pi ( self ) :
        """
        Preselection for Eta -> pipipi
        
        """
        sel = self._selection ( 'Eta23pi_Selection')
        if sel : return sel
        
        ## 
        _alg     = CombineParticles (
            ##
            DecayDescriptor = " eta -> pi+ pi- pi0" ,
            ##
            Preambulo       = self['Preambulo'],
            ##
            CombinationCut  = """
            ( APT > %s ) & ( ADAMASS ( 'eta' ) < 100 * MeV )
            """ % ( 0.9 * min ( self['X0PT'] , self['X0PTK'] ) ) ,
            ##
            MotherCut       = """
            ( PT     > %s ) &
            ( chi2vx <  9 ) 
            """ % min ( self['X0PT'] , self['X0PTK'] ) ,
            ## 
            ParticleCombiners = { '' : "LoKi::VertexFitter:PUBLIC" }
            )
        ## 
        _sel  = Selection (
            "SelPreEta23piFor" + self.name()          ,
            Algorithm          =   _alg                ,
            RequiredSelections = [ self.pions () ,
                                   self.pi0   () ] 
            )
        ##
        _tag = Pi0Veto__Tagger2g (
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25016     ## unique ! 
            )
        ## 
        sel  = Selection (
            "SelEta23piFor" + self.name() ,
            Algorithm          =   _tag   ,
            RequiredSelections = [ _sel   ] 
            )
        ##
        return self._add_selection( 'Eta23pi_Selection' , sel ) 
    
    ## selection for  etap -> rho0 gamma 
    def etap2rhog ( self ) :
        """
        Preselection for Eta' -> rho gamma
        
        """
        sel = self._selection ( 'Etap2rhog_Selection')
        if sel : return sel
        
        ## 
        _alg     = CombineParticles (
            ##
            DecayDescriptor = " eta_prime -> pi+ pi- gamma" ,
            ##
            Preambulo       = self['Preambulo'],
            ##
            DaughtersCuts   = {
            'gamma' :  self['GammaCut']
            } ,
            ## 
            CombinationCut  = """
            ( APT > %s ) & 
            in_range  ( 500 * MeV , AM12 , 950 * MeV ) & 
            ( ADAMASS ( 'eta_prime' ) <    100 * MeV ) 
            """ % ( 0.9 * min ( self['X0PT'] , self['X0PTK'] ) ),
            ##
            MotherCut       = """
            ( PT     > %s ) &
            ( chi2vx <  9 )
            """ % min ( self['X0PT'] , self['X0PTK'] ) ,
            ##   
            ParticleCombiners = { '' : "LoKi::VertexFitter:PUBLIC" }
            )
        ## 
        _sel  = Selection (
            "SelPreEtap2rhogFor" + self.name()  ,
            Algorithm          =   _alg ,
            RequiredSelections = [ self.pions () ,
                                   self.gamma () ] 
            )
        ##
        _tag = Pi0Veto__Tagger (
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25017     ## unique ! 
            )
        ## 
        sel  = Selection (
            "SelEtap2rhogFor" + self.name()  ,
            Algorithm          =   _tag   ,
            RequiredSelections = [ _sel   ] 
            )
        ##
        return self._add_selection( 'Etap2rhog_Selection' , sel ) 
    
    ## selection for  etap -> pi+ pi- eta  
    def etap2pipieta ( self ) :
        """
        Preselection for Eta' -> pi+ pi- eta         
        """
        sel = self._selection ( 'Etap2pipieta_Selection')
        if sel : return sel
        
        ## 
        _alg     = CombineParticles (
            ##
            DecayDescriptor = " eta_prime -> pi+ pi- eta" ,
            ##
            Preambulo       = self['Preambulo'],
            ##
            CombinationCut  = """
            ( APT > %s ) & ( ADAMASS ( 'eta_prime' ) < 100 * MeV )
            """ % ( 0.9 * min ( self['X0PT'] , self['X0PTK'] ) ) ,
            ##
            MotherCut       = """
            ( PT     > %s ) &
            ( chi2vx < 9  )
            """ % min ( self['X0PT'] , self['X0PTK'] ) ,
            ## 
            ParticleCombiners = {'' : "LoKi::VertexFitter:PUBLIC" }
            )
        ## 
        _sel  = Selection (
            "SelPreEtap2pipietagFor" + self.name()  ,
            Algorithm          =   _alg ,
            RequiredSelections = [  self.pions ()  ,
                                    self.eta   ()  ] 
            )
        ##
        _tag = Pi0Veto__Tagger2g (
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25018     ## unique ! 
            )
        ## 
        sel  = Selection (
            "SelEtap2pipietaFor" + self.name()  ,
            Algorithm          =   _tag   ,
            RequiredSelections = [ _sel   ] 
            )
        ##
        return self._add_selection( 'Etap2pipieta_Selection' , sel ) 
    
    ## omega -> 3 pi 
    def omega ( self ) :
        """
        omega -> 3 pi selection 
        """
        sel = self._selection ( 'Omega_Selection')
        if sel : return sel
        
        ## 
        _alg     = CombineParticles (
            ##
            DecayDescriptor = " omega(782) -> pi+ pi- pi0" ,
            ##
            Preambulo       = self['Preambulo'],
            ##
            CombinationCut  = """
            ( APT > %s ) & ( ADAMASS ( 'omega(782)' ) < 100 * MeV )
            """ % ( 0.9 * min ( self['X0PT'] , self['X0PTK'] ) ),
            ##
            MotherCut       = """
            ( PT     > %s ) &
            ( chi2vx <  9 )
            """ % min ( self['X0PT'] , self['X0PTK'] ) , 
            ## 
            ParticleCombiners = {'' : "LoKi::VertexFitter:PUBLIC" }
            )
        ## 
        _sel  = Selection (
            "SelPreOmegaFor"   + self.name()    ,
            Algorithm          =   _alg         ,
            RequiredSelections = [ self.pions() ,
                                   self.pi0  () ] 
            )
        ##
        _tag = Pi0Veto__Tagger2g (
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25019     ## unique ! 
            )
        ## 
        sel  = Selection (
            "SelOmegaFor"  + self.name()  ,
            Algorithm          =   _tag   ,
            RequiredSelections = [ _sel   ] 
            )
        ##
        return self._add_selection( 'Omega_Selection' , sel ) 

    
    ## psi(') -> mu+ mu-
    def psi ( self ) :
        """
        psi(') -> mu+ mu- 
        """
        sel = self._selection ( 'Psi_Selection')
        if sel : return sel

        _alg     = CombineParticles (
            ##
            DecayDescriptor = " J/psi(1S) -> mu+ mu- " ,
            ##
            Preambulo       = self['Preambulo'] , 
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
            """ ,
            ## 
            )
        
        sel  = Selection (
            "SelPsiFor"        + self.name()    ,
            Algorithm          =   _alg         ,
            RequiredSelections = [ self.muons() ] 
            )
        ##
        return self._add_selection( 'Psi_Selection' , sel ) 
    
    # ============================================================================
    ## Beauty -> psi(') X0
    # ============================================================================
    
    # B -> psi(') ( eta -> gg ) 
    def b2eta2gg ( self ) :
        """
        B -> psi(') ( eta -> gg ) 
        """
        sel = self._selection ( 'B2PsiEta2gg_Selection')
        if sel : return sel
        
        _alg = CombineParticles (
            ## 
            DecayDescriptor = "B_s0 -> J/psi(1S) eta" ,
            ##
            Preambulo       = self['Preambulo'] ,
            ##
            DaughtersCuts   = {
            'eta' : ' PT > %s ' % self['X0PT']
            } ,
            ##
            CombinationCut  = "mb_acut" ,
            ##
            # MotherCut = " ALL " ,
            MotherCut       = """
            mb_cut      & 
            ( ctau > %s ) 
            """ % self['CTAU'],
            ##
            ParticleCombiners = {'' : "LoKi::VertexFitter:PUBLIC" }
            )
        ##
        sel  = Selection (
            "SelB2PsiEta2ggFor" + self.name() ,
            Algorithm           =   _alg      ,
            RequiredSelections  = [ self.psi    () ,
                                    self.eta2gg () ] 
            )
        ##
        return self._add_selection( 'B2PsiEta2gg_Selection' , sel ) 
    
    # B -> psi(') ( eta -> 3pi ) 
    def b2eta23pi ( self ) :
        """
        B -> psi(') ( eta -> 3pi ) 
        """
        sel = self._selection ( 'B2PsiEta23pi_Selection')
        if sel : return sel
        
        _alg = CombineParticles (
            ## 
            DecayDescriptor = "B_s0 -> J/psi(1S) eta" ,
            ##
            Preambulo = self['Preambulo'] ,
            #
            DaughtersCuts   = {
            'eta' : ' PT > %s ' % self['X0PT']
            } ,
            ##
            CombinationCut  = " mb_acut " , 
            ##
            MotherCut = """
            mb_cut             & 
            ( chi2vxNDF <  6 ) &  
            ( ctau      > %s ) 
            """ % self ['CTAU'],
            ##
            ParticleCombiners = {'' : "LoKi::VertexFitter:PUBLIC" }
            )
        ##
        sel  = Selection (
            "SelB2PsiEta23piFor" + self.name() ,
            Algorithm            =   _alg      ,
            RequiredSelections   = [ self.psi     () ,
                                     self.eta23pi () ] 
            )
        ##
        return self._add_selection( 'B2PsiEta23pi_Selection' , sel ) 
    
    # B -> psi(') ( eta' -> rhog ) 
    def b2etap2rhog ( self ) :
        """
        B -> psi(') ( eta' -> rhog )         
        """
        sel = self._selection ( 'B2PsiEtap2rhog_Selection')
        if sel : return sel
        
        _alg = CombineParticles (
            ## 
            DecayDescriptor = "B_s0 -> J/psi(1S) eta_prime" ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            DaughtersCuts   = {
            'eta_prime' : ' PT > %s ' % self['X0PT']
            } ,
            ## 
            CombinationCut  = "mb_acut " ,
            ##
            # MotherCut = " ALL " ,
            MotherCut = """
            mb_cut             & 
            ( chi2vxNDF <  6 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'],
            ##
            ParticleCombiners = {'' : "LoKi::VertexFitter:PUBLIC" }
            )
        ##
        sel  = Selection (
            "SelB2PsiEtap2rhogFor" + self.name() ,
            Algorithm              =   _alg      ,
            RequiredSelections     = [ self.psi       () ,
                                       self.etap2rhog () ] 
            )
        ##
        return self._add_selection( 'B2PsiEtap2rhog_Selection' , sel ) 

    # B -> psi(') ( eta' -> pi pi eta  ) 
    def b2etap2pipieta ( self ) :
        """
        B -> psi(') ( eta' -> pi pi eta  ) 
        """
        sel = self._selection ( 'B2PsiEtap2pipieta_Selection')
        if sel : return sel
        
        _alg = CombineParticles (
            ## 
            DecayDescriptor = "B_s0 -> J/psi(1S) eta_prime" ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            DaughtersCuts   = {
            'eta_prime' : ' PT > %s ' % self['X0PT']
            } ,
            ## 
            ##
            CombinationCut  = "mb_acut ", 
            ##
            # MotherCut = " ALL " ,
            MotherCut = """
            mb_cut             & 
            ( chi2vxNDF <  6 ) &  
            ( ctau      > %s ) 
            """ % self ['CTAU'] ,
            ##
            ParticleCombiners = {'' : "LoKi::VertexFitter:PUBLIC" }
            )
        ##
        sel  = Selection (
            "SelB2PsiEtap2pipietaFor" + self.name() ,
            Algorithm                 =   _alg      ,
            RequiredSelections        = [ self.psi          () ,
                                          self.etap2pipieta () ] 
            )
        ##
        return self._add_selection( 'B2PsiEtap2pipieta_Selection' , sel ) 
    
    # B -> psi(') (omega -> 3pi  ) 
    def b2omega ( self ) :
        """
        """
        sel = self._selection ( 'B2PsiOmega_Selection')
        if sel : return sel
        
        _alg = CombineParticles (
            ## 
            DecayDescriptor = "B0 -> J/psi(1S) omega(782)" ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            DaughtersCuts   = {
            'omega(782)' : ' PT > %s ' % self['X0PT']
            } ,
            ##
            CombinationCut  = " mb_acut " , 
            ##
            # MotherCut = " ALL " ,
            MotherCut = """
            mb_cut             & 
            ( chi2vxNDF <  6 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'] ,
            ##
            ParticleCombiners = {'' : "LoKi::VertexFitter:PUBLIC" }
            )
        ##
        sel  = Selection (
            "SelB2PsiOmegaFor" + self.name() ,
            Algorithm          =   _alg      ,
            RequiredSelections = [ self.psi   () ,
                                   self.omega () ] 
            )
        ##
        return self._add_selection( 'B2PsiOmega_Selection' , sel ) 
    
    # ============================================================================
    ## Beauty -> psi(') X0  K (K)
    # ============================================================================
    
    # B -> psi(') ( eta -> gg ) K (K) 
    def b2Keta2gg ( self ) :
        """
        B -> psi(') ( eta -> gg ) K (K) 
        """
        sel = self._selection ( 'B2PsiKEta2gg_Selection')
        if sel : return sel
        
        _alg = CombineParticles (
            ## 
            DecayDescriptors = [
            "[B+   -> J/psi(1S) K+    eta]cc " ,
            " B_s0 -> J/psi(1S) K+ K- eta    " ,
            ] , 
            ##
            Preambulo       = self['Preambulo'] ,
            ##
            DaughtersCuts   = {
            'eta' : ' PT > %s ' % self['X0PTK']
            } ,
            ##
            CombinationCut  = "mb_acut" ,
            ##
            MotherCut       = """
            mb_cut             &
            ( chi2vxNDF <  6 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'],
            ##
            ParticleCombiners = {'' : "LoKi::VertexFitter:PUBLIC" }
            )
        ##
        sel  = Selection (
            "SelB2PsiKEta2ggFor" + self.name() ,
            Algorithm            =   _alg      ,
            RequiredSelections   = [ self.psi    () ,
                                     self.kaons  () , 
                                     self.eta2gg () , ] 
            )
        ##
        return self._add_selection( 'B2PsiKEta2gg_Selection' , sel ) 

    
    # B -> psi(') ( eta -> 3pi ) K(K)
    def b2Keta23pi ( self ) :
        """
        B -> psi(') ( eta -> 3pi ) K(K)
        """
        sel = self._selection ( 'B2PsiKEta23pi_Selection')
        if sel : return sel
        
        _alg = CombineParticles (
            ## 
            DecayDescriptors = [
            "[B+   -> J/psi(1S) K+    eta]cc " ,
            " B_s0 -> J/psi(1S) K+ K- eta    " ,
            ] , 
            ##
            Preambulo = self['Preambulo'] ,
            #
            DaughtersCuts   = {
            'eta' : ' PT > %s ' % self['X0PTK']
            } ,
            ##
            CombinationCut  = " mb_acut " , 
            ##
            MotherCut = """
            mb_cut             & 
            ( chi2vxNDF <  6 ) &  
            ( ctau      > %s ) 
            """ % self ['CTAU'],
            ##
            ParticleCombiners = {'' : "LoKi::VertexFitter:PUBLIC" }
            )
        ##
        sel  = Selection (
            "SelB2PsiKEta23piFor" + self.name() ,
            Algorithm            =   _alg      ,
            RequiredSelections   = [ self.psi     () ,
                                     self.kaons   () ,
                                     self.eta23pi () ] 
            )
        ##
        return self._add_selection( 'B2PsiKEta23pi_Selection' , sel ) 
    
    # B -> psi(') ( eta' -> rhog ) K (K)
    def b2Ketap2rhog ( self ) :
        """
        B -> psi(') ( eta' -> rhog ) K (K)    
        """
        sel = self._selection ( 'B2PsiKEtap2rhog_Selection')
        if sel : return sel
        
        _alg = CombineParticles (
            ## 
            DecayDescriptors = [
            "[B+   -> J/psi(1S) K+    eta_prime]cc " ,
            " B_s0 -> J/psi(1S) K+ K- eta_prime    " ,
            ] ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            DaughtersCuts   = {
            'eta_prime' : ' PT > %s ' % self['X0PTK']
            } ,
            ## 
            CombinationCut  = "mb_acut " ,
            ##
            MotherCut = """
            mb_cut             & 
            ( chi2vxNDF <  6 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'],
            ##
            ParticleCombiners = {'' : "LoKi::VertexFitter:PUBLIC" }
            )
        ##
        sel  = Selection (
            "SelB2PsiKEtap2rhogFor" + self.name() ,
            Algorithm              =   _alg      ,
            RequiredSelections     = [ self.psi       () ,
                                       self.kaons     () ,
                                       self.etap2rhog () ] 
            )
        ##
        return self._add_selection( 'B2PsiKEtap2rhog_Selection' , sel ) 
    
    # B -> psi(') ( eta' -> pi pi eta  ) K (K)
    def b2Ketap2pipieta ( self ) :
        """
        B -> psi(') ( eta' -> pi pi eta  ) K (K)
        """
        sel = self._selection ( 'B2PsiKEtap2pipieta_Selection')
        if sel : return sel
        
        _alg = CombineParticles (
            ## 
            DecayDescriptors = [
            "[B+   -> J/psi(1S) K+    eta_prime]cc" , 
            " B_s0 -> J/psi(1S) K+ K- eta_prime   "
            ] ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            DaughtersCuts   = {
            'eta_prime' : ' PT > %s ' % self['X0PTK']
            } ,
            ## 
            CombinationCut  = "mb_acut ", 
            ##
            MotherCut = """
            mb_cut             & 
            ( chi2vxNDF <  6 ) &  
            ( ctau      > %s ) 
            """ % self ['CTAU'] ,
            ##
            ParticleCombiners = {'' : "LoKi::VertexFitter:PUBLIC" }
            )
        ##
        sel  = Selection (
            "SelB2PsiKEtap2pipietaFor" + self.name() ,
            Algorithm                 =   _alg      ,
            RequiredSelections        = [ self.psi          () ,
                                          self.kaons        () ,
                                          self.etap2pipieta () ] 
        )
        ##
        return self._add_selection( 'B2PsiKEtap2pipieta_Selection' , sel ) 
    
    # B -> psi(') (omega -> 3pi  ) K ( K ) 
    def b2Komega ( self ) :
        """
        B -> psi(') (omega -> 3pi  ) K ( K ) 
        """
        sel = self._selection ( 'B2PsiKOmega_Selection')
        if sel : return sel
        
        _alg = CombineParticles (
            ## 
            DecayDescriptors = [
            "[B+   -> J/psi(1S) K+    omega(782)]cc" , 
            " B_s0 -> J/psi(1S) K+ K- omega(782)   "
            ],
            ##
            Preambulo = self['Preambulo'] ,
            ##
            DaughtersCuts   = {
            'omega(782)' : ' PT > %s ' % self['X0PTK']
            } ,
            ##
            CombinationCut  = " mb_acut " , 
            ##
            MotherCut = """
            mb_cut             & 
            ( chi2vxNDF <  6 ) &  
            ( ctau      > %s ) 
            """ % self['CTAU'] ,
            ##
            ParticleCombiners = {'' : "LoKi::VertexFitter:PUBLIC" }
            )
        ##
        sel  = Selection (
            "SelB2PsiKOmegaFor" + self.name() ,
            Algorithm          =   _alg       ,
            RequiredSelections = [ self.psi   () ,
                                   self.kaons () ,  
                                   self.omega () ] 
            )
        ##
        return self._add_selection( 'B2PsiKOmega_Selection' , sel ) 
    
    
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

