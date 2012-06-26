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

>>> from StrippingSelections import StrippingV0ForPID

>>> V0ForPID = StrippingV0ForPID.StrippingV0ForPIDConf ('noPIDV0',StrippingV0ForPID.default_config)

>>> stream.appendLines( V0ForPID.lines() )

Results from running over:
$STRIPPINGSELECTIONSROOT/tests/data/Reco09-Stripping13_SDSTs.py

with prescales above prescales:

StrippingReport                                                INFO Event 70000, Good event 32178
 |                                    *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*| *Errs*|*Incds*| *Slow*|
 |_StrippingGlobal_                                  |  0.7303|       235|       |  21.119|       |       |       |
 |_StrippingSequenceStreamTest_                      |  0.7303|       235|       |  21.112|       |       |       |
 |!StrippingK0SLLLineV0ForPID                        |  0.0963|        31|  1.129|   0.126|      0|      0|      1|
 |!StrippingK0SDDLineV0ForPID                        |  0.1088|        35|  1.057|   0.145|      0|      0|      1|
 |!StrippingLam0LLLine1V0ForPID                      |  0.1367|        44|  1.045|   0.286|      0|      0|      0|
 |!StrippingLam0LLLine2V0ForPID                      |  0.1181|        38|  1.000|   2.306|      0|      0|     25|
 |!StrippingLam0DDLineV0ForPID                       |  0.1026|        33|  1.061|   0.427|      0|      0|      0|
 |!StrippingLam0LLIsMUONLine1V0ForPID                |  0.0746|        24|  1.042|   6.382|      0|      0|     81|
 |!StrippingLam0LLIsMUONLine2V0ForPID                |  0.0062|         2|  1.000|   0.040|      0|      0|      0|
 |!StrippingLam0DDIsMUONLineV0ForPID                 |  0.0994|        32|  1.000|  10.971|      0|      0|      0|

============================================================================================================================================
                                               AlgorithmCorrelationsAlg.AlgorithmCorrelations
============================================================================================================================================
    Algorithm                             Eff.       1        2        3        4        5        6        7        8        9        10  
--------------------------------------------------------------------------------------------------------------------------------------------
  1 StrippingGlobal                       0.730% |  ####### 100.000% 100.000% 100.000% 100.000% 100.000% 100.000% 100.000% 100.000% 100.000%
  2 StrippingSequenceStreamTest           0.730% | 100.000%  ####### 100.000% 100.000% 100.000% 100.000% 100.000% 100.000% 100.000% 100.000%
  3 StrippingK0SLLLineV0ForPID            0.096% |  13.191%  13.191%  #######   0.000%   0.000%   0.000%   0.000%   0.000%   0.000%   0.000%
  4 StrippingK0SDDLineV0ForPID            0.109% |  14.894%  14.894%   0.000%  #######   0.000%   0.000%   0.000%   4.167%   0.000%   0.000%
  5 StrippingLam0LLLine1V0ForPID          0.137% |  18.723%  18.723%   0.000%   0.000%  #######   0.000%   0.000%   0.000%   0.000%   0.000%
  6 StrippingLam0LLLine2V0ForPID          0.118% |  16.170%  16.170%   0.000%   0.000%   0.000%  #######   0.000%   0.000%   0.000%   0.000%
  7 StrippingLam0DDLineV0ForPID           0.103% |  14.043%  14.043%   0.000%   0.000%   0.000%   0.000%  #######   0.000%   0.000%   3.125%
  8 StrippingLam0LLIsMUONLine1V0ForPID    0.075% |  10.213%  10.213%   0.000%   2.857%   0.000%   0.000%   0.000%  ####### 100.000%   0.000%
  9 StrippingLam0LLIsMUONLine2V0ForPID    0.006% |   0.851%   0.851%   0.000%   0.000%   0.000%   0.000%   0.000%   8.333%  #######   0.000%
 10 StrippingLam0DDIsMUONLineV0ForPID     0.099% |  13.617%  13.617%   0.000%   0.000%   0.000%   0.000%   3.030%   0.000%   0.000%  #######
============================================================================================================================================
 
"""
# =============================================================================
__author__  = 'Andrew Powell a.powell1@physics.ox.ac.uk'
__date__    = '2011-05-30'
__version__ = 'CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $'
__all__ = ('StrippingV0ForPIDConf')
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
default_config = {
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
    'KS0LL_Prescale'           : 0.007 ,
    'KS0DD_Prescale'           : 0.008 , 
    'LamLL_Prescale_LoP'       : 0.027 , 
    'LamLL_Prescale_HiP'       : 0.308 ,
    'LamDD_Prescale'           : 0.033 , 
    'LamLLIsMUON_Prescale_LoP' : 1.000 ,
    'LamLLIsMUON_Prescale_HiP' : 1.000 ,
    'LamDDIsMUON_Prescale'     : 1.000   

    }

# =============================================================================
class StrippingV0ForPIDConf(LineBuilder) :    
    """
    Helper class to configure 'V0ForPID'-lines
    """
    __configuration_keys__ = tuple ( default_config.keys() )

    ## get the default configuration 
    #@staticmethod
    def defaultConfiguration( key = None ) :
        
        from copy import deepcopy
        _config = deepcopy ( default_config )
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
            algos      = [ self.K0S_LL_Bin1() ]
            ) ,
            ## 
            StrippingLine (
            "K0SDDLine" + self._name ,
            prescale   = self._KS0DDPrescale ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.K0S_DD_Bin1() ]
            ) ,
            ## 
            StrippingLine (
            "Lam0LLLine1" + self._name ,
            prescale   = self._LamLLPrescale_LoP ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.Lam0_LL_Bin1() ]
            ) ,
            ## 
            StrippingLine (
            "Lam0LLLine2" + self._name ,
            prescale   = self._LamLLPrescale_HiP ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.Lam0_LL_Bin2() ]
            ) ,
            ## 
            StrippingLine (
            "Lam0DDLine" + self._name ,
            prescale   = self._LamDDPrescale ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.Lam0_DD_Bin1() ]
            ),
            ## 
            StrippingLine (
            "Lam0LLIsMUONLine1" + self._name ,
            prescale   = self._LamLLIsMUONPrescale_LoP ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.Lam0_LL_Bin1_IsMUON() ]
            )
            ,
            ## 
            StrippingLine (
            "Lam0LLIsMUONLine2" + self._name ,
            prescale   = self._LamLLIsMUONPrescale_HiP ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.Lam0_LL_Bin2_IsMUON() ]
            )
            ,
            ## 
            StrippingLine (
            "Lam0DDIsMUONLine" + self._name ,
            prescale   = self._LamDDIsMUONPrescale ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            #HLT        = self._hlt , 
            algos      = [ self.Lam0_DD_Bin1_IsMUON() ]
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
        

        self.Lam0_LL = Selection("SelLam02PPi_LL",
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
        self.Lam0_DD = Selection("SelLam02PiPi_DD",
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
    
# =============================================================================
# The END 
# =============================================================================

