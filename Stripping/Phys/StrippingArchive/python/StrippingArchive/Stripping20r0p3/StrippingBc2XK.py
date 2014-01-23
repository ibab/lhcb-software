#!/usr/bin/env python
# =============================================================================
# $Id: StrippingBc2KstK.py 15436155$
# =============================================================================
## @file
#
# =============================================================================
"""A stripping line for Bc+ -> Kst K+, Kst -> K+ pi-

This file was based on StrippingBc3h.py taken from Phys/Stripping on 2014-01-10
(made by Vanya BELYAEV Ivan.Belyaev@itep.ru). 

"""
# =============================================================================
__author__  = 'Nathanael FARLEY nathanael.a.s.farley@cern.ch'
# previously 2010-08-03
__date__    = '2014-01-10'
# Not sure which versiont his refers to
__version__ = '$Revision: 154361 $'
# =============================================================================
__all__ = (
    'Bc2XKConf',
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

from Configurables import FilterDesktop, CombineParticles
from StrippingConf.StrippingLine import StrippingLine
from PhysSelPython.Wrappers import DataOnDemand, Selection

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
    ## cuts for kstar, phi and bachelor kaons
    # 
    'KstarCut'   : """
    ( PT          > 1000 * MeV               ) & 
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 5 * GeV  , P   , 500 * GeV ) &
    ( MIPCHI2DV()  > 9.    ) &
    ( CHILDCUT ( ( PT> 300*MeV) , 1 ) ) &
    ( CHILDCUT ( ( PT> 300*MeV) , 2 ) ) 
    """ ,
    #
        'PhiCut'   : """
    ( PT          > 750 * MeV               ) & 
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 1 * GeV  , P   , 500 * GeV ) &
    ( MIPCHI2DV()  > 9.    ) &
    ( CHILDCUT ( ( PT> 300*MeV) , 1 ) ) &
    ( CHILDCUT ( ( PT> 300*MeV) , 2 ) ) 
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
    ## useful shortcuts:
    #
    'Preambulo' : [
    ## shortcut for chi2 of vertex fit 
    'chi2vx = VFASPF(VCHI2)   '                                , 
    ## shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light"        , 
    ## use the embedded cut for chi2(LifetimeFit)<25
    #c_light was c_tau
    "ctau      = BPVLTIME ( 25 ) * c_light "                   , 
    "light_9    = BPVLTIME (  9 ) * c_light "                   ,
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
    'Bc2KstKPrescale'  : 1.0 ,
    'Bc2phiKPrescale'  : 1.0 ,
    # =========================================================================
    }

# =============================================================================
## @class  Bc2KstKConf
#  @author Nathanael FARLEY nathanael.a.s.farley@cern.ch
#  @date 2014-01-10
class Bc2XKConf(LineBuilder) :
    """
    Helper class to configure 'Bc->X K'-lines
    """
    __configuration_keys__ = tuple ( _default_configuration_.keys() )
    
    ## get the default configuration 
    #@staticmethod
    #def defaultConfiguration( key = None ) :
    #    """
    #    Get the default/recommended configurtaion
    #    
    #    >>> conf = Bc2KstK.defaultConfiguration()
    #    
    #    """
    #    from copy import deepcopy
    #    _config = deepcopy ( _default_configuration_ )
    #    if key : return _config[ key ]
    #    return _config
    
    ## constructor
    def __init__ ( self , name , config ) :
        """
        Constructor
        """
        # check the names 
        if 'Bc2XK' != name :
            logger.warning ( 'The non-default name is specified "%s"' % name  ) 
            
        from copy import deepcopy
        _config = deepcopy ( _default_configuration_ )

        if isinstance ( config , dict ):
            _config.update ( config )
            LineBuilder.__init__( self , name , _config )
        else :
            LineBuilder.__init__( self , name ,  config )

        # private set of selections 
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
                logger.debug ('new configuration: %-16s : %s ' \
                              % ( key , _config[key] ) )
                
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
        Get all lines
        """
        # TODO I suspect that docstring is no longer correct: fix it.
        sel = self._selection ( 'Bc2KstK_Lines' )
        if sel : return sel
        #
        sel = [
            #
            StrippingLine (
            "Bc2KstK"    + self.name()              ,
            prescale        = self ['Bc2KstKPrescale'] , 
            checkPV         = self ['CheckPV']         ,
            FILTER          = self ['FILTER' ]         ,
            ODIN            = self ['ODIN'   ]         ,
            L0DU            = self ['L0DU'   ]         ,
            HLT             = self ['L0DU'   ]         ,
            algos           = [ self.Bc2KstK ()   ]  ) ,

            StrippingLine (
            "Bc2phiK"    + self.name()              ,
            prescale        = self ['Bc2phiKPrescale'] , 
            checkPV         = self ['CheckPV']         ,
            FILTER          = self ['FILTER' ]         ,
            ODIN            = self ['ODIN'   ]         ,
            L0DU            = self ['L0DU'   ]         ,
            HLT             = self ['L0DU'   ]         ,
            algos           = [ self.Bc2phiK ()   ]  ) ,
            ]
        ## 
        return self._add_selection ( 'Bc2KstK_Lines' , sel ) 

    ############################################################################
    # As the following functions are not called, there is no need to comment
    # them out. They could be useful to build my own functions
    ############################################################################
    
    ## kaons :
    def kaons     ( self ) :
        """
        Kaons for   Bc -> X K lines 
        """
        sel = self._selection ( 'Kaon_Selection')
        if sel : return sel
        
        alg  = FilterDesktop (
            "kaonFilter",
            Preambulo = self['Preambulo'] ,
            ##
            Code = self['KaonCut'] ,
            ##
            )
        
        from StandardParticles       import StdAllNoPIDsKaons as input_kaons 
        sel  = Selection (
            "SelKFor"      + self.name()       ,
            Algorithm          =   alg         ,
            RequiredSelections = [ input_kaons ]  
            )
        
        return self._add_selection( 'Kaon_Selection' , sel ) 

    ## kstar :
    def kstar     ( self ) :
        """
        Kstar for   Bc -> Bc2KstK line 
        """
        # If the dictionary entry for 'Kstar_Selection' exists, don't bother
        # executing the body of function
        sel = self._selection ( 'Kstar_Selection')
        if sel : return sel

        alg  = FilterDesktop (
            "kstarFilter",
            Preambulo = self['Preambulo'] ,
            ##
            Code = self['KstarCut'] ,
            ##
            )
        
        input_kstar = DataOnDemand(Location =
                                   'Phys/StdLooseKstar2Kpi/Particles')
        
        sel  = Selection (
            "SelKstarFor"         + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ input_kstar ]  
            )
        
        return self._add_selection( 'Kstar_Selection' , sel )

        ## pions :
    def phi     ( self ) :
        """
        Phi for   Bc -> Phi K line 
        """
        # If the dictionary entry for 'Phi_Selection' exists, don't bother
        # executing the body of function
        sel = self._selection ( 'Phi_Selection')
        if sel : return sel

        alg  = FilterDesktop (
            "phiFilter",
            Preambulo = self['Preambulo'] ,
            ##
            Code = self['PhiCut'] ,
            ##
            )
        
        input_phi=DataOnDemand(Location='Phys/StdLoosePhi2KK/Particles')

        sel  = Selection (
            "SelPhiFor"         + self.name()     ,
            Algorithm          =   alg           ,
            RequiredSelections = [ input_phi ]  
            )
        
        return self._add_selection( 'Phi_Selection' , sel )
    
    # Bc -> Kst K
    def Bc2KstK ( self ) :
        """
        Bc -> K*(892)0 K+
        """
        sel = self._selection ( 'Bc2KstK_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            "bc2kstkComb",
            DecayDescriptor = "[B_c+ -> K*(892)~0 K+]cc" ,
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
        
        sel  = Selection (
            "SelBc2kstkFor"     + self.name()       ,
            Algorithm          =   alg             ,
            RequiredSelections = [ self.kaons   () ,
                                   self.kstar   () ] 
            )
        
        return self._add_selection( 'Bc2KstK_Selection' , sel ) 

    def Bc2phiK ( self ) :
        """
        Bc -> phi K+
        """
        sel = self._selection ( 'Bc2phiK_Selection')
        if sel : return sel
        
        alg  = CombineParticles (
            "bc2phiKComb",
            DecayDescriptor = "[B_c+ -> phi(1020) K+]cc" ,
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
        
        sel  = Selection (
            "SelBc2phiKFor"     + self.name()       ,
            Algorithm          =   alg             ,
            RequiredSelections = [ self.kaons   () ,
                                   self.phi     () ] 
            )
        
        return self._add_selection( 'Bc2phiK_Selection' , sel ) 
    
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
    _conf = Bc2KstKConf ( 'Bc2KstK' , config = {}  )
    for l in _conf.lines() :
        print ' \t ', l.outputLocation  () , l
    print 80*'*'
    print 80*'*'
        
# =============================================================================
# The END 
# =============================================================================
