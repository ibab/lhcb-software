#!/usr/bin/env python
# =============================================================================
# $Id: StrippingCharmAssociative.py 176885 2014-08-26 14:00:22Z ibelyaev $
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
#                   $Revision: 176885 $
# Last modification $Date: 2013-01-05 21:21:10 +0100
#                by $Author: ibelyaev $
# =============================================================================
"""The attempt for stripping of associative ``onium'' production
 
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
__version__ = '$Revision: 176885 $'
# =============================================================================
__all__ = (
    'StrippingCharmAssociativeConf',
    'default_config'
    )
# =============================================================================
from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits             import GeV, MeV, mm
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import   Selection
#
# Attention: we need prompt onia, thus "All" Loose muons here 
from StandardParticles      import ( StdAllLooseMuons   , ## PROMPT muons!
                                     StdLooseAllPhotons ) 
#
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder
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
    'PhotonCuts'      : ' PT > 3.0 * GeV  '                                 , 
    'MuonCuts'        : ' ISMUON & ( PT > 650 * MeV ) & ( TRCHI2DOF < 5 ) ' ,
    #
    ## photons from chi_(c,b)
    #
    'GammaChi'        : ' ( PT > 400 * MeV ) & ( CL > 0.05 ) ' , 
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
    "psi_tight       = ( ADMASS ( 'J/psi(1S)' ) < 110 * MeV ) & dimu_tight " ,
    "psi_prime_tight = ( ADMASS (   'psi(2S)' ) < 110 * MeV ) & dimu_tight " ,
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
## the mandatory element for stripping framework 
default_config = {
    #
    'NAME'        :   'CharmAssociative'     ,
    'WGs'         : [ 'BandQ' ]              ,
    'CONFIG'      : _default_configuration_  , 
    'BUILDERTYPE' :   'StrippingCharmAssociativeConf'            ,
    'STREAMS'     : { 'Leptonic' : [ 'StrippingDiMuonAndGammaForCharmAssociative', 
                                     'StrippingDoubleDiMuonForCharmAssociative'  , 
                                     'StrippingChiAndDiMuonForCharmAssociative'  , 
                                     'StrippingDiChiForCharmAssociative'         , 
                                     'StrippingDiMuonAndWForCharmAssociative'    ,
                                     'StrippingChiAndWForCharmAssociative'       ] } 
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
                logger.debug ('new configuration: %-16s : %s ' % ( key , _config[key] ) )

        keys = _config.keys()
        for key in keys :
            
            if not key in _default_configuration_ :
                raise KeyError("Invalid key is specified: '%s'" % key )
            
            val = _config[key]
            if val != _default_configuration_ [ key ] : 
                logger.debug ('new configuration: %-16s : %s ' % ( key , _config[key] ) )

        ## cehck for prescales 
        for keys in self.keys() :
            if 0 > key.find('Prescale') and 0 > key.find('prescale') :  continue
            if 1 != self[key] : logger.warning ( '%s is %s' % ( key , self[key] ) ) 
            

        for line in self._lines_associative_onia () :
            self.registerLine(line)
            logger.debug ( "Register line: %s" %  line.name () ) 

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
        Pure Technical method for creation of simple 1-step selections 
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
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles  
        ## ATTENITON: we need PROMPT onia, thus 'AllMuons' are here
        from StandardParticles                     import StdAllLooseMuons as inpts  
        return self.make_selection (
            'DiMuon'               ,
            CombineParticles       ,
            [ inpts ]              ,
            DecayDescriptor = 'J/psi(1S) -> mu+ mu-' ,
            DaughtersCuts   = { 'mu+' : self.muonCuts() } , 
            CombinationCut  = " psi | psi_prime | ( 8 * GeV < AM ) " , 
            MotherCut       = " chi2vx < 20 " 
            )
    
    ## get the dimuons & gamma 
    def DiMuonAndGamma ( self ) :
        """
        Get dimuon & gamma 
        Select events with dimuon and high-pt photon 
        at least one dimuon 
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles  
        from StandardParticles                     import StdLooseAllPhotons as gamma 
        return self.make_selection (
            'DiMuonAdGamma'        ,
            CombineParticles       ,
            [ self.DiMuon() , gamma ]              ,
            DecayDescriptor = " chi_b2(1P)  -> J/psi(1S) gamma"   ,
            DaughtersCuts   = { 'J/psi(1S)' : " dimuon_tight "    ,   
                                'gamma'     :  self.photonCuts () } ,
            CombinationCut = " AALL "  ,
            MotherCut      = "  ALL "  
            )
    
    ## get the double dimuons
    def DoubleDiMuon ( self ) :
        """
        Get 2xdimuon
        Select events with two dimuons
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles  
        return self.make_selection (
            'DoubleDiMuon'    ,
            CombineParticles  ,
            [ self.DiMuon() ] ,
            DecayDescriptor   = "chi_b2(1P)  -> J/psi(1S) J/psi(1S)" ,
            CombinationCut    = " AALL " ,
            MotherCut         = "  ALL "
            )
    
    ## get chi_(c,b) & dimuon 
    def ChiAndDiMuon ( self ) : 
        """ Construct Chi_(c,b) + dumuon line """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles  
        from StandardParticles                     import StdLooseAllPhotons as gamma 
        pre_chidimu = self.make_selection (
            'PreChiAndDiMuon'         ,
            CombineParticles          ,
            [ self.DoubleDiMuon() , self.DiMuon() , gamma ] ,  ## the first one is fake
            DecayDescriptor  =  "chi_b2(1P)  -> J/psi(1S) J/psi(1S) gamma " ,
            DaughtersCuts    = { 'gamma'     :  self['GammaChi'] } ,
            ## mimic chi_(c,b):
            CombinationCut   = """
            ( AM13 - AM1 < 1.05 * GeV ) |
            ( AM23 - AM2 < 1.05 * GeV ) 
            """ ,
            MotherCut        = "  ALL " 
            )
        ## apply pi0-veto-tagger ! 
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger
        return self.make_selection (
            'ChiAndDiMuon'   ,
            Pi0Veto__Tagger  ,
            [ pre_chidimu ]  ,
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25011     ## unique ! 
            )
    
    ## get 2xXhi_(c,b) & dimuon 
    def DiChi ( self ) : 
        """ Construct 2xChi_(c,b) line """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles  
        from StandardParticles                     import StdLooseAllPhotons as gamma 
        pre_dichi = self.make_selection (
            'PreDiChi'         ,
            CombineParticles   ,
            [ self.DoubleDiMuon() , self.DiMuon() , gamma ] ,  ## the first one is fake
            DecayDescriptor =  "chi_b2(1P)  -> J/psi(1S) J/psi(1S) gamma gamma" ,
            DaughtersCuts   = { 'gamma' :  self [ 'GammaChi' ] } ,
            ## mimic 2xchi_(c,b):
            CombinationCut   = """
            ( ( AM13 - AM1 < 1.05 * GeV ) & ( AM24 - AM2 < 1.05 * GeV ) ) | 
            ( ( AM14 - AM1 < 1.05 * GeV ) & ( AM23 - AM2 < 1.05 * GeV ) ) 
            """ ,
            MotherCut        = "  ALL " 
            )
        ## apply pi0-veto-tagger ! 
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger
        return self.make_selection (
            'DiChi'          ,
            Pi0Veto__Tagger  ,
            [ pre_dichi   ]  ,
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25012     ## unique ! 
            )
    
    ## W+- selection 
    def W ( self )  :
        """ Get simple  W+-selection """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop 
        from StandardParticles                     import StdAllLooseMuons as inpts 
        return self.make_selection (
            'W'                    ,
            FilterDesktop          ,
            [ inpts ]              ,
            Code   = self['WCuts']
            )
    
    ## select dimuon + W 
    def DiMuonAndW ( self ) :
        """ Select dimuon + W+ """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
        return self.make_selection (
            'DiMuW'                     ,
            CombineParticles            ,
            [ self.W () , self.DiMuon() ] , 
            DecayDescriptor =  "[chi_b2(2P)  -> J/psi(1S) mu+]cc" ,
            CombinationCut  = " AALL " ,
            MotherCut       = "  ALL " 
            )
    
    ## select chi + W 
    def ChiAndW ( self ) :
        """ Select chi & W+ """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
        from StandardParticles                     import StdLooseAllPhotons as gamma 
        pre_chiw = self.make_selection (
            'PreChiAndW'        ,
            CombineParticles    ,
            [ self.DiMuon()     , self.W() , gamma ] ,
            DecayDescriptor =  "[chi_b2(2P)  -> J/psi(1S) gamma mu+]cc" ,
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
        ## apply pi0-veto-tagger ! 
        from GaudiConfUtils.ConfigurableGenerators import Pi0Veto__Tagger
        return self.make_selection (
            'ChiAndW'        ,
            Pi0Veto__Tagger  ,
            [ pre_chiw ]     , 
            MassWindow     = 25 * MeV  ,
            MassChi2       = -1        ,
            ExtraInfoIndex = 25013     ## unique ! 
            )
    
    ## get all dicharm lines 
    def _lines_associative_onia ( self ) :
        
        sel = self._selection ( 'OniaAndXLines' )
        if sel : return sel 
        
        sel = [
            ##
            StrippingLine (
            "DiMuonAndGammaFor" + self._name             ,
            prescale = self [ 'DiMuonAndGammaPrescale' ] , ## ATTENTION! Prescale here !!
            checkPV  = self [ 'CheckPV']                 ,
            algos    = [ self . DiMuonAndGamma () ]      ,
            MDSTFlag = True                              , ## try to save it on MDST.DST 
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
if '__main__' == __name__ :
    
    logger.info ( 80*'*'  ) 
    logger.info (  __doc__ ) 
    logger.info ( ' Author :  %s' % __author__ ) 
    logger.info ( ' Date   :  %s' % __date__   ) 
    logger.info ( 80 * '*' ) 
    ##
    clines = set() 
    logger.info ( 70 * '-' ) 
    logger.info ( ' %-15s | %-40s ' % ( 'STREAM' , 'LINE' ) )
    logger.info ( 70 * '-' ) 
    for stream in default_config['STREAMS'] :
        lines = default_config['STREAMS'][stream]
        for l in lines :
            logger.info ( ' %-15s | %-40s ' % ( stream , l ) )
            clines.add ( l )
    logger.info ( 80 * '*' ) 
    ##
    logger.info ( ' The output locations for the default configuration: ' )
    ##
    _conf = StrippingCharmAssociativeConf ( 'CharmAssociative' ,
                                            config = default_config['CONFIG']  )
    ##
    ## 
    _ln   = ' ' + 61*'-' + '+' + 60*'-'
    logger.info ( _ln ) 
    logger.info ( '  %-60s| %-45s | %s ' % ( 'Output location'     ,
                                             'Stripping line name' ,
                                             'MDST.DST'            ) ) 
    logger.info ( _ln )
    for l in _conf.lines() :
        lout  = l.outputLocation()
        lname = l.name()
        flag  = l.MDSTFlag
        logger.info ( '  %-60s| %-45s | %s ' % ( lout, lname , flag ) )
        if not lname in clines :
            raise AttributeError ('Unknown Line %s' % lname )
        clines.remove ( lname )
    logger.info ( _ln ) 
    logger.info ( 80*'*'  ) 
    if clines :
        raise AttributeError('Undeclared lines: %s' % clines )

    keys = default_config['CONFIG'].keys()
    keys.sort()
    prescale = [ i for i in keys if 0 <= i.find('Prescale') ]
    other    = [ i for i in keys if not i in prescale       ] 
    logger.info ( 'Configuration keys are: %s' % other    ) 
    logger.info ( 'Prescale      keys are: %s' % prescale ) 
    logger.info ( 80*'*' )
    
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

