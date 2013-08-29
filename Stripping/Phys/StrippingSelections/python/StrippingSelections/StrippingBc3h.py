#!/usr/bin/env python
# =============================================================================
# $Id: StrippingBc3h.py 154361$
# =============================================================================
## @file
#
# =============================================================================
"""
A bit naive attempt to make  Bc->3h stripping

 |                *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
 |_StrippingGlobal_              |  0.1050|       105|       |  14.896|
 |_StrippingSequenceStreamTest_  |  0.1050|       105|       |  14.892|
 |!StrippingBc3pppiForBc3h       |  0.0110|        11|  1.182|   7.484|
 |!StrippingBc3ppkForBc3h        |  0.0040|         4|  1.000|   1.289|
 |!StrippingBc3piForBc3h         |  0.0430|        43|  1.163|   1.325|
 |!StrippingBc3kForBc3h          |  0.0000|         0|  0.000|   1.284|
 |!StrippingBc3kpiForBc3h        |  0.0670|        67|  1.388|   0.087|

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = '2010-08-03'
__version__ = '$Revision: 154361 $'
# =============================================================================
__all__ = (
    'Bc3hConf',
    )
# =============================================================================
from Gaudi.Configuration import *
# =============================================================================
## logging
# =============================================================================
import logging
logger = logging.getLogger(__name__)
if not logger.handlers :
    logging.basicConfig()
logger.setLevel(logging.INFO)
# =============================================================================
from GaudiKernel.SystemOfUnits             import GeV, MeV, mm, micrometer 
from StrippingUtils.Utils                  import LineBuilder

# =============================================================================
# use a bit faster fitter 
FITTER = 'LoKi::VertexFitter:PUBLIC'
## ============================================================================

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
    'CTAU'      : 140 * micrometer , 
    'CTAU_BC'   :  80 * micrometer , 
    #
    ## pions and kaons
    # 
    'PionCut'   : """
    ( CLONEDIST   > 5000   ) & 
    ( TRCHI2DOF   < 4      ) &
    ( TRGHOSTPROB < 0.4    ) &
    ( PT          > 750 * MeV               ) & 
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                  &
    ( PROBNNpi     > 0.15  ) &
    ( MIPCHI2DV()  > 9.    )
    """ ,
    #
    'KaonCut'   : """
    ( CLONEDIST   > 5000   ) & 
    ( TRCHI2DOF   < 4      ) & 
    ( TRGHOSTPROB < 0.4    ) & 
    ( PT          > 750 * MeV               ) & 
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                  &
    ( PROBNNk      > 0.20  ) &
    ( MIPCHI2DV()  > 9.    ) 
    """ ,
    #
    'ProtonCut'   : """
    ( CLONEDIST   > 5000    ) & 
    ( TRCHI2DOF   < 4       ) & 
    ( TRGHOSTPROB < 0.4     ) & 
    ( PT          > 500 * MeV              ) & 
    in_range (  2        , ETA , 4.9       ) &
    in_range ( 10 * GeV  , P   , 150 * GeV ) &
    HASRICH                   &
    ( PROBNNp      > 0.15   ) &
    ( MIPCHI2DV()  > 4.     ) 
    """ ,
    #
    ## useful shortcuts:
    #
    'Preambulo' : [
    ## shortcut for chi2 of vertex fit 
    'chi2vx = VFASPF(VCHI2)   '                                , 
    ## shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light"        , 
    ## use the embedded cut for chi2(LifetimeFit)<25
    "ctau      = BPVLTIME ( 25 ) * c_light "                   ,
    "ctau_9    = BPVLTIME (  9 ) * c_light "                   ,
    "ctau_16   = BPVLTIME ( 16 ) * c_light "                   ,
    ## Combination mass-cut for beauty particles 
    "mbp_acut  = in_range ( 5.050 * GeV , AM , 5.550 * GeV ) " ,
    "mbc_acut  = in_range ( 6.000 * GeV , AM , 6.600 * GeV ) " ,
    ## mass-cut for beauty particles 
    "mbp_cut   = in_range ( 5.100 * GeV ,  M , 5.500 * GeV ) " ,
    "mbc_cut   = in_range ( 6.050 * GeV ,  M , 6.550 * GeV ) " ,
    ] ,
    # =========================================================================
    ## Prescales 
    # =========================================================================
    'Bc3piPrescale'    : 1.0 ,
    'Bc3kPrescale'     : 1.0 ,
    'Bc3kpiPrescale'   : 1.0 ,
    'Bc3kpiPrescale'   : 1.0 ,
    'Bc3pppiPrescale'  : 1.0 ,
    'Bc3ppkPrescale'   : 1.0 ,
    # =========================================================================
    }

# =============================================================================
## @class  Bc3hConf
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-08-29
class Bc3hConf(LineBuilder) :
    """
    Helper class to configure 'Bc->3h'-lines
    """
    __configuration_keys__ = tuple ( _default_configuration_.keys() )
    
    ## get the default configuration 
    @staticmethod
    def defaultConfiguration( key = None ) :
        """
        Get the default/recommended configurtaion
        
        >>> conf = Bc3h.defaultConfiguration()
        
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
        if 'Bc3h' != name :
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
        
        for line in self._lines_Bc () :
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
    def _lines_Bc   ( self ) :
        """
        Get all psiX lines 
        """
        sel = self._selection ( 'Bc3h_Lines' )
        if sel : return sel
        #
        from StrippingConf.StrippingLine import StrippingLine
        sel = [
            # =================================================================
            #
            ## 3h
            #
            StrippingLine (
            "Bc3pppiFor"    + self.name()              ,
            prescale        = self ['Bc3pppiPrescale'] , 
            checkPV         = self ['CheckPV']         ,
            FILTER          = self ['FILTER' ]         ,
            ODIN            = self ['ODIN'   ]         ,
            L0DU            = self ['L0DU'   ]         ,
            HLT             = self ['L0DU'   ]         ,
            algos           = [ self.Bc3pppi ()   ]  ) ,
            #
            StrippingLine (
            "Bc3ppkFor"     + self.name()              ,
            prescale        = self ['Bc3ppkPrescale']  , 
            checkPV         = self ['CheckPV']         ,
            FILTER          = self ['FILTER' ]         ,
            ODIN            = self ['ODIN'   ]         ,
            L0DU            = self ['L0DU'   ]         ,
            HLT             = self ['L0DU'   ]         ,
            algos           = [ self.Bc3ppk ()   ]  )  ,
            #
            StrippingLine (
            "Bc3piFor"      + self.name()              ,
            prescale        = self ['Bc3piPrescale'] , 
            checkPV         = self ['CheckPV']         ,
            FILTER          = self ['FILTER' ]         ,
            ODIN            = self ['ODIN'   ]         ,
            L0DU            = self ['L0DU'   ]         ,
            HLT             = self ['L0DU'   ]         ,
            algos           = [ self.Bc3pi ()      ]  ) ,
            ## 
            StrippingLine (
            "Bc3kFor"      + self.name()              ,
            prescale        = self ['Bc3kPrescale'] , 
            checkPV         = self ['CheckPV']         ,
            FILTER          = self ['FILTER' ]         ,
            ODIN            = self ['ODIN'   ]         ,
            L0DU            = self ['L0DU'   ]         ,
            HLT             = self ['L0DU'   ]         ,
            algos           = [ self.Bc3k ()      ]  ) ,
            #
            StrippingLine (
            "Bc3kpiFor"     + self.name()              ,
            prescale        = self ['Bc3kpiPrescale']  , 
            checkPV         = self ['CheckPV']         ,
            FILTER          = self ['FILTER' ]         ,
            ODIN            = self ['ODIN'   ]         ,
            L0DU            = self ['L0DU'   ]         ,
            HLT             = self ['L0DU'   ]         ,
            algos           = [ self.Bc3kpi ()    ]  ) ,
            ]
        ## 
        return self._add_selection ( 'Bc3h_Lines' , sel ) 
    
    ## pions :
    def pions     ( self ) :
        """
        Pions for   Bc -> 3pi lines 
        """        
        sel = self._selection ( 'Pion_Selection')
        if sel : return sel

        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        alg  = FilterDesktop (
            ##
            Preambulo = self['Preambulo'] ,
            ##
            Code = self['PionCut'] ,
            ##
            )
        
        from PhysSelPython.Wrappers  import Selection
        from StandardParticles       import StdAllNoPIDsPions as input_pions 
        sel  = Selection (
            "SelPiFor"         + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ input_pions ]  
            )
        
        return self._add_selection( 'Pion_Selection' , sel ) 
    
    ## kaons :
    def kaons     ( self ) :
        """
        Kaons for   Bc -> 3h lines 
        """
        sel = self._selection ( 'Kaon_Selection')
        if sel : return sel
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        alg  = FilterDesktop (
            ##
            Preambulo = self['Preambulo'] ,
            ##
            Code = self['KaonCut'] ,
            ##
            )
        
        from PhysSelPython.Wrappers  import Selection
        from StandardParticles       import StdAllNoPIDsKaons as input_kaons 
        sel  = Selection (
            "SelKFor"      + self.name()       ,
            Algorithm          =   alg         ,
            RequiredSelections = [ input_kaons ]  
            )
        
        return self._add_selection( 'Kaon_Selection' , sel ) 

    ## protons :
    def protons    ( self ) :
        """
        Protons for   Bc -> 3h  lines 
        """
        sel = self._selection ( 'Proton_Selection')
        if sel : return sel
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        alg  = FilterDesktop (
            ##
            Preambulo = self['Preambulo'] ,
            ##
            Code = self['ProtonCut'] ,
            ##
            )
        
        from PhysSelPython.Wrappers  import Selection
        from StandardParticles       import StdAllNoPIDsProtons as input_protons 
        sel  = Selection (
            "SelPFor"          +   self.name()   ,
            Algorithm          =   alg           ,
            RequiredSelections = [ input_protons ]  
            )
        
        return self._add_selection( 'Proton_Selection' , sel ) 
    
    # Bc -> 3 pi 
    def Bc3pi ( self ) :
        """
        Bc -> 3 pi
        """
        sel = self._selection ( 'Bc3pi_Selection')
        if sel : return sel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
        alg  = CombineParticles (
            ##
            DecayDescriptor = "[B_c+ -> pi+ pi- pi+ ]cc" ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mbp_acut | mbc_acut
            """ ,
            ## 
            MotherCut = """
            ( chi2vx    < 16    )  &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
            ##
            , ParticleCombiners = { '' : FITTER }
            )
        
        from PhysSelPython.Wrappers  import Selection
        sel  = Selection (
            "SelBc3piFor"      + self.name()      ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.pions () ] 
            )
        
        return self._add_selection( 'Bc3pi_Selection' , sel ) 

    # Bc -> 3 k 
    def Bc3k ( self ) :
        """
        Bc -> 3 k
        """
        sel = self._selection ( 'Bc3k_Selection')
        if sel : return sel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
        alg  = CombineParticles (
            ##
            DecayDescriptor = "[B_c+ -> K+ K- K+]cc" ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mbp_acut | mbc_acut
            """ ,
            ## 
            MotherCut = """
            ( chi2vx    < 16    )  &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
            ##
            , ParticleCombiners = { '' : FITTER }
            )
        
        from PhysSelPython.Wrappers  import Selection
        sel  = Selection (
            "SelBc3kFor"      + self.name()      ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.kaons () ] 
            )
        
        return self._add_selection( 'Bc3k_Selection' , sel ) 

    # Bc -> 3kpi 
    def Bc3kpi ( self ) :
        """
        Bc -> 3 k pi 
        """
        sel = self._selection ( 'Bc3kpi_Selection')
        if sel : return sel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
        alg  = CombineParticles (
            ##
            DecayDescriptors = [
            "[B_c+ -> K+  K-  pi+ ]cc" ,
            "[B_c+ -> K+  pi- K+  ]cc" ,
            "[B_c+ -> K+  pi- pi+ ]cc" ,
            "[B_c+ -> pi+ K-  pi+ ]cc" ,
            ] ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mbp_acut | mbc_acut
            """ ,
            ## 
            MotherCut = """
            ( chi2vx    < 16    )  &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
            ##
            , ParticleCombiners = { '' : FITTER }
            )
        
        from PhysSelPython.Wrappers  import Selection
        sel  = Selection (
            "SelBc3kpiFor"     + self.name()       ,
            Algorithm          =   alg           ,
            RequiredSelections = [ self.kaons () ,
                                   self.pions () ] 
            )
        
        return self._add_selection( 'Bc3kpi_Selection' , sel ) 
    
    # Bc -> pppi
    def Bc3pppi ( self ) :
        """
        Bc -> pppi 
        """
        sel = self._selection ( 'Bc3pppi_Selection')
        if sel : return sel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
        alg  = CombineParticles (
            ##
            DecayDescriptor = "[B_c+ -> p+  p~- pi+ ]cc" ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mbp_acut | mbc_acut
            """ ,
            ## 
            MotherCut = """
            ( chi2vx    < 16    )  &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
            ##
            , ParticleCombiners = { '' : FITTER }
            )
        
        from PhysSelPython.Wrappers  import Selection
        sel  = Selection (
            "SelBc3pppiFor"    + self.name()       ,
            Algorithm          =   alg             ,
            RequiredSelections = [ self.protons () ,
                                   self.pions   () ] 
            )

        
        return self._add_selection( 'Bc3pppi_Selection' , sel ) 
    
    # Bc -> ppK
    def Bc3ppk ( self ) :
        """
        Bc -> ppK
        """
        sel = self._selection ( 'Bc3ppk_Selection')
        if sel : return sel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
        alg  = CombineParticles (
            ##
            DecayDescriptor = "[B_c+ -> p+  p~- K+ ]cc" ,
            ##
            Preambulo = self['Preambulo'] ,
            ##
            CombinationCut = """
            mbp_acut | mbc_acut
            """ ,
            ## 
            MotherCut = """
            ( chi2vx    < 16    )  &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
            ##
            , ParticleCombiners = { '' : FITTER }
            )
        
        from PhysSelPython.Wrappers  import Selection
        sel  = Selection (
            "SelBc3ppkFor"     + self.name()       ,
            Algorithm          =   alg             ,
            RequiredSelections = [ self.protons () ,
                                   self.kaons   () ] 
            )
        
        return self._add_selection( 'Bc3ppk_Selection' , sel ) 

    
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
    _conf = Bc3hConf ( 'Bc3h' , config = {}  )
    for l in _conf.lines() :
        print ' \t ', l.outputLocation  () , l
    print 80*'*'
    print 80*'*'
        
# =============================================================================
# The END 
# =============================================================================
