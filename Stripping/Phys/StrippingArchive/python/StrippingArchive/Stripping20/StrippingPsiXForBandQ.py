##!/usr/bin/env python
# =============================================================================
# $Id:$
# =============================================================================
## @file
# 
#   The coherent (miroDST) stripping for B -> psi(') + X for B&Q 
#
#   @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#   @date   2012-02-19
#
#                   $Revision: 135537 $
# Last modification $Date: 2012-02-19 15:18:08 +0100 (Sun, 19 Feb 2012) $
#                by $Author: ibelyaev $
# =============================================================================
"""
The coherent (microDST) stripping for B -> psi(') + X for B&Q

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = '2012-02-19'
__version__ = '$Revision$'
# =============================================================================
__all__ = (
    'PsiX_BQ_Conf' ,
    )
# =============================================================================
from GaudiKernel.SystemOfUnits             import GeV, MeV, mm, micrometer 
from StrippingConf.StrippingLine           import StrippingLine
from StrippingUtils.Utils                  import LineBuilder
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger2g 
from PhysSelPython.Wrappers                import Selection
# =============================================================================
# Standard Particles 
# =============================================================================
from StandardParticles  import ( StdLoosePions       ,
                                 StdLooseKaons       ,
                                 StdLooseMuons       , 
                                 StdLoosePi02gg      ) ## for gamma-eff
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
    ## Global filter
    # 
    'FILTER'    : None ,   ## VOID filter 
    'ODIN'      : None ,   ## ODIN filter 
    'L0DU'      : None ,   ## L0   filter 
    'HLT'       : None ,   ## HLT  filter
    #
    ## c*tau cut for B-hadrons 
    #
    'CTAU'      : 100 * micrometer , 
    'CTAU_BC'   :  50 * micrometer , 
    'CTAU_Kst'  : 150 * micrometer , 
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
    ( CLONEDIST   > 5000 ) & 
    ( TRGHOSTPROB < 0.5  ) &
    ( MIPCHI2DV() > 9    ) 
    """ , 
    'KaonCut'   : """
    ( CLONEDIST   > 5000 ) & 
    ( TRGHOSTPROB < 0.5  ) &
    ( MIPCHI2DV() > 9    ) 
    """ , 
    #
    ## tight (K,pi) for multibody decays
    #
    'TightPi' : ' ( P > 3.2 * GeV ) & HASRICH & ( PIDpi - PIDK  > 0 ) ' , 
    'TightK'  : ' ( P > 3.2 * GeV ) & HASRICH & ( PIDK  - PIDpi > 0 ) ' ,
    #
    ## useful shortcuts:
    #
    'Preambulo' : [
    ## shortcut for chi2 of vertex fit 
    'chi2vx = VFASPF(VCHI2) '                                  , 
    ## shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light"        , 
    ## use the embedded cut for chi2(LifetimeFit)<16
    "ctau      = BPVLTIME ( 25 ) * c_light "                   ,
    "ctau_16   = BPVLTIME ( 16 ) * c_light "                   ,
    "ctau_9    = BPVLTIME (  9 ) * c_light "                   ,
    "APT23     = LoKi.AParticles.TransverseMomentum ( 2 , 3 )" ,
    ## Combination mass-cut for neutral beauty particles 
    "mb0_acut  = in_range ( 5.100 * GeV , AM , 5.550 * GeV ) " ,
    "mbc_acut  = in_range ( 5.100 * GeV , AM , 6.550 * GeV ) " ,
    ## mass-cut for beauty particles 
    "mb0_cut   = in_range ( 5.150 * GeV ,  M , 5.500 * GeV ) " ,
    "mbc_cut   = in_range ( 5.150 * GeV ,  M , 6.500 * GeV ) " ,
    ] ,
    # =========================================================================
    ## Prescales 
    # =========================================================================
    'B2PsiPiPrescale'   : 1.0 ,
    'B2PsiKPrescale'    : 1.0 ,
    #
    'B2Psi2PiPrescale'  : 1.0 ,
    'B2PsiKPiPrescale'  : 1.0 ,
    'B2Psi2KPrescale'   : 1.0 ,
    #
    'B2Psi3KPrescale'   : 1.0 ,
    'B2Psi3PiPrescale'  : 1.0 ,
    'B2Psi3KPiPrescale' : 1.0 ,
    #
    'B2Psi4PiPrescale'  : 1.0 ,
    'B2Psi4KPiPrescale' : 1.0 ,
    'B2Psi4KPrescale'   : 1.0 ,
    #
    'B2Psi5PiPrescale'  : 1.0 ,
    'B2Psi5KPiPrescale' : 1.0 ,
    #
    'B2Psi6PiPrescale'  : 1.0 ,
    'B2Psi6KPiPrescale' : 1.0 ,
    # =========================================================================
    'B2PsiKstPrescale'  : 1.0 
    # =========================================================================
    }
## ============================================================================

## ============================================================================
## @class  PsiX_BQ_Conf
#  psi(') X configuration file 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2012-02-19
class PsiX_BQ_Conf(LineBuilder) :
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
        if 'PsiX' != name :
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
        
        for line in self._lines_psiX () :
            self.registerLine(line)
            logger.info ( "Register line: %s" %  line.name () ) 
            
            
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
        if not self.__selections_.has_key ( self.name() ) :
            self.__selections_[ self.name() ] = {} 
        
        if self.__selections_[ self.name()].has_key( nick ) :
            raise AttributeError , "Selection '%s'already exists " % nick
        
        self.__selections_[ self.name() ][ nick ] = sel
        
        return sel
    
   ## get all single charm lines 
    def _lines_psiX   ( self ) :
        """
        Get all psiX lines 
        """
        sel = self._selection ( 'PsiX_Lines' )
        if sel : return sel
        #
        sel = [
            # =================================================================
            #
            ## 1h
            #            
            StrippingLine (
            "B2PsiPiFor"    + self.name()              ,
            prescale        = self ['B2PsiPiPrescale'] , 
            checkPV         = self ['CheckPV']         ,
            FILTER          = self ['FILTER' ]         ,
            ODIN            = self ['ODIN'   ]         ,
            L0DU            = self ['L0DU'   ]         ,
            HLT             = self ['L0DU'   ]         ,
            algos           = [ self.psi_pi ()      ]  ) ,
            ## 
            StrippingLine (
            "B2PsiKFor"     + self.name()              ,
            prescale        = self ['B2PsiKPrescale' ] , 
            checkPV         = self ['CheckPV']         ,
            FILTER          = self ['FILTER' ]         ,
            ODIN            = self ['ODIN'   ]         ,
            L0DU            = self ['L0DU'   ]         ,
            HLT             = self ['L0DU'   ]         ,
            algos           = [ self.psi_K ()       ]  ) ,
            #
            ## 2h
            #
            StrippingLine (
            "B2Psi2PiFor"   + self.name()                ,
            prescale        = self ['B2Psi2PiPrescale' ] , 
            checkPV         = self ['CheckPV']           ,
            FILTER          = self ['FILTER' ]           ,
            ODIN            = self ['ODIN'   ]           ,
            L0DU            = self ['L0DU'   ]           ,
            HLT             = self ['L0DU'   ]           ,
            algos           = [ self.psi_2pi ()       ]  ) ,            
            ##
            StrippingLine (
            "B2PsiKPiFor"   + self.name()                ,
            prescale        = self ['B2PsiKPiPrescale' ] , 
            checkPV         = self ['CheckPV']           ,
            FILTER          = self ['FILTER' ]           ,
            ODIN            = self ['ODIN'   ]           ,
            L0DU            = self ['L0DU'   ]           ,
            HLT             = self ['L0DU'   ]           ,
            algos           = [ self.psi_Kpi ()       ]  ) ,
            #
            StrippingLine (
            "B2Psi2KFor"    + self.name()               ,
            prescale        = self ['B2Psi2KPrescale' ] , 
            checkPV         = self ['CheckPV']          ,
            FILTER          = self ['FILTER' ]          ,
            ODIN            = self ['ODIN'   ]          ,
            L0DU            = self ['L0DU'   ]          ,
            HLT             = self ['L0DU'   ]          ,
            algos           = [ self.psi_2K ()       ]  ) ,
            #
            ## 3h
            #
            StrippingLine (
            "B2Psi3KFor"    + self.name()               ,
            prescale        = self ['B2Psi3KPrescale' ] , 
            checkPV         = self ['CheckPV']          ,
            FILTER          = self ['FILTER' ]          ,
            ODIN            = self ['ODIN'   ]          ,
            L0DU            = self ['L0DU'   ]          ,
            HLT             = self ['L0DU'   ]          ,
            algos           = [ self.psi_3K ()       ]  ) ,  
            ##
            StrippingLine (
            "B2Psi3PiFor"   + self.name()                ,
            prescale        = self ['B2Psi3PiPrescale' ] , 
            checkPV         = self ['CheckPV']           ,
            FILTER          = self ['FILTER' ]           ,
            ODIN            = self ['ODIN'   ]           ,
            L0DU            = self ['L0DU'   ]           ,
            HLT             = self ['L0DU'   ]           ,
            algos           = [ self.psi_3pi ()       ]  ) ,
            ##
            StrippingLine (
            "B2Psi3KPiFor"  + self.name()                 ,
            prescale        = self ['B2Psi3KPiPrescale' ] , 
            checkPV         = self ['CheckPV']            ,
            FILTER          = self ['FILTER' ]            ,
            ODIN            = self ['ODIN'   ]            ,
            L0DU            = self ['L0DU'   ]            ,
            HLT             = self ['L0DU'   ]            ,
            algos           = [ self.psi_3Kpi ()       ]  ) ,
            #
            ## 4h
            #
            StrippingLine (
            "B2Psi4PiFor"   + self.name()                ,
            prescale        = self ['B2Psi4PiPrescale' ] , 
            checkPV         = self ['CheckPV']           ,
            FILTER          = self ['FILTER' ]           ,
            ODIN            = self ['ODIN'   ]           ,
            L0DU            = self ['L0DU'   ]           ,
            HLT             = self ['L0DU'   ]           ,
            algos           = [ self.psi_4pi ()       ]  ) ,
            ##
            StrippingLine (
            "B2Psi4KPiFor"  + self.name()                 ,
            prescale        = self ['B2Psi4KPiPrescale' ] , 
            checkPV         = self ['CheckPV']            ,
            FILTER          = self ['FILTER' ]            ,
            ODIN            = self ['ODIN'   ]            ,
            L0DU            = self ['L0DU'   ]            ,
            HLT             = self ['L0DU'   ]            ,
            algos           = [ self.psi_4Kpi ()       ]  ) ,
            ##
            StrippingLine (
            "B2Psi4KFor"    + self.name()               ,
            prescale        = self ['B2Psi4KPrescale' ] , 
            checkPV         = self ['CheckPV']          ,
            FILTER          = self ['FILTER' ]          ,
            ODIN            = self ['ODIN'   ]          ,
            L0DU            = self ['L0DU'   ]          ,
            HLT             = self ['L0DU'   ]          ,
            algos           = [ self.psi_4K ()       ]  ) ,
            #
            ## 5h
            #
            StrippingLine (
            "B2Psi5PiFor"   + self.name()                ,
            prescale        = self ['B2Psi5PiPrescale' ] , 
            checkPV         = self ['CheckPV']           ,
            FILTER          = self ['FILTER' ]           ,
            ODIN            = self ['ODIN'   ]           ,
            L0DU            = self ['L0DU'   ]           ,
            HLT             = self ['L0DU'   ]           ,
            algos           = [ self.psi_5pi ()       ]  ) ,
            ##
            StrippingLine (
            "B2Psi5KPiFor"  + self.name()                 ,
            prescale        = self ['B2Psi5KPiPrescale' ] , 
            checkPV         = self ['CheckPV']            ,
            FILTER          = self ['FILTER' ]            ,
            ODIN            = self ['ODIN'   ]            ,
            L0DU            = self ['L0DU'   ]            ,
            HLT             = self ['L0DU'   ]            ,
            algos           = [ self.psi_5Kpi ()       ]  ) ,
            #
            ## 6h
            #
            StrippingLine (
            "B2Psi6PiFor"   + self.name()                ,
            prescale        = self ['B2Psi6PiPrescale' ] , 
            checkPV         = self ['CheckPV']           ,
            FILTER          = self ['FILTER' ]           ,
            ODIN            = self ['ODIN'   ]           ,
            L0DU            = self ['L0DU'   ]           ,
            HLT             = self ['L0DU'   ]           ,
            algos           = [ self.psi_5pi ()       ]  ) ,
            ##
            StrippingLine (
            "B2Psi6KPiFor"  + self.name()                 ,
            prescale        = self [ 'B2Psi6KPiPrescale' ], 
            checkPV         = self [ 'CheckPV' ]          ,
            FILTER          = self [ 'FILTER'  ]          ,
            ODIN            = self [ 'ODIN'    ]          ,
            L0DU            = self [ 'L0DU'    ]          ,
            HLT             = self [ 'L0DU'    ]          ,
            algos           = [ self.psi_5Kpi ()       ]  ) ,
            ##
            # =================================================================
            # Helper line to study gamma/pi0 reconstruction efficiency
            # =================================================================
            StrippingLine (
            "B2PsiKstFor"   + self.name()                 ,
            prescale        = self [ 'B2PsiKstPrescale' ] , 
            checkPV         = self [ 'CheckPV' ]          ,
            FILTER          = self [ 'FILTER'  ]          ,
            ODIN            = self [ 'ODIN'    ]          ,
            L0DU            = self [ 'L0DU'    ]          ,
            HLT             = self [ 'L0DU'    ]          ,
            algos           = [ self.psi_Kst ()       ]   ) ,
            ##
            # =================================================================
            ]
            ## 
        return self._add_selection ( 'PsiX_Lines' , sel ) 
    
    ## get the selections
    def _selections_private ( self ) :
        
        sel = self._selections ( 'Selections' )
        if sel : return sel
        
        sel =  [
            ## basic 
            self.muons        () ,
            self.pions        () ,
            self.kaons        () ,
            self.pionsTight   () ,
            self.kaonsTight   () ,
            ## composite
            self.psi          () ,
            self.psi_prompt   () ,
            ## beauty 
            self.psi_pi       () ,
            self.psi_K        () ,
            self.psi_2pi      () ,
            self.psi_Kpi      () ,
            self.psi_2K       () ,
            ## 
            self.psi_3pi      () ,
            self.psi_3K       () ,
            self.psi_3Kpi     () ,
            ##
            self.psi_4pi      () ,
            self.psi_4Kpi     () ,
            self.psi_4K       () ,
            ##
            self.psi_5pi      () ,
            self.psi_5Kpi     () ,
            ##
            self.psi_6pi      () ,
            self.psi_6Kpi     () ,
            ##
            self.psi_Kst      () ,
            ]
        
        return self._add_selection ( 'Selections' , sel )
    
    
    ## muons 
    def muons     ( self ) : return StdLooseMuons 
    
    ## pi0s :
    def pi0s      ( self ) : return StdLoosePi02gg 
    
    ## pions :
    def pions     ( self ) :
        """
        Pions for   B -> psi X lines 
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
        
        return self._add_selection( 'Pion_Selection' , sel ) 
    
    ## kaons :
    def kaons     ( self ) :
        """
        Kaons for   B -> psi X lines 
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
            "SelKFor"      + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ StdLooseKaons ]  
            )
        
        return self._add_selection( 'Kaon_Selection' , sel ) 

    ## tight pions :
    def pionsTight     ( self ) :
        """
        Tight Pions for   B -> psi X lines 
        """        
        sel = self._selection ( 'TightPion_Selection')
        if sel : return sel
        
        alg  = FilterDesktop (
            ##
            Preambulo = self['Preambulo'] ,
            ##
            Code = self['TightPi'] ,
            ##
            )
        
        sel  = Selection (
            "SelTightPiFor"      + self.name()   ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.pions()  ]  
            )
        
        return self._add_selection( 'TightPion_Selection' , sel ) 

    ## tight kaons :
    def kaonsTight     ( self ) :
        """
        Tight kaons for   B -> psi X lines 
        """
        sel = self._selection ( 'TightKaon_Selection')
        if sel : return sel
        
        alg  = FilterDesktop (
            ##
            Preambulo = self['Preambulo'] ,
            ##
            Code = self['TightK'] ,
            ##
            )
        
        sel  = Selection (
            "SelTightKFor"     + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.kaons()  ]  
            )
        
        return self._add_selection( 'TightKaon_Selection' , sel ) 
    
    ## psi(') -> mu+ mu-
    def psi ( self ) :
        """
        psi(') -> mu+ mu- 
        """
        sel = self._selection ( 'Psi_Selection')
        if sel : return sel
        
        alg     = CombineParticles (
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
            Algorithm          =   alg          ,
            RequiredSelections = [ self.muons() ] 
            )
        
        return self._add_selection( 'Psi_Selection' , sel ) 
    
    # B -> psi(') pi 
    def psi_pi ( self ) :
        """
        
        """
        sel = self._selection ( 'PsiPi_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptors = [
            "[B+ -> J/psi(1S) pi+        ]cc" ,
            ],
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mbc_acut 
            """ ,
            ## 
            MotherCut = """
            mbc_cut            &
            ( chi2vx    < 16 ) &
            ( ctau      > %s ) 
            """ % min ( self['CTAU'] , self['CTAU_BC'] ) 
            ## 
            )
        
        sel  = Selection (
            "SelPsiPiFor"      + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi   () , 
                                   self.pions () ] 
            )
        
        return self._add_selection( 'PsiPi_Selection' , sel ) 
    
    # B -> psi(') K
    def psi_K ( self ) :
        """
        
        """
        sel = self._selection ( 'PsiK_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptors = [
            "[B+ -> J/psi(1S) K+      ]cc" ,
            ] ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mbc_acut 
            """ ,
            ## 
            MotherCut = """
            mbc_cut            &
            ( chi2vx    < 16 ) &
            ( ctau      > %s ) 
            """ % min ( self['CTAU'] , self['CTAU_BC'] ) 
            ## 
            )
        
        sel  = Selection (
            "SelPsiKFor"      + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi   () , 
                                   self.kaons () ] 
            )
        
        return self._add_selection( 'PsiK_Selection' , sel ) 

    # B -> psi(') ( K* -> Kpi0)
    def psi_Kst ( self ) :
        """
        This is just a control line to study
        the reconstruction efficiency for pi0 and gamma
        see LHCb-INT-2012-001 
        
        """
        sel = self._selection ( 'PsiK*+_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptors = [
            "[B+ -> J/psi(1S) K+ pi0 ]cc" ,
            ] ,
            ##
            DaughtersCuts = {
            'J/psi(1S)' : " M < 3.3 * GeV "       ,    ## keep only J/psi
            ## 'pi0'       : """
            ## ( abs ( LV01 ) < 0.9 ) &
            ## ( MINTREE ( 'gamma' == ID , PT ) > 250 * MeV ) 
            ## """ , 
            } , 
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            in_range ( 4.95 * GeV , AM   , 5.75 * GeV ) &
            in_range ( 750  * MeV , AM23 , 1050 * MeV ) &
            ( APT23 > 1.5 * GeV ) 
            """ ,
            ## 
            MotherCut = """
            in_range ( 5.0 * GeV , M , 5.7 * GeV ) & 
            ( chi2vx    < 16 ) &
            ( ctau_9    > %s ) 
            """ % self['CTAU_Kst'] , 
            ## 
            ParticleCombiners = {'' : "LoKi::VertexFitter" }
            )
        #
        sel_ = Selection (
            "SelPrePsiK*+For"  + self.name ()    ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi   () , 
                                   self.kaons () ,
                                   self.pi0s  () ] 
            )
        
        tag = Pi0Veto__Tagger2g (
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25020     ## unique ! 
            )
        
        sel  = Selection (
            "SelPsiK*+For"     + self.name ()   ,
            Algorithm          =   tag           ,
            RequiredSelections = [ sel_          ]  
            )
        
        return self._add_selection( 'PsiK*+_Selection' , sel ) 
    
    
    # B -> psi(') pipi
    def psi_2pi ( self ) :
        """
        
        """
        sel = self._selection ( 'Psi2Pi_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptor = "B0 -> J/psi(1S) pi+ pi-" ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mb0_acut 
            """ ,
            ## 
            MotherCut = """
            mb0_cut            &
            ( chi2vx    < 25 ) &
            ( ctau      > %s ) 
            """ % self['CTAU'] 
            ## 
            )
        
        sel  = Selection (
            "SelPsi2PiFor"       + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi   () , 
                                   self.pions () ] 
            )
        
        return self._add_selection( 'Psi2Pi_Selection' , sel ) 

    
    # B -> psi(') Kpi
    def psi_Kpi ( self ) :
        """
        
        """
        sel = self._selection ( 'PsiKPi_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptor = "[B0 -> J/psi(1S) K+ pi-]cc" ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mb0_acut 
            """ ,
            ## 
            MotherCut = """
            mb0_cut & 
            ( chi2vx    < 25 ) &
            ( ctau      > %s ) 
            """ % self['CTAU'] 
            ## 
            )
        
        sel  = Selection (
            "SelPsiKPiFor"     + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi   () , 
                                   self.kaons () ,
                                   self.pions () ]
            )
        
        return self._add_selection( 'PsiKPi_Selection' , sel ) 


    # B -> psi(') KK
    def psi_2K ( self ) :
        """
        
        """
        sel = self._selection ( 'Psi2K_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptor = "B_s0 -> J/psi(1S) K+ K-" ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mb0_acut 
            """ ,
            ## 
            MotherCut = """
            mb0_cut & 
            ( chi2vx    < 25 ) &
            ( ctau      > %s ) 
            """ % self['CTAU'] 
            ## 
            )
        
        sel  = Selection (
            "SelPsi2KFor"      + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi   () , 
                                   self.kaons () ] 
            )
        
        return self._add_selection( 'Psi2K_Selection' , sel ) 

    
    # B -> psi(') 3pi 
    def psi_3pi ( self ) :
        """
        
        """
        sel = self._selection ( 'Psi3Pi_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptors = [
            "[B+ -> J/psi(1S) pi+ pi+ pi-]cc" ,
            ],
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mbc_acut 
            """ ,
            ## 
            MotherCut = """
            mbc_cut            &
            ( chi2vx    < 36 ) &
            ( ctau      > %s ) 
            """ % min ( self['CTAU'] , self['CTAU_BC'] ) 
            ## 
            )
        
        sel  = Selection (
            "SelPsi3PiFor"     + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi        () , 
                                   self.pionsTight () ] 
            )
        
        return self._add_selection( 'Psi3Pi_Selection' , sel ) 
    
    # B -> psi(') 3K
    def psi_3K ( self ) :
        """
        
        """
        sel = self._selection ( 'Psi3K_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptors = [
            "[B+ -> J/psi(1S) K+ K+ K- ]cc" ,
            ] ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mbc_acut 
            """ ,
            ## 
            MotherCut = """
            mbc_cut            &
            ( chi2vx    < 36 ) &
            ( ctau      > %s ) 
            """ % min ( self['CTAU'] , self['CTAU_BC'] ) 
            ## 
            )
        
        sel  = Selection (
            "SelPsi3KFor"      + self.name()          ,
            Algorithm          =   alg                ,
            RequiredSelections = [ self.psi        () , 
                                   self.kaonsTight () ] 
            )
        
        return self._add_selection( 'Psi3K_Selection' , sel ) 
    
    # B -> psi(') 3(Kpi) 
    def psi_3Kpi ( self ) :
        """
        
        """
        sel = self._selection ( 'Psi3KPi_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptors = [
            "[B+ -> J/psi(1S) K+ pi+ pi-]cc" ,
            "[B+ -> J/psi(1S) K+ K-  pi+]cc" ,
            ],
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mbc_acut 
            """ ,
            ## 
            MotherCut = """
            mbc_cut            &
            ( chi2vx    < 36 ) &
            ( ctau      > %s ) 
            """ % min ( self['CTAU'] , self['CTAU_BC'] ) 
            ## 
            )
        
        sel  = Selection (
            "SelPsi3KPiFor"    + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi        () , 
                                   self.kaonsTight () ,
                                   self.pionsTight () ] 
            )
        
        return self._add_selection( 'Psi3KPi_Selection' , sel )
    
    # B -> psi(') 4pi
    def psi_4pi ( self ) :
        """
        
        """
        sel = self._selection ( 'Psi4Pi_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptor = "B_s0 -> J/psi(1S) pi+ pi+ pi- pi-" ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mb0_acut 
            """ ,
            ## 
            MotherCut = """
            mb0_cut            &
            ( chi2vx    < 49 ) &
            ( ctau      > %s ) 
            """ % self['CTAU'] 
            ## 
            )
        
        sel  = Selection (
            "SelPsi4PiFor"     + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi        () , 
                                   self.pionsTight () ] 
            )
        
        return self._add_selection( 'Psi4Pi_Selection' , sel ) 

    # B -> psi(') 4K
    def psi_4K ( self ) :
        """
        
        """
        sel = self._selection ( 'Psi4K_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptor = "B_s0 -> J/psi(1S) K+ K+ K- K-" ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mb0_acut 
            """ ,
            ## 
            MotherCut = """
            mb0_cut            &
            ( chi2vx    < 49 ) &
            ( ctau      > %s ) 
            """ % self['CTAU'] 
            ## 
            )
        
        sel  = Selection (
            "SelPsi4KFor"      + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi   () , 
                                   self.kaons () ] 
            )
        
        return self._add_selection( 'Psi4K_Selection' , sel ) 

    # B -> psi(') 4(Kpi)
    def psi_4Kpi ( self ) :
        """
        
        """
        sel = self._selection ( 'Psi4KPi_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptors = [
            "[ B_s0 -> J/psi(1S) K+ pi+ pi- pi- ]cc" ,
            "[ B_s0 -> J/psi(1S) K+ pi+ K-  pi- ]cc" ,
            "[ B_s0 -> J/psi(1S) K+ K+  K-  pi- ]cc"
            ] ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mb0_acut 
            """ ,
            ## 
            MotherCut = """
            mb0_cut            &
            ( chi2vx    < 49 ) &
            ( ctau      > %s ) 
            """ % self['CTAU'] 
            ## 
            )
        
        sel  = Selection (
            "SelPsi4KPiFor"    + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi        () ,
                                   self.kaonsTight () , 
                                   self.pionsTight () ] 
            )
        
        return self._add_selection( 'Psi4KPi_Selection' , sel ) 

    # B -> psi(') 5pi 
    def psi_5pi ( self ) :
        """
        B -> psi(') 5pi 
        
        """
        sel = self._selection ( 'Psi5Pi_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptors = [
            "[B+ -> J/psi(1S) pi+ pi+ pi+ pi- pi-]cc" ,
            ],
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mbc_acut          &  
            ADOCACHI2CUT ( 9 , '' )
            """ ,
            ## 
            MotherCut = """
            mbc_cut            &
            ( chi2vx    < 64 ) &
            ( ctau      > %s ) 
            """ % min ( self['CTAU'] , self['CTAU_BC'] ) 
            ## 
            )
        
        sel  = Selection (
            "SelPsi5PiFor"      + self.name()     ,
            Algorithm          =   alg            ,
            RequiredSelections = [ self.psi        () , 
                                   self.pionsTight () ] 
            )
        
        return self._add_selection( 'Psi5Pi_Selection' , sel ) 
    

    # B -> psi(') 5(K,pi)
    def psi_5Kpi ( self ) :
        """
        B -> psi(') 5(K,pi)
        
        """
        sel = self._selection ( 'Psi5KPi_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptors = [
            "[B+ -> J/psi(1S) K+ pi+ pi+ pi- pi-]cc" ,
            "[B+ -> J/psi(1S) K+ pi+ pi+ K-  pi-]cc" ,            
            "[B+ -> J/psi(1S) K+ K+  pi+ K-  pi-]cc" ,            
            "[B+ -> J/psi(1S) K+ K+  pi+ K-  K- ]cc" ,            
            ],
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mbc_acut         &
            ADOCACHI2CUT ( 9 , '') 
            """ ,
            ## 
            MotherCut = """
            mbc_cut            &
            ( chi2vx    < 64 ) &
            ( ctau      > %s ) 
            """ % min ( self['CTAU'] , self['CTAU_BC'] ) 
            ## 
            )
        
        sel  = Selection (
            "SelPsi5KPiFor"    + self.name()    ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi        () ,
                                   self.kaonsTight () ,
                                   self.pionsTight () ]
            
            )
        
        return self._add_selection( 'Psi5KPi_Selection' , sel ) 
    
    # B -> psi(') 6pi
    def psi_6pi ( self ) :
        """
        B -> psi(') 6pi
        
        """
        sel = self._selection ( 'Psi6Pi_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptor = "B_s0 -> J/psi(1S) pi+ pi+ pi+ pi- pi- pi-" ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mb0_acut         &
            ADOCACHI2CUT ( 9 , '')             
            """ ,
            ## 
            MotherCut = """
            mb0_cut            &
            ( chi2vx    < 64 ) &
            ( ctau      > %s ) 
            """ % self['CTAU'] 
            ## 
            )
        
        sel  = Selection (
            "SelPsi6PiFor"       + self.name()     ,
            Algorithm          =   alg             ,
            RequiredSelections = [ self.psi        () , 
                                   self.pionsTight () ] 
            )
        
        return self._add_selection( 'Psi6Pi_Selection' , sel ) 

    # B -> psi(') 6(K,pi)
    def psi_6Kpi ( self ) :
        """
        B -> psi(') 6(K,pi)
        
        """
        sel = self._selection ( 'Psi6KPi_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptors = [
            "[B_s0 -> J/psi(1S) K+ pi+ pi+ pi- pi- pi-]cc" ,
            "[B_s0 -> J/psi(1S) K+ pi+ pi+ K-  pi- pi-]cc" ,
            "[B_s0 -> J/psi(1S) K+ K+  pi+ K-  pi- pi-]cc" ,
            "[B_s0 -> J/psi(1S) K+ K+  pi+ K-  K-  pi-]cc" 
            ],
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mb0_acut         &
            ADOCACHI2CUT ( 9 , '')             
            """ ,
            ## 
            MotherCut = """
            mb0_cut            &
            ( chi2vx    < 64 ) &
            ( ctau      > %s ) 
            """ % self['CTAU'] 
            ## 
            )
        
        sel  = Selection (
            "SelPsi6KPiFor"    + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi        () , 
                                   self.kaonsTight () ,
                                   self.pionsTight () ] 
            )
        
        return self._add_selection( 'Psi6KPi_Selection' , sel ) 

        
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
