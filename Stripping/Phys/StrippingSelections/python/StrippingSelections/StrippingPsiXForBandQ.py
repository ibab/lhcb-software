#!/usr/bin/env python
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
    'CTAU'      : 125 * micrometer , 
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
    ( CLONEDIST   > 5000   ) & 
    ( TRGHOSTPROB < 0.5    ) &
    ( TRCHI2DOF   < 4      ) & 
    in_range ( 2 , ETA , 5 ) &
    HASRICH                  &
    ( PIDpi - PIDK > -5    ) &
    ( MIPCHI2DV()  >  9    )
    """ , 
    'KaonCut'   : """
    ( CLONEDIST   > 5000   ) & 
    ( TRCHI2DOF   < 4      ) & 
    ( TRGHOSTPROB < 0.5    ) & 
    in_range ( 2 , ETA , 5 ) &
    HASRICH                  &
    ( PIDK - PIDpi > -5    ) &
    ( MIPCHI2DV()  >  9    ) 
    """ , 
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
    "ctau_9    = BPVLTIME (  9 ) * c_light "                   ,
    "ctau_16   = BPVLTIME ( 16 ) * c_light "                   ,
    "APT23     = LoKi.AParticles.TransverseMomentum ( 2 , 3 )" ,
    ## Combination mass-cut for neutral beauty particles 
    "mb0_acut  = in_range ( 5.100 * GeV , AM , 5.550 * GeV ) " ,
    "mbp_acut  = in_range ( 5.100 * GeV , AM , 5.550 * GeV ) " ,
    "mbc_acut  = in_range ( 6.050 * GeV , AM , 6.550 * GeV ) " ,
    ## mass-cut for beauty particles 
    "mb0_cut   = in_range ( 5.150 * GeV ,  M , 5.500 * GeV ) " ,
    "mbp_cut   = in_range ( 5.150 * GeV ,  M , 5.500 * GeV ) " ,
    "mbc_cut   = in_range ( 6.100 * GeV ,  M , 6.500 * GeV ) " ,
    ] ,
    # =========================================================================
    ## Prescales 
    # =========================================================================
    'B2PsiPiPrescale'   : 1.0 ,
    'B2PsiKPrescale'    : 1.0 ,
    #
    'B2Psi2PiPrescale'  : 1.0 ,
    'B2Psi2KPiPrescale' : 1.0 ,
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
    'B2Psi5KPrescale'   : 1.0 ,
    #
    'B2Psi6PiPrescale'  : 1.0 ,
    'B2Psi6KPiPrescale' : 1.0 ,
    #
    'B2PsiD0Prescale'   : 1.0 ,
    'B2PsiDpPrescale'   : 1.0 ,
    'B2PsiDsPrescale'   : 1.0 ,
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
            "B2Psi2KPiFor"  + self.name()                 ,
            prescale        = self ['B2Psi2KPiPrescale' ] , 
            checkPV         = self ['CheckPV']            ,
            FILTER          = self ['FILTER' ]            ,
            ODIN            = self ['ODIN'   ]            ,
            L0DU            = self ['L0DU'   ]            ,
            HLT             = self ['L0DU'   ]            ,
            algos           = [ self.psi_2Kpi ()    ]  )  ,
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
            StrippingLine (
            "B2Psi5KFor  "  + self.name()                 ,
            prescale        = self ['B2Psi5KPrescale'   ] , 
            checkPV         = self ['CheckPV']            ,
            FILTER          = self ['FILTER' ]            ,
            ODIN            = self ['ODIN'   ]            ,
            L0DU            = self ['L0DU'   ]            ,
            HLT             = self ['L0DU'   ]            ,
            algos           = [ self.psi_5K  ()        ]  ) ,
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
            algos           = [ self.psi_6pi ()       ]  ) ,
            ##
            StrippingLine (
            "B2Psi6KPiFor"  + self.name()                 ,
            prescale        = self [ 'B2Psi6KPiPrescale' ], 
            checkPV         = self [ 'CheckPV' ]          ,
            FILTER          = self [ 'FILTER'  ]          ,
            ODIN            = self [ 'ODIN'    ]          ,
            L0DU            = self [ 'L0DU'    ]          ,
            HLT             = self [ 'L0DU'    ]          ,
            algos           = [ self.psi_6Kpi ()       ]  ) ,
            #
            ## psi + charm 
            #
            StrippingLine (
            "B2PsiD0For"    + self.name()                 ,
            prescale        = self [ 'B2PsiD0Prescale' ]  , 
            checkPV         = self [ 'CheckPV' ]          ,
            FILTER          = self [ 'FILTER'  ]          ,
            ODIN            = self [ 'ODIN'    ]          ,
            L0DU            = self [ 'L0DU'    ]          ,
            HLT             = self [ 'L0DU'    ]          ,
            algos           = [ self.psi_D0 ()       ]  ) ,
            #
            StrippingLine (
            "B2PsiDpFor"    + self.name()                 ,
            prescale        = self [ 'B2PsiDpPrescale' ]  , 
            checkPV         = self [ 'CheckPV' ]          ,
            FILTER          = self [ 'FILTER'  ]          ,
            ODIN            = self [ 'ODIN'    ]          ,
            L0DU            = self [ 'L0DU'    ]          ,
            HLT             = self [ 'L0DU'    ]          ,
            algos           = [ self.psi_Dp ()       ]  ) ,
            #
            #
            StrippingLine (
            "B2PsiDsFor"    + self.name()                 ,
            prescale        = self [ 'B2PsiDsPrescale' ]  , 
            checkPV         = self [ 'CheckPV' ]          ,
            FILTER          = self [ 'FILTER'  ]          ,
            ODIN            = self [ 'ODIN'    ]          ,
            L0DU            = self [ 'L0DU'    ]          ,
            HLT             = self [ 'L0DU'    ]          ,
            algos           = [ self.psi_Ds ()       ]  ) ,
            #
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
            ## composite
            self.psi          () ,
            self.psi_prompt   () ,
            ## beauty 
            self.psi_pi       () ,
            self.psi_K        () ,
            #
            self.psi_2pi      () ,
            self.psi_2Kpi     () ,
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
            self.psi_5K       () ,
            ##
            self.psi_6pi      () ,
            self.psi_6Kpi     () ,
            ##
            self.D0           () ,
            self.Dp           () ,
            self.Ds           () ,
            ## 
            self.psi_D0       () ,
            self.psi_Dp       () ,
            self.psi_Ds       () ,
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
            """ 
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
        B -> psi(') pi 
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
            mbp_acut | mbc_acut
            """ ,
            ## 
            MotherCut = """
            ( chi2vx    < 16    ) &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
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
        B -> psi(') K
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
            mbp_acut | mbc_acut 
            """ ,
            ## 
            MotherCut = """
            ( chi2vx    < 16 ) &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
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
            """ % self['CTAU_Kst'] 
            ## 
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
        B -> psi(') pipi
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
    def psi_2Kpi ( self ) :
        """
        B -> psi(') Kpi
        """
        sel = self._selection ( 'Psi2KPi_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptor = "[B0 -> J/psi(1S) K+ pi-]cc" ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mb0_acut & 
            ADOCACHI2CUT ( 9 , '' )
            """ ,
            ## 
            MotherCut = """
            mb0_cut & 
            ( chi2vx    < 25 ) &
            ( ctau      > %s ) 
            """ % self['CTAU'] ,
            ## 
            )
        
        sel  = Selection (
            "SelPsi2KPiFor"     + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi   () , 
                                   self.kaons () ,
                                   self.pions () ]
            )
        
        return self._add_selection( 'Psi2KPi_Selection' , sel ) 
    
    
    # B -> psi(') KK

    def psi_2K ( self ) :
        """
        B -> psi(') KK
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
            mb0_acut & 
            ADOCACHI2CUT ( 9 , '' )
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
        B -> psi(') 3pi         
        """

    def psi_2K ( self ) :
        """
        B -> psi(') KK
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
            mb0_acut & 
            ADOCACHI2CUT ( 9 , '' )
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
        B -> psi(') 3pi         
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
            ( mbp_acut | mbc_acut ) & 
            ADOCACHI2CUT ( 9 , '' )
            """ ,
            ## 
            MotherCut = """
            ( chi2vx < 36 ) &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
            ## 
            )
        
        sel  = Selection (
            "SelPsi3PiFor"     + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi   () , 
                                   self.pions () ] 
            )
        
        return self._add_selection( 'Psi3Pi_Selection' , sel ) 
    
    # B -> psi(') 3K
    def psi_3K ( self ) :
        """
        B -> psi(') 3K        
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
            ( mbp_acut | mbc_acut ) & 
            ADOCACHI2CUT ( 9 , '' )
            """ ,
            ## 
            MotherCut = """
            ( chi2vx < 36 ) &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
            ## 
            )
        
        sel  = Selection (
            "SelPsi3KFor"      + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi   () , 
                                   self.kaons () ] 
            )
        
        return self._add_selection( 'Psi3K_Selection' , sel ) 
    
    # B -> psi(') 3(Kpi) 
    def psi_3Kpi ( self ) :
        """
        B -> psi(') 3(Kpi) 
        """
        sel = self._selection ( 'Psi3KPi_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptors = [
            "[B+ -> J/psi(1S) K+ pi+ pi-]cc" ,
            "[B+ -> J/psi(1S) K+ K-  pi+]cc" ,
            "[B+ -> J/psi(1S) K- pi+ pi+]cc" , ## ATTENTION: wrong charge combination!!!
            ],
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            ( mbp_acut | mbc_acut ) & 
            ADOCACHI2CUT ( 9 , '' )
            """ ,
            ## 
            MotherCut = """
            ( chi2vx < 36 ) &
            ( ( mbp_cut & ( ctau > %s  ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
            ## 
            )
        
        sel  = Selection (
            "SelPsi3KPiFor"    + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi   () , 
                                   self.kaons () ,
                                   self.pions () ] 
            )
        
        return self._add_selection( 'Psi3KPi_Selection' , sel )
    
    # B -> psi(') 4pi
    def psi_4pi ( self ) :
        """
        B -> psi(') 4pi
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
            mb0_acut &
            ADOCACHI2CUT ( 9 , '' )
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
            RequiredSelections = [ self.psi   () , 
                                   self.pions () ] 
            )
        
        return self._add_selection( 'Psi4Pi_Selection' , sel ) 
    
    # B -> psi(') 4K
    def psi_4K ( self ) :
        """
        B -> psi(') 4K
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
            mb0_acut & 
            ADOCACHI2CUT ( 9 , '' )
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
        B -> psi(') 4(Kpi)        
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
            mb0_acut & 
            ADOCACHI2CUT ( 9 , '' )
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
            RequiredSelections = [ self.psi   () ,
                                   self.kaons () , 
                                   self.pions () ] 
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
            ( mbp_acut | mbc_acut ) &
            ADOCACHI2CUT ( 9 , '' )
            """ ,
            ## 
            MotherCut = """
            ( chi2vx < 64 ) &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) ,
            ## 
            )
        
        sel  = Selection (
            "SelPsi5PiFor"      + self.name()     ,
            Algorithm          =   alg            ,
            RequiredSelections = [ self.psi    () , 
                                   self.pions  () ] 
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
            ( mbp_acut | mbc_acut ) &
            ADOCACHI2CUT ( 9 , '' ) 
            """ ,
            ## 
            MotherCut = """
            ( chi2vx    < 64 ) &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
            ## 
            )
        
        sel  = Selection (
            "SelPsi5KPiFor"    + self.name()      ,
            Algorithm          =   alg            ,
            RequiredSelections = [ self.psi    () ,
                                   self.kaons  () ,
                                   self.pions  () ]
            
            )
        
        return self._add_selection( 'Psi5KPi_Selection' , sel ) 

    # B -> psi(') 5K
    def psi_5K ( self ) :
        """
        B -> psi(') 5K
        """
        sel = self._selection ( 'Psi5K_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            ##
            DecayDescriptor = "[B+ -> J/psi(1S) K+  K+  K+ K- K- ]cc" ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            ( mbp_acut | mbc_acut ) & 
            ADOCACHI2CUT ( 9 , '' ) 
            """ ,
            ## 
            MotherCut = """
            ( chi2vx    < 64 ) &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
            ## 
            )
        
        sel  = Selection (
            "SelPsi5KFor"     + self.name()      ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi   () ,
                                   self.kaons () ] 
            
            )
        
        return self._add_selection( 'Psi5K_Selection' , sel ) 

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
            ( chi2vx    < 81 ) &
            ( ctau      > %s ) 
            """ % self['CTAU'] 
            ## 
            )
        
        sel  = Selection (
            "SelPsi6PiFor"       + self.name()   ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi   () , 
                                   self.pions () ] 
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
            "[B_s0 -> J/psi(1S) K+ pi+ pi+ pi- pi- pi-]cc" ,  ## 1K 
            "[B_s0 -> J/psi(1S) K+ pi+ pi+ K-  pi- pi-]cc" ,  ## 2K
            "[B_s0 -> J/psi(1S) K+ K+  pi+ K-  pi- pi-]cc" ,  ## 3K
            "[B_s0 -> J/psi(1S) K+ K+  pi+ K-  K-  pi-]cc"    ## 4K
            ],
            ##
            DaughtersCuts = {
            "K+"  : "PIDK  - PIDpi >  2" , ## make tigth kaons here to suppress combinators 
            "pi+" : "PIDpi - PIDK  > -2" , ## make tigth pions here to suppress combinators 
            } , 
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
            ( chi2vx    < 81 ) &
            ( ctau      > %s ) 
            """ % self['CTAU'] 
            ## 
            )
        
        sel  = Selection (
            "SelPsi6KPiFor"    +   self.name  () ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.psi   () , 
                                   self.kaons () ,
                                   self.pions () ] 
            )
        
        return self._add_selection( 'Psi6KPi_Selection' , sel )
    
    ## D0 -> Kpi candidates
    def D0 ( self ) :
        """
        D0 -> K pi candidates 
        """
        sel = self._selection ( 'D0_Selection')
        if sel : return sel

        alg = CombineParticles (
            ## the decays to be reconstructed
            DecayDescriptor = "[D0  -> K-  pi+]cc ", 
            ##
            ## combination cut : wide mass-cut & PT-cut
            CombinationCut = """
            ( ADAMASS('D0') <  80 * MeV ) &
            ( APT           > 900 * MeV )
            """ , 
            ## mother cut
            MotherCut      = """
            ( chi2vx       <  9       ) &
            ( PT           >  1 * GeV ) &
            ( ADMASS('D0') < 75 * MeV ) 
            """ ,
            ## 
            ParticleCombiners = { '' : 'LoKi::VertexFitter' } 
            )
        
        sel = Selection (
            'SelD0For'         +   self.name  () ,
            Algorithm          = alg             ,
            RequiredSelections = [ self.kaons () ,
                                   self.pions () ]
            )
        
        return self._add_selection( 'D0_Selection' , sel )
    
    
    ## D+ -> K pi pi
    def Dp ( self ) :
        """
        D+ -> K- pi pi 
        """
        sel = self._selection ( 'Dp_Selection')
        if sel : return sel
        
        alg = CombineParticles (
            ## the decays to be reconstructed
            DecayDescriptor = "[D+  -> K-  pi+ pi+]cc ", 
            ##
            ## combination cut : wide mass-cut & PT-cut
            CombinationCut = """
            ( ADAMASS('D+') <  60 * MeV ) &
            ( APT           > 900 * MeV )
            """ , 
            ## mother cut
            MotherCut      = """
            ( chi2vx       < 25       ) &
            ( PT           >  1 * GeV ) &
            ( ADMASS('D+') < 50 * MeV ) 
            """ ,
            ## 
            ParticleCombiners = { '' : 'LoKi::VertexFitter' } 
            )
        
        sel = Selection (
            'SelDpFor'         +   self.name  () ,
            Algorithm          = alg             ,
            RequiredSelections = [ self.kaons () ,
                                   self.pions () ]
            )
        
        return self._add_selection( 'Dp_Selection' , sel )
    
    ## Ds -> phi pi
    def Ds ( self ) :
        """
        Ds -> phi pi 
        """
        sel = self._selection ( 'Ds_Selection')
        if sel : return sel

        alg = CombineParticles (
            ## the decays to be reconstructed
            DecayDescriptor = "[D_s+ -> K-  K+ pi+]cc ", 
            ##
            ## combination cut : wide mass-cut & PT-cut
            CombinationCut = """
            (   AM12           < 1050 * MeV ) & 
            ( ( ADAMASS('D+')  <   60 * MeV ) | ( ADAMASS('D_s+') <   60 * MeV ) ) & 
            (   APT            >  900 * MeV )
            """ , 
            ## mother cut
            MotherCut      = """
            (   chi2vx        < 25       ) &
            (   PT            >  1 * GeV ) &
            ( ( ADMASS('D+')  < 50 * MeV ) | ( ADMASS('D_s+') < 50 * MeV ) ) 
            """ ,
            ## 
            ParticleCombiners = { '' : 'LoKi::VertexFitter' } 
            )
        
        sel = Selection (
            'SelDsFor'         +   self.name  () ,
            Algorithm          = alg             ,
            RequiredSelections = [ self.kaons () ,
                                   self.pions () ]
            )
        
        return self._add_selection( 'Ds_Selection' , sel )
 
    ## B -> psi D0 
    def psi_D0 ( self ) :
        """
        B -> psi D0 
        """
        sel = self._selection ( 'PsiD0_Selection')
        if sel : return sel
       
        alg = CombineParticles (
            ## the decays to be reconstructed
            DecayDescriptor = "[ B0 -> J/psi(1S) D0 ]cc ", 
            ##
            Preambulo      = self['Preambulo'] , 
            ## combination cut : 
            CombinationCut = """
            in_range ( 5 * GeV , AM ,  7 * GeV ) 
            """ , 
            ## mother cut
            MotherCut      = """
            ( chi2vx  < 16              ) &
            ( ctau    > 50 * micrometer ) 
            """ ,
            ## 
            ParticleCombiners = { '' : 'LoKi::VertexFitter' } 
            )

        ## make the selection
        sel = Selection (
            'B2PsiD0For'       +   self.name () ,
            Algorithm          =   alg          ,
            RequiredSelections = [ self.psi  () ,
                                   self.D0   () ]
            )
        
        return self._add_selection( 'PsiD0_Selection' , sel )

    ## B -> psi D+
    def psi_Dp ( self ) :
        """
        B -> psi D+ 
        """
        sel = self._selection ( 'PsiDp_Selection')
        if sel : return sel

        alg = CombineParticles (
            ## the decays to be reconstructed
            DecayDescriptor = "[ B+ -> J/psi(1S) D+ ]cc ", 
            ##
            Preambulo      = self['Preambulo'] , 
            ## combination cut : 
            CombinationCut = """
            in_range ( 5 * GeV , AM ,  7 * GeV ) 
            """ , 
            ## mother cut
            MotherCut      = """
            ( chi2vx  < 16              ) &
            ( ctau    > 50 * micrometer ) 
            """ , 
            ## 
            ParticleCombiners = { '' : 'LoKi::VertexFitter' } 
            )
        
        ## make the selection
        sel = Selection (
            'B2PsiDpFor'       +   self.name () ,
            Algorithm          =   alg          ,
            RequiredSelections = [ self.psi  () ,
                                   self.Dp   () ]
            )
        
        return self._add_selection( 'PsiDp_Selection' , sel )
    
    ## B -> psi Ds+
    def psi_Ds ( self ) :
        """
        B -> psi Ds+ 
        """
        sel = self._selection ( 'PsiDs_Selection')
        if sel : return sel

        
        alg = CombineParticles (
            ## the decays to be reconstructed
            DecayDescriptor = "[ B_c+ -> J/psi(1S) D_s+ ]cc ", 
            ##
            Preambulo      = self['Preambulo'] , 
            ## combination cut : 
            CombinationCut = """
            in_range ( 5 * GeV , AM ,  7 * GeV ) 
            """ , 
            ## mother cut
            MotherCut      = """
            ( chi2vx  < 16              ) &
            ( ctau    > 50 * micrometer ) 
            """ ,
            ## 
            ParticleCombiners = { '' : 'LoKi::VertexFitter' } 
            )
        
        ## make the selection
        sel = Selection (
            'B2PsiDsFor'       +   self.name () ,
            Algorithm          =   alg          ,
            RequiredSelections = [ self.psi  () ,
                                   self.Ds   () ]
            )
        
        return self._add_selection( 'PsiDs_Selection' , sel )

    


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
