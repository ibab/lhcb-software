# =============================================================================
"""
Strip tau to p mu mu events: 

     - tau+ -> p+ mu+ mu-                candidates (OS)
     - tau+ -> p~- mu+ mu+               candidates (SS)

 The selection criteria are the same for both lines:

   tau+ :
   
      - two muons, pt > 300 MeV, track fit chi2 < 3, PIDmu > -5 and (PIDmu-PIDK) > 0
      - one proton, pt > 300 MeV, track fit chi2 < 3
      - tau pt > 4 GeV
      - vertex fit chi2 < 20
      - cos phi > 0.999
      - ctau > 50 microns		
      - +-250 MeV mass-window
  
   The Global Event Selection:

    - each line requres at least one primary vertex

 Usage:

     from StrippingSelections.StrippingTau2PMuMu import  StrippingTau2PMuMuConf
     
     stream.appendLines ( StrippingTau2PMuMuConf ( {} ).lines() )
"""
# =============================================================================
__author__  = 'Jonathan HARRISON jonathan.harrison@hep.manchester.ac.uk based on StrippingTriMuons.py by Vanya BELYAEV'
__date__    = '12-08-2011'
__version__ = '$Revision 2.0$'
# =============================================================================

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, VoidEventSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
import GaudiKernel.SystemOfUnits as units
from Configurables import LoKi__VoidFilter
from StandardParticles import StdLooseMuons, StdLooseProtons


__all__ = ('StrippingTau2PMuMuConf',
           'confdict')

confdict = {
    #
    # Selection of basic muons and protons 
    #
    'GoodMuons' : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 3 ) & ( PIDmu > -5 ) & ( (PIDmu - PIDK) > 0 ) & ( BPVIPCHI2 () >  9 )" ,
    'GoodProtons' : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 3 )& ( BPVIPCHI2 () >  9 )" ,
    #
    # Prescale 
    #
    'pmumu-Prescale' : 1.00 , 
    #
    # Technicalities:
    #
    'Preambulo' : [
    ## shortcut for chi2 of vertex fit 
    'chi2vx = VFASPF(VCHI2) ' ,
    ## shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light" ,
    "ctau = BPVLTIME ( ) * c_light "  
    ]
    #
    }
# =============================================================================

class StrippingTau2PMuMuConf(LineBuilder) :
    """
    """
    __configuration_keys__ = (
        'GoodMuons',
	'GoodProtons',
        'pmumu-Prescale',
        'Preambulo'
        )

    __confdict__={}    

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.__confdict__= config
        self.name = name
        #
        ## 2 tau -> pmumu stripping lines for opposite sign (OS) and same sign (SS) muon pairs
        #
        self.lineTauPmumuOS =    StrippingLine (
            self.name+'Tau2PMuMuOS'                  ,
            prescale = self.__confdict__['pmumu-Prescale']  ,              
            algos    = [ self.tau2PMuMuOS() ]
            )
        self.registerLine(self.lineTauPmumuOS)

        self.lineTauPmumuSS =    StrippingLine (
            self.name+'Tau2PMuMuSS'                  ,
            prescale = self.__confdict__['pmumu-Prescale']  ,              
            algos    = [ self.tau2PMuMuSS() ]
            )
        self.registerLine(self.lineTauPmumuSS)
    ## get the selections - both stripping lines share the same particle selections 
    def selections ( self , name ) :
        
        if hasattr ( self , '_Selections' ) : return self._Selections
        
        self._Selections =  [ self.goodMuons           () ,
                              self.goodProtons	       () ,
                              self.twoGoodMuons        () ,
                              self.tau2PMuMuOS         () ,
                              self.tau2PMuMuSS         () ]
       
        return self._Selections
    # =========================================================================
    # get good muons 
    # =========================================================================
    def goodMuons      ( self ) :
        """
        Get selection for good muons 
        """
        if hasattr ( self , 'GoodMuons_Selection' ) :
            return self.GoodMuons_Selection
        
        _LooseMuons = StdLooseMuons
        
        ## helper selection of 'good' muons 
        _GoodMuons = FilterDesktop (
            Code = self.__confdict__['GoodMuons']
            )
        
        ## make (pre)-selection
        self.GoodMuons_Selection = Selection (
            ##
            self.name + "GoodMuonsForPMuMu"                 ,
            ##
            Algorithm          = _GoodMuons    ,
            ##
            RequiredSelections = [ _LooseMuons ]
            )
        
        return self.GoodMuons_Selection
    # =========================================================================
    # get good protons 
    # =========================================================================
    def goodProtons      ( self ) :
        """
        Get selection for good protons 
        """
        if hasattr ( self , 'GoodProtons_Selection' ) :
            return self.GoodProtons_Selection
        
        _LooseProtons = StdLooseProtons
        
        ## helper selection of 'good' protons 
        _GoodProtons = FilterDesktop (
            Code = self.__confdict__['GoodProtons']
            )
        
        ## make (pre)-selection
        self.GoodProtons_Selection = Selection (
            ##
            self.name + "GoodProtonsForPMuMu"                 ,
            ##
            Algorithm          = _GoodProtons    ,
            ##
            RequiredSelections = [ _LooseProtons ]
            )
        
        return self.GoodProtons_Selection
    # =========================================================================
    # get two good muons 
    # =========================================================================
    def twoGoodMuons      ( self ) :
        """
        Get two good muons 
        """
        if hasattr ( self , 'TwoGoodMuons_EventSelection' ) :
            return self.TwoGoodMuons_EventSelection
        
        _goodmu = self.goodMuons()
        
        ## require at least 2 good muons
        self.TwoGoodMuons_EventSelection =VoidEventSelection (
            #
	    self.name + "TwoGoodMuonsEventSelection",
	    #
            Code = " CONTAINS('%s') > 1.5 " %  ( _goodmu.outputLocation() ) ,
            #
	    RequiredSelection = _goodmu
            )
        
        return self.TwoGoodMuons_EventSelection
    # =========================================================================
    # get tau+ -> p+mu+mu- 
    # =========================================================================
    def tau2PMuMuOS ( self ) :
        """
        get tau+ -> p+mu+mu- 
        """
        
        if hasattr ( self , 'Tau2PMuMuOppositeSign_Selection' ) : return self.Tau2PMuMuOppositeSign_Selection
        
        _Tau2PMuMuOppositeSign = CombineParticles (
            ##
            DecayDescriptor  = " [ tau+ -> p+ mu+ mu- ]cc" ,
            ##
            Preambulo        = self.__confdict__['Preambulo'] , 
            ## 
            CombinationCut  = """
            ( ADAMASS('tau+') < 250 * MeV ) 
            """ , 
            ##
            MotherCut       = """
            ( chi2vx < 15 ) &
            ( ctau   > 100 * micrometer ) &
            ( BPVIPCHI2() < 225 )
            """ 
            )
        
        ## make selection
        self.Tau2PMuMuOppositeSign_Selection  = Selection (
            ##
            self.name + "SelTau2PMuMuOppositeSign" ,
            ##
            Algorithm          = _Tau2PMuMuOppositeSign  ,
            ##
            RequiredSelections = [ self.twoGoodMuons(), self.goodProtons() ] 
            )
        ##
        return self.Tau2PMuMuOppositeSign_Selection
    # =========================================================================
    # get tau+ -> p~-mu+mu+ 
    # =========================================================================
    def tau2PMuMuSS ( self ) :
        """
        get tau+ -> p~-mu+mu+ 
        """
        
        if hasattr ( self , 'Tau2PMuMuSameSign_Selection' ) : return self.Tau2PMuMuSameSign_Selection
        
	_Tau2PMuMuSameSign = CombineParticles (
            ##
            DecayDescriptor  = " [ tau+ -> p~- mu+ mu+ ]cc" ,
            ##
            Preambulo        = self.__confdict__['Preambulo'] , 
            ## 
            CombinationCut  = """
            ( ADAMASS('tau+') < 250 * MeV )
            """ , 
            ##
            MotherCut       = """
              ( chi2vx < 15 ) &
            ( ctau   > 100 * micrometer ) &
            ( BPVIPCHI2() < 225 )
            """ 
            )
	
        ## make selection
	self.Tau2PMuMuSameSign_Selection  = Selection (
            ##
            self.name + "SelTau2PMuMuSameSign" ,
            ##
            Algorithm          = _Tau2PMuMuSameSign  ,
            ##
            RequiredSelections = [ self.twoGoodMuons(), self.goodProtons() ] 
            )
        ##
        return self.Tau2PMuMuSameSign_Selection
