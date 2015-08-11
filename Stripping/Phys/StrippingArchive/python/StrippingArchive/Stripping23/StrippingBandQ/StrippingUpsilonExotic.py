##!/usr/bin/env python
# =============================================================================
# $Id:$
# =============================================================================
## @file
# 
#  The attempt for coherent stripping for X -> Upsilon X0 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-03-22
#

#                   $Revision: 135537 $
# Last modification $Date: 2012-02-19 15:18:08 +0100 (Sun, 19 Feb 2012) $
#                by $Author: ibelyaev $
# =============================================================================
""" The attempt for coherent stripping for X -> Upsilon X0 

- phi 
- eho 
- eta 
- eta'
- omega 
- proton
- prton + kaon 
"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = '2015-03-22'
__version__ = '$Revision:$'
# =============================================================================
__all__ = (
    'UpsilonExoticConf' ,
    'default_config'    , 
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
    'NOPIDHADRONS'   : False ,  ## USE FOR SIMULATION "True"
    ## use for B&Q wg production
    'DIMUONLINE'     : None  ,  ## USE FOR B&Q WG-selection  
    #
    'ETA_PT'    : 1.5 * GeV ,
    'ETAP_PT'   : 1.5 * GeV ,
    'OMEGA_PT'  : 1.5 * GeV ,    
    #
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
    'Eta2ggCut' : " 250    * MeV < MINTREE ( 'gamma' == ID , PT ) " , 
    #
    ## muon selection for  Y -> mu+ mu-
    #
    'MuonCut'   : """
    ISMUON &
    ( PT            >  750 * MeV ) &
    ( PIDmu - PIDpi >    0       ) &
    ( CLONEDIST     > 5000       )     
    """ ,
    #
    ## pions 
    # 
    'PionCut'   : """
    ( PT          > 200 * MeV ) & 
    ( CLONEDIST   > 5000      ) & 
    ( TRGHOSTPROB < 0.5       ) &
    ( TRCHI2DOF   < 4         ) & 
    in_range ( 2          , ETA , 5         ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                     
    """ ,
    #
    ## kaons
    # 
    'KaonCut'   : """
    ( PT          > 200 * MeV ) & 
    ( CLONEDIST   > 5000      ) & 
    ( TRGHOSTPROB < 0.5       ) &
    ( TRCHI2DOF   < 4         ) & 
    in_range ( 2          , ETA , 5         ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                     
    """ ,
    #
    ## protons 
    #
    'ProtonCut'   : """
    ( PT           > 200 * MeV ) & 
    ( CLONEDIST    > 5000      ) & 
    ( TRCHI2DOF    < 4         ) & 
    ( TRGHOSTPROB  < 0.5       ) & 
    in_range ( 2         , ETA , 5         ) &
    in_range ( 10 * GeV  , P   , 150 * GeV ) &
    HASRICH                      
    """ ,
    #
    'UpsilonCut' : """
    ( M > 7 * GeV ) & 
    DECTREE   ('Meson -> mu+ mu-'  )                      &
    CHILDCUT( 1 , HASMUON & ISMUON )                      &
    CHILDCUT( 2 , HASMUON & ISMUON )                      & 
    ( MINTREE ( 'mu+' == ABSID , PT ) > 1 * GeV         ) &
    ( MAXTREE ( ISBASIC & HASTRACK , TRCHI2DOF ) < 4    ) & 
    ( MINTREE ( ISBASIC & HASTRACK , CLONEDIST ) > 5000 ) & 
    ( VFASPF  ( VPCHI2 ) > 0.5/100 ) 
    & ( abs ( BPV ( VZ    ) ) <  0.5 * meter     ) 
    & (       BPV ( vrho2 )   < ( 10 * mm ) ** 2 ) 
    """ ,
 
    #
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
    'vrho2     = VX**2 + VY**2'                                , 
    ] ,
    }
## ============================================================================
## the mandatory element for stripping framework 
default_config = {
    #
    'NAME'        :   'UpsilonExotic'        ,
    'WGs'         : [ 'BandQ' ]              ,
    'CONFIG'      : _default_configuration_  , 
    'BUILDERTYPE' :   'UpsilonExoticConf'    ,
    'STREAMS'     : { 'Dimuon'    : [] }
    }
# =============================================================================
## @class  UpsilonExoticConf
#  B-> psi(') X0 configuration file 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2012-02-19
class UpsilonExoticConf(LineBuilder) :
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
        if 'UpsilonExotic' != name :
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
        
        for line in self._lines_UpsilonExotic () :
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
    
    ## empty, no real stripping lines here 
    def _lines_UpsilonExotic ( self ) : return [] 

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
            self.upsilons          () ,
            #
            self.pi0               () ,
            self.eta0              () ,
            self.eta_prime         () ,
            self.omega             () ,
            #
            self.upsilon_phi       () ,
            self.upsilon_rho       () ,
            self.upsilon_eta       () ,
            self.upsilon_eta_prime () ,
            self.upsilon_omega     () ,
            self.upsilon_proton    () ,
            self.upsilon_pK        () 
            #
            ]
        
        return self._add_selection ( 'Selections' , sel )
    
    # ========================================================================
    ## pions :
    # ========================================================================
    def pions    ( self ) :
        """
        Pions for  X -> Upsilon X lines 
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
        Kaons for X -> Upsilon X 
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
    def protons     ( self ) :
        """
        Kaons for X -> Upsilon P 
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        ## 
        if self['NOPIDHADRONS'] :
            from StandardParticles import   StdAllNoPIDsProtons as inpts
            kaoncut = self['ProtonCut']
        else                    :
            from StandardParticles import StdAllLooseANNProtons as inpts 
            kaoncut = "(%s)&(%s)" % ( self['ProtonCut'] , self['ProtonPIDCut'] ) 
        #
        ##
        return self.make_selection (
            'Proton'               ,
            FilterDesktop          ,
            [ inpts ]              ,
            Code = protoncut       ,
            )
    
    # ========================================================================
    ## muons 
    # ========================================================================
    def muons    ( self ) :
        """
        Muons for  Upsilon -> mu+ mu-
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from StandardParticles                     import StdAllLooseMuons  as inpts 
        ##
        return self.make_selection (
            'Muons'                 ,
            FilterDesktop           ,
            [ inpts ]               ,
            Code = self['MuonCut']  ,
            )

    # =========================================================================
    ## make heavy dimuons 
    # =========================================================================
    def dimuon ( self ) :
        """
        Create heavy dimuons
        """
        #
        ups_name = 'HeavyDiMion'
        sel_name = 'Sel%sFor%s' % ( ups_name , self.name() )
        sel_tag  = '%s_Selection'
        sel      = self._selection( sel_tag )        
        if sel  : return sel
        #
        dimu_line = self[ 'DIMUONLINE']
        if dimu_line :
            ## list of lines 
            from PhysSelPython.Wrappers import AutomaticData 
            sel = AutomaticData( dimu_line ) 
            return self._add_selection ( sel_tag , sel ) 
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        return self.make_selection (
            ups_name         ,
            CombineParticles ,
            [ self.muons() ] ,
            DecayDescriptor = " J/psi -> mu+ mu- " ,
            CombinationCut  = "in_range ( 6.5 * GeV , AM , 20 * GeV ) ", 
            MotherCut       = "chi2vx < 20" 
            )
    
    # =========================================================================
    ## get good upsilons
    # =========================================================================
    def upsilons ( self ) :
        """
        Select good upsilons, cuts ar einspire by variosu Y analyses at LHCb
        """
        ##
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        ##
        return self.make_selection  (
            'Upsilon'           ,
            FilterDesktop       , 
            [ self.dimuon () ]  , 
            ## algorithm parameters 
            Code     = self['UpsilonCut'] ,
            ## 
            ReFitPVs = True
            ##
            )
    
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
            'EtaStd'                  ,
            FilterDesktop             ,
            [ inpts ]                 ,
            Code = self['Eta2ggCut']  ,
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
            """ % self['ETA_PT']  
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
            """ % ( 0.9 * self['ETA_PT'] ) ,
            ##
            MotherCut       = """
            ( PT     > %s ) &
            ( chi2vx <  9 ) 
            """ %         self['ETA_PT']
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
        #
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
            """ % ( 0.9 * self['ETAP_PT'] ),
            ##
            MotherCut       = """
            ( PT     > %s ) &
            ( chi2vx <  9 )
            """ %         self['ETAP_PT'] 
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
            DaughtersCuts   = {
            'eta' : " ( ADMASS ('eta') < 100 * MeV ) "
            } ,
            ## 
            Combination12Cut  = """  ( AM < 600 * MeV  ) &
            ( ACHI2DOCA(1,2) < 12 ) 
            """ , 
            CombinationCut  = """
            ( APT > %s ) & 
            ( ADAMASS ( 'eta_prime' ) <    100 * MeV ) 
            """ % ( 0.9 * self['ETAP_PT'] ),
            ##
            MotherCut       = """
            ( PT     > %s ) &
            ( chi2vx <  9 )
            """ %         self['ETAP_PT'] 
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
    ## all eta's 
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
            """ % ( 0.9 * self['OMEGA_PT'] ),
            ##
            MotherCut       = """
            ( PT     > %s ) &
            ( chi2vx <  9 )
            """ %         self['OMEGA_PT']
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
    # X -> Upsilon phi(K+K-)
    # =========================================================================
    def  upsilon_phi( self ) :
        """
        X -> Upsilon phi(K+ K-)
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        #
        return self.make_selection  (
            'YKK'                 ,
            DaVinci__N3BodyDecays ,
            [ self.upsilons() , self.kaons() ] ,
            ## algorithm properties 
            DecayDescriptor  = " Upsilon(4S) -> J/psi(1S) K+ K-" ,
            Combination12Cut = """
            ( AM < 15 * GeV       ) &
            ( ACHI2DOCA(1,2) < 16 )
            """ ,
            CombinationCut = """
            ( AM                  < 15   * GeV ) &
            ( AM23                < 1050 * MeV ) & 
            ( ( AM - AM1 - AM23 ) < 2.5  * GeV ) &
            ( ACHI2DOCA(1,3)      < 16  ) &
            ( ACHI2DOCA(2,3)      < 16  )
            """ ,
            MotherCut      = " chi2vxndf<10" ,
            )
    
    # =========================================================================
    # X -> Upsilon rho0 
    # =========================================================================
    def  upsilon_rho( self ) :
        """
        X -> Upsilon rho0
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        #
        return self.make_selection  (
            'Ypipi'               ,
            DaVinci__N3BodyDecays ,
            [ self.upsilons() , self.pions() ] ,
            ## algorithm properties 
            DecayDescriptor  = " Upsilon(4S) -> J/psi(1S) pi+ pi-" ,
            Combination12Cut = """
            ( AM < 15 * GeV       ) &
            ( ACHI2DOCA(1,2) < 16 )
            """ ,
            CombinationCut = """
            ( AM                  < 15   * GeV ) &
            ( AM23                < 1050 * MeV ) & 
            ( ( AM - AM1 - AM23 ) < 2.5  * GeV ) &
            ( ACHI2DOCA(1,3)      < 16         ) &
            ( ACHI2DOCA(2,3)      < 16         )
            """ ,
            MotherCut      = " chi2vxndf< 10 " ,
            )

    # =========================================================================
    # X -> Upsilon proton 
    # =========================================================================
    def  upsilon_proton ( self ) :
        """
        X -> Upsilon proton
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        #
        return self.make_selection  (
            'YP'                 ,
            CombineParticles ,
            [ self.upsilons() , self.protons() ] ,
            ## algorithm properties 
            DecayDescriptor  = "[Upsilon(4S) -> J/psi(1S) p+]cc" ,
            CombinationCut   = """
            ( AM                 < 15   * GeV ) &
            ( ( AM - AM1 - AM2 ) < 2.5  * GeV ) &
            ( ACHI2DOCA(1,2)     < 16  ) 
            """ ,
            MotherCut      = " chi2vxndf<10" ,
            )

    # =========================================================================
    # X -> Upsilon proton kaon 
    # =========================================================================
    def  upsilon_pK ( self ) :
        """
        X -> Upsilon proton kaon 
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays
        #
        return self.make_selection  (
            'YPK'                   ,
            DaVinci__N3BodyDecays   ,
            [ self.upsilons() , self.protons() , self.kaons() ] ,
            ## algorithm properties 
            DecayDescriptor  = "[Upsilon(4S) -> J/psi(1S) p+ K-]cc" ,
            Combination12Cut = """
            ( AM < 15 * GeV       ) &
            ( ACHI2DOCA(1,2) < 16 )
            """ ,
            CombinationCut = """
            ( AM                  < 15   * GeV ) &
            ( AM23                < 2000 * MeV ) & 
            ( ( AM - AM1 - AM23 ) < 2.5  * GeV ) &
            ( ACHI2DOCA(1,3)      < 16         ) &
            ( ACHI2DOCA(2,3)      < 16         )
            """ ,
            MotherCut      = " chi2vxndf< 10 " ,
            )            
    
    # =========================================================================
    # X -> Upsilon eta
    # =========================================================================
    def  upsilon_eta ( self ) :
        """
        X -> Upsilon eta
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
        #
        return self.make_selection  (
            'Yeta'                 ,
            CombineParticles       ,
            [ self.upsilons() , self.eta0() ] ,
            ## algorithm properties 
            DecayDescriptor = " chi_b0(1P) -> J/psi(1S) eta" ,
            CombinationCut  = """
            (   AM               < 15  * GeV ) &
            ( ( AM - AM1 - AM2 ) < 2.5 * GeV )
            """,
            MotherCut      = " chi2vxndf< 10 " 
            )
    
    # =========================================================================
    # X -> Upsilon eta'
    # =========================================================================
    def  upsilon_eta_prime ( self ) :
        """
        X -> Upsilon eta'
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
        #
        return self.make_selection  (
            'Yeta_prime'           ,
            CombineParticles       ,
            [ self.upsilons() , self.eta_prime () ] ,
            ## algorithm properties 
            DecayDescriptor = " chi_b1(1P) -> J/psi(1S) eta_prime" ,
            CombinationCut  = """
            (   AM               < 15  * GeV ) &
            ( ( AM - AM1 - AM2 ) < 2.5 * GeV )
            """,
            MotherCut      = " chi2vxndf< 10 " 
            )
    
    # =========================================================================
    # X -> Upsilon omega
    # =========================================================================
    def  upsilon_omega ( self ) :
        """
        X -> Upsilon omega
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
        #
        return self.make_selection  (
            'Yomega'           ,
            CombineParticles       ,
            [ self.upsilons() , self.omega () ] ,
            ## algorithm properties 
            DecayDescriptor = " chi_b2(1P) -> J/psi(1S) omega(782)" ,
            CombinationCut  = """
            (   AM               < 15  * GeV ) &
            ( ( AM - AM1 - AM2 ) < 2.5 * GeV )
            """,
            MotherCut      = " chi2vxndf< 10 " 
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
    _conf = UpsilonExoticConf ( 'UpsilonExotic' , 
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
 
