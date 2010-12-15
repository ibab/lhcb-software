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
# Acknowledgemens:
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

Configuration file for V0 ( K0S and  Lambda0) stripping.

This stripping is intended for 2009 data, and probably
 it can serve as some starting point for refined stripping for 2010 data

Acknowledgemens:

   - Matt & Manuel for track quality definition
   - Matt for maximum-Z cut 

For more information see:

  http://indico.cern.ch/conferenceDisplay.py?confId=74536
  http://indico.cern.ch/materialDisplay.py?contribId=13&sessionId=2&materialId=0&confId=78767

Usage of configurable is fairly trivial:

  v0 = StrippingV0ForPIDConf ( .... )

Get the list of all configured lines for K0S :

  lines_for_K0S_LL     =  v0.K0S_LL_Lines()
  lines_for_K0S_DD     =  v0.K0S_DD_Lines()

Get the list of all configured lines for Lambda0 :

  lines_for_Lambda0_LL =  v0.Lam0_LL_Lines()
  lines_for_Lambda0_DD =  v0.Lam0_DD_Lines()

Get the list of *ALL* configured lines
    
   all_lines =  v0.All_Lines()
    

"""
# =============================================================================
__author__  = 'Andrew Powell a.powell1@physics.ox.ac.uk'
__date__    = '2010-01-14'
__version__ = 'CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $'
# =============================================================================

from Gaudi.Configuration       import *
from LHCbKernel.Configuration  import *
from GaudiKernel.SystemOfUnits import mm, cm , MeV, GeV

# =============================================================================
# @class StrippingV0ForPIDConf
#
# Configurable for PID V0 ( K0S and  Lambda0) stripping.
##
# @author Andrew Powell a.powell1@physics.ox.ac.uk
# @date 2010-09-05
# =============================================================================
from StrippingConf.Configuration import __enroll__
from Configurables import CombineParticles, FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand        

# =============================================================================

class StrippingV0ForPIDConf(LHCbConfigurableUser):
    """
    Configurable for PID V0 ( K0S and  Lambda0 ) stripping.
    
    Usage of configurable is fairly trivial:
    
       v0 = StrippingV0ForPIDConf ( .... )
    
    Get the list of all K0S Long-Long configured lines:
    
       lines_for_K0S_LL     =  v0.K0S_LL_Lines()
    
    Get the list of all K0S Down-Down configured lines:
    
       lines_for_K0S_DD     =  v0.K0S_DD_Lines()
    
    Get the list of all Lambda0 Long-Long configured lines:
    
       lines_for_Lambda0_LL =  v0.Lam0_LL_Lines()
    
    Get the list of all Lambda0 Down-Down configured lines:
    
       lines_for_Lambda0_DD =  v0.Lam0_DD_Lines()
    
    Get the list of *ALL* configured lines
    
       all_lines =  v0.All_Lines ()
    
       """
    __slots__ = { 
        'TrackQuality'           :  5                 ## Track Chi2/ndof quality 
        , 'VertexChi2'           :  9                 ## Cut on Vertex chi2-quality
        , 'DeltaMassK0S'         :  50 * MeV          ## Mass-window (half)-width for K0S 
        , 'DeltaMassLambda'      :  25 * MeV          ## Mass-window (half)-width for Lambda 
        , 'MaxZ'                 : 220 * cm           ## Maximal allowed vertex Z-position
        , 'DaughtersIPChi2'      :  25                ## minimal IP-chi2 for daughter particles  
        , 'LTimeFitChi2'         :  9                 ## Chi2 from LifeTime Fitter
        ##
        , 'WrongMassK0S'         :   9 * MeV          ## Mass-window (half)-width for ppi hypthesis
        , 'WrongMassK0S_DD'      :  18 * MeV          ## Mass-window (half)-width for ppi hypthesis (DD-case)
        ##
        , 'WrongMassLambda'      :  20 * MeV          ## Mass-window (half)-width for pipi hypthesis
        , 'WrongMassLambda_DD'   :  40 * MeV          ## Mass-window (half)-width for pipi hypthesis (DD-case)
        ##
        , 'CTauK0S'              :   1 * mm           ## Cut on c*tau for K0S 
        , 'CTauK0S_DD'           :  10 * mm           ## Cut on c*tau for K0S (DD-case)
        ##
        , 'CTauLambda0'          :   5 * mm           ## Cut on c*tau for Lambda0
        , 'CTauLambda0_DD'       :  20 * mm           ## Cut on c*tau for Lambda0 (DD-case)
        ## 
        , 'Monitor'              : False              ## Activate the monitoring ?
        ##
        , "HLT"                  : "HLT_PASS_RE('Hlt1MB.*Decision')" ## HLT-cut
        ##
        , 'K0SBinA_MaxPT_LL'     : 600 * MeV          ## Max PT of K0S in BinA
        , 'K0SBinA_MinPT_LL'     : 250 * MeV          ## Min PT of K0S in BinA
        , 'K0SBinA_MaxY_LL'      : 3.8                ## Max Y of K0S in BinA
        , 'K0SBinA_MinY_LL'      : 3.2                ## Min Y of K0S in BinA
        , 'K0SBinB_MaxPT_LL'     : 1200 * MeV         ## Max PT of K0S in BinB
        , 'K0SBinB_MinPT_LL'     : 250 * MeV          ## Min PT of K0S in BinB
        , 'K0SBinB_MaxY_LL'      : 4.4                ## Max Y of K0S in BinB
        , 'K0SBinB_MinY_LL'      : 2.4                ## Min Y of K0S in BinB
        , 'K0SBinC_MinPT_LL'     : 50 * MeV           ## Min PT of K0S in BinC
        , 'K0SBinC_MaxY_LL'      : 4.2                ## Max Y of K0S in BinC
        , 'K0SBinC_MinY_LL'      : 2.6                ## Min Y of K0S in BinC
        ##
        , 'K0SBin1_LLPrescale'   : 0.019                ## Prescale K0S LL-combinations in Bin1
        , 'K0SBin2_LLPrescale'   : 0.029                ## Prescale K0S LL-combinations in Bin2
        , 'K0SBin3_LLPrescale'   : 0.116                ## Prescale K0S LL-combinations in Bin3
        ##
        , 'Lambda0BinA_MaxPT_LL'     : 1000 * MeV         ## Max PT of Lambda0 in BinA
        , 'Lambda0BinA_MinPT_LL'     : 400 * MeV          ## Min PT of Lambda0 in BinA
        , 'Lambda0BinA_MaxY_LL'      : 3.4                ## Max Y of Lambda0 in BinA
        , 'Lambda0BinA_MinY_LL'      : 2.8                ## Min Y of Lambda0 in BinA
        , 'Lambda0BinB_MaxPT_LL'     : 1800 * MeV         ## Max PT of Lambda0 in BinB
        , 'Lambda0BinB_MinPT_LL'     : 400 * MeV         ## Min PT of Lambda0 in BinB
        , 'Lambda0BinB_MaxY_LL'      : 4.0                ## Max Y of Lambda0 in BinB
        , 'Lambda0BinB_MinY_LL'      : 2.4                ## Min Y of Lambda0 in BinB
        , 'Lambda0BinC_MinPT_LL'     : 150 * MeV           ## Min PT of Lambda0 in BinC
        , 'Lambda0BinC_MaxY_LL'      : 3.8                ## Max Y of Lambda0 in BinC
        , 'Lambda0BinC_MinY_LL'      : 2.6                ## Min Y of Lambda0 in BinC
        ##
        , 'Lambda0Bin1_LLPrescale'   : 0.128                ## Prescale Lambda0 LL-combinations in Bin1
        , 'Lambda0Bin2_LLPrescale'   : 0.179                ## Prescale Lambda0 LL-combinations in Bin2
        , 'Lambda0Bin3_LLPrescale'   : 0.893                ## Prescale K0S LL-combinations in Bin3
        ##
        , 'K0SBinA_MaxPT_DD'         : 1200 * MeV         ## Max PT of K0S in BinA
        , 'K0SBinA_MinPT_DD'         : 350 * MeV          ## Min PT of K0S in BinA
        , 'K0SBinA_MaxY_DD'          : 4.3                ## Max Y of K0S in BinA
        , 'K0SBinA_MinY_DD'          : 2.5                ## Min Y of K0S in BinA
        , 'K0SBinB_MinPT_DD'         : 50 * MeV           ## Min PT of K0S in BinB
        , 'K0SBinB_MaxY_DD'          : 3.9                ## Max Y of K0S in BinB
        , 'K0SBinB_MinY_DD'          : 2.8                ## Min Y of K0S in BinB
        ##
        , 'K0SBin1_DDPrescale'       : 0.028                ## Prescale K0S DD-combinations in Bin1   
        , 'K0SBin2_DDPrescale'       : 0.113                ## Prescale K0S DD-combinations in Bin2   
        ##
        , 'Lambda0BinA_MaxPT_DD'     : 1500 * MeV         ## Max PT of Lambda0 in BinA
        , 'Lambda0BinA_MinPT_DD'     : 600 * MeV          ## Min PT of Lambda0 in BinA
        , 'Lambda0BinA_MaxY_DD'      : 4.0                ## Max Y of Lambda0 in BinA
        , 'Lambda0BinA_MinY_DD'      : 2.6                ## Min Y of Lambda0 in BinA
        , 'Lambda0BinB_MinPT_DD'     : 300 * MeV          ## Min PT of Lambda0 in BinB
        , 'Lambda0BinB_MaxY_DD'      : 3.7                ## Max Y of Lambda0 in BinB
        , 'Lambda0BinB_MinY_DD'      : 2.6                ## Min Y of Lambda0 in BinB
        , 'Lambda0BinC_MinPT_DD'     : 100 * MeV          ## Min PT of Lambda0 in BinC
        , 'Lambda0BinC_MaxY_DD'      : 3.3                ## Max Y of Lambda0 in BinC
        , 'Lambda0BinC_MinY_DD'      : 2.6                ## Min Y of Lambda0 in BinC
        ##
        , 'Lambda0Bin1_DDPrescale'   : 0.134                ## Prescale Lambda0 DD-combinations in Bin1
        , 'Lambda0Bin2_DDPrescale'   : 0.670                ## Prescale Lambda0 DD-combinations in Bin2
                
        }
    
    _propertyDocDct = {  
        'TrackQuality'            : """ Track quality selection                  """ 
        , 'VertexChi2'            : """ Cut on Vertex chi2-quality               """ 
        , 'DeltaMassK0S'          : """ Mass-window (half)width for K0S          """ 
        , 'DeltaMassLambda0'      : """ Mass-window (half)width for Lambda0      """ 
        , 'MaxZ'                  : """ Maximal allowed vertex Z-position        """ 
        , 'LTimeFitChi2'          : """ Chi2 from LifeTime Fitter                """
        , 'WrongMassK0S'          : """ Mass-window (half)-width under ppi hypthesis """
        , 'WrongMassK0S_DD'       : """ Mass-window (half)-width under ppi hypthesis for DD-case"""
        , 'WrongMassLambda'       : """ Mass-window (half)-width under pipi hypthesis """
        , 'WrongMassLambda_DD'    : """ Mass-window (half)-width under pipi hypthesis for DD-case"""
        , 'CTau'                  : """ Cut on c*tau                             """
        , 'CTauForDD'             : """ Cut on c*tau cut for DD-case             """
        , 'Monitor'               : """ Activate the monitoring ?                """
        , 'HLT'                   : """ LoKi functor, acting on HLT-lines        """
        , 'K0SBinA_MaxPT_LL'      : """ Max PT of K0S in BinA                    """
        , 'K0SBinA_MinPT_LL'      : """ Min PT of K0S in BinA                    """
        , 'K0SBinA_MaxY_LL'       : """ Max Y of K0S in BinA                     """
        , 'K0SBinA_MinY_LL'       : """ Min Y of K0S in BinA                     """
        , 'K0SBinB_MaxPT_LL'      : """ Max PT of K0S in BinB                    """
        , 'K0SBinB_MaxY_LL'       : """ Max Y of K0S in BinB                     """
        , 'K0SBinB_MinY_LL'       : """ Min Y of K0S in BinB                     """
        , 'K0SBin1_LLPrescale'    : """ Prescale K0S LL-combinations in Bin1     """
        , 'K0SBin2_LLPrescale'    : """ Prescale K0S LL-combinations in Bin2     """
        , 'K0SBin3_LLPrescale'    : """ Prescale K0S LL-combinations in Bin3     """
        , 'LambdaBinA_MaxPT_LL'   : """ Max PT of Lambda in BinA                    """
        , 'LambdaBinA_MinPT_LL'   : """ Min PT of Lambda in BinA                    """
        , 'LambdaBinA_MaxY_LL'    : """ Max Y of Lambda in BinA                     """
        , 'LambdaBinA_MinY_LL'    : """ Min Y of Lambda in BinA                     """
        , 'LambdaBinB_MaxPT_LL'   : """ Max PT of Lambda in BinB                    """
        , 'LambdaBinB_MaxY_LL'    : """ Max Y of Lambda in BinB                     """
        , 'LambdaBinB_MinY_LL'    : """ Min Y of Lambda in BinB                     """
        , 'LambdaBin1_LLPrescale' : """ Prescale Lambda LL-combinations in Bin1     """
        , 'LambdaBin2_LLPrescale' : """ Prescale Lambda LL-combinations in Bin2     """
        , 'LambdaBin3_LLPrescale' : """ Prescale Lambda LL-combinations in Bin3     """
        
        }

    selK0S = None    
    def K0S ( self ) :
        """
        Selection for K0S 
        """
        if StrippingV0ForPIDConf.selK0S == None :
            Pions = DataOnDemand( Location = "Phys/StdNoPIDsPions" )
            _K0S = CombineParticles("StripK0S")
            _K0S.DecayDescriptor = "KS0 -> pi+ pi-"
            _K0S.DaughtersCuts   = self._k0s_daughtersCuts ()
            _K0S.CombinationCut  = "AM < 1.0 * GeV " 
            _K0S.MotherCut       = self._k0s_motherCut()        
            _K0S.Preambulo       = self._preambulo()
            
            if self.getProp ( 'Monitor' ) :    
                _K0S.Preambulo    += self._k0s_histos() 
                _K0S.Monitor       = True
                _K0S.HistoProduce  = True
                _K0S.HistoPrint    = True
                _K0S.MotherMonitor = """ process ( switch ( LL , massLL , massDD ) )
                >> process ( switch ( LL , pTLL , pTDD ) )
                >> process ( switch ( LL , yLL , yDD ) )
                >> EMPTY
                """
            K0S = Selection("SelK0S2PiPi_LL",
                            Algorithm = _K0S,
                            RequiredSelections = [ Pions ]) 

            StrippingV0ForPIDConf.selK0S = K0S    
        return StrippingV0ForPIDConf.selK0S

    selK0SDD = None    
    def K0SDD ( self ) :
        """
        Selection for K0S DD 
        """
        if StrippingV0ForPIDConf.selK0SDD == None :
            Pions = DataOnDemand( Location = "Phys/StdNoPIDsDownPions" )
            _K0S = CombineParticles("StripK0S_DD")
            _K0S.DecayDescriptor = "KS0 -> pi+ pi-"
            _K0S.DaughtersCuts   = self._k0s_daughtersCuts () 
            _K0S.CombinationCut  = "AM < 1.0 * GeV " 
            _K0S.MotherCut       = self._k0s_motherCut()        
            _K0S.Preambulo       = self._preambulo()
            
            if self.getProp ( 'Monitor' ) :    
                _K0S.Preambulo    += self._k0s_histos() 
                _K0S.Monitor       = True
                _K0S.HistoProduce  = True
                _K0S.HistoPrint    = True
                _K0S.MotherMonitor = """ process ( switch ( LL , massLL , massDD ) )
                >> process ( switch ( LL , pTLL , pTDD ) )
                >> process ( switch ( LL , yLL , yDD ) )
                >> EMPTY
                """
            K0SDD = Selection("SelK0S2PiPi_DD",
                            Algorithm = _K0S,
                            RequiredSelections = [ Pions ]) 

            StrippingV0ForPIDConf.selK0SDD = K0SDD    
        return StrippingV0ForPIDConf.selK0SDD

    selLam0 = None    
    def Lam0 ( self ) :
        """
        Selection for Lam0 
        """
        if StrippingV0ForPIDConf.selLam0 == None :
            Pions = DataOnDemand( Location = "Phys/StdNoPIDsPions" )
            Protons = DataOnDemand( Location = "Phys/StdNoPIDsProtons" )
            _Lam0 = CombineParticles("StripLam0")
            _Lam0.DecayDescriptor = "[ Lambda0 -> p+ pi-]cc"
            _Lam0.DaughtersCuts   = self._lam0_daughtersCuts () 
            _Lam0.CombinationCut  = "AM < 1.5 * GeV " 
            _Lam0.MotherCut       = self._lam0_motherCut()        
            _Lam0.Preambulo       = self._preambulo()
            
            if self.getProp ( 'Monitor' ) :    
                _Lam0.Preambulo    += self._lam0_histos() 
                _Lam0.Monitor       = True
                _Lam0.HistoProduce  = True
                _Lam0.HistoPrint    = True
                _Lam0.MotherMonitor = """ process ( switch ( LL , massLL , massDD ) )
                >> process ( switch ( LL , pTLL , pTDD ) )
                >> process ( switch ( LL , yLL , yDD ) )
                >> EMPTY
                """
            Lam0 = Selection("SelLam02PPi_LL",
                            Algorithm = _Lam0,
                            RequiredSelections = [ Pions, Protons ]) 

            StrippingV0ForPIDConf.selLam0 = Lam0    
        return StrippingV0ForPIDConf.selLam0

    selLam0DD = None    
    def Lam0DD ( self ) :
        """
        Selection for Lam0 DD 
        """
        if StrippingV0ForPIDConf.selLam0DD == None :
            Pions = DataOnDemand( Location = "Phys/StdNoPIDsDownPions" )
            Protons = DataOnDemand( Location = "Phys/StdNoPIDsDownProtons" )
            _Lam0 = CombineParticles("StripLam0_DD")
            _Lam0.DecayDescriptor = "[ Lambda0 -> p+ pi-]cc"
            _Lam0.DaughtersCuts   = self._lam0_daughtersCuts () 
            _Lam0.CombinationCut  = "AM < 1.5 * GeV " 
            _Lam0.MotherCut       = self._lam0_motherCut()        
            _Lam0.Preambulo       = self._preambulo()
            
            if self.getProp ( 'Monitor' ) :    
                _Lam0.Preambulo    += self._lam0_histos() 
                _Lam0.Monitor       = True
                _Lam0.HistoProduce  = True
                _Lam0.HistoPrint    = True
                _Lam0.MotherMonitor = """ process ( switch ( LL , massLL , massDD ) )
                >> process ( switch ( LL , pTLL , pTDD ) )
                >> process ( switch ( LL , yLL , yDD ) )
                >> EMPTY
                """
            Lam0DD = Selection("SelLam02PiPi_DD",
                            Algorithm = _Lam0,
                            RequiredSelections = [ Pions , Protons]) 

            StrippingV0ForPIDConf.selLam0DD = Lam0DD    
        return StrippingV0ForPIDConf.selLam0DD

    def K0S_LL_Bin1 ( self ) :
        """
        Selection for K0S LL Bin1
        """
        K0S = self.K0S()
        _K0S_LL_Bin1_Filter = FilterDesktop (
            'K0S_LL_Bin1_Filter' ,
            Code = self._k0s_bin1Cut_LL()
            )

        if self.getProp( 'Monitor' ) :
            _K0S_LL_Bin1_Filter.Preambulo += self._k0s_histos()
            _K0S_LL_Bin1_Filter.Monitor = True
            _K0S_LL_Bin1_Filter.HistoProduce = True
            _K0S_LL_Bin1_Filter.PostMonitor = """ process ( switch ( LL , massLL , massDD ) )
            >> process ( switch ( LL , pTLL , pTDD ) )
            >> process ( switch ( LL , yLL , yDD ) )
            >> EMPTY
            """
            
        K0S_LL_Bin1 = Selection(
            "SelK0S_LL_Bin1",
            Algorithm = _K0S_LL_Bin1_Filter,
            RequiredSelections = [ K0S ]
            )
        return K0S_LL_Bin1

    def K0S_LL_Bin2 ( self ) :
        """
        Selection for K0S LL Bin2
        """
        K0S = self.K0S()
        _K0S_LL_Bin2_Filter = FilterDesktop (
            'K0S_LL_Bin2_Filter' ,
            Code = self._k0s_bin2Cut_LL()
            )

        if self.getProp( 'Monitor' ) :
            _K0S_LL_Bin2_Filter.Preambulo += self._k0s_histos()
            _K0S_LL_Bin2_Filter.Monitor = True
            _K0S_LL_Bin2_Filter.HistoProduce = True
            _K0S_LL_Bin2_Filter.PostMonitor = """ process ( switch ( LL , massLL , massDD ) )
            >> process ( switch ( LL , pTLL , pTDD ) )
            >> process ( switch ( LL , yLL , yDD ) )
            >> EMPTY
            """
        
        K0S_LL_Bin2 = Selection(
            "SelK0S_LL_Bin2",
            Algorithm = _K0S_LL_Bin2_Filter,
            RequiredSelections = [ K0S ]
            )
        return K0S_LL_Bin2


    def K0S_LL_Bin3 ( self ) :
        """
        Selection for K0S LL Bin3
        """
        K0S = self.K0S()
        _K0S_LL_Bin3_Filter = FilterDesktop (
            'K0S_LL_Bin3_Filter' ,
            Code = self._k0s_bin3Cut_LL()
            )

        if self.getProp( 'Monitor' ) :
            _K0S_LL_Bin3_Filter.Preambulo += self._k0s_histos()
            _K0S_LL_Bin3_Filter.Monitor = True
            _K0S_LL_Bin3_Filter.HistoProduce = True
            _K0S_LL_Bin3_Filter.PostMonitor = """ process ( switch ( LL , massLL , massDD ) )
            >> process ( switch ( LL , pTLL , pTDD ) )
            >> process ( switch ( LL , yLL , yDD ) )
            >> EMPTY
            """
        
        K0S_LL_Bin3 = Selection(
            "SelK0S_LL_Bin3",
            Algorithm = _K0S_LL_Bin3_Filter,
            RequiredSelections = [ K0S ]
            )
        return K0S_LL_Bin3

    def K0S_LL_Bin1_Line ( self ) :
        """
        SelectionLine for K0S LL Bin1
        """
        from StrippingConf.StrippingLine import StrippingLine
        K0S_LL_Bin1_Sel = self.K0S_LL_Bin1()

        return StrippingLine (
            'K0SLine_1' ,
            prescale   = self.getProp('K0SBin1_LLPrescale') ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            HLT        = self.getProp('HLT') , 
            algos      = [ K0S_LL_Bin1_Sel ]

            )
    
    def K0S_LL_Bin2_Line ( self ) :
        """
        SelectionLine for K0S LL Bin2
        """
        from StrippingConf.StrippingLine import StrippingLine
        K0S_LL_Bin2_Sel = self.K0S_LL_Bin2()

        return StrippingLine (
            'K0SLine_2' ,
            prescale   = self.getProp('K0SBin2_LLPrescale') ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            HLT        = self.getProp('HLT') , 
            algos      = [ K0S_LL_Bin2_Sel ]
            )

    def K0S_LL_Bin3_Line ( self ) :
        """
        SelectionLine for K0S LL Bin3
        """
        from StrippingConf.StrippingLine import StrippingLine
        K0S_LL_Bin3_Sel = self.K0S_LL_Bin3()
        
        return StrippingLine (
            'K0SLine_3' ,
            prescale   = self.getProp('K0SBin3_LLPrescale') ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            HLT        = self.getProp('HLT') , 
            algos      = [ K0S_LL_Bin3_Sel ]
            )

    def K0S_DD_Bin1 ( self ) :
        """
        Selection for K0S DD Bin1
        """
        K0SDD = self.K0SDD()
        _K0S_DD_Bin1_Filter = FilterDesktop (
            'K0S_DD_Bin1_Filter' ,
            Code = self._k0s_bin1Cut_DD()
            )

        if self.getProp( 'Monitor' ) :
            _K0S_DD_Bin1_Filter.Preambulo += self._k0s_histos()
            _K0S_DD_Bin1_Filter.Monitor = True
            _K0S_DD_Bin1_Filter.HistoProduce = True
            _K0S_DD_Bin1_Filter.PostMonitor = """ process ( switch ( LL , massLL , massDD ) )
            >> process ( switch ( LL , pTLL , pTDD ) )
            >> process ( switch ( LL , yLL , yDD ) )
            >> EMPTY
            """

        K0S_DD_Bin1 = Selection(
            "SelK0S_DD_Bin1",
            Algorithm = _K0S_DD_Bin1_Filter,
            RequiredSelections = [ K0SDD ]
            )
        return K0S_DD_Bin1

    def K0S_DD_Bin2 ( self ) :
        """
        Selection for K0S DD Bin2
        """
        K0SDD = self.K0SDD()
        _K0S_DD_Bin2_Filter = FilterDesktop (
            'K0S_DD_Bin2_Filter' ,
            Code = self._k0s_bin2Cut_DD()
            )

        if self.getProp( 'Monitor' ) :
            _K0S_DD_Bin2_Filter.Preambulo += self._k0s_histos()
            _K0S_DD_Bin2_Filter.Monitor = True
            _K0S_DD_Bin2_Filter.HistoProduce = True
            _K0S_DD_Bin2_Filter.PostMonitor = """ process ( switch ( LL , massLL , massDD ) )
            >> process ( switch ( LL , pTLL , pTDD ) )
            >> process ( switch ( LL , yLL , yDD ) )
            >> EMPTY
            """
        
        K0S_DD_Bin2 = Selection(
            "SelK0S_DD_Bin2",
            Algorithm = _K0S_DD_Bin2_Filter,
            RequiredSelections = [ K0SDD ]
            )
        return K0S_DD_Bin2

    def K0S_DD_Bin1_Line ( self ) :
        """
        SelectionLine for K0S DD Bin1
        """
        from StrippingConf.StrippingLine import StrippingLine
        K0S_DD_Bin1_Sel = self.K0S_DD_Bin1()

        return StrippingLine (
            'K0SDDLine_1' ,
            prescale   = self.getProp('K0SBin1_DDPrescale') ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            HLT        = self.getProp('HLT') , 
            algos      = [ K0S_DD_Bin1_Sel ]

            )
    
    def K0S_DD_Bin2_Line ( self ) :
        """
        SelectionLine for K0S DD Bin2
        """
        from StrippingConf.StrippingLine import StrippingLine
        K0S_DD_Bin2_Sel = self.K0S_DD_Bin2()

        return StrippingLine (
            'K0SDDLine_2' ,
            prescale   = self.getProp('K0SBin2_DDPrescale') ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            HLT        = self.getProp('HLT') , 
            algos      = [ K0S_DD_Bin2_Sel ]
            )


    def K0S_LL_Lines ( self ) :

        lines = []

        lines.append( self.K0S_LL_Bin1_Line() )
        lines.append( self.K0S_LL_Bin2_Line() )
        lines.append( self.K0S_LL_Bin3_Line() )

        return lines

    def K0S_DD_Lines ( self ) :
        
        lines = []

        lines.append( self.K0S_DD_Bin1_Line() )
        lines.append( self.K0S_DD_Bin2_Line() )

        return lines
    
    def Lam0_LL_Bin1 ( self ) :
        """
        Selection for Lam0 LL Bin1
        """
        Lam0 = self.Lam0()
        _Lam0_LL_Bin1_Filter = FilterDesktop (
            'Lam0_LL_Bin1_Filter' ,
            Code = self._lam0_bin1Cut_LL()
            )

        if self.getProp( 'Monitor' ) :
            _Lam0_LL_Bin1_Filter.Preambulo += self._lam0_histos()
            _Lam0_LL_Bin1_Filter.Monitor = True
            _Lam0_LL_Bin1_Filter.HistoProduce = True
            _Lam0_LL_Bin1_Filter.PostMonitor = """ process ( switch ( LL , massLL , massDD ) )
            >> process ( switch ( LL , pTLL , pTDD ) )
            >> process ( switch ( LL , yLL , yDD ) )
            >> EMPTY
            """
        
        Lam0_LL_Bin1 = Selection(
            "SelLam0_LL_Bin1",
            Algorithm = _Lam0_LL_Bin1_Filter,
            RequiredSelections = [ Lam0 ]
            )
        return Lam0_LL_Bin1

    def Lam0_LL_Bin2 ( self ) :
        """
        Selection for Lam0 LL Bin2
        """
        Lam0 = self.Lam0()
        _Lam0_LL_Bin2_Filter = FilterDesktop (
            'Lam0_LL_Bin2_Filter' ,
            Code = self._lam0_bin2Cut_LL()
            )

        if self.getProp( 'Monitor' ) :
            _Lam0_LL_Bin2_Filter.Preambulo += self._lam0_histos()
            _Lam0_LL_Bin2_Filter.Monitor = True
            _Lam0_LL_Bin2_Filter.HistoProduce = True
            _Lam0_LL_Bin2_Filter.PostMonitor = """ process ( switch ( LL , massLL , massDD ) )
            >> process ( switch ( LL , pTLL , pTDD ) )
            >> process ( switch ( LL , yLL , yDD ) )
            >> EMPTY
            """
        
        Lam0_LL_Bin2 = Selection(
            "SelLam0_LL_Bin2",
            Algorithm = _Lam0_LL_Bin2_Filter,
            RequiredSelections = [ Lam0 ]
            )
        return Lam0_LL_Bin2


    def Lam0_LL_Bin3 ( self ) :
        """
        Selection for Lam0 LL Bin3
        """
        Lam0 = self.Lam0()
        _Lam0_LL_Bin3_Filter = FilterDesktop (
            'Lam0_LL_Bin3_Filter' ,
            Code = self._lam0_bin3Cut_LL()
            )

        if self.getProp( 'Monitor' ) :
            _Lam0_LL_Bin3_Filter.Preambulo += self._lam0_histos()
            _Lam0_LL_Bin3_Filter.Monitor = True
            _Lam0_LL_Bin3_Filter.HistoProduce = True
            _Lam0_LL_Bin3_Filter.PostMonitor = """ process ( switch ( LL , massLL , massDD ) )
            >> process ( switch ( LL , pTLL , pTDD ) )
            >> process ( switch ( LL , yLL , yDD ) )
            >> EMPTY
            """
        
        Lam0_LL_Bin3 = Selection(
            "SelLam0_LL_Bin3",
            Algorithm = _Lam0_LL_Bin3_Filter,
            RequiredSelections = [ Lam0 ]
            )
        return Lam0_LL_Bin3

    def Lam0_LL_Bin1_Line ( self ) :
        """
        SelectionLine for Lam0 LL Bin1
        """
        from StrippingConf.StrippingLine import StrippingLine
        Lam0_LL_Bin1_Sel = self.Lam0_LL_Bin1()

        return StrippingLine (
            'Lam0Line_1' ,
            prescale   = self.getProp('Lambda0Bin1_LLPrescale') ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            HLT        = self.getProp('HLT') , 
            algos      = [ Lam0_LL_Bin1_Sel ]

            )
    
    def Lam0_LL_Bin2_Line ( self ) :
        """
        SelectionLine for Lam0 LL Bin2
        """
        from StrippingConf.StrippingLine import StrippingLine
        Lam0_LL_Bin2_Sel = self.Lam0_LL_Bin2()

        return StrippingLine (
            'Lam0Line_2' ,
            prescale   = self.getProp('Lambda0Bin2_LLPrescale') ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            HLT        = self.getProp('HLT') , 
            algos      = [ Lam0_LL_Bin2_Sel ]
            )

    def Lam0_LL_Bin3_Line ( self ) :
        """
        SelectionLine for Lam0 LL Bin3
        """
        from StrippingConf.StrippingLine import StrippingLine
        Lam0_LL_Bin3_Sel = self.Lam0_LL_Bin3()
        
        return StrippingLine (
            'Lam0Line_3' ,
            prescale   = self.getProp('Lambda0Bin3_LLPrescale') ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            HLT        = self.getProp('HLT') , 
            algos      = [ Lam0_LL_Bin3_Sel ]
            )

    def Lam0_DD_Bin1 ( self ) :
        """
        Selection for Lam0 DD Bin1
        """
        Lam0DD = self.Lam0DD()
        _Lam0_DD_Bin1_Filter = FilterDesktop (
            'Lam0_DD_Bin1_Filter' ,
            Code = self._lam0_bin1Cut_DD()
            )

        if self.getProp( 'Monitor' ) :
            _Lam0_DD_Bin1_Filter.Preambulo += self._lam0_histos()
            _Lam0_DD_Bin1_Filter.Monitor = True
            _Lam0_DD_Bin1_Filter.HistoProduce = True
            _Lam0_DD_Bin1_Filter.PostMonitor = """ process ( switch ( LL , massLL , massDD ) )
            >> process ( switch ( LL , pTLL , pTDD ) )
            >> process ( switch ( LL , yLL , yDD ) )
            >> EMPTY
            """
        
        Lam0_DD_Bin1 = Selection(
            "SelLam0_DD_Bin1",
            Algorithm = _Lam0_DD_Bin1_Filter,
            RequiredSelections = [ Lam0DD ]
            )
        return Lam0_DD_Bin1

    def Lam0_DD_Bin2 ( self ) :
        """
        Selection for Lam0 DD Bin2
        """
        Lam0DD = self.Lam0DD()
        _Lam0_DD_Bin2_Filter = FilterDesktop (
            'Lam0_DD_Bin2_Filter' ,
            Code = self._lam0_bin2Cut_DD()
            )

        if self.getProp( 'Monitor' ) :
            _Lam0_DD_Bin2_Filter.Preambulo += self._lam0_histos()
            _Lam0_DD_Bin2_Filter.Monitor = True
            _Lam0_DD_Bin2_Filter.HistoProduce = True
            _Lam0_DD_Bin2_Filter.PostMonitor = """ process ( switch ( LL , massLL , massDD ) )
            >> process ( switch ( LL , pTLL , pTDD ) )
            >> process ( switch ( LL , yLL , yDD ) )
            >> EMPTY
            """
        
        Lam0_DD_Bin2 = Selection(
            "SelLam0_DD_Bin2",
            Algorithm = _Lam0_DD_Bin2_Filter,
            RequiredSelections = [ Lam0DD ]
            )
        return Lam0_DD_Bin2

    def Lam0_DD_Bin1_Line ( self ) :
        """
        SelectionLine for Lam0 DD Bin1
        """
        from StrippingConf.StrippingLine import StrippingLine
        Lam0_DD_Bin1_Sel = self.Lam0_DD_Bin1()

        return StrippingLine (
            'Lam0DDLine_1' ,
            prescale   = self.getProp('Lambda0Bin1_DDPrescale') ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            HLT        = self.getProp('HLT') , 
            algos      = [ Lam0_DD_Bin1_Sel ]

            )
    
    def Lam0_DD_Bin2_Line ( self ) :
        """
        SelectionLine for Lam0 DD Bin2
        """
        from StrippingConf.StrippingLine import StrippingLine
        Lam0_DD_Bin2_Sel = self.Lam0_DD_Bin2()

        return StrippingLine (
            'Lam0DDLine_2' ,
            prescale   = self.getProp('Lambda0Bin2_DDPrescale') ,
            checkPV    = True , ## attention! PV is required!
            postscale  = 1 ,
            HLT        = self.getProp('HLT') , 
            algos      = [ Lam0_DD_Bin2_Sel ]
            )


    def Lam0_LL_Lines ( self ) :

        lines = []

        lines.append( self.Lam0_LL_Bin1_Line() )
        lines.append( self.Lam0_LL_Bin2_Line() )
        lines.append( self.Lam0_LL_Bin3_Line() )

        return lines

    def Lam0_DD_Lines ( self ) :
        
        lines = []

        lines.append( self.Lam0_DD_Bin1_Line() )
        lines.append( self.Lam0_DD_Bin2_Line() )

        return lines

    def All_Lines ( self ) :

	allLines = []
        allLines += self.K0S_LL_Lines()
	allLines += self.K0S_DD_Lines()
	allLines += self.Lam0_LL_Lines()
	allLines += self.Lam0_DD_Lines()
	return allLines

    
    ## define daughter cuts for K0S 
    def _k0s_daughtersCuts ( self ) :
        """
        Define 'DaughtersCuts' for K0S
        """
        cuts = {}
        cuts [''] = " ( 2.0 * GeV < P) & ( ISLONG | ISDOWN ) & ( TRCHI2DOF < %(TrackQuality)g ) & ( MIPCHI2DV(PRIMARY) > %(DaughtersIPChi2)g ) " % self.getProps()
        return cuts
    
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
        ( ADMASS ( 'KS0') < %(DeltaMassK0S)g  )
        & in_range ( 0 , VFASPF ( VCHI2 ) ,  %(VertexChi2)g   )
        & ( VFASPF ( VZ   ) < %(MaxZ)g          )
        & in_range ( 0 , BPVLTFITCHI2()   ,  %(LTimeFitChi2)g )
        & ( BPVLTIME()*c_light > switch ( DD , %(CTauK0S_DD)g , %(CTauK0S)g ) )
        & ( ADWM( 'Lambda0' , WM( 'p+' , 'pi-') ) > switch( DD, %(WrongMassK0S_DD)g , %(WrongMassK0S)g ) )
        & ( ADWM( 'Lambda0' , WM( 'pi+' , 'p~-') ) > switch( DD, %(WrongMassK0S_DD)g , %(WrongMassK0S)g ) )
        """
        cut = cut % self.getProps()
        return cut.replace ( '\n' , ' ' )  
    
    ## define "MotherCut" for Lambda0
    def _lam0_motherCut      ( self ) :
        """
        Define 'MotherCut' for Lambda0
        """
        cut       = """
        ( ADMASS ( 'Lambda0')   < %(DeltaMassLambda)g )
        & in_range ( 0 , VFASPF ( VCHI2 ) ,  %(VertexChi2)g   )
        & ( VFASPF ( VZ       ) < %(MaxZ)g            )
        & in_range ( 0 , BPVLTFITCHI2()   ,  %(LTimeFitChi2)g )
        & ( BPVLTIME()*c_light > switch ( DD , %(CTauLambda0_DD)g , %(CTauLambda0)g ) )
        & ( ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > switch( DD, %(WrongMassLambda_DD)g  , %(WrongMassLambda)g ) ) 
        """
        cut = cut % self.getProps()
        return cut.replace ( '\n' , ' ' ) 

    ## define the common preambulo 
    def _preambulo ( self ) :
        """
        define the common preambulo
        """
        return [
            ## import c_light
            "from GaudiKernel.PhysicalConstants import c_light" ,
            ## define DD-category of K0S 
            "DD =    CHILDCUT ( ISDOWN , 1 ) & CHILDCUT ( ISDOWN , 2 ) " ,
            ## define LL-category of K0S 
            "LL =    CHILDCUT ( ISLONG , 1 ) & CHILDCUT ( ISLONG , 2 ) "
            ]
        
    ## define the list of K0S monitoring histograms 
    def _k0s_histos ( self ) :
        """
        Define the list of K0S monitoring histograms 
        """
        
        return [
            ## define historam type (shortcut)
            "Histo  = Gaudi.Histo1DDef"  ,
            ## monitor LL-case
            "massLL     = monitor ( M / GeV , Histo ( 'K0S, LL-case' , 0.45 , 0.55 , 100 ) , 'M_LL'     ) " ,
            "pTLL       = monitor ( PT / GeV , Histo ( 'K0S, LL-case' , 0.00 , 1.20 , 100 ) , 'pT_LL'   ) " ,
            "yLL        = monitor ( Y , Histo ( 'K0S, LL-case' , 1.60 , 5.40 , 100 ) , 'y_LL'           ) " ,
            ## monitor DD-case
            "massDD     = monitor ( M / GeV , Histo ( 'K0S, DD-case' , 0.45 , 0.55 , 100 ) , 'M_DD'       ) " ,
            "pTDD       = monitor ( PT / GeV , Histo ( 'K0S, DD-case' , 0.00 , 2.00 , 100 ) , 'pT_DD'   ) " ,
            "yDD        = monitor ( Y , Histo ( 'K0S, DD-case' , 1.60 , 5.40 , 100 ) , 'y_DD'           ) " ,
            ]

    ## define the list of Lambda0 monitoring histograms 
    def _lam0_histos ( self ) :
        """
        Define the list of Lambda0 monitoring histograms 
        """
        return  [
            ## define historam type (shortcut)
            "Histo  = Gaudi.Histo1DDef"  ,
            ## monitor LL-case
            "massLL     = monitor ( M / GeV , Histo ( 'Lambda0, LL-case' , 1.090 , 1.140 , 100 ) , 'M_LL'     ) " ,
            "pTLL       = monitor ( PT / GeV , Histo ( 'Lambda0, LL-case' , 0.00 , 2.00 , 100 ) , 'pT_LL'   ) " ,
            "yLL        = monitor ( Y , Histo ( 'Lambda0, LL-case' , 1.60 , 5.40 , 100 ) , 'y_LL'           ) " ,
            ## monitor DD-case
            "massDD     = monitor ( M / GeV , Histo ( 'Lambda0, DD-case' , 1.080 , 1.140 , 100 ) , 'M_DD'     ) " ,
            "pTDD       = monitor ( PT / GeV , Histo ( 'Lambda0, DD-case' , 0.00 , 2.00 , 100 ) , 'pT_DD'   ) " ,
            "yDD        = monitor ( Y , Histo ( 'Lambda0, DD-case' , 1.60 , 5.40 , 100 ) , 'y_DD'           ) " ,
            ]

    ## define "Bin1 Cut" for K0S LL
    def _k0s_bin1Cut_LL ( self ) :
        """
        Define 'Bin1 Cut' for K0S LL
        """
        cut       = """
        ( in_range ( %(K0SBinA_MinPT_LL)g , PT , %(K0SBinA_MaxPT_LL)g )
        & in_range ( %(K0SBinA_MinY_LL)g , Y , %(K0SBinA_MaxY_LL)g ) )
        """
        cut = cut % self.getProps()
        return cut.replace ( '\n' , ' ' ) 

        ## define "Bin2 Cut" for K0S LL
    def _k0s_bin2Cut_LL ( self ) :
        """
        Define 'Bin2 Cut' for K0S LL
        """
        cut        =  """
        ( (in_range ( %(K0SBinB_MinPT_LL)g , PT , %(K0SBinB_MaxPT_LL)g )
        & in_range ( %(K0SBinB_MinY_LL)g , Y , %(K0SBinB_MaxY_LL)g ) )
        | ( in_range( %(K0SBinC_MinPT_LL)g , PT , %(K0SBinB_MinPT_LL)g )
        & in_range ( %(K0SBinC_MinY_LL)g , Y , %(K0SBinC_MaxY_LL)g ) ) )
        & ~( in_range ( %(K0SBinA_MinPT_LL)g , PT , %(K0SBinA_MaxPT_LL)g )
        & in_range ( %(K0SBinA_MinY_LL)g , Y , %(K0SBinA_MaxY_LL)g ) )
        """

        cut = cut % self.getProps()
        return cut.replace ( '\n' , ' ' ) 

    ## define "Bin3 Cut" for K0S LL
    def _k0s_bin3Cut_LL ( self ) :
        """
        Define 'Bin3 Cut' for K0S LL
        """
        cut       = """
        ~( in_range( %(K0SBinB_MinPT_LL)g , PT , %(K0SBinB_MaxPT_LL)g )
        & in_range ( %(K0SBinB_MinY_LL)g , Y , %(K0SBinB_MaxY_LL)g ) )
        & ~( in_range( %(K0SBinC_MinPT_LL)g , PT , %(K0SBinB_MinPT_LL)g )
        & in_range ( %(K0SBinC_MinY_LL)g , Y , %(K0SBinC_MaxY_LL)g ) )
        """

        cut = cut % self.getProps()
        return cut.replace ( '\n' , ' ' ) 

    ## define "Bin1 Cut" for Lambda0 LL
    def _lam0_bin1Cut_LL ( self ) :
        """
        Define 'Bin1 Cut' for Lambda0 LL
        """
        cut       = """
        ( in_range ( %(Lambda0BinA_MinPT_LL)g , PT , %(Lambda0BinA_MaxPT_LL)g )
        & in_range ( %(Lambda0BinA_MinY_LL)g , Y , %(Lambda0BinA_MaxY_LL)g ) )
        """
        cut = cut % self.getProps()
        return cut.replace ( '\n' , ' ' ) 

        ## define "Bin2 Cut" for Lambda0 LL
    def _lam0_bin2Cut_LL ( self ) :
        """
        Define 'Bin2 Cut' for Lambda0 LL
        """
        cut        =  """
        ( (in_range ( %(Lambda0BinB_MinPT_LL)g , PT , %(Lambda0BinB_MaxPT_LL)g )
        & in_range ( %(Lambda0BinB_MinY_LL)g , Y , %(Lambda0BinB_MaxY_LL)g ) )
        | ( in_range( %(Lambda0BinC_MinPT_LL)g , PT , %(Lambda0BinB_MinPT_LL)g)
        & in_range ( %(Lambda0BinC_MinY_LL)g , Y , %(Lambda0BinC_MaxY_LL)g ) ) )
        & ~( in_range ( %(Lambda0BinA_MinPT_LL)g , PT , %(Lambda0BinA_MaxPT_LL)g )
        & in_range ( %(Lambda0BinA_MinY_LL)g , Y , %(Lambda0BinA_MaxY_LL)g ) )
        """

        cut = cut % self.getProps()
        return cut.replace ( '\n' , ' ' ) 

    ## define "Bin3 Cut" for Lambda0 LL
    def _lam0_bin3Cut_LL ( self ) :
        """
        Define 'Bin3 Cut' for Lambda0 LL
        """
        cut       = """
        ~( in_range( %(Lambda0BinB_MinPT_LL)g , PT , %(Lambda0BinB_MaxPT_LL)g )
        & in_range ( %(Lambda0BinB_MinY_LL)g , Y , %(Lambda0BinB_MaxY_LL)g ) )
        & ~( in_range( %(Lambda0BinC_MinPT_LL)g , PT , %(Lambda0BinB_MinPT_LL)g)
        & in_range ( %(Lambda0BinC_MinY_LL)g , Y , %(Lambda0BinC_MaxY_LL)g ) )
        """

        cut = cut % self.getProps()
        return cut.replace ( '\n' , ' ' ) 


    ## define "Bin1 Cut" for K0S DD
    def _k0s_bin1Cut_DD ( self ) :
        """
        Define 'Bin1 Cut' for K0S DD
        """
        cut       = """
        ( in_range ( %(K0SBinA_MinPT_DD)g , PT , %(K0SBinA_MaxPT_DD)g )
        & in_range ( %(K0SBinA_MinY_DD)g , Y , %(K0SBinA_MaxY_DD)g ) )
        | ( in_range ( %(K0SBinB_MinPT_DD)g , PT , %(K0SBinA_MinPT_DD)g )
        & in_range ( %(K0SBinB_MinY_DD)g , Y , %(K0SBinB_MaxY_DD)g ) )
        
        """
        cut = cut % self.getProps()
        return cut.replace ( '\n' , ' ' ) 

    ## define "Bin2 Cut" for K0S DD
    def _k0s_bin2Cut_DD ( self ) :
        """
        Define 'Bin2 Cut' for K0S DD
        """
        cut        =  """
        ~( in_range( %(K0SBinA_MinPT_DD)g , PT , %(K0SBinA_MaxPT_DD)g)
        & in_range ( %(K0SBinA_MinY_DD)g , Y , %(K0SBinA_MaxY_DD)g ) )
        & ~( in_range( %(K0SBinB_MinPT_DD)g , PT , %(K0SBinA_MinPT_DD)g )
        & in_range ( %(K0SBinB_MinY_DD)g , Y , %(K0SBinB_MaxY_DD)g ) )
        """

        cut = cut % self.getProps()
        return cut.replace ( '\n' , ' ' ) 

    ## define "Bin1 Cut" for Lambda0 DD
    def _lam0_bin1Cut_DD ( self ) :
        """
        Define 'Bin1 Cut' for Lambda0 DD
        """
        cut       = """
        ( in_range( %(Lambda0BinA_MinPT_DD)g , PT , %(Lambda0BinA_MaxPT_DD)g)
        & in_range ( %(Lambda0BinA_MinY_DD)g , Y , %(Lambda0BinA_MaxY_DD)g ) )
        | ( in_range( %(Lambda0BinB_MinPT_DD)g , PT , %(Lambda0BinA_MinPT_DD)g )
        & in_range ( %(Lambda0BinB_MinY_DD)g , Y , %(Lambda0BinB_MaxY_DD)g ) )
        | ( in_range( %(Lambda0BinC_MinPT_DD)g , PT , %(Lambda0BinB_MinPT_DD)g )
        & in_range ( %(Lambda0BinC_MinY_DD)g , Y , %(Lambda0BinC_MaxY_DD)g ) )
        """
        cut = cut % self.getProps()
        return cut.replace ( '\n' , ' ' ) 

    ## define "Bin2 Cut" for Lambda0 DD
    def _lam0_bin2Cut_DD ( self ) :
        """
        Define 'Bin2 Cut' for Lambda0 DD
        """
        cut        =  """
        ~( in_range( %(Lambda0BinA_MinPT_DD)g , PT , %(Lambda0BinA_MaxPT_DD)g)
        & in_range ( %(Lambda0BinA_MinY_DD)g , Y , %(Lambda0BinA_MaxY_DD)g ) )
        & ~( in_range( %(Lambda0BinB_MinPT_DD)g , PT , %(Lambda0BinA_MinPT_DD)g )
        & in_range ( %(Lambda0BinB_MinY_DD)g , Y , %(Lambda0BinB_MaxY_DD)g ) )
        & ~( in_range( %(Lambda0BinC_MinPT_DD)g , PT , %(Lambda0BinB_MinPT_DD)g )
        & in_range ( %(Lambda0BinC_MinY_DD)g , Y , %(Lambda0BinC_MaxY_DD)g ) )
        """

        cut = cut % self.getProps()
        return cut.replace ( '\n' , ' ' ) 


    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d





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

