#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
## @file
# 
#  The attempt for stripping of associative ``onium'' production
#
#    - dimuon + dimuon 
#    - dimuon + high-pt gamma
#
#   Parasitic:
#
#    - dimuon + ( dimuon + gamma ) [ mimic   Chi_(b,c) + dimuon  ] 
#    -       2x ( dimuon + gamma ) [ mimic 2xChi_(b,c)           ] 
#
#   Accociative W+ production:
#    - dimuon             & W+  
#    - ( dimuon + gamma ) & W+      [ mimic Chi_(b,c)] 
#
#
# +-------------------------------------+--------+------+-------+
# | StrippingReport   INFO Event 276300, Good event 276300      |
# +-------------------------------------+--------+------+-------+
# |    Decision name                    | Rate,% | Acc. | ms/evt|
# +-------------------------------------+--------+------+-------+
# |                     double charm                            |
# +-------------------------------------+--------+------+-------+
# | <>DiMuonAndGammaForCharmAssociative | 0.0467 | 129  | 0.668 |
# | <>DoubleDiMuonForCharmAssociative   | 0.0405 | 112  | 0.037 |
# | <>ChiAndDiMuonForCharmAssociative   | 0.0405 | 112  | 0.037 |
# | <>DiChiForCharmAssociative          | 0.0405 | 112  | 0.037 |
# +-------------------------------------+--------+------+-------+
# |                     charm & W+                              |
# +-------------------------------------+--------+------+-------+
# | <>DiMuonAndWForCharmAssociative     | 0.0076 |  21  | 0.060 |
# | <>ChiAndWForCharmAssociative        | 0.0054 |  15  | 0.033 |
# +-------------------------------------+--------+------+-------+
#
# @author Vanya BELYAEV Ivan.Belyaev@cern.ch
# @date   2011-05-26
# 
#                   $Revision$
# Last modification $Date: 2013-01-05 21:21:10 +0100
#                by $Author$
# =============================================================================
"""
   The attempt for stripping of associative ``onium'' production
 
     - dimuon + dimuon 
     - dimuon + high-pt gamma
 
    Parasitic:
 
     - dimuon + ( dimuon + gamma ) [ mimic   Chi_(b,c) + dimuon  ] 
     -       2x ( dimuon + gamma ) [ mimic 2xChi_(b,c)           ] 
 
    Accociative W+ production:
     - dimuon             & W+  
     - ( dimuon + gamma ) & W+      [ mimic Chi_(b,c)] 
 
 
  +-------------------------------------+--------+------+-------+
  | StrippingReport   INFO Event 276300, Good event 276300      |
  +-------------------------------------+--------+------+-------+
  |    Decision name                    | Rate,% | Acc. | ms/evt|
  +-------------------------------------+--------+------+-------+
  |                     double charm                            |
  +-------------------------------------+--------+------+-------+
  | <>DiMuonAndGammaForCharmAssociative | 0.0467 | 129  | 0.668 |
  | <>DoubleDiMuonForCharmAssociative   | 0.0405 | 112  | 0.037 |
  | <>ChiAndDiMuonForCharmAssociative   | 0.0405 | 112  | 0.037 |
  | <>DiChiForCharmAssociative          | 0.0405 | 112  | 0.037 |
  +-------------------------------------+--------+------+-------+
  |                     charm & W+                              |
  +-------------------------------------+--------+------+-------+
  | <>DiMuonAndWForCharmAssociative     | 0.0076 |  21  | 0.060 |
  | <>ChiAndWForCharmAssociative        | 0.0054 |  15  | 0.033 |
  +-------------------------------------+--------+------+-------+

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = '2011-05-26'
__version__ = '$Revision$'
# =============================================================================
__all__ = (
    'StrippingCharmAssociativeConf',
    'default_config'
    )
# =============================================================================
from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits             import GeV, MeV, mm

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
#
from PhysSelPython.Wrappers import   Selection

#
# Attention: we need prompt onia, thus "All" Loose muons here 
from StandardParticles      import ( StdAllLooseMuons   , ## PROMPT muons!
                                     StdLooseAllPhotons ) 

from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

# =============================================================================
## logging 
# =============================================================================
import logging
logger = logging.getLogger(__name__)
if not logger.handlers :
    logging.basicConfig()
logger.setLevel(logging.INFO)
# =============================================================================
## Define the default configuration 
_default_configuration_ = {
    #
    ## Selection of basic particles 
    #
    'PhotonCuts'      : ' PT > 4.0 * GeV  '                                 , 
    'MuonCuts'        : ' ISMUON & ( PT > 650 * MeV ) & ( TRCHI2DOF < 5 ) ' ,
    #
    ## photons from chi_(c,b)
    #
    'GammaChi'        : ' ( PT > 450 * MeV ) & ( CL > 0.05 ) ' , 
    #
    ## W+- selection
    #
    'WCuts'           : " ( 'mu+'== ABSID ) & ( PT > 15 * GeV )" ,
    #
    ## Global Event cuts 
    #
    'CheckPV'         : True , 
    #
    ## Technicalities:
    #
    'Preambulo'       : [
    #
    ## shortcut for chi2 of vertex fit 
    'chi2vx = VFASPF(VCHI2) '                    ,
    #
    ## shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light" , 
    #
    ## dimuons:
    "psi             = ADAMASS ( 'J/psi(1S)'  ) < 125 * MeV"         ,
    "psi_prime       = ADAMASS (   'psi(2S)'  ) < 125 * MeV"         ,
    "mu2_tight       = ( chi2vx < 10    ) & ( MINTREE ( 'mu+' == ABSID , PT ) > 900 * MeV ) " ,
    "dimu_tight      = ( PT > 3.0 * GeV ) & mu2_tight " ,
    "psi_tight       = ( ADMASS ( 'J/psi(1S)' ) < 100 * MeV ) & dimu_tight " ,
    "psi_prime_tight = ( ADMASS (   'psi(2S)' ) < 100 * MeV ) & dimu_tight " ,
    "dimuon_heavy    = ( M > 4.9 * GeV ) & dimu_tight "                      ,
    "dimuon_tight    = psi_tight | psi_prime_tight | dimuon_heavy " ,
    ] ,
    #
    ## monitoring ?
    'Monitor'     : False ,
    #
    ## pescales 
    'DiMuonAndGammaPrescale' : 1.0 ,
    'DoubleDiMuonPrescale'   : 1.0 ,
    'ChiAndDiMuonPrescale'   : 1.0 ,
    'DiChiPrescale'          : 1.0 ,
    'DiMuonAndWPrescale'     : 1.0 , 
    'ChiAndWPrescale'        : 1.0
    # =========================================================================
    }
# =============================================================================
## @class  StrippingCharmAssociativeConf
#  Helper class required by Tom & Greig 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date 2011-05-26
class StrippingCharmAssociativeConf(LineBuilder) :
    """
    Helper class to configure 'CharmAssociative/PromptCharm'-lines
    """
    __configuration_keys__ = tuple ( _default_configuration_.keys() ) 
    
    ## private set of selections 
    __selections_ = {}
    
    ## get the default configuration 
    @staticmethod
    def defaultConfiguration( key = None ) :
        """
        Get the default configurtaion
        
        >>> conf = StrippingCharmAssociativeConf.defaultConfiguration()
        
        Get the elements of default configurtaion:
        
        >>> prescale = StrippingCharmAssociativeConf.defaultConfiguration( 'DoubleDiMuon' )
        """
        from copy import deepcopy
        _config = deepcopy ( _default_configuration_ )
        if key : return _config[ key ]
        return _config
    
    ## constructor
    def __init__( self , name , config ) :
        """
        Constructor
        """
        # check the names 
        if not name : name = 'CharmAssociative'
        # check the names 
        if 'CharmAssociative' != name :
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

        keys = _config.keys()
        for key in keys :
            
            if not key in _default_configuration_ :
                raise KeyError("Invalid key is specified: '%s'" % key )
            
            val = _config[key]
            if val != _default_configuration_ [ key ] : 
                logger.warning ('new configuration: %-16s : %s ' % ( key , _config[key] ) )

        ## cehck for prescales 
        for keys in self.keys() :
            if 0 > key.find('Prescale') and 0 > key.find('prescale') :  continue
            if 1 != self[key] : logger.warning ( '%s is %s' % ( key , self[key] ) ) 
            

        for line in self._lines_associative_onia () :
            self.registerLine(line)
            logger.debug ( "Register line: %s" %  line.name () ) 
                        

    ## get the common preambulo: 
    def preambulo    ( self ) : return self [ 'Preambulo' ] 

    ## get photon cuts 
    def photonCuts   ( self ) : return self [ 'PhotonCuts' ] 
    
    ## get muon cuts 
    def muonCuts     ( self ) : return self [ 'MuonCuts' ] 

    ## get the dimuons 
    def DiMuon ( self ) :
        """
        Get the dimuons  
        """
        sel = self._selection ( 'DiMuon_Selection' )
        if sel : return sel 
        
        ## prepare Di-muons
        _Combine = CombineParticles (
            #
            ## the decays to be reconstructed
            DecayDescriptor = 'J/psi(1S) -> mu+ mu-' ,
            DaughtersCuts   = {
            'mu+' : self.muonCuts() 
            } , 
            Preambulo       = self.preambulo() ,
            #
            ## combination cut 
            CombinationCut  = """
            psi            |
            psi_prime      |
            ( 8 * GeV < AM )
            """ , 
            #
            ##      mother cut 
            MotherCut       = " chi2vx < 20 " 
            )
        ##
        sel = Selection (
            ##
            'SelDiMuonFor' + self._name ,
            ##
            Algorithm          = _Combine   ,
            #
            ## ATTENITON: we need PROMPT onia, thus 'AllMuons' are here
            RequiredSelections = [ StdAllLooseMuons ]  ## we need 'All' muons 
            )
        
        return self._add_selection ( 'DiMuon_Selection' , sel )
    
    ## get the dimuons & charn 
    def DiMuonAndGamma ( self ) :
        """
        get dimuon & gamma 
        Select events with dimuon and high-pt photon 
        at least one dimuon 
        """
        sel = self._selection ( 'DiMuonAndGamma_Selection' )
        if sel : return sel 
        
        _Filter = CombineParticles (
            ## 
            DecayDescriptors = [
            "chi_b2(1P)  -> J/psi(1S) gamma" ,
            ] ,
            ##
            Preambulo = self.preambulo() ,
            ## 
            DaughtersCuts   = {
            'J/psi(1S)' : " dimuon_tight " ,  
            'gamma'     :  self.photonCuts ()
            } ,
            ## 
            CombinationCut = " AALL "  ,
            MotherCut      = "  ALL " 
            )
        
        sel = Selection  (
            "SelDiMuonAndGammaFor" + self._name  ,
            Algorithm = _Filter ,
            RequiredSelections = [
            self.DiMuon      () ,
            StdLooseAllPhotons  , 
            ]
            )
        
        return self._add_selection ( 'DiMuonAndGamma_Selection' , sel ) 

    
    ## get the double dimuons
    def DoubleDiMuon ( self ) :
        """
        get 2xdimuon
        Select events with two dimuons
        at least one dimuon 
        """
        sel = self._selection ( 'DoubleDiMuon_Selection' )
        if sel : return sel 
        
        _alg = CombineParticles (
            #
            DecayDescriptor  = "chi_b2(1P)  -> J/psi(1S) J/psi(1S)" ,
            CombinationCut   = " AALL " ,
            MotherCut        = "  ALL "
            #
            )
        
        sel = Selection  (
            "SelDoubleDiMuonFor" + self._name  ,
            Algorithm          =   _alg ,
            RequiredSelections = [ self.DiMuon ()  ]
            )
        
        return self._add_selection ( 'DoubleDiMuon_Selection' , sel )    
    
    ## get chi_(c,b) & dimuon 
    def ChiAndDiMuon ( self ) : 
        """
        Construct Chi_(c,b) + dumuon line 
        """
        sel = self._selection ( 'ChiAndDiMuon_Selection' )
        if sel : return sel 
        
        _alg = CombineParticles (
            ##
            DecayDescriptor =  "chi_b2(1P)  -> J/psi(1S) J/psi(1S) gamma " ,
            ##
            DaughtersCuts = {
            'gamma'     :  self['GammaChi']
            } ,
            ## mimic chi_(c,b):
            CombinationCut   = """
            ( AM13 - AM1 < 1.05 * GeV ) |
            ( AM23 - AM2 < 1.05 * GeV ) 
            """ ,
            MotherCut        = "  ALL " 
            )
        
        _sel = Selection  (
            "SelPreChiAndDiMuonFor" + self._name  ,
            Algorithm          = _alg ,
            RequiredSelections = [ self.DoubleDiMuon () , ##fake! 
                                   self.DiMuon       () ,
                                   StdLooseAllPhotons   ] 
            )
        
        ## apply pi0-veto-tagger ! 
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger 
        _tag = Pi0Veto__Tagger (
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25011     ## unique ! 
            )
        
        ## the final selection 
        sel = Selection  (
            "SelChiAndDiMuonFor" + self._name  ,
            Algorithm          =   _tag  ,
            RequiredSelections = [ _sel ]
            )
        
        return self._add_selection ( 'ChiAndDiMuon_Selection' , sel )    


     ## get 2xXhi_(c,b) & dimuon 
    def DiChi ( self ) : 
        """
        Construct 2xChi_(c,b) line 
        """
        sel = self._selection ( 'DiChi_Selection' )
        if sel : return sel 
        
        _alg = CombineParticles (
            ##
            DecayDescriptor =  "chi_b2(1P)  -> J/psi(1S) J/psi(1S) gamma gamma" ,
            ##
            DaughtersCuts = {
            'gamma' :  self [ 'GammaChi' ]
            } ,
            ## mimic 2xchi_(c,b):
            CombinationCut   = """
            ( ( AM13 - AM1 < 1.05 * GeV ) & ( AM24 - AM2 < 1.05 * GeV ) ) | 
            ( ( AM14 - AM1 < 1.05 * GeV ) & ( AM23 - AM2 < 1.05 * GeV ) ) 
            """ ,
            MotherCut        = "  ALL " 
            )
        
        _sel = Selection  (
            "SelPreDiChiFor"     + self._name  ,
            Algorithm          = _alg ,
            RequiredSelections = [ self.ChiAndDiMuon () , ##fake! 
                                   self.DiMuon       () ,
                                   StdLooseAllPhotons   ] 
            )
        
        ## apply pi0-veto-tagger ! 
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger 
        _tag = Pi0Veto__Tagger (
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25012     ## unique ! 
            )
        
        sel = Selection  (
            "SelDiChiFor"     + self._name  ,
            Algorithm          =   _tag ,
            RequiredSelections = [ _sel ]
            )
        
        return self._add_selection ( 'DiChi_Selection' , sel )    

    ## W+- selection 
    def W ( self )  :
        """
        Get simple  W+-selection 
        """
        
        sel = self._selection ( 'W_Selection' )
        if sel : return sel 
        
        _Filter = FilterDesktop ( Code = self [ 'WCuts'] )
        
        sel = Selection  (
            "SelWFor" + self._name  ,
            Algorithm = _Filter ,
            RequiredSelections = [
            StdAllLooseMuons  ,     ## ATTENTION! prompt muons are here!
            ]
            )
        
        return self._add_selection ( 'W_Selection' , sel )    
    
    ## select dimuon + W 
    def DiMuonAndW ( self ) :
        """
        select dimuon + W+
        """
        sel = self._selection ( 'DiMuW_Selection' )
        if sel : return sel 
        
        ## almost fake selection 
        alg = CombineParticles (
            ##
            DecayDescriptor =  "[chi_b2(2P)  -> J/psi(1S) mu+]cc" ,
            ##
            ## combination cut : accept all 
            CombinationCut  = " AALL " ,
            ## mother      cut : accept all 
            MotherCut       = "  ALL " 
            )
        
        sel = Selection  (
            "SelDiMuAndWFor"   + self._name         ,
            Algorithm          = alg                ,
            RequiredSelections = [ self . W      () ,
                                   self . DiMuon () ] 
            )
        
        return self._add_selection ( 'DiMuW_Selection' , sel )    


    ## select chi + W 
    def ChiAndW ( self ) :
        """
        select dimuon + W+
        """
        sel = self._selection ( 'ChiW_Selection' )
        if sel : return sel 

        _alg = CombineParticles (
            ##
            DecayDescriptor =  "[chi_b2(2P)  -> J/psi(1S) gamma mu+]cc" ,
            ##
            DaughtersCuts = {
            "J/psi(1S)" : " ( M  < 3.21 * GeV ) | in_range ( 8.5 * GeV , M , 12.0 * GeV ) " ,  
            'gamma'     :  self['GammaChi']
            } ,
            ## mimic chi_(c,b):
            CombinationCut   = """
            ( AM12 - AM1 < 1.05 * GeV ) | 
            ( AM12 - AM1 < 1.05 * GeV )  
            """ ,
            MotherCut        = "  ALL " 
            )
        
        _sel = Selection  (
            "SelPreChiAndWFor" + self._name  ,
            Algorithm          = _alg        ,
            RequiredSelections = [ self . DiMuonAndW () , ## fake!
                                   self . W          () ,
                                   self . DiMuon     () ,
                                   StdLooseAllPhotons   ] 
            )
        
        ## apply pi0-veto-tagger ! 
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger 
        _tag = Pi0Veto__Tagger (
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25013     ## unique ! 
            )
        
        sel = Selection  (
            "SelChiAndWFor"    + self._name  ,
            Algorithm          =   _tag ,
            RequiredSelections = [ _sel ]
            )
        
        return self._add_selection ( 'ChiW_Selection' , sel )    


    ## get all dicharm lines 
    def _lines_associative_onia ( self ) :
        
        sel = self._selection ( 'OniaAndXLines' )
        if sel : return sel 
        
        sel = [
            ##
            StrippingLine (
            "DiMuonAndGammaFor" + self._name            ,
            prescale = self [ 'DiMuonAndGammaPrescale' ] , ## ATTENTION! Prescale here !!              
            checkPV  = self [ 'CheckPV']                 ,
            algos    = [ self . DiMuonAndGamma () ]            
            ) ,
            ##
            StrippingLine (
            "DoubleDiMuonFor" + self._name             ,
            prescale = self [ 'DoubleDiMuonPrescale' ] , ## ATTENTION! Prescale here !!              
            checkPV  = self [ 'CheckPV' ]              ,
            algos    = [ self . DoubleDiMuon () ]            
            ) ,
            ##
            StrippingLine (
            "ChiAndDiMuonFor" + self._name             ,
            prescale = self [ 'ChiAndDiMuonPrescale' ] , ## ATTENTION! Prescale here !!              
            checkPV  = self [ 'CheckPV' ]              ,
            algos    = [ self . ChiAndDiMuon () ]            
            ) ,
            ## 
            StrippingLine (
            "DiChiFor" + self._name             ,
            prescale = self [ 'DiChiPrescale' ] , ## ATTENTION! Prescale here !!              
            checkPV  = self [ 'CheckPV']        ,
            algos    = [ self . DiChi       () ]            
            ) ,
            ##
            StrippingLine (
            "DiMuonAndWFor" + self._name             ,
            prescale = self [ 'DiMuonAndWPrescale' ] , ## ATTENTION! Prescale here !!              
            checkPV  = self [ 'CheckPV'  ]           ,
            algos    = [ self . DiMuonAndW ()  ]             
            ) ,
            ##
            StrippingLine (
            "ChiAndWFor" + self._name             ,
            prescale = self [ 'ChiAndWPrescale' ] , ## ATTENTION! Prescale here !!              
            checkPV  = self [ 'CheckPV'  ]           ,
            algos    = [ self . ChiAndW ()  ]             
            ) ,
            ##
            ]
        #
        return self._add_selection ( 'OniaAndXLines' , sel ) 
    
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
        Add the selection, associated with some nickname name
        """
        if not self.__selections_.has_key ( self.name() ) :
            self.__selections_[ self.name() ] = {} 
        
        if self.__selections_[ self.name()].has_key( nick ) :
            raise AttributeError , "Selection '%s'already exists " % nick
        
        self.__selections_[ self.name() ][ nick ] = sel
        
        return sel 
    
    
# =============================================================================
default_config = {
    #
    'DiMuonAndGammaPrescale' : 1.0 ,
    'DoubleDiMuonPrescale'   : 1.0 ,
    'ChiAndDiMuonPrescale'   : 1.0 ,
    'DiChiPrescale'          : 1.0 ,
    'DiMuonAndWPrescale'     : 1.0 , 
    'ChiAndWPrescale'        : 1.0
    }    
# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author :  %s' % __author__
    print ' Date   :  %s' % __date__
    print ' The output locations for default configuration: '
    _conf = StrippingCharmAssociativeConf ( 'CharmAssociative' ,
                                            config = default_config  ) 
    for l in _conf.lines() :
        print ' \t ', l.outputLocation  () , l 

    print 80*'*'
        
# =============================================================================
# The END 
# =============================================================================

