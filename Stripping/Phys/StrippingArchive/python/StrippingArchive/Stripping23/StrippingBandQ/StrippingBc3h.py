#!/usr/bin/env python
# =============================================================================
# $Id:$
# =============================================================================
## @file
#  A bit naive way to run stripping for Bc -> 3h
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-08-03
#                    $Revision:$
#  Last modifictaion $Date:$
#                 by $Author:$
# =============================================================================
""" A bit naive attempt to make  Bc->3h stripping

- Bc -> pi+ pi+ pi-
- Bc -> K+  K+  K-
- Bc -> K+  K-  pi+
- Bc -> K+  K+  pi-
- Bc -> p   p~  K+
- Bc -> p   p~  pi+

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = '2010-08-03'
__version__ = '$Revision: 154361 $'
# =============================================================================
__all__ = (
    'Bc3hConf'       , ## the builder 
    'default_config' , ## configuration 
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
## Define the default configuration
# =============================================================================
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
    'HLT1'      : None ,   ## HLT  filter
    'HLT2'      : None ,   ## HLT  filter
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
    ( TRGHOSTPROB < 0.4    ) &
    ( PT          > 500 * MeV               ) & 
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                  &
    ( PROBNNpi     > 0.15  ) &
    ( MIPCHI2DV()  > 9.    )
    """ ,
    #
    'KaonCut'   : """
    ( CLONEDIST   > 5000   ) & 
    ( TRGHOSTPROB < 0.4    ) & 
    ( PT          > 500 * MeV               ) & 
    in_range ( 2          , ETA , 4.9       ) &
    in_range ( 3.2 * GeV  , P   , 150 * GeV ) &
    HASRICH                  &
    ( PROBNNk      > 0.20  ) &
    ( MIPCHI2DV()  > 9.    ) 
    """ ,
    #
    'ProtonCut'   : """
    ( CLONEDIST   > 5000    ) & 
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
## the mandatory element for stripping framework 
default_config = {
    'NAME'        :   'Bc3h'                 ,
    'WGs'         : [ 'BandQ' ]              ,
    'BUILDERTYPE' :   'Bc3hConf'             ,
    'CONFIG'      : _default_configuration_  , 
    'STREAMS'     : { 'Bhadron' : [ 'StrippingBc3piForBc3h'   ,
                                    'StrippingBc3kForBc3h'    ,
                                    'StrippingBc3kpiForBc3h'  , 
                                    'StrippingBc3pppiForBc3h' , 
                                    'StrippingBc3ppkForBc3h'  ] } 
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
    def defaultConfiguration ( key = None ) :
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
                logger.debug ('new configuration: %-16s : %s ' % ( key , _config[key] ) )
                
        self._name         = name
        
        for line in self._lines_Bc () :
            self.registerLine(line)
            logger.debug ( "Register line: %s" %  line.name () ) 
            
            
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
        if not kwargs.has_key ( 'Preambulo'         ) :
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
            HLT1            = self ['HLT1'   ]         ,
            HLT2            = self ['HLT2'   ]         ,
            algos           = [ self.Bc3pppi ()   ]  ) ,
            #
            StrippingLine (
            "Bc3ppkFor"     + self.name()              ,
            prescale        = self ['Bc3ppkPrescale']  , 
            checkPV         = self ['CheckPV']         ,
            FILTER          = self ['FILTER' ]         ,
            ODIN            = self ['ODIN'   ]         ,
            L0DU            = self ['L0DU'   ]         ,
            HLT1            = self ['HLT1'   ]         ,
            HLT2            = self ['HLT2'   ]         ,
            algos           = [ self.Bc3ppk ()   ]  )  ,
            #
            StrippingLine (
            "Bc3piFor"      + self.name()              ,
            prescale        = self ['Bc3piPrescale'] , 
            checkPV         = self ['CheckPV']         ,
            FILTER          = self ['FILTER' ]         ,
            ODIN            = self ['ODIN'   ]         ,
            L0DU            = self ['L0DU'   ]         ,
            HLT1            = self ['HLT1'   ]         ,
            HLT2            = self ['HLT2'   ]         ,
            algos           = [ self.Bc3pi ()      ]  ) ,
            ## 
            StrippingLine (
            "Bc3kFor"      + self.name()              ,
            prescale        = self ['Bc3kPrescale'] , 
            checkPV         = self ['CheckPV']         ,
            FILTER          = self ['FILTER' ]         ,
            ODIN            = self ['ODIN'   ]         ,
            L0DU            = self ['L0DU'   ]         ,
            HLT1            = self ['HLT1'   ]         ,
            HLT2            = self ['HLT2'   ]         ,
            algos           = [ self.Bc3k ()      ]  ) ,
            #
            StrippingLine (
            "Bc3kpiFor"     + self.name()              ,
            prescale        = self ['Bc3kpiPrescale']  , 
            checkPV         = self ['CheckPV']         ,
            FILTER          = self ['FILTER' ]         ,
            ODIN            = self ['ODIN'   ]         ,
            L0DU            = self ['L0DU'   ]         ,
            HLT1            = self ['HLT1'   ]         ,
            HLT2            = self ['HLT2'   ]         ,
            algos           = [ self.Bc3kpi ()    ]  ) ,
            ]
        ## 
        return self._add_selection ( 'Bc3h_Lines' , sel ) 
    
    ## pions :
    def pions     ( self ) :
        """ Pions for   Bc -> 3h lines """
        ##
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from StandardParticles                     import StdAllLooseANNPions as inpts 
        return self.make_selection (
            'Pion'                 ,
            FilterDesktop          ,
            [ inpts ]              ,
            Code = self['PionCut'] ,
            )
    
    ## kaons :
    def kaons     ( self ) :
        """ Kaons for   Bc -> 3h lines """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from StandardParticles                     import StdAllLooseANNKaons as inpts  
        return self.make_selection (
            'Kaon'                 ,
            FilterDesktop          ,
            [ inpts ]              ,
            Code = self['KaonCut'] ,
            )
    
    ## protons :
    def protons    ( self ) :
        """ Protons for   Bc -> 3h  lines  """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from StandardParticles                     import StdAllLooseANNProtons as inpts 
        return self.make_selection (
            'Proton'                 ,
            FilterDesktop            ,
            [ inpts ]                ,
            Code = self['ProtonCut'] ,
            )
    
    # Bc -> 3 pi 
    def Bc3pi ( self ) :
        """ Bc -> 3 pi """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        return self.make_selection (
            'Bc3pi'               , ## selection name 
            DaVinci__N3BodyDecays , ## the algorithm type 
            [ self.pions() ]      , ## input selections 
            #
            ## algorithm configurtaion
            #
            DecayDescriptor  = "[B_c+ -> pi+ pi- pi+ ]cc" ,
            Combination12Cut = """ ( AM < 7 * GeV )
            & ( ACHI2DOCA(1,2) < 20 )  
            """  , 
            CombinationCut   = """ ( mbp_acut | mbc_acut )
            & ( ACHI2DOCA(1,3) < 20 ) 
            & ( ACHI2DOCA(2,3) < 20 ) 
            """ , 
            MotherCut        = """
            ( chi2vx    < 16            ) &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
            )
    
    # Bc -> 3 k 
    def Bc3k ( self ) :
        """ Bc -> 3 k """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        return self.make_selection (
            'Bc3k'                , ## selection name 
            DaVinci__N3BodyDecays , ## the algorithm type 
            [ self.kaons() ]      , ## input selections 
            #
            ## algorithm configurtaion
            #
            DecayDescriptor  = "[B_c+ -> K+ K- K+ ]cc" ,
            Combination12Cut = """ ( AM < 7 * GeV )
            & ( ACHI2DOCA(1,2) < 20 )  
            """  , 
            CombinationCut   = """ ( mbp_acut | mbc_acut )
            & ( ACHI2DOCA(1,3) < 20 ) 
            & ( ACHI2DOCA(2,3) < 20 ) 
            """ , 
            MotherCut        = """
            ( chi2vx    < 16            ) &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
            )

    # Bc -> 3kpi 
    def Bc3kpi ( self ) :
        """ Bc -> 3 k pi """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        return self.make_selection (
            'Bc3kpi'                         , ## selection name 
            DaVinci__N3BodyDecays            , ## the algorithm type 
            [ self.kaons() , self.pions() ]  , ## input selections 
            #
            ## algorithm configurtaion
            #
            DecayDescriptors = [
            "[B_c+ -> K+  K-  pi+ ]cc" ,
            "[B_c+ -> K+  pi- pi+ ]cc" ,
            ] ,
            Combination12Cut = """ ( AM < 7 * GeV )
            & ( ACHI2DOCA(1,2) < 20 )  
            """  , 
            CombinationCut   = """ ( mbp_acut | mbc_acut )
            & ( ACHI2DOCA(1,3) < 20 ) 
            & ( ACHI2DOCA(2,3) < 20 ) 
            """ , 
            MotherCut        = """
            ( chi2vx    < 16            ) &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
            )
    
    # Bc -> pppi
    def Bc3pppi ( self ) :
        """ Bc -> pppi"""
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        return self.make_selection (
            'Bc3pppi'                         , ## selection name 
            DaVinci__N3BodyDecays             , ## the algorithm type 
            [ self.protons() , self.pions() ] , ## input selections 
            #
            ## algorithm configurtaion
            #
            DecayDescriptor  = "[B_c+ -> p+  p~- pi+ ]cc" ,
            Combination12Cut = """ ( AM < 7 * GeV )
            & ( ACHI2DOCA(1,2) < 20 )  
            """  , 
            CombinationCut   = """ ( mbp_acut | mbc_acut )
            & ( ACHI2DOCA(1,3) < 20 ) 
            & ( ACHI2DOCA(2,3) < 20 ) 
            """ , 
            MotherCut        = """
            ( chi2vx    < 16            ) &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
            )
    
    # Bc -> ppK
    def Bc3ppk ( self ) :
        """
        Bc -> ppK
        """
        from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays 
        return self.make_selection (
            'Bc3ppk'                          , ## selection name 
            DaVinci__N3BodyDecays             , ## the algorithm type 
            [ self.protons() , self.kaons() ] , ## input selections 
            #
            ## algorithm configurtaion
            #
            DecayDescriptor  = "[B_c+ -> p+  p~- K+ ]cc" ,
            Combination12Cut = """ ( AM < 7 * GeV )
            & ( ACHI2DOCA(1,2) < 20 )  
            """  , 
            CombinationCut   = """ ( mbp_acut | mbc_acut )
            & ( ACHI2DOCA(1,3) < 20 ) 
            & ( ACHI2DOCA(2,3) < 20 ) 
            """ , 
            MotherCut        = """
            ( chi2vx    < 16            ) &
            ( ( mbp_cut & ( ctau > %s ) ) | ( mbc_cut & ( ctau > %s ) ) ) 
            """ % ( self['CTAU'] , self['CTAU_BC'] ) 
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
    _conf = Bc3hConf ( 'Bc3h' , config = default_config['CONFIG'] )
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
