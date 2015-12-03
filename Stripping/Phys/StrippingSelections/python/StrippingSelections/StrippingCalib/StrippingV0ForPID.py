#!/usr/bin/env python
# =============================================================================
# @file StripingSelections/StrippingV0ForPID.py
#
# Configuration file for PID V0 ( K0S and  Lambda0) stripping.
#
# This series of stripping lines are designed for the purpose of PID
# calibration/monitoring of the RICH and MUON systems. Tracks from these
# samples will be used to determine ID and mis-ID in each of these
# sub-detectors. In order to enhance the number of tracks reconstructed in
# the periphery of phase-space, the selections are binned in terms of the
# V0's rapidity and pT. Each bin is then assigned its own
# pre-scale.
# 
#
# Acknowledgements:
#
# Vanya Belyaev
# The selection cuts used in this series of lines are essentially a tightened
# version of those developed by Vanya Belyaev for the original V0 stripping
# performed in 2009 at /sqrt{s} = 0.9 TeV. Furthermore, the selection code
# borrows heavily on his original stripping lines (StrippingV0Conf).
#
# For more information see:
#
#  http://indico.cern.ch/conferenceDisplay.py?confId=74536
#  http://indico.cern.ch/materialDisplay.py?contribId=13&sessionId=2&materialId=0&confId=78767
#
# @author Andrew Powell a.powell1@physics.ox.ac.uk
# @date 2010-09-05
# =============================================================================
"""
Usage:
 >>> stream = ...
 >>> from StrippingSelections.StrippingV0ForPID import StrippingV0ForPIDConf as builder_v0
 >>> from StrippingSelections.StrippingV0ForPID import default_config as config_v0
 >>> lb_v0  = builder_v0( name='noPIDV0', config=config_v0)
 >>> stream.appendLines( lb_v0.lines()  )
 
Results from running over:
$STRIPPINGSELECTIONSROOT/tests/data/Reco13c_Run124134.py

StrippingReport                                                INFO Event 100000, Good event 100000
|                                    *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
|!StrippingK0SLLLinenoPIDV0                         |  0.3290|       329|  1.116|   0.388|
|!StrippingK0SDDLinenoPIDV0                         |  0.3190|       319|  1.135|   0.931|
|!StrippingLam0LLLine1noPIDV0                       |  0.5050|       505|  1.085|   2.783|
|!StrippingLam0LLLine2noPIDV0                       |  0.2960|       296|  1.017|  16.511|
|!StrippingLam0DDLinenoPIDV0                        |  0.2920|       292|  1.051|   4.443|
|!StrippingLam0LLIsMUONLine1noPIDV0                 |  0.1640|       164|  1.043|   0.117|
|!StrippingLam0LLIsMUONLine2noPIDV0                 |  0.0080|         8|  1.000|   0.116|
|!StrippingLam0DDIsMUONLinenoPIDV0                  |  0.1270|       127|  1.031|  39.297|                
"""
# =============================================================================
__author__  = 'Andrew Powell a.powell1@physics.ox.ac.uk'
__date__    = '27-08-2011'
__version__ = '$Revision: 1.7 $'
__all__ = ('StrippingV0ForPIDConf','default_config')
# =============================================================================

from Gaudi.Configuration       import *
from LHCbKernel.Configuration  import *
from GaudiKernel.SystemOfUnits import mm, cm , MeV, GeV

# =============================================================================
# @class StrippingV0ForPIDConf
#
# Configurable for PID V0 ( K0S and  Lambda0 ) stripping.
##
# @author Andrew Powell a.powell1@physics.ox.ac.uk
# @date 2010-09-05
# =============================================================================

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand        
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsPions, StdNoPIDsDownPions, StdAllNoPIDsProtons, StdNoPIDsDownProtons

#import logging
#log = logging.getLogger('StrippingPromptCharm')

# =============================================================================
## Define the default configuration
_default_configuration_ = {
    'TrackChi2'            :  5       ,          ## Track Chi2/ndof quality 
    'VertexChi2'           :  16      ,          ## Cut on Vertex chi2-quality
    'DeltaMassK0S'         :  50 * MeV,          ## Mass-window (half)-width for K0S 
    'DeltaMassLambda'      :  25 * MeV,          ## Mass-window (half)-width for Lambda 
    'MaxZ'                 : 220 * cm ,          ## Maximal allowed vertex Z-position
    'DaughtersIPChi2'      :  25      ,          ## minimal IP-chi2 for daughter particles  
    'LTimeFitChi2'         :  49      ,          ## Chi2 from LifeTime Fitter
    ##
    'WrongMassK0S'         :   9 * MeV,          ## Mass-window (half)-width for ppi hypthesis
    'WrongMassK0S_DD'      :  18 * MeV,          ## Mass-window (half)-width for ppi hypthesis (DD-case)
    ##
    'WrongMassLambda'      :  20 * MeV,          ## Mass-window (half)-width for pipi hypthesis
    'WrongMassLambda_DD'   :  40 * MeV,          ## Mass-window (half)-width for pipi hypthesis (DD-case)
    ##
    'CTauK0S'              :   1 * mm ,          ## Cut on c*tau for K0S 
    'CTauK0S_DD'           :  10 * mm ,          ## Cut on c*tau for K0S (DD-case)
    ##
    'CTauLambda0'          :   5 * mm ,          ## Cut on c*tau for Lambda0
    'CTauLambda0_DD'       :  20 * mm ,          ## Cut on c*tau for Lambda0 (DD-case)
    ##
    'Proton_IsMUONCut'     :  "(INTREE( (ABSID=='p+') & ISMUON ) )" , 
    ## 
    # Technicalities:
    ##
    'Monitor'              :  False    ,          ## Activate the monitoring ?
    ##
    "HLT"                  : "HLT_PASS_RE('Hlt1MB.*Decision')" ,  ## HLT-cut
    ##
    'Preambulo'       : [
    ## import c_light
    "from GaudiKernel.PhysicalConstants import c_light" ,
    ## define DD-category of K0S 
    "DD =    CHILDCUT ( ISDOWN , 1 ) & CHILDCUT ( ISDOWN , 2 ) " ,
    ## define LL-category of K0S 
    "LL =    CHILDCUT ( ISLONG , 1 ) & CHILDCUT ( ISLONG , 2 ) "
    ] ,
    'KS0LL_Prescale'           : 0.020 ,
    'KS0DD_Prescale'           : 0.024 , 
    'LamLL_Prescale_LoP'       : 0.140 , 
    'LamLL_Prescale_HiP'       : 1.000 ,
    'LamDD_Prescale'           : 0.100 , 
    'LamLLIsMUON_Prescale_LoP' : 1.000 ,
    'LamLLIsMUON_Prescale_HiP' : 1.000 ,
    'LamDDIsMUON_Prescale'     : 1.000   

    }

## the mandatory element

default_config ={
    'NAME'        :   'VznoPID'       ,
    'WGs'         : [ 'Calib'] ,
    'CONFIG'      : _default_configuration_  , 
    'BUILDERTYPE' :   'StrippingV0ForPIDConf'            ,
    'STREAMS'     :  [ 'PID']
              }    



# =============================================================================
class StrippingV0ForPIDConf(LineBuilder) :    
    """
    Helper class to configure 'V0ForPID'-lines
    """
    __configuration_keys__ = tuple ( _default_configuration_.keys() )

    ## get the default configuration 
    #@staticmethod
    def defaultConfiguration( key = None ) :
        
        from copy import deepcopy
        _config = deepcopy ( _default_configuration_ )
        if key : return _config[ key ]
        return _config
    
    ## constructor
    def __init__(self, name, config) :
        """
        Constructor
        """        
                
        self._name            = name

        self._trackChi2       = config[ 'TrackChi2' ]
        self._vertexChi2      = config[ 'VertexChi2' ]
        self._deltaMassK0S    = config[ 'DeltaMassK0S' ]
        self._deltaMassLambda = config[ 'DeltaMassLambda' ]
        self._maxZ            = config[ 'MaxZ' ]
        self._daughtersIPChi2 = config[ 'DaughtersIPChi2' ]
        self._lTimeFitChi2    = config[ 'LTimeFitChi2' ]
        
        self._wrongMassK0S       = config[ 'WrongMassK0S' ]
        self._wrongMassK0S_DD    = config[ 'WrongMassK0S_DD' ]
        self._wrongMassLambda    = config[ 'WrongMassLambda' ]
        self._wrongMassLambda_DD = config[ 'WrongMassLambda_DD' ] 
        
        self._cTauK0S        = config[ 'CTauK0S' ]
        self._cTauK0S_DD     = config[ 'CTauK0S_DD' ]
        self._cTauLambda0    = config[ 'CTauLambda0' ]
        self._cTauLambda0_DD = config[ 'CTauLambda0_DD' ]

        self._protonIsmuon   = config[ 'Proton_IsMUONCut' ]
        
        self._KS0LLPrescale            = config[ 'KS0LL_Prescale' ]
        self._KS0DDPrescale            = config[ 'KS0DD_Prescale' ] 
        self._LamLLPrescale_HiP        = config[ 'LamLL_Prescale_HiP' ]
        self._LamLLPrescale_LoP        = config[ 'LamLL_Prescale_LoP' ]
        self._LamDDPrescale            = config[ 'LamDD_Prescale' ]
        self._LamLLIsMUONPrescale_HiP  = config[ 'LamLLIsMUON_Prescale_HiP' ]
        self._LamLLIsMUONPrescale_LoP  = config[ 'LamLLIsMUON_Prescale_LoP' ]
        self._LamDDIsMUONPrescale      = config[ 'LamDDIsMUON_Prescale' ]

        self._monitor      = config[ 'Monitor' ]
        self._hlt          = config[ 'HLT' ]
        self._Preambulo    = config[ 'Preambulo' ]
        
       
        LineBuilder.__init__( self , name , config )
        
        for line in self._lines_inner() :
            self.registerLine(line)

    def _lines_inner ( self ) :
        if hasattr ( self , '_Lines' ) : return self._Lines

        self._Lines = [
            ##
            StrippingLine (
            "K0SLLLine" + self._name ,
            prescale   = self._KS0LLPrescale ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.K0S_LL_Bin1() ],
            RequiredRawEvents = ["Muon"],    
            RelatedInfoTools = [ addRelInfoMuonIsolation("KS0 -> pi+ pi-")  ]  
            ) ,
            ## 
            StrippingLine (
            "K0SDDLine" + self._name ,
            prescale   = self._KS0DDPrescale ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.K0S_DD_Bin1() ],
            RequiredRawEvents = ["Muon"],
            RelatedInfoTools = [ addRelInfoMuonIsolation("KS0 -> pi+ pi-")  ] 
            ) ,
            ## 
            StrippingLine (
            "Lam0LLLine1" + self._name ,
            prescale   = self._LamLLPrescale_LoP ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.Lam0_LL_Bin1() ],
            RequiredRawEvents = ["Muon"],
            RelatedInfoTools = [  addRelInfoMuonIsolation("[ Lambda0 -> p+ pi-]CC")  ]     
                ) ,
            ## 
            StrippingLine (
            "Lam0LLLine2" + self._name ,
            prescale   = self._LamLLPrescale_HiP ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.Lam0_LL_Bin2() ],
            RequiredRawEvents = ["Muon"],
            RelatedInfoTools = [  addRelInfoMuonIsolation("[ Lambda0 -> p+ pi-]CC")  ] 
            ) ,
            ## 
            StrippingLine (
            "Lam0DDLine" + self._name ,
            prescale   = self._LamDDPrescale ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.Lam0_DD_Bin1() ],
            RequiredRawEvents = ["Muon"],
            RelatedInfoTools = [  addRelInfoMuonIsolation("[ Lambda0 -> p+ pi-]CC")  ] 
            ),
            ## 
            StrippingLine (
            "Lam0LLIsMUONLine1" + self._name ,
            prescale   = self._LamLLIsMUONPrescale_LoP ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.Lam0_LL_Bin1_IsMUON() ],
            RequiredRawEvents = ["Muon"],
            RelatedInfoTools = [  addRelInfoMuonIsolation("[ Lambda0 -> p+ pi-]CC")  ] 
            )
            ,
            ## 
            StrippingLine (
            "Lam0LLIsMUONLine2" + self._name ,
            prescale   = self._LamLLIsMUONPrescale_HiP ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.Lam0_LL_Bin2_IsMUON() ],
            RequiredRawEvents = ["Muon"],
            RelatedInfoTools = [  addRelInfoMuonIsolation("[ Lambda0 -> p+ pi-]CC")  ] 
            )
            ,
            ## 
            StrippingLine (
            "Lam0DDIsMUONLine" + self._name ,
            prescale   = self._LamDDIsMUONPrescale ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.Lam0_DD_Bin1_IsMUON() ],
            RequiredRawEvents = ["Muon"],
            RelatedInfoTools = [  addRelInfoMuonIsolation("[ Lambda0 -> p+ pi-]CC")  ] 
            )
            ]

        return self._Lines
    
    def K0S ( self ) :
        """
        Selection for K0S 
        """
        if hasattr ( self , 'K0S_LL' ) :
            return self.K0S_LL

        _K0S = CombineParticles(
            #self.name + "StripK0S" ,
            DecayDescriptor = "KS0 -> pi+ pi-" ,
            Preambulo       = self._Preambulo,
            DaughtersCuts   = {
            '' : self._k0s_daughtersCuts ()
            } ,
            CombinationCut  = "AM < 1.0 * GeV " , 
            MotherCut       = self._k0s_motherCut()
            )

        if self._monitor == True :
            _K0S.Preambulo       += [
                ## define historam type (shortcut)
                "Histo  = Gaudi.Histo1DDef"  ,
                ## monitor LL-case
                "massLL     = monitor ( M / GeV ,             Histo ( 'K0S, LL-case' , 0.45 , 0.55 , 100 ) , 'M_LL'    ) " ,
                "p1LL       = monitor ( CHILD( P, 1 ) / GeV , Histo ( 'K0S, LL-case' , 0.00 , 120 ,  100 ) , 'p1_LL'   ) " ,
                "p2LL       = monitor ( CHILD( P, 2 ) / GeV , Histo ( 'K0S, LL-case' , 0.00 , 120 ,  100 ) , 'p2_LL'   ) " ,
                ]
            _K0S.Monitor         = True ,
            _K0S.HistoProduce    = True ,
            _K0S.MotherMonitor = """ process ( massLL )
            >> process ( p1LL )
            >> process ( p2LL )
            >> EMPTY
            """
        
        self.K0S_LL = Selection("SelK0S2PiPi_LL_" + self._name,
                                Algorithm = _K0S,
                                RequiredSelections = [ StdAllNoPIDsPions ]) 
            
        return self.K0S_LL

    def K0SDD ( self ) :
        """
        Selection for K0S DD 
        """
        if hasattr ( self , 'K0S_DD' ) :
            return self.K0S_DD
        
        _K0SDD = CombineParticles(
            #self.name + "StripK0S_DD" ,
            DecayDescriptor = "KS0 -> pi+ pi-" ,
            Preambulo       = self._Preambulo,
            DaughtersCuts   = {
            '' : self._k0s_daughtersCuts ()
            } ,
            CombinationCut  = "AM < 1.0 * GeV " ,
            MotherCut       = self._k0s_motherCut()
            )

        if self._monitor == True :
            _K0SDD.Preambulo       += [
                ## define historam type (shortcut)
                "Histo  = Gaudi.Histo1DDef"  ,
                ## monitor LL-case
                "massDD     = monitor ( M / GeV ,             Histo ( 'K0S, DD-case' , 0.45 , 0.55 , 100 ) , 'M_DD'    ) " ,
                "p1DD       = monitor ( CHILD( P, 1 ) / GeV , Histo ( 'K0S, DD-case' , 0.00 , 120 ,  100 ) , 'p1_DD'   ) " ,
                "p2DD       = monitor ( CHILD( P, 2 ) / GeV , Histo ( 'K0S, DD-case' , 0.00 , 120 ,  100 ) , 'p2_DD'   ) " ,
                ]
            _K0SDD.Monitor         = True ,
            _K0SDD.HistoProduce    = True ,
            _K0SDD.MotherMonitor = """ process ( massDD )
            >> process ( p1DD )
            >> process ( p2DD )
            >> EMPTY
            """
        
        self.K0S_DD = Selection("SelK0S2PiPi_DD_" + self._name,
                                Algorithm = _K0SDD,
                                RequiredSelections = [ StdNoPIDsDownPions ]) 

        return self.K0S_DD

    def Lam0 ( self ) :
        """
        Selection for Lam0 
        """
        if hasattr ( self , 'Lam0_LL' ) :
            return self.Lam0_LL

        _Lam0 = CombineParticles(
            #self.name + "StripLam0" ,
            DecayDescriptor = "[ Lambda0 -> p+ pi-]cc" ,
            Preambulo       = self._Preambulo,
            DaughtersCuts   = {
            '' : self._lam0_daughtersCuts ()
            } ,
            CombinationCut  = "AM < 1.5 * GeV " ,
            MotherCut       = self._lam0_motherCut()
            )

        if  self._monitor == True :
            _Lam0.Preambulo       += [
                ## define historam type (shortcut)
                "Histo  = Gaudi.Histo1DDef"  ,
                ## monitor LL-case
                "massLL     = monitor ( M / GeV ,             Histo ( 'Lambda0, LL-case' , 1.080 , 1.140 , 100 ) , 'M_LL'    ) " ,
                "p1LL       = monitor ( CHILD( P, 1 ) / GeV , Histo ( 'Lambda0, LL-case' , 0.00  , 120   , 100 ) , 'p1_LL'   ) " ,
                "p2LL       = monitor ( CHILD( P, 2 ) / GeV , Histo ( 'Lambda0, LL-case' , 0.00  , 120   , 100 ) , 'p2_LL'   ) " ,
                ]
            _Lam0.Monitor         = True ,
            _Lam0.HistoProduce    = True ,
            _Lam0.MotherMonitor = """ process ( massLL )
            >> process ( p1LL )
            >> process ( p2LL )
            >> EMPTY
            """
        

        self.Lam0_LL = Selection("SelLam02PPi_LL" + self._name,
                                 Algorithm = _Lam0,
                                 RequiredSelections = [ StdAllNoPIDsPions, StdAllNoPIDsProtons ]) 

        return self.Lam0_LL

    def Lam0DD ( self ) :
        """
        Selection for Lam0 DD 
        """
        if hasattr ( self , 'Lam0_DD' ) :
            return self.Lam0_DD

        _Lam0DD = CombineParticles(
            #self.name + "StripLam0_DD" ,
            DecayDescriptor = "[ Lambda0 -> p+ pi-]cc" ,
            Preambulo       = self._Preambulo,
            DaughtersCuts   = {
            '' : self._lam0_daughtersCuts (),
            } ,
            CombinationCut  = "AM < 1.5 * GeV " ,
            MotherCut       = self._lam0_motherCut()        
            )

        if  self._monitor == True :
            _Lam0DD.Preambulo       += [
                ## define historam type (shortcut)
                "Histo  = Gaudi.Histo1DDef"  ,
                ## monitor LL-case
                "massDD     = monitor ( M / GeV ,             Histo ( 'Lambda0, DD-case' , 1.080 , 1.140 , 100 ) , 'M_DD'    ) " ,
                "p1DD       = monitor ( CHILD( P, 1 ) / GeV , Histo ( 'Lambda0, DD-case' , 0.00  , 120   , 100 ) , 'p1_DD'   ) " ,
                "p2DD       = monitor ( CHILD( P, 2 ) / GeV , Histo ( 'Lambda0, DD-case' , 0.00  , 120   , 100 ) , 'p2_DD'   ) " ,
                ]
            _Lam0DD.Monitor         = True ,
            _Lam0DD.HistoProduce    = True ,
            _Lam0DD.MotherMonitor = """ process ( massDD )
            >> process ( p1DD )
            >> process ( p2DD )
            >> EMPTY
            """
        self.Lam0_DD = Selection("SelLam02PiPi_DD" + self._name ,
                                 Algorithm = _Lam0DD,
                                 RequiredSelections = [ StdNoPIDsDownPions , StdNoPIDsDownProtons]) 
        
        return self.Lam0_DD

    def K0S_LL_Bin1 ( self ) :
        """
        Selection for K0S LL Bin1
        """
        if hasattr(self, 'K0S_LL_Bin1_V0ForPID') :
            return self.K0S_LL_Bin1_V0ForPID

        K0S = self.K0S()
        _K0S_LL_Bin1_Filter = FilterDesktop (
            #self.name + 'K0S_LL_Bin1_Filter' ,
            Code         = self._k0s_bin1Cut_LL()
            )

        if self._monitor == True :
            _K0S_LL_Bin1_Filter.Preambulo    = [
                ## define historam type (shortcut)
                "Histo  = Gaudi.Histo1DDef"  ,
                ## monitor LL-case
                "massLL     = monitor ( M / GeV ,             Histo ( 'K0S, LL-case' , 0.45 , 0.55 , 100 ) , 'M_LL'    ) " ,
                "p1LL       = monitor ( CHILD( P, 1 ) / GeV , Histo ( 'K0S, LL-case' , 0.00 , 120 ,  100 ) , 'p1_LL'   ) " ,
                "p2LL       = monitor ( CHILD( P, 2 ) / GeV , Histo ( 'K0S, LL-case' , 0.00 , 120 ,  100 ) , 'p2_LL'   ) " ,
                ]
            _K0S_LL_Bin1_Filter.Monitor      = True  ,
            _K0S_LL_Bin1_Filter.HistoProduce = True  ,
            _K0S_LL_Bin1_Filter.PostMonitor  = """ process ( massLL )
            >> process ( p1LL )
            >> process ( p2LL )
            >> EMPTY
            """

        self.K0S_LL_Bin1_V0ForPID = Selection(
            "SelK0S_LL_Bin1" + self._name,
            Algorithm          = _K0S_LL_Bin1_Filter,
            RequiredSelections = [ K0S ]
            )
        return self.K0S_LL_Bin1_V0ForPID

    def K0S_DD_Bin1 ( self ) :
        """
        Selection for K0S DD Bin1
        """
        if hasattr(self, 'K0S_DD_Bin1_V0ForPID') :
            return self.K0S_DD_Bin1_V0ForPID

        _K0S_DD_Bin1_Filter = FilterDesktop (
            #self.name + 'K0S_DD_Bin1_Filter' ,
            Code         = self._k0s_bin1Cut_DD()
            )

        if self._monitor == True :
            _K0S_DD_Bin1_Filter.Preambulo    = [
                ## define historam type (shortcut)
                "Histo  = Gaudi.Histo1DDef"  ,
                ## monitor DD-case
                "massDD     = monitor ( M / GeV ,             Histo ( 'K0S, DD-case' , 0.45 , 0.55 , 100 ) , 'M_DD'    ) " ,
                "p1DD       = monitor ( CHILD( P, 1 ) / GeV , Histo ( 'K0S, DD-case' , 0.00 , 120 ,  100 ) , 'p1_DD'   ) " ,
                "p2DD       = monitor ( CHILD( P, 2 ) / GeV , Histo ( 'K0S, DD-case' , 0.00 , 120 ,  100 ) , 'p2_DD'   ) " ,
                ]
            _K0S_DD_Bin1_Filter.Monitor      = True  ,
            _K0S_DD_Bin1_Filter.HistoProduce = True  ,
            _K0S_DD_Bin1_Filter.PostMonitor  = """ process ( massDD )
            >> process ( p1DD )
            >> process ( p2DD )
            >> EMPTY
            """

        self.K0S_DD_Bin1_V0ForPID = Selection(
            "SelK0S_DD_Bin1" + self._name,
            Algorithm          = _K0S_DD_Bin1_Filter,
            RequiredSelections = [ self.K0SDD() ]
            )
        return self.K0S_DD_Bin1_V0ForPID


    def Lam0_LL_Bin1 ( self ) :
        """
        Selection for Lam0 LL Bin1
        """
        if hasattr(self, 'Lam0_LL_Bin1_V0ForPID') :
            return self.Lam0_LL_Bin1_V0ForPID

        Lam0 = self.Lam0()
        _Lam0_LL_Bin1_Filter = FilterDesktop (
            #self.name + 'Lam0_LL_Bin1_Filter' ,
            Code         = self._lam0_bin1Cut_LL()
            )
        
        if self._monitor == True :
            _Lam0_LL_Bin1_Filter.Preambulo    = [
                ## define historam type (shortcut)
                "Histo  = Gaudi.Histo1DDef"  ,
                ## monitor LL-case
                "massLL     = monitor ( M / GeV ,             Histo ( 'Lambda0, LL-case' , 1.080 , 1.140 , 100 ) , 'M_LL'    ) " ,
                "p1LL       = monitor ( CHILD( P, 1 ) / GeV , Histo ( 'Lambda0, LL-case' , 0.00  , 120   , 100 ) , 'p1_LL'   ) " ,
                "p2LL       = monitor ( CHILD( P, 2 ) / GeV , Histo ( 'Lambda0, LL-case' , 0.00  , 120   , 100 ) , 'p2_LL'   ) " ,
                ]
            _Lam0_LL_Bin1_Filter.Monitor      = True  ,
            _Lam0_LL_Bin1_Filter.HistoProduce = True ,
            _Lam0_LL_Bin1_Filter.PostMonitor  = """ process ( massLL )
            >> process ( p1LL )
            >> process ( p2LL )
            >> EMPTY
            """

        self.Lam0_LL_Bin1_V0ForPID = Selection(
            "SelLam0_LL_Bin1" + self._name,
            Algorithm          = _Lam0_LL_Bin1_Filter,
            RequiredSelections = [ Lam0 ]
            )
        return self.Lam0_LL_Bin1_V0ForPID

    def Lam0_LL_Bin2 ( self ) :
        """
        Selection for Lam0 LL Bin2
        """
        if hasattr(self, 'Lam0_LL_Bin2_V0ForPID') :
            return self.Lam0_LL_Bin2_V0ForPID

        Lam0 = self.Lam0()
        _Lam0_LL_Bin2_Filter = FilterDesktop (
            #self.name + 'Lam0_LL_Bin2_Filter' ,
            Code         = self._lam0_bin2Cut_LL()
            )

        if self._monitor == True :
            _Lam0_LL_Bin2_Filter.Preambulo    = [
                ## define historam type (shortcut)
                "Histo  = Gaudi.Histo1DDef"  ,
                ## monitor LL-case
                "massLL     = monitor ( M / GeV ,             Histo ( 'Lambda0, LL-case' , 1.080 , 1.140 , 100 ) , 'M_LL'    ) " ,
                "p1LL       = monitor ( CHILD( P, 1 ) / GeV , Histo ( 'Lambda0, LL-case' , 0.00  , 120   , 100 ) , 'p1_LL'   ) " ,
                "p2LL       = monitor ( CHILD( P, 2 ) / GeV , Histo ( 'Lambda0, LL-case' , 0.00  , 120   , 100 ) , 'p2_LL'   ) " ,
                ]
            _Lam0_LL_Bin2_Filter.Monitor      = True  ,
            _Lam0_LL_Bin2_Filter.HistoProduce = True  ,
            _Lam0_LL_Bin2_Filter.PostMonitor  = """ process ( massLL )
            >> process ( p1LL )
            >> process ( p2LL )
            >> EMPTY
            """

        self.Lam0_LL_Bin2_V0ForPID = Selection(
            "SelLam0_LL_Bin2" + self._name,
            Algorithm          = _Lam0_LL_Bin2_Filter,
            RequiredSelections = [ Lam0 ]
            )
        return self.Lam0_LL_Bin2_V0ForPID


    def Lam0_LL_Bin1_IsMUON ( self ) :
        """
        Selection for Lam0 LL Bin1
        """
        if hasattr(self, 'Lam0_LL_Bin1_IsMUON_V0ForPID') :
            return self.Lam0_LL_Bin1_IsMUON_V0ForPID
        
        Lam0 = self.Lam0()
        _Lam0_LL_Bin1_IsMUON_Filter = FilterDesktop (
            #self.name + 'Lam0_LL_Bin1_Filter' ,
            Code         = self._lam0_bin1Cut_LL_IsMUON()
            )

        if self._monitor == True :
            _Lam0_LL_Bin1_IsMUON_Filter.Preambulo    = [
                ## define historam type (shortcut)
                "Histo  = Gaudi.Histo1DDef"  ,
                ## monitor LL-case
                "massLL     = monitor ( M / GeV ,             Histo ( 'Lambda0, LL-case' , 1.080 , 1.140 , 100 ) , 'M_LL'    ) " ,
                "p1LL       = monitor ( CHILD( P, 1 ) / GeV , Histo ( 'Lambda0, LL-case' , 0.00  , 120   , 100 ) , 'p1_LL'   ) " ,
                "p2LL       = monitor ( CHILD( P, 2 ) / GeV , Histo ( 'Lambda0, LL-case' , 0.00  , 120   , 100 ) , 'p2_LL'   ) " ,
                ]
            _Lam0_LL_Bin1_IsMUON_Filter.Monitor      = True  ,
            _Lam0_LL_Bin1_IsMUON_Filter.HistoProduce = True ,
            _Lam0_LL_Bin1_IsMUON_Filter.PostMonitor  = """ process ( massLL )
            >> process ( p1LL )
            >> process ( p2LL )
            >> EMPTY
            """

        self.Lam0_LL_Bin1_IsMUON_V0ForPID = Selection(
            "SelLam0_LL_Bin1_ISMUON" + self._name,
            Algorithm          = _Lam0_LL_Bin1_IsMUON_Filter,
            RequiredSelections = [ Lam0 ]
            )
        return self.Lam0_LL_Bin1_IsMUON_V0ForPID

    def Lam0_LL_Bin2_IsMUON ( self ) :
        """
        Selection for Lam0 LL Bin2
        """
        if hasattr(self, 'Lam0_LL_Bin2_IsMUON_V0ForPID') :
            return self.Lam0_LL_Bin2_IsMUON_V0ForPID
        
        Lam0 = self.Lam0()
        _Lam0_LL_Bin2_IsMUON_Filter = FilterDesktop (
            #self.name + 'Lam0_LL_Bin2_Filter' ,
            Code         = self._lam0_bin2Cut_LL_IsMUON()
            )
        
        if self._monitor == True :
            _Lam0_LL_Bin2_IsMUON_Filter.Preambulo    = [
                ## define historam type (shortcut)
                "Histo  = Gaudi.Histo1DDef"  ,
                ## monitor LL-case
                "massLL     = monitor ( M / GeV ,             Histo ( 'Lambda0, LL-case' , 1.080 , 1.140 , 100 ) , 'M_LL'    ) " ,
                "p1LL       = monitor ( CHILD( P, 1 ) / GeV , Histo ( 'Lambda0, LL-case' , 0.00  , 120   , 100 ) , 'p1_LL'   ) " ,
                "p2LL       = monitor ( CHILD( P, 2 ) / GeV , Histo ( 'Lambda0, LL-case' , 0.00  , 120   , 100 ) , 'p2_LL'   ) " ,
                ]
            _Lam0_LL_Bin2_IsMUON_Filter.Monitor      = True  ,
            _Lam0_LL_Bin2_IsMUON_Filter.HistoProduce = True  ,
            _Lam0_LL_Bin2_IsMUON_Filter.PostMonitor  = """ process ( massLL )
            >> process ( p1LL )
            >> process ( p2LL )
            >> EMPTY
            """

        self.Lam0_LL_Bin2_IsMUON_V0ForPID = Selection(
            "SelLam0_LL_Bin2_ISMUON" + self._name,
            Algorithm          = _Lam0_LL_Bin2_IsMUON_Filter,
            RequiredSelections = [ Lam0 ]
            )
        return self.Lam0_LL_Bin2_IsMUON_V0ForPID

    def Lam0_DD_Bin1 ( self ) :
        """
        Selection for Lam0 DD Bin1
        """
        if hasattr(self, 'Lam0_DD_Bin1_V0ForPID') :
            return self.Lam0_DD_Bin1_V0ForPID
        
        Lam0DD = self.Lam0DD()
        _Lam0_DD_Bin1_Filter = FilterDesktop (
            ##
            #self.name + 'Lam0_DD_Bin1_Filter' ,
            ##
            Code         = self._lam0_bin1Cut_DD()
            )
        
        if self._monitor == True :
            _Lam0_DD_Bin1_Filter.Preambulo    = [
                ## define historam type (shortcut)
                "Histo  = Gaudi.Histo1DDef"  ,
                ## monitor DD-case
                "massDD     = monitor ( M / GeV ,             Histo ( 'Lambda0, DD-case' , 1.080 , 1.140 , 100 ) , 'M_DD'    ) " ,
                "p1DD       = monitor ( CHILD( P, 1 ) / GeV , Histo ( 'Lambda0, DD-case' , 0.00  , 120   , 100 ) , 'p1_DD'   ) " ,
                "p2DD       = monitor ( CHILD( P, 2 ) / GeV , Histo ( 'Lambda0, DD-case' , 0.00  , 120   , 100 ) , 'p2_DD'   ) " ,
                ]
            _Lam0_DD_Bin1_Filter.Monitor      = True  ,
            _Lam0_DD_Bin1_Filter.HistoProduce = True ,
            _Lam0_DD_Bin1_Filter.PostMonitor  = """ process ( massDD )
            >> process ( p1DD )
            >> process ( p2DD )
            >> EMPTY
            """

        self.Lam0_DD_Bin1_V0ForPID = Selection(
            "SelLam0_DD_Bin1" + self._name,
            Algorithm          = _Lam0_DD_Bin1_Filter,
            RequiredSelections = [ Lam0DD ]
            )
        return self.Lam0_DD_Bin1_V0ForPID

    def Lam0_DD_Bin1_IsMUON ( self ) :
        """
        Selection for Lam0 DD Bin1
        """
        if hasattr(self, 'Lam0_DD_Bin1_IsMUON_V0ForPID') :
            return self.Lam0_DD_Bin1_IsMUON_V0ForPID
        
        Lam0DD = self.Lam0DD()
        _Lam0_DD_Bin1_IsMUON_Filter = FilterDesktop (
            ##
            #self.name + 'Lam0_DD_Bin1_Filter' ,
            ##
            Code         = self._lam0_bin1Cut_LL_IsMUON()
            )
        
        if self._monitor == True :
            _Lam0_DD_Bin1_IsMUON_Filter.Preambulo    = [
                ## define historam type (shortcut)
                "Histo  = Gaudi.Histo1DDef"  ,
                ## monitor DD-case
                "massDD     = monitor ( M / GeV ,             Histo ( 'Lambda0, DD-case' , 1.080 , 1.140 , 100 ) , 'M_DD'    ) " ,
                "p1DD       = monitor ( CHILD( P, 1 ) / GeV , Histo ( 'Lambda0, DD-case' , 0.00  , 120   , 100 ) , 'p1_DD'   ) " ,
                "p2DD       = monitor ( CHILD( P, 2 ) / GeV , Histo ( 'Lambda0, DD-case' , 0.00  , 120   , 100 ) , 'p2_DD'   ) " ,
                ]
            _Lam0_DD_Bin1_IsMUON_Filter.Monitor      = True  ,
            _Lam0_DD_Bin1_IsMUON_Filter.HistoProduce = True ,
            _Lam0_DD_Bin1_IsMUON_Filter.PostMonitor  = """ process ( massDD )
            >> process ( p1DD )
            >> process ( p2DD )
            >> EMPTY
            """

        self.Lam0_DD_Bin1_IsMUON_V0ForPID = Selection(
            "SelLam0_DD_Bin1_ISMUON" + self._name,
            Algorithm          = _Lam0_DD_Bin1_IsMUON_Filter,
            RequiredSelections = [ Lam0DD ]
            )
        return self.Lam0_DD_Bin1_IsMUON_V0ForPID

    def trackChi2       ( self ) : return self._trackChi2
    def daughtersIPChi2 ( self ) : return self._daughtersIPChi2
    
    ## define daughter cuts for K0S 
    def _k0s_daughtersCuts ( self ) :
        """
        Define 'DaughtersCuts' for K0S
        """
        cuts = """
        ( 2.0 * GeV < P )
        & ( ISLONG | ISDOWN )
        & ( TRCHI2DOF < %g )
        & ( MIPCHI2DV(PRIMARY) > %g )
        """
        cuts = cuts %( self._trackChi2, self._daughtersIPChi2 )
        return cuts.replace ( '\n' , ' ' ) 
    
    ## define daughter cuts for Lambda0
    def _lam0_daughtersCuts ( self ) :
        """
        Define 'DaughtersCuts' for Lambda0
        """
        return self._k0s_daughtersCuts () 

    ## define "MotherCut" for K0s 
    def _k0s_motherCut      ( self ) :
        """
        Define 'MotherCut' for K0S
        """
        cut       = """
        ( ADMASS ( 'KS0') < %g  )
        & in_range ( 0 , VFASPF ( VCHI2 ) ,  %g   )
        & ( VFASPF ( VZ   ) < %g          )
        & in_range ( 0 , BPVLTFITCHI2()   ,  %g )
        & ( BPVLTIME()*c_light > switch ( DD , %g , %g ) )
        & ( ADWM( 'Lambda0' , WM( 'p+' , 'pi-') ) > switch( DD, %g , %g ) )
        & ( ADWM( 'Lambda0' , WM( 'pi+' , 'p~-') ) > switch( DD, %g , %g ) )
        """
        cut = cut %(self._deltaMassK0S,
                    self._vertexChi2,
                    self._maxZ,
                    self._lTimeFitChi2,
                    self._cTauK0S_DD,
                    self._cTauK0S,
                    self._wrongMassK0S_DD,
                    self._wrongMassK0S,
                    self._wrongMassK0S_DD,
                    self._wrongMassK0S
                    )
        
        return cut.replace ( '\n' , ' ' )  
    
    ## define "MotherCut" for Lambda0
    def _lam0_motherCut      ( self ) :
        """
        Define 'MotherCut' for Lambda0
        """
        cut       = """
        ( ADMASS ( 'Lambda0')   < %g )
        & in_range ( 0 , VFASPF ( VCHI2 ) ,  %g   )
        & ( VFASPF ( VZ       ) < %g            )
        & in_range ( 0 , BPVLTFITCHI2()   ,  %g )
        & ( BPVLTIME()*c_light > switch ( DD , %g , %g ) )
        & ( ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > switch( DD, %g  , %g ) ) 
        """
        cut = cut %(self._deltaMassLambda,
                    self._vertexChi2,
                    self._maxZ,
                    self._lTimeFitChi2,
                    self._cTauLambda0_DD,
                    self._cTauLambda0,
                    self._wrongMassLambda_DD,
                    self._wrongMassLambda
                    )
        
        return cut.replace ( '\n' , ' ' ) 
        

    ## define "Bin1 Cut" for K0S LL
    def _k0s_bin1Cut_LL ( self ) :
        """
        Define 'Bin1 Cut' for K0S LL
        """
        cut       = """
        ALL
        """
        cut = cut % locals()
        return cut.replace ( '\n' , ' ' ) 

    ## define "Bin1 Cut" for Lambda0 LL
    def _lam0_bin1Cut_LL ( self ) :
        """
        Define 'Bin1 Cut' for Lambda0 LL
        """
        cut       = """
        ALL
        """
        cut = cut % locals()
        return cut.replace ( '\n' , ' ' ) 

    ## define "Bin2 Cut" for Lambda0 LL
    def _lam0_bin2Cut_LL ( self ) :
        """
        Define 'Bin2 Cut' for Lambda0 LL
        """
        cut       = """
        (INTREE( (ABSID=='p+') & ( P > 40 * GeV ) ) )
        """
        cut = cut % locals()
        return cut.replace ( '\n' , ' ' ) 


    ## define "Bin1 IsMUON Cut" for Lambda0 LL 
    def _lam0_bin1Cut_LL_IsMUON ( self ) :
        """
        Define 'Bin1 Cut' for Lambda0 LL
        """
        cut       = """
        (INTREE( (ABSID=='p+') & ISMUON ) )
        """
        cut = cut % locals()
        return cut.replace ( '\n' , ' ' ) 

    ## define "Bin1 IsMUON Cut" for Lambda0 LL 
    def _lam0_bin2Cut_LL_IsMUON ( self ) :
        """
        Define 'Bin2 Cut' for Lambda0 LL
        """
        cut       = """
        (INTREE( (ABSID=='p+') & ( ISMUON & (P > 40 * GeV) ) ) )
        """
        cut = cut % locals()
        return cut.replace ( '\n' , ' ' ) 


    ## define "Bin1 Cut" for K0S DD
    def _k0s_bin1Cut_DD ( self ) :
        """
        Define 'Bin1 Cut' for K0S DD
        """
        cut       = """
        ALL
        """
        cut = cut % locals()
        return cut.replace ( '\n' , ' ' ) 

 

    ## define "Bin1 Cut" for Lambda0 DD
    def _lam0_bin1Cut_DD ( self ) :
        """
        Define 'Bin1 Cut' for Lambda0 DD
        """
        cut       = """
        ALL
        """
        cut = cut % locals()
        return cut.replace ( '\n' , ' ' ) 


if '__main__' == __name__ :

    print __doc__
    print 'Author : %s ' % __author__
    print 'Date   : %s ' % __date__
    print __version__

    conf = StrippingV0ForPIDConf()
    
    print conf

    props = [ 'DaughtersCuts'   ,
              'CombinationCut'  ,
              'MotherCut'       ,
              'DecayDescriptor' ,
              'DecayDescriptors'] 
                           
    for l in conf.K0S     () : print __enroll__ ( l , lst = props ) 
    for l in conf.Lambda0 () : print __enroll__ ( l , lst = props )
    

def addRelInfoMuonIsolation( decdes ):    
    import re
    _DauLoc={}
    _daughters = re.match(r'(.*)->([ |\[]*)([^\]]+)(.*)', decdes)
    if _daughters:    
        _particles = _daughters.group(3).split()
        _ip=1
        _gp=1
        for _p in _particles:
            if re.match(r'(pi|p|K|e|mu)[\+|-]',_p):
                _key= _daughters.group(1)+"->"+_daughters.group(2)
                _jp=1
                for _p2 in _particles:
                    _key+=" "
                    if _jp==_ip: _key+="^"
                    _key+=_p2
                    _jp=_jp+1
                _key+=_daughters.group(4).replace("cc","CC")
                _DauLoc[_key] = "MudetIso"+str(_gp)
                _gp=_gp+1
            _ip=_ip+1
    else:
        return {}
    return {  "Type" : "RelInfoMuonIsolation", "DaughterLocations" : _DauLoc}


# =============================================================================
# The END 
# =============================================================================

