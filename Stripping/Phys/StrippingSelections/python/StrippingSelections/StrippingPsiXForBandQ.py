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
from StrippingUtils.Utils                  import LineBuilder
from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger2g 
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
    'CTAU'      :  75 * micrometer , 
    'CTAU_BC'   :  50 * micrometer , 
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
    ( PROBNNpi     > 0.1      ) &
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
    ( PROBNNk      > 0.1      ) &
    ( MIPCHI2DV()  > 4        )
    """ ,
    #
    'ProtonCut'   : """
    ( PT           > 200 * MeV ) & 
    ( CLONEDIST    > 5000      ) & 
    ( TRCHI2DOF    < 4         ) & 
    ( TRGHOSTPROB  < 0.5       ) & 
    in_range ( 2         , ETA , 5         ) &
    in_range ( 10 * GeV  , P   , 150 * GeV ) &
    HASRICH                     &
    ( PROBNNp      > 0.1      ) &
    ( MIPCHI2DV()  > 4        ) 
    """ ,
    #
    ## useful shortcuts:
    #
    'Preambulo' : [
    ## shortcut for chi2 of vertex fit 
    'chi2vx    = VFASPF(VCHI2)     '                           , 
    'chi2vxndf = VFASPF(VCHI2PDOF) '                           , 
    ## shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light"        , 
    ## use the embedded cut for chi2(LifetimeFit)<16
    "ctau      = BPVLTIME ( 25 ) * c_light "                   ,
    "ctau_9    = BPVLTIME (  9 ) * c_light "                   ,
    "ctau_16   = BPVLTIME ( 16 ) * c_light "                   ,
    "APT23     = LoKi.AParticles.TransverseMomentum ( 2 , 3 )" ,
    ## Combination mass-cut for neutral beauty particles
    "mb0_ahigh = 5.550 * GeV " , 
    "mbu_ahigh = mb0_ahigh   " , 
    "mlb_ahigh = 5.850 * GeV " , 
    "mbc_ahigh = 6.555 * GeV " , 
    "mb0_acut  = in_range ( 5.100 * GeV , AM , mb0_ahigh   ) " ,
    "mbu_acut  = in_range ( 5.100 * GeV , AM , mbu_ahigh   ) " ,
    "mlb_acut  = in_range ( 5.350 * GeV , AM , mlb_ahigh   ) " ,
    "mbc_acut  = in_range ( 6.050 * GeV , AM , mbc_ahigh   ) " ,
    "mbp_acut  = mbu_acut    " ,
    ## mass-cut for beauty particles 
    "mb0_high  = 5.510 * GeV " , 
    "mbu_high  = mb0_high    " , 
    "mlb_high  = 5.810 * GeV " , 
    "mb0_cut   = in_range ( 5.140 * GeV ,  M , mb0_high    ) " ,
    "mbu_cut   = in_range ( 5.140 * GeV ,  M , mbu_high    ) " ,
    "mlb_cut   = in_range ( 5.390 * GeV ,  M , 5.810 * GeV ) " ,
    "mbc_cut   = in_range ( 6.090 * GeV ,  M , 6.510 * GeV ) " ,
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
    'Lb2PsiPKPrescale'      : 1.0 ,
    'Lb2PsiPPiPrescale'     : 1.0 ,
    'Lb2PsiPKPiPiPrescale'  : 1.0 ,
    'Lb2PsiPPiPiPiPrescale' : 1.0 ,
    #
    'B2PsiPPPrescale'       : 1.0 ,
    'B2PsiPPPiPrescale'     : 1.0 ,
    'B2PsiPPKPrescale'      : 1.0 ,
    'B2PsiPPPiPiPrescale'   : 1.0 ,
    'B2PsiPPKPiPiPrescale'  : 1.0 ,
    'B2PsiPPPiPiPiPrescale' : 1.0 ,
    # =========================================================================
    }

## ============================================================================
## @class  PsiX_BQ_Conf
#  psi(') X configuration file 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2012-02-19
class PsiX_BQ_Conf(LineBuilder) :
    """
    Helper class to configure 'PsiX'-lines
    """
    __configuration_keys__ = tuple ( _default_configuration_.keys() )
    
    ## get the default configuration 
    @staticmethod
    def defaultConfiguration( key = None ) :
        """
        Get the default/recommended configurtaion
        
        >>> conf = PsiX.defaultConfiguration()
        
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
                logger.debug ('new configuration: %-16s : %s ' % ( key , _config[key] ) )
                
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
    
    ## the basic Mother cuts for all charged modea
    def _chargedB ( self ) :
        """
        The basic MotherCuts for all charged modea
        """
        return """
        ( chi2vxndf < 12  ) &
        ( ( mbu_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
        """ % ( self['CTAU'] , self['CTAU_BC'] ) 
    
    ## the basic Mother cuts for all charged modea
    def _neutralB ( self ) :
        """
        The basic MotherCuts for all neutral modea
        """
        return """
        mb0_cut            &
        ( chi2vxndf < 10 ) &
        ( ctau      > %s ) 
        """ % self['CTAU']
    
    ## the basic Mother cuts for baryons 
    def _lambdaB ( self ) :
        """
        The basic MotherCuts for baruons 
        """
        return """
        mlb_cut            &
        ( chi2vxndf < 10 ) &
        ( ctau      > %s ) 
        """ % self['CTAU'] 

    ## get all single charm lines 
    def _lines_psiX   ( self ) :
        """
        Get all psiX lines 
        """
        sel = self._selection ( 'PsiX_Lines' )
        if sel : return sel
        #
        from StrippingConf.StrippingLine import StrippingLine
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
            ## 1 proton
            #
            StrippingLine (
            "Lb2PsiPKFor"   + self.name()                 ,
            prescale        = self [ 'Lb2PsiPKPrescale'  ] , 
            checkPV         = self [ 'CheckPV' ]          ,
            FILTER          = self [ 'FILTER'  ]          ,
            ODIN            = self [ 'ODIN'    ]          ,
            L0DU            = self [ 'L0DU'    ]          ,
            HLT             = self [ 'L0DU'    ]          ,
            algos           = [ self.psi_pK ()         ]  ) ,
            #            
            StrippingLine (
            "Lb2PsiPPiFor"  + self.name()                 ,
            prescale        = self [ 'Lb2PsiPPiPrescale' ] , 
            checkPV         = self [ 'CheckPV' ]          ,
            FILTER          = self [ 'FILTER'  ]          ,
            ODIN            = self [ 'ODIN'    ]          ,
            L0DU            = self [ 'L0DU'    ]          ,
            HLT             = self [ 'L0DU'    ]          ,
            algos           = [ self.psi_ppi ()        ]  ) ,
            #
            StrippingLine (
            "Lb2PsiPKPiPiFor"  + self.name()                 ,
            prescale        = self [ 'Lb2PsiPKPiPiPrescale' ] , 
            checkPV         = self [ 'CheckPV' ]          ,
            FILTER          = self [ 'FILTER'  ]          ,
            ODIN            = self [ 'ODIN'    ]          ,
            L0DU            = self [ 'L0DU'    ]          ,
            HLT             = self [ 'L0DU'    ]          ,
            algos           = [ self.psi_pKpipi ()      ]  ) ,
            #
            #
            StrippingLine (
            "Lb2PsiPPiPiPiFor"  + self.name()                 ,
            prescale        = self [ 'Lb2PsiPPiPiPiPrescale' ] , 
            checkPV         = self [ 'CheckPV' ]          ,
            FILTER          = self [ 'FILTER'  ]          ,
            ODIN            = self [ 'ODIN'    ]          ,
            L0DU            = self [ 'L0DU'    ]          ,
            HLT             = self [ 'L0DU'    ]          ,
            algos           = [ self.psi_ppipipi ()       ]  ) ,
            #
            ## two protons
            #
            StrippingLine (
            "B2PsiPPFor"    + self.name()                 ,
            prescale        = self [ 'B2PsiPPPrescale' ]  , 
            checkPV         = self [ 'CheckPV' ]          ,
            FILTER          = self [ 'FILTER'  ]          ,
            ODIN            = self [ 'ODIN'    ]          ,
            L0DU            = self [ 'L0DU'    ]          ,
            HLT             = self [ 'L0DU'    ]          ,
            algos           = [ self.psi_pp ()       ]  ) ,
            #
            StrippingLine (
            "B2PsiPPPiFor"  + self.name()                 ,
            prescale        = self [ 'B2PsiPPPiPrescale' ]  , 
            checkPV         = self [ 'CheckPV' ]          ,
            FILTER          = self [ 'FILTER'  ]          ,
            ODIN            = self [ 'ODIN'    ]          ,
            L0DU            = self [ 'L0DU'    ]          ,
            HLT             = self [ 'L0DU'    ]          ,
            algos           = [ self.psi_pppi ()       ]  ) ,
            #
            StrippingLine (
            "B2PsiPPKFor"  + self.name()                 ,
            prescale        = self [ 'B2PsiPPKPrescale' ]  , 
            checkPV         = self [ 'CheckPV' ]          ,
            FILTER          = self [ 'FILTER'  ]          ,
            ODIN            = self [ 'ODIN'    ]          ,
            L0DU            = self [ 'L0DU'    ]          ,
            HLT             = self [ 'L0DU'    ]          ,
            algos           = [ self.psi_ppK ()       ]  ) ,
            #
            StrippingLine (
            "B2PsiPPPiPiFor"  + self.name()                 ,
            prescale        = self [ 'B2PsiPPPiPiPrescale' ]  , 
            checkPV         = self [ 'CheckPV' ]          ,
            FILTER          = self [ 'FILTER'  ]          ,
            ODIN            = self [ 'ODIN'    ]          ,
            L0DU            = self [ 'L0DU'    ]          ,
            HLT             = self [ 'L0DU'    ]          ,
            algos           = [ self.psi_pppipi ()       ]  ) ,
            #
            StrippingLine (
            "B2PsiPPKPiPiFor"  + self.name()                 ,
            prescale        = self [ 'B2PsiPPKPiPiPrescale' ]  , 
            checkPV         = self [ 'CheckPV' ]          ,
            FILTER          = self [ 'FILTER'  ]          ,
            ODIN            = self [ 'ODIN'    ]          ,
            L0DU            = self [ 'L0DU'    ]          ,
            HLT             = self [ 'L0DU'    ]          ,
            algos           = [ self.psi_ppKpipi ()       ]  ) ,
            #
            StrippingLine (
            "B2PsiPPPiPiPiFor"  + self.name()                 ,
            prescale        = self [ 'B2PsiPPPiPiPiPrescale' ]  , 
            checkPV         = self [ 'CheckPV' ]          ,
            FILTER          = self [ 'FILTER'  ]          ,
            ODIN            = self [ 'ODIN'    ]          ,
            L0DU            = self [ 'L0DU'    ]          ,
            HLT             = self [ 'L0DU'    ]          ,
            algos           = [ self.psi_pppipipi ()       ]  ) 
            #            
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
            self.protons      () ,
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
            self.psi_pK       () ,
            self.psi_ppi      () ,
            self.psi_pKpipi   () ,
            self.psi_ppipipi  () ,
            ## 
            self.psi_pp       () ,
            self.psi_pppi     () ,
            self.psi_ppK      () ,
            self.psi_pppipi   () ,
            self.psi_ppKpipi  () ,
            self.psi_pppipipi () ,
            ]
        
        return self._add_selection ( 'Selections' , sel )
    
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
        ## create new seleciton
        #
        alg = algotype ( *args , **kwargs )
        # 
        from PhysSelPython.Wrappers import Selection
        sel = Selection (
            sel_name                    , 
            Algorithm          = alg    ,
            RequiredSelections = inputs
            )
        # 
        return self._add_selection( sel_tag , sel ) 

        
    ## muons 
    def muons     ( self ) :    
        from StandardParticles import StdLooseMuons
        return StdLooseMuons
    
    ## pi0s :
    def pi0s      ( self ) :
        from StandardParticles import StdLoosePi02gg
        return StdLoosePi02gg 
    
    # ========================================================================
    ## pions :
    # ========================================================================
    def pions    ( self ) :
        """
        Pions for   B -> psi X lines 
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from StandardParticles                     import StdAllLoosePions as inpts 
        ##
        return self.make_selection (
            'Pion'                 ,
            FilterDesktop          ,
            [ inpts ]              ,
            Code = self['PionCut'] ,
            )
    
    # ========================================================================
    ## pions :
    # ========================================================================
    def kaons     ( self ) :
        """
        Kaons for   B -> psi X lines 
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from StandardParticles                     import StdAllLooseKaons as inpts 
        ##
        return self.make_selection (
            'Kaon'                 ,
            FilterDesktop          ,
            [ inpts ]              ,
            Code = self['KaonCut'] ,
            )

    # ========================================================================
    ## protons :
    # ========================================================================
    def protons    ( self ) :
        """
        Protons for   b -> psi X lines 
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from StandardParticles                     import StdAllLooseProtons as inpts 
        ##
        return self.make_selection (
            'Proton'                 ,
            FilterDesktop            ,
            [ inpts ]                ,
            Code = self['ProtonCut'] ,
            )
    
    # =========================================================================
    ## psi(') -> mu+ mu-
    # =========================================================================
    def psi ( self ) :
        """
        psi(') -> mu+ mu- 
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        ##
        return self.make_selection (
            'Psi' ,
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
    
    # =========================================================================
    # B -> psi(') K
    # =========================================================================
    def psi_K ( self ) :
        """
        B -> psi(') K
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        return self.make_selection (
            ## the unique tag 
            'PsiK'                        ,
            ## algorithm type to be used 
            CombineParticles              ,
            ## input selections 
            [ self.psi() , self.kaons() ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B+ -> J/psi(1S) pi+ ]cc" ,
            ##
            CombinationCut  = " mbu_acut | mbc_acut "    ,
            ## 
            MotherCut       = self._chargedB() 
            )
    
    # =========================================================================
    ## B -> psi(') pi
    # =========================================================================
    def psi_pi ( self ) :
        """
        B -> psi(') pi 
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        ##
        return self.make_selection (
            ## the unique tag 
            'PsiPi'                       ,
            ## algorithm type to be used 
            CombineParticles              ,
            ## input selections 
            [ self.psi() , self.pions() ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B+ -> J/psi(1S) pi+ ]cc"  ,
            ##
            CombinationCut  = " mbu_acut | mbc_acut "     ,
            ## 
            MotherCut       = self._chargedB() 
            ##
            )    
    
    # =========================================================================
    # B -> psi(') K+ K- 
    # =========================================================================
    def psi_2K ( self ) :
        """
        B -> psi(') K+ K-
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        return self.make_selection (
            ## the unique tag 
            'Psi2K'                       ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons() ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = " B0 -> J/psi(1S) K+ K- " ,
            ##
            Combination12Cut = """ ( AM < mb0_ahigh )   &
            ( ACHI2DOCA(1,2) < 20 )  
            """  , 
            CombinationCut   = """        mb0_acut      &
            ( ACHI2DOCA(1,3) < 20 ) &
            ( ACHI2DOCA(2,3) < 20 ) 
            """  ,
            ## 
            MotherCut       = self._neutralB() 
            )
    
    # =========================================================================
    # B -> psi(') K+ pi- 
    # =========================================================================
    def psi_2Kpi ( self ) :
        """
        B -> psi(') K+ K-
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        return self.make_selection (
            ## the unique tag 
            'Psi2KPi'                     ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons() , self.pions() ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[ B0 -> J/psi(1S) K+ pi-]cc" ,
            ##
            Combination12Cut = """ ( AM < mb0_ahigh )  &
            ( ACHI2DOCA(1,2) <  20 ) 
            """  , 
            CombinationCut   = """        mb0_acut     &
            ( ACHI2DOCA(1,3) <  20 ) &   
            ( ACHI2DOCA(2,3) <  20 )   
            """   ,
            ## 
            MotherCut       = self._neutralB() 
            )
    
    # =========================================================================
    # B -> psi(') pi+ pi- 
    # =========================================================================
    def psi_2pi ( self ) :
        """
        B -> psi(') pi+ pi-
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        return self.make_selection (
            ## the unique tag 
            'Psi2Pi'                      ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.psi() , self.pions() ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "B0 -> J/psi(1S) pi+ pi-"  ,
            ##
            Combination12Cut = """ ( AM < mb0_ahigh )  &
            ( ACHI2DOCA(1,2) <  20 ) 
            """  , 
            CombinationCut   = """        mb0_acut     &
            ( ACHI2DOCA(1,3) <  20 ) &   
            ( ACHI2DOCA(2,3) <  20 )   
            """   ,
            ## 
            MotherCut       = self._neutralB() 
            )

    # ========================================================================
    # B -> psi(') 3K
    # ========================================================================
    def psi_3K ( self ) :
        """
        B -> psi(') 3K        
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays
        ## 
        return self.make_selection (
            ## the unique tag 
            'Psi3K'                       ,
            ## algorithm type to be used
            DaVinci__N4BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B+ -> J/psi(1S) K+ K+ K-]cc"  ,
            ##
            Combination12Cut = """  ( AM < mbc_ahigh )      &
            ( ACHI2DOCA(1,2) <  20 ) 
            """ , 
            Combination123Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,3) <  20 ) &  
            ( ACHI2DOCA(2,3) <  20 ) 
            """ , 
            CombinationCut   = """  ( mbp_acut | mbc_acut ) &
            ( ACHI2DOCA(1,4) <  20 ) &
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """ ,
            ## 
            MotherCut       = self._chargedB() 
            )

    # ========================================================================
    # B -> psi(') 3Kpi
    # ========================================================================
    def psi_3Kpi ( self ) :
        """
        B -> psi(') 3Kpi
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays
        ## 
        return self.make_selection (
            ## the unique tag 
            'Psi3KPi'                     ,
            ## algorithm type to be used
            DaVinci__N4BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons () , self.pions () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptors = [
            "[B+ -> J/psi(1S) K+ pi+ pi-]cc" ,
            "[B+ -> J/psi(1S) K+ K-  pi+]cc" ,
            "[B+ -> J/psi(1S) K- pi+ pi+]cc"  ## ATTENTION: wrong charge combination!!!
            ],
            ##
            Combination12Cut = """  ( AM < mbc_ahigh )      &
            ( ACHI2DOCA(1,2) <  20 ) 
            """ , 
            Combination123Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,3) <  20 ) &
            ( ACHI2DOCA(2,3) <  20 ) 
            """ , 
            CombinationCut   = """  ( mbp_acut | mbc_acut ) &
            ( ACHI2DOCA(1,4) <  20 ) &
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """ ,
            ## 
            MotherCut       = self._chargedB() 
            )


    # ========================================================================
    # B -> psi(') 3pi
    # ========================================================================
    def psi_3pi ( self ) :
        """
        B -> psi(') 3pi
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays
        ## 
        return self.make_selection (
            ## the unique tag 
            'Psi3Pi'                      ,
            ## algorithm type to be used
            DaVinci__N4BodyDecays         ,
            ## input selections 
            [ self.psi() , self.pions () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B+ -> J/psi(1S) pi+ pi+ pi-]cc" ,
            ##
            Combination12Cut = """  ( AM < mbc_ahigh )      &
            ( ACHI2DOCA(1,2) <  20 ) 
            """ , 
            Combination123Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,3) <  20 ) &
            ( ACHI2DOCA(2,3) <  20 ) 
            """ , 
            CombinationCut   = """  ( mbp_acut | mbc_acut ) &
            ( ACHI2DOCA(1,4) <  20 ) &
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """ ,
            ## 
            MotherCut       = self._chargedB() 
            )



    # ========================================================================
    # B -> psi(') 4K
    # ========================================================================
    def psi_4K ( self ) :
        """
        B -> psi(') 4K
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N5BodyDecays
        ## 
        return self.make_selection (
            ## the unique tag 
            'Psi4k'                       ,
            ## algorithm type to be used
            DaVinci__N5BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor    = "B_s0 -> J/psi(1S) K+ K+ K- K-"  ,
            ##
            Combination12Cut   = """ ( AM < mb0_ahigh )      &
            ( ACHI2DOCA(1,2) <  20 ) 
            """ , 
            Combination123Cut  = """ ( AM < mb0_ahigh )      & 
            ( ACHI2DOCA(1,3) <  20 ) &
            ( ACHI2DOCA(2,3) <  20 ) 
            """ , 
            Combination1234Cut = """ ( AM < mb0_ahigh )      & 
            ( ACHI2DOCA(1,4) <  20 ) &
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(4,4) <  20 ) 
            """ , 
            CombinationCut   = """  mb0_acut  &
            ( ACHI2DOCA(1,5) <  20 ) &
            ( ACHI2DOCA(2,5) <  20 ) &
            ( ACHI2DOCA(3,5) <  20 ) &
            ( ACHI2DOCA(4,5) <  20 ) 
            """ ,
            ## 
            MotherCut       = self._neutralB() 
            )

    # ========================================================================
    # B -> psi(') 4Kpi
    # ========================================================================
    def psi_4Kpi ( self ) :
        """
        B -> psi(') 4Kpi
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N5BodyDecays
        ## 
        return self.make_selection (
            ## the unique tag 
            'Psi4KPi'                     ,
            ## algorithm type to be used
            DaVinci__N5BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons () , self.pions () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptors = [
            "[B0 -> J/psi(1S) K+ pi+ pi- pi-]cc" ,
            " B0 -> J/psi(1S) K+ pi+ K-  pi-   " ,
            "[B0 -> J/psi(1S) K+ K+  K-  pi-]cc" ,
            ],
            ##
            Combination12Cut  = """  ( AM < mb0_ahigh )      &
            ( ACHI2DOCA(1,2) <  20 ) 
            """ , 
            Combination123Cut  = """ ( AM < mb0_ahigh )      & 
            ( ACHI2DOCA(1,3) <  20 ) &
            ( ACHI2DOCA(2,3) <  20 ) 
            """ , 
            Combination1234Cut = """ ( AM < mb0_ahigh )      & 
            ( ACHI2DOCA(1,4) <  20 ) &
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """ , 
            CombinationCut   = """  mb0_acut  &
            ( ACHI2DOCA(1,5) <  20 ) &
            ( ACHI2DOCA(2,5) <  20 ) &
            ( ACHI2DOCA(3,5) <  20 ) &
            ( ACHI2DOCA(4,5) <  20 ) 
            """ ,
            ## 
            MotherCut       = self._neutralB() 
            )

    # ========================================================================
    # B -> psi(') 4pi
    # ========================================================================
    def psi_4pi ( self ) :
        """
        B -> psi(') 4pi
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N5BodyDecays
        ## 
        return self.make_selection (
            ## the unique tag 
            'Psi4Pi'                      ,
            ## algorithm type to be used
            DaVinci__N5BodyDecays         ,
            ## input selections 
            [ self.psi() , self.pions () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor   = "B0 -> J/psi(1S) pi+ pi+ pi- pi-" ,
            ##
            Combination12Cut  = """  ( AM < mb0_ahigh )      &
            ( ACHI2DOCA(1,2) <  20 ) 
            """ , 
            Combination123Cut  = """ ( AM < mb0_ahigh )      & 
            ( ACHI2DOCA(1,3) <  20 ) & 
            ( ACHI2DOCA(2,3) <  20 ) 
            """ ,            
            Combination1234Cut = """ ( AM < mb0_ahigh )      & 
            ( ACHI2DOCA(1,4) <  20 ) & 
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """ ,
            CombinationCut   = """  mb0_acut &
            ( ACHI2DOCA(1,5) <  20 ) &
            ( ACHI2DOCA(2,5) <  20 ) &
            ( ACHI2DOCA(3,5) <  20 ) & 
            ( ACHI2DOCA(4,5) <  20 ) 
            """ ,
            ## 
            MotherCut       = self._neutralB() 
            )


    # ========================================================================
    # B -> psi(') 5K
    # ========================================================================
    def psi_5K ( self ) :
        """
        B -> psi(') 5K        
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N6BodyDecays
        ## 
        return self.make_selection (
            ## the unique tag 
            'Psi5K'                       ,
            ## algorithm type to be used
            DaVinci__N6BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B_c+ -> J/psi(1S) K+ K+ K+ K- K-]cc"  ,
            ##
            Combination12Cut = """  ( AM < mbc_ahigh )      &
            ( ACHI2DOCA(1,2) <  20 ) 
            """ , 
            Combination123Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,3) <  20 ) &  
            ( ACHI2DOCA(2,3) <  20 ) 
            """ , 
            Combination1234Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,4) <  20 ) &  
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """ ,            
            Combination12345Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,5) <  20 ) &  
            ( ACHI2DOCA(2,5) <  20 ) &
            ( ACHI2DOCA(3,5) <  20 ) &  
            ( ACHI2DOCA(4,5) <  20 ) 
            """ ,
            CombinationCut   = """  ( mbp_acut | mbc_acut ) &
            ( ACHI2DOCA(1,6) <  20 ) &  
            ( ACHI2DOCA(2,6) <  20 ) &
            ( ACHI2DOCA(3,6) <  20 ) &  
            ( ACHI2DOCA(4,6) <  20 ) &
            ( ACHI2DOCA(5,6) <  20 ) 
            """ ,
            ##
            MotherCut       = self._chargedB() 
            )

    # ========================================================================
    # B -> psi(') 5Kpi
    # ========================================================================
    def psi_5Kpi ( self ) :
        """
        B -> psi(') 5Kpi
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N6BodyDecays
        ## 
        return self.make_selection (
            ## the unique tag 
            'Psi5KPi'                     ,
            ## algorithm type to be used
            DaVinci__N6BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons () , self.pions () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptors = [
            "[B+ -> J/psi(1S) K+ pi+ pi+ pi- pi-]cc" ,
            "[B+ -> J/psi(1S) K+ pi+ pi+ K-  pi-]cc" ,
            "[B+ -> J/psi(1S) K+ K+  pi+ K-  pi-]cc" ,
            "[B+ -> J/psi(1S) K+ K+  pi+ K-  K- ]cc" 
            ],
            ##
            Combination12Cut = """  ( AM < mbc_ahigh )      &
            ( ACHI2DOCA(1,2) <  20 ) 
            """ , 
            Combination123Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,3) <  20 ) &  
            ( ACHI2DOCA(2,3) <  20 ) 
            """ , 
            Combination1234Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,4) <  20 ) &  
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """ ,            
            Combination12345Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,5) <  20 ) &  
            ( ACHI2DOCA(2,5) <  20 ) &
            ( ACHI2DOCA(3,5) <  20 ) &  
            ( ACHI2DOCA(4,5) <  20 ) 
            """ ,
            CombinationCut   = """  ( mbp_acut | mbc_acut ) &
            ( ACHI2DOCA(1,6) <  20 ) &  
            ( ACHI2DOCA(2,6) <  20 ) &
            ( ACHI2DOCA(3,6) <  20 ) &  
            ( ACHI2DOCA(4,6) <  20 ) &
            ( ACHI2DOCA(5,6) <  20 ) 
            """ ,
            ## 
            MotherCut       = self._chargedB() 
            )



    # ========================================================================
    # B -> psi(') 5pi
    # ========================================================================
    def psi_5pi ( self ) :
        """
        B -> psi(') 5pi
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N6BodyDecays
        ## 
        return self.make_selection (
            ## the unique tag 
            'Psi5Pi'                      ,
            ## algorithm type to be used
            DaVinci__N6BodyDecays         ,
            ## input selections 
            [ self.psi() , self.pions () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B+ -> J/psi(1S) pi+ pi+ pi+ pi- pi-]cc" ,
            ##
            ##
            Combination12Cut = """  ( AM < mbc_ahigh )    &
            ( ACHI2DOCA(1,2) <  20 ) 
            """ , 
            Combination123Cut = """ ( AM < mbc_ahigh )    & 
            ( ACHI2DOCA(1,3) <  20 ) &  
            ( ACHI2DOCA(2,3) <  20 ) 
            """ , 
            Combination1234Cut = """ ( AM < mbc_ahigh )    & 
            ( ACHI2DOCA(1,4) <  20 ) &  
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """ ,            
            Combination12345Cut = """ ( AM < mbc_ahigh )    & 
            ( ACHI2DOCA(1,5) <  20 ) &  
            ( ACHI2DOCA(2,5) <  20 ) &
            ( ACHI2DOCA(3,5) <  20 ) &  
            ( ACHI2DOCA(4,5) <  20 ) 
            """ ,
            CombinationCut   = """  ( mbp_acut | mbc_acut ) &
            ( ACHI2DOCA(1,6) <  20 ) &  
            ( ACHI2DOCA(2,6) <  20 ) &
            ( ACHI2DOCA(3,6) <  20 ) &  
            ( ACHI2DOCA(4,6) <  20 ) &
            ( ACHI2DOCA(5,6) <  20 ) 
            """ ,
            ## 
            MotherCut       = self._chargedB() 
            )

    
    # ========================================================================
    # B -> psi(') 6K
    # ========================================================================
    
    # ========================================================================
    # B -> psi(') 6Kpi
    # ========================================================================
    def psi_6Kpi ( self ) :
        """
        B -> psi(') 6Kpi
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N7BodyDecays
        ## 
        return self.make_selection (
            ## the unique tag 
            'Psi6KPi'                     ,
            ## algorithm type to be used
            DaVinci__N7BodyDecays         ,
            ## input selections 
            [ self.psi() , self.kaons () , self.pions () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptors = [
            "[B0 -> J/psi(1S) K+ pi+ pi+ pi- pi- pi-]cc" ,
            " B0 -> J/psi(1S) K+ pi+ pi+ K-  pi- pi-   " ,
            "[B0 -> J/psi(1S) K+ K+  pi+ K-  pi- pi-]cc" ,
            " B0 -> J/psi(1S) K+ K+  pi+ K-  K-  pi-   " ,
            ],
            ##
            Combination12Cut     = """  ( AM < mb0_ahigh )      &
            ( ACHI2DOCA(1,2) <  20 ) 
            """ , 
            Combination123Cut    = """ ( AM < mb0_ahigh )      & 
            ( ACHI2DOCA(1,3) <  20 ) &
            ( ACHI2DOCA(2,3) <  20 ) 
            """ , 
            Combination1234Cut   = """ ( AM < mb0_ahigh )      & 
            ( ACHI2DOCA(1,4) <  20 ) &
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """ , 
            Combination12345Cut  = """ ( AM < mb0_ahigh )      & 
            ( ACHI2DOCA(1,5) <  20 ) &
            ( ACHI2DOCA(2,5) <  20 ) &
            ( ACHI2DOCA(3,5) <  20 ) &
            ( ACHI2DOCA(4,5) <  20 ) 
            """ , 
            Combination123456Cut = """ ( AM < mb0_ahigh )      & 
            ( ACHI2DOCA(1,6) <  20 ) &
            ( ACHI2DOCA(2,6) <  20 ) &
            ( ACHI2DOCA(3,6) <  20 ) & 
            ( ACHI2DOCA(4,6) <  20 ) &
            ( ACHI2DOCA(5,6) <  20 ) 
            """ , 
            CombinationCut   = """  mb0_acut  &
            ( ACHI2DOCA(1,7) <  20 ) &
            ( ACHI2DOCA(2,7) <  20 ) &
            ( ACHI2DOCA(3,7) <  20 ) &
            ( ACHI2DOCA(4,7) <  20 ) &
            ( ACHI2DOCA(5,7) <  20 ) &
            ( ACHI2DOCA(6,7) <  20 ) 
            """ ,
            ## 
            MotherCut       = self._neutralB() 
            )

    # ========================================================================
    # B -> psi(') 6pi
    # ========================================================================
    def psi_6pi ( self ) :
        """
        B -> psi(') 6pi
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N7BodyDecays
        ## 
        return self.make_selection (
            ## the unique tag 
            'Psi6Pi'                      ,
            ## algorithm type to be used
            DaVinci__N7BodyDecays         ,
            ## input selections 
            [ self.psi() , self.pions () ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor   = "B0 -> J/psi(1S) pi+ pi+ pi+ pi- pi- pi-" ,
            ##
            Combination12Cut  = """  ( AM < mb0_ahigh )      &
            ( ACHI2DOCA(1,2) <  20 ) 
            """ , 
            Combination123Cut  = """ ( AM < mb0_ahigh )      & 
            ( ACHI2DOCA(1,3) <  20 ) & 
            ( ACHI2DOCA(2,3) <  20 ) 
            """ ,            
            Combination1234Cut = """ ( AM < mb0_ahigh )      & 
            ( ACHI2DOCA(1,4) <  20 ) & 
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """ ,
            Combination12345Cut = """ ( AM < mb0_ahigh )      & 
            ( ACHI2DOCA(1,5) <  20 ) & 
            ( ACHI2DOCA(2,5) <  20 ) &
            ( ACHI2DOCA(3,5) <  20 ) &
            ( ACHI2DOCA(4,5) <  20 ) 
            """ ,
            Combination123456Cut = """ ( AM < mb0_ahigh )      & 
            ( ACHI2DOCA(1,6) <  20 ) & 
            ( ACHI2DOCA(2,6) <  20 ) &
            ( ACHI2DOCA(3,6) <  20 ) &
            ( ACHI2DOCA(4,6) <  20 ) &
            ( ACHI2DOCA(5,6) <  20 ) 
            """ ,
            CombinationCut   = """  mb0_acut &
            ( ACHI2DOCA(1,7) <  20 ) & 
            ( ACHI2DOCA(2,7) <  20 ) &
            ( ACHI2DOCA(3,7) <  20 ) &
            ( ACHI2DOCA(4,7) <  20 ) &
            ( ACHI2DOCA(5,7) <  20 ) & 
            ( ACHI2DOCA(6,7) <  20 )
            """ ,
            ## 
            MotherCut       = self._neutralB() 
            )


    # ===========================================================================
    # make use of protons 
    # =========================================================================
    
    # =========================================================================
    # Lb -> psi(') pK
    # =========================================================================
    def psi_pK ( self ) :
        """
        Lb -> psi(') pK
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        return self.make_selection (
            ## the unique tag 
            'PsiPK'                      ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.psi() , self.protons() , self.kaons() ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[Lambda_b0 -> J/psi(1S) p+ K-]cc" ,
            ##
            Combination12Cut = """ ( AM < mlb_ahigh )  &
            ( ACHI2DOCA(1,2) <  20 ) 
            """  , 
            CombinationCut   = """        mlb_acut     &
            ( ACHI2DOCA(1,3) <  20 ) &   
            ( ACHI2DOCA(2,3) <  20 )   
            """   ,
            ##
            MotherCut       = self._lambdaB() 
            )

    # =========================================================================
    # Lb -> psi(') ppi
    # =========================================================================
    def psi_ppi ( self ) :
        """
        Lb -> psi(') p pi
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        return self.make_selection (
            ## the unique tag 
            'PsiPPi'                      ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.psi() , self.protons() , self.pions() ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[Lambda_b0 -> J/psi(1S) p+ pi-]cc" ,
            ##
            Combination12Cut = """ ( AM < mlb_ahigh )  &
            ( ACHI2DOCA(1,2) <  20 ) 
            """  , 
            CombinationCut   = """        mlb_acut     &
            ( ACHI2DOCA(1,3) <  20 ) &   
            ( ACHI2DOCA(2,3) <  20 )   
            """   ,
            ## 
            MotherCut       = self._lambdaB() 
            )

    # =========================================================================
    # Lb -> psi(') pK pipi
    # =========================================================================
    def psi_pKpipi ( self ) :
        """
        Lb -> psi(') pK pi pi
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N5BodyDecays 
        return self.make_selection (
            ## the unique tag 
            'PsiPKpipi'                   ,
            ## algorithm type to be used
            DaVinci__N5BodyDecays         ,
            ## input selections 
            [ self.psi() , self.protons() , self.kaons() , self.pions() ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor    = "[Lambda_b0 -> J/psi(1S) p+ K- pi+ pi- ]cc" ,
            ##
            Combination12Cut   = """ ( AM < mlb_ahigh )  &
            ( ACHI2DOCA(1,2) <  20 ) 
            """  , 
            Combination123Cut  = """ ( AM < mlb_ahigh )  &
            ( ACHI2DOCA(1,3) <  20 ) &
            ( ACHI2DOCA(2,3) <  20 ) 
            """  , 
            Combination1234Cut = """ ( AM < mlb_ahigh )  &
            ( ACHI2DOCA(1,4) <  20 ) &
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """  , 
            CombinationCut   = """        mlb_acut       &
            ( ACHI2DOCA(1,5) <  20 ) &
            ( ACHI2DOCA(2,5) <  20 ) &
            ( ACHI2DOCA(3,5) <  20 ) &
            ( ACHI2DOCA(4,5) <  20 ) 
            """   ,
            ## 
            MotherCut       = self._lambdaB() 
            )


    # =========================================================================
    # Lb -> psi(') p pipipi
    # =========================================================================
    def psi_ppipipi ( self ) :
        """
        Lb -> psi(') p pipipi
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N5BodyDecays 
        return self.make_selection (
            ## the unique tag 
            'PsiPpipipi'                  ,
            ## algorithm type to be used
            DaVinci__N5BodyDecays         ,
            ## input selections 
            [ self.psi() , self.protons() , self.pions() ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor    = "[Lambda_b0 -> J/psi(1S) p+ pi+ pi- pi- ]cc" ,
            ##
            Combination12Cut   = """ ( AM < mlb_ahigh )  &
            ( ACHI2DOCA(1,2) <  20 ) 
            """  , 
            Combination123Cut  = """ ( AM < mlb_ahigh )  &
            ( ACHI2DOCA(1,3) <  20 ) &
            ( ACHI2DOCA(2,3) <  20 ) 
            """  , 
            Combination1234Cut = """ ( AM < mlb_ahigh )  &
            ( ACHI2DOCA(1,4) <  20 ) &
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """  , 
            CombinationCut   = """        mlb_acut       &
            ( ACHI2DOCA(1,5) <  20 ) &
            ( ACHI2DOCA(2,5) <  20 ) &
            ( ACHI2DOCA(3,5) <  20 ) &
            ( ACHI2DOCA(4,5) <  20 ) 
            """   ,
            ## 
            MotherCut       = self._lambdaB() 
            )
    
    
    # =========================================================================
    # B -> psi(') pp
    # =========================================================================    
    def psi_pp ( self ) :
        """
        B -> psi(') pp
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        return self.make_selection (
            ## the unique tag 
            'PsiPP'                       ,
            ## algorithm type to be used
            DaVinci__N3BodyDecays         ,
            ## input selections 
            [ self.psi() , self.protons() ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "B_s0 -> J/psi(1S) p+ p~-" ,
            ##
            Combination12Cut = """ ( AM < mlb_ahigh )  &
            ( ACHI2DOCA(1,2) <  20 ) 
            """  , 
            CombinationCut   = """        mlb_acut     &
            ( ACHI2DOCA(1,3) <  20 ) &   
            ( ACHI2DOCA(2,3) <  20 )   
            """   ,
            ## 
            MotherCut       = self._neutralB() 
            )
    
    # =========================================================================
    # B -> psi(') pp pi
    # =========================================================================    
    def psi_pppi( self ) :
        """
        B -> psi(') pp pi
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays 
        return self.make_selection (
            ## the unique tag 
            'PsiPPpi'                     ,
            ## algorithm type to be used
            DaVinci__N4BodyDecays         ,
            ## input selections 
            [ self.psi() , self.protons() , self.pions() ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B+ -> J/psi(1S) p+ p~- pi+]cc" ,
            ##
            Combination12Cut = """  ( AM < mbc_ahigh )      &
            ( ACHI2DOCA(1,2) <  20 ) 
            """ , 
            Combination123Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,3) <  20 ) &
            ( ACHI2DOCA(2,3) <  20 ) 
            """ , 
            CombinationCut   = """  ( mbp_acut | mbc_acut ) &
            ( ACHI2DOCA(1,4) <  20 ) &
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """ ,
            ## 
            MotherCut       = self._chargedB() 
            )

    # =========================================================================
    # B -> psi(') pp pi
    # =========================================================================    
    def psi_ppK( self ) :
        """
        B -> psi(') pp K
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays 
        return self.make_selection (
            ## the unique tag 
            'PsiPPK'                   ,
            ## algorithm type to be used
            DaVinci__N4BodyDecays         ,
            ## input selections 
            [ self.psi() , self.protons() , self.kaons() ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B+ -> J/psi(1S) p+ p~- K+]cc" ,
            ##
            Combination12Cut = """  ( AM < mbc_ahigh )      &
            ( ACHI2DOCA(1,2) <  20 ) 
            """ , 
            Combination123Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,3) <  20 ) &
            ( ACHI2DOCA(2,3) <  20 ) 
            """ , 
            CombinationCut   = """  ( mbp_acut | mbc_acut ) &
            ( ACHI2DOCA(1,4) <  20 ) &
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """ ,
            ## 
            MotherCut       = self._chargedB() 
            )


    # =========================================================================
    # B -> psi(') pp pipi
    # =========================================================================    
    def psi_pppipi( self ) :
        """
        B -> psi(') pp pi pi
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N5BodyDecays 
        return self.make_selection (
            ## the unique tag 
            'PsiPPpipi'                   ,
            ## algorithm type to be used
            DaVinci__N5BodyDecays         ,
            ## input selections 
            [ self.psi() , self.protons() , self.pions() ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "B_s0 -> J/psi(1S) p+ p~- pi+ pi-" ,
            ##
            Combination12Cut  = """  ( AM < mb0_ahigh )      &
            ( ACHI2DOCA(1,2) <  20 ) 
            """ , 
            Combination123Cut  = """ ( AM < mb0_ahigh )      & 
            ( ACHI2DOCA(1,3) <  20 ) & 
            ( ACHI2DOCA(2,3) <  20 ) 
            """ ,            
            Combination1234Cut = """ ( AM < mb0_ahigh )      & 
            ( ACHI2DOCA(1,4) <  20 ) & 
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """ ,
            CombinationCut   = """  mb0_acut &
            ( ACHI2DOCA(1,5) <  20 ) &
            ( ACHI2DOCA(2,5) <  20 ) &
            ( ACHI2DOCA(3,5) <  20 ) & 
            ( ACHI2DOCA(4,5) <  20 ) 
            """ ,
            ## 
            MotherCut       = self._neutralB() 
            )


    # =========================================================================
    # B -> psi(') pp pipipi
    # =========================================================================    
    def psi_pppipipi( self ) :
        """
        B -> psi(') pp pi pi pi
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N6BodyDecays 
        return self.make_selection (
            ## the unique tag 
            'PsiPPpipipi'                     ,
            ## algorithm type to be used
            DaVinci__N6BodyDecays         ,
            ## input selections 
            [ self.psi() , self.protons() , self.pions() ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor = "[B+ -> J/psi(1S) p+ p~- pi+ pi+ pi-]cc" ,
            ##
            Combination12Cut = """  ( AM < mbc_ahigh )      &
            ( ACHI2DOCA(1,2) <  20 ) 
            """ , 
            Combination123Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,3) <  20 ) &
            ( ACHI2DOCA(2,3) <  20 ) 
            """ , 
            Combination1234Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,4) <  20 ) &
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """ , 
            Combination12345Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,5) <  20 ) &
            ( ACHI2DOCA(2,5) <  20 ) &
            ( ACHI2DOCA(3,5) <  20 ) &
            ( ACHI2DOCA(4,5) <  20 ) 
            """ , 
            CombinationCut   = """  ( mbp_acut | mbc_acut ) &
            ( ACHI2DOCA(1,6) <  20 ) &
            ( ACHI2DOCA(2,6) <  20 ) &
            ( ACHI2DOCA(3,6) <  20 ) &
            ( ACHI2DOCA(4,6) <  20 ) &
            ( ACHI2DOCA(5,6) <  20 ) 
            """ ,
            ## 
            MotherCut       = self._chargedB() 
            )

    # =========================================================================
    # B -> psi(') pp Kpipi
    # =========================================================================    
    def psi_ppKpipi( self ) :
        """
        B -> psi(') pp K pi pi
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N6BodyDecays 
        return self.make_selection (
            ## the unique tag 
            'PsiPPKpipi'                  ,
            ## algorithm type to be used
            DaVinci__N6BodyDecays         ,
            ## input selections 
            [ self.psi() , self.protons() , self.kaons() , self.pions() ] ,
            #
            ## algorithm configuration
            #
            DecayDescriptor  = "[B+ -> J/psi(1S) p+ p~- K+ pi+ pi-]cc" ,
            ##
            Combination12Cut = """  ( AM < mbc_ahigh )      &
            ( ACHI2DOCA(1,2) <  20 ) 
            """ , 
            Combination123Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,3) <  20 ) &
            ( ACHI2DOCA(2,3) <  20 ) 
            """ , 
            Combination1234Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,4) <  20 ) &
            ( ACHI2DOCA(2,4) <  20 ) &
            ( ACHI2DOCA(3,4) <  20 ) 
            """ , 
            Combination12345Cut = """ ( AM < mbc_ahigh )      & 
            ( ACHI2DOCA(1,5) <  20 ) &
            ( ACHI2DOCA(2,5) <  20 ) &
            ( ACHI2DOCA(3,5) <  20 ) &
            ( ACHI2DOCA(4,5) <  20 ) 
            """ , 
            CombinationCut   = """  ( mbp_acut | mbc_acut ) &
            ( ACHI2DOCA(1,6) <  20 ) &
            ( ACHI2DOCA(2,6) <  20 ) &
            ( ACHI2DOCA(3,6) <  20 ) &
            ( ACHI2DOCA(4,6) <  20 ) &
            ( ACHI2DOCA(5,6) <  20 ) 
            """ ,
            ## 
            MotherCut       = self._chargedB() 
            )
    
    
    # =========================================================================
    ## merged few basic B-hadrons: B+ , B- & Bs 
    # =========================================================================
    def beauty ( self ) :
        """
        Merged selection of B-, B+ & Bs  
        """
        sel = self._selection ( 'Beauty_Selection')
        if sel : return sel
        
        from PhysSelPython.Wrappers import MergedSelection
        sel = MergedSelection (
            'SelBeautyFor' + self.name() ,
            RequiredSelections = [ self.psi_K    () ,  ## B+
                                   self.psi_2Kpi () ,  ## B0 
                                   self.psi_2K   () ,  ## B_s0 
                                   self.psi_3K   () ,  ## B+ 
                                   self.psi_3Kpi () ]  ## B+ 
            )
        #
        return self._add_selection ( 'Beauty_Selection' , sel )
        
# =============================================================================
if '__main__' == __name__ :

    print 80*'*'
    print __doc__
    print ' Author :  %s' % __author__
    print ' Date   :  %s' % __date__
    print 80*'*'
    print __doc__
    print ' Author :  %s' % __author__
    print ' Date   :  %s' % __date__
    print ' The output locations for default configuration: '
    _conf = PsiX_BQ_Conf ( 'PsiX' , config = {}  )
    for l in _conf.lines() :
        print ' \t ', l.outputLocation  () , l
    print 80*'*'
    print 80*'*'
        
# =============================================================================
# The END 
# =============================================================================
