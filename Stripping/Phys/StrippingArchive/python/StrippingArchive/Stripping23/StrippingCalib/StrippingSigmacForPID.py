# =============================================================================
__author__  = 'Laurence Carson'
__date__    = '2013-03-24'
# =============================================================================
__all__ = (
    'SigmacForPIDConf',
    'default_config'
    )
# =============================================================================
"""
Stripping selection for Sigma_c -> Lambda_c(->p K pi)pi, without PID cuts
on the proton from Lambda_c. Adapted from SigmaCForPromptCharm line in
StrippingPromptCharm.py.
"""
from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits             import GeV, MeV, mm
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import   Selection, MergedSelection
from StandardParticles      import ( StdNoPIDsPions     ,
                                     StdLoosePions      ,
                                     StdLooseKaons      ,
                                     StdNoPIDsProtons    ,  
                                     ## for soft pion from Sigma_c
                                     StdAllNoPIDsPions  
                                    )


from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

# =============================================================================
## Define the default configuration
_default_configuration_ = {
    #
    #
    ## PT-cuts
    #
    'pT(Lc+)'          :  3.0 * GeV ,    ## pt-cut for  prompt   Lc+
    #
    # Selection of basic particles
    #
    'TrackCuts'       : """
    ( TRCHI2DOF < 4     ) &
    ( PT > 250 * MeV    ) &
    ( TRGHOSTPROB < 0.5 ) & 
    in_range  ( 2 , ETA , 5 )
    """ ,
    'BasicCuts'       : ' & ( 9 < MIPCHI2DV() ) ' ,
    'KaonCuts'        : ' & ( 2 < PIDK  - PIDpi ) & ( -5 < PIDK  - PIDp ) '   ,
    'PionCuts'        : ' & ( 2 < PIDpi - PIDK  ) & ( -5 < PIDpi - PIDp  ) '   ,
    'SlowPionCuts'    : ' TRCHI2DOF < 5 '                                     ,
    #
    # Global Event cuts
    #
    'CheckPV'         : True ,
    #
    # Technicalities:
    #
    'Preambulo'       : [
    # shortcut for chi2 of vertex fit
    'chi2vx = VFASPF(VCHI2) '                    ,
    # shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light" ,
    "ctau_9   = BPVLTIME (   9 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<9
    ] ,
    ## monitoring ?
    'Monitor'     : False ,
    ## prescale
    'SigmaCPrescale'         : 1.0 ,  
    ## ========================================================================
    }

default_config = {
  'NAME' : 'SigmacForPID',
  'BUILDERTYPE' : 'SigmacForPIDConf',
  'CONFIG'      : _default_configuration_  ,
  'STREAMS' : [ 'PID' ],
  'WGs' : [ 'ALL' ]
}


# =============================================================================
# use a bit faster fitter 
FITTER = 'LoKi::VertexFitter:PUBLIC'
## ============================================================================
class SigmacForPIDConf(LineBuilder) :
    """
    Helper class to configure the line
    """
    __configuration_keys__ = tuple ( _default_configuration_.keys() )


    ## constructor
    def __init__ ( self , name , config ) :
        """
        Constructor
        """
        # check the names
        if not name : name = 'PID'

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
                raise KeyError("Invalid key is specified: '%s'" % key )

        for line in self._lines_PID() :
            self.registerLine(line)

    ## get the selections
    def _selections_private ( self ) :

        sel = self._selections ( 'Selections' )
        if sel : return sel

        sel =  [ self.LamC           () ,
                 self.SigC           () ,
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


    ## get the lines
    def _lines_PID   ( self ) :
        """
        Get the lines
        """
        sel = self._selection ( 'PIDLines' )
        if sel : return sel
        #
        sel = [
            StrippingLine (
            self.name() + "Line" ,
            prescale = self['SigmaCPrescale'] , 
            checkPV  = self['CheckPV'       ] ,
            algos    =     [ self.SigC ()   ]
            )          
            ]
        #
        return self._add_selection ( 'PIDLines' , sel )


    ## get all stripping lines
    def _lines_private ( self ) :
        """
        get the lines
        """
        return self._lines_PID ()

    def trackCuts    ( self ) : return                    self [ 'TrackCuts'    ]
    def basicCuts    ( self ) : return self.trackCuts() + self [ 'BasicCuts'    ]
    def kaonCuts     ( self ) : return self.basicCuts() + self [ 'KaonCuts'     ]
    def pionCuts     ( self ) : return self.basicCuts() + self [ 'PionCuts'     ]
    def protonCuts   ( self ) : return self.basicCuts()
    def slowPionCuts ( self ) : return                    self [ 'SlowPionCuts' ]
  
    ## get the selection of kaons
    def kaons ( self ) :
        """
        Get the kaons
        """
        sel = self._selection ( 'KaonSelection')
        if sel : return sel

        _KaonFilter = FilterDesktop ( Code = """
        ( 'K+'  == ABSID ) & ( %s )
        """ % self.kaonCuts() )
        #
        sel = Selection (
            'SelKaonsFor'      + self.name() ,
            Algorithm          = _KaonFilter ,
            RequiredSelections = [ StdLooseKaons ]
            )

        return self._add_selection ( 'KaonSelection' , sel )

    ## get the pion selection
    def pions ( self ) :
        """
        Get the pions
        """
        sel = self._selection ( 'PionSelection')
        if sel : return sel

        _PionFilter = FilterDesktop (
            Code = """
            ( 'pi+' == ABSID ) & ( %s )
            """ % self.pionCuts() )
        #
        sel = Selection (
            'SelPionsFor'      + self.name() ,
            Algorithm          = _PionFilter ,
            RequiredSelections = [ StdLoosePions ]
            )
        #
        return self._add_selection ( 'PionSelection' , sel )


    ## get the proton selection
    def protons ( self ) :
        """
        Get the proton selection
        """
        sel = self._selection ( 'ProtonSelection')
        if sel : return sel

        _ProtonFilter = FilterDesktop (
            Code = """
            ( 'p+'  == ABSID ) & ( %s )
            """ % self.protonCuts() )

        sel = Selection (
            'SelProtonsFor' + self.name() ,
            Algorithm          = _ProtonFilter ,
            RequiredSelections = [ StdNoPIDsProtons ]
            )
        #
        return self._add_selection ( 'ProtonSelection' , sel )

    ## get the meson selection
    def mesons ( self ) :
        """
        Get the meson ( kaon + pion ) selection
        """
        sel = self._selection ( 'MesonSelection')
        if sel : return sel

        #
        sel = MergedSelection (
            'SelBasicMesonsFor' + self.name() ,
            RequiredSelections = [ self.pions() , self.kaons() ]
            )
        #
        return self._add_selection ( 'MesonSelection' , sel )

    ## get the common preambulo:
    def preambulo ( self ) : return self['Preambulo']

  
    # =============================================================================
    # Lambda_C -> ( pKpi )  selection
    # =============================================================================
    def LamC ( self ) :

        sel = self._selection ( 'LambdaCForPID_Selection' )
        if sel : return sel

        cmb = CombineParticles(
            ##
            Monitor      = self['Monitor'] ,
            HistoProduce = self['Monitor'] ,
            ##
            DecayDescriptors = [
            " [ Lambda_c+ -> p+  K-  pi+ ]cc" ,
            ] ,
            ##
            Preambulo = self.preambulo() + [
            "hpKpi = Gaudi.Histo1DDef ( 'mass p K pi' , 2240 , 2340 , 60 )"
            ] ,
            ##
            CombinationCut = """
            ( ADAMASS('Lambda_c+') < 55 * MeV ) &
            ( APT > %s )
            """ % ( 0.95 * self['pT(Lc+)' ] ) ,
            ##
            MotherCut      = """
            ( chi2vx  < 25 )                   &
            ( PT      > %s                   ) &
            ( ADMASS('Lambda_c+') < 40 * MeV ) &
            ( ctau_9 > 100 * micrometer      )
            """ %  self [ 'pT(Lc+)' ] , 
            ##
            MotherMonitor  = """
            process ( monitor ( M , hpKpi , 'mass p K pi ' ) ) >> ~EMPTY
            """ , 
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } ,
            #
            )

        # convert it to selection
        sel = Selection (
            "SelLambdaCFor" + self.name()       ,
            Algorithm          = cmb               ,
            RequiredSelections = [ self.mesons  () ,
                                   self.protons () ]
            )

        return self._add_selection( 'LambdaCForPID_Selection' ,  sel )

       
    # =============================================================================
    # Sigma_C -> Lambda_C pi selection
    # =============================================================================
    def SigC ( self ) :

        sel = self._selection ( 'SigmaCForPID_Selection' )
        if sel : return sel

        cmb = CombineParticles(
            ##
            Monitor      = self['Monitor'] ,
            HistoProduce = self['Monitor'] ,
            ##
            DecayDescriptors = [
            " [ Sigma_c0  -> Lambda_c+ pi- ]cc" ,
            " [ Sigma_c++ -> Lambda_c+ pi+ ]cc" ,
            ] ,
            ##
            DaughtersCuts = {
            #'Lambda_c+' : ' INTREE ( good_proton ) ' , 
            'pi+'       :  self.slowPionCuts()
            } ,
            ##
            Preambulo = self.preambulo() + [
            "hDmSigC = Gaudi.Histo1DDef ( 'dm(Sigma_c)' , 100 , 270 , 150 )"
            ] ,
            ##
            CombinationCut = """
            ( AM - AM1 < 270 * MeV )
            """ ,
            ##
            MotherCut      = """
            ( chi2vx  < 16 )
            """ ,
            ##
            MotherMonitor  = """
            process ( monitor ( M-M1 , hDmSigC , 'dm(Sigma_c)' ) ) >> ~EMPTY
            """ ,
            #
            ## make the selection faster
            #
            ParticleCombiners = {'' : FITTER } ,
            #
            )

        # convert it to selection
        sel = Selection (
            "SelSigmaCFor"     +   self.name()       ,
            Algorithm          =   cmb               ,
            RequiredSelections = [ self.LamC    ()   ,
                                   StdAllNoPIDsPions ] ## slow prompt pion!
            )

        return self._add_selection( 'SigmaCForPID_Selection' ,  sel )
 
  
# =============================================================================
# The END
# =============================================================================

