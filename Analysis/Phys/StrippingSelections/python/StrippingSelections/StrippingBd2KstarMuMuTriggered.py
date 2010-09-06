__author__ = 'Thomas Blake'
__date__ = '25/08/2010'
__version__ = '$Revision: 1.1 $'

"""
Stripping selections for nominal Bd -> K* Mu Mu lines. 
"""

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

from Configurables import CombineParticles, FilterDesktop
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import bindMembers
from StrippingSelections.Utils import checkConfig


__all__ = ("StrippingBd2KstarMuMuConf")


defaultConfig = {
    'BMassLow'           : 5050          # MeV, low mass cut
    ,  'BMassHighWin'       :  500       # MeV, high mass window
    ,  'BDIRA'              : 0.9999     # dimensionless
    ,  'BFlightCHI2'        : 100        # dimensionless
    ,  'BIPCHI2'            : 64         # dimensionless 
    ,  'DaughterVertexChi2' : 20         # dimensionless
    ,  'BVertexCHI2'        : 20         # dimensionless
    ,  'IntDIRA'            : -0.9       # dimensionless
    ,  'IntFlightCHI2'      : 9          # dimensionless
    ,  'TrackChi2'          : 10         # dimensionless
    ,  'TrackIPChi2'      : 4            # dimensionless
    ,  'KstarHighMass'     : 2500.       # MeV
    ,  'KstarLowMass'      : 500.        # MeV
    }
    
defaultLines = {
    'Prescaled' : 1.0 ,
    'Triggered' : 1.0 ,
    'WideKstarTriggered' : 1.0 ,
    'SameSign' : 1.0 ,
    'SameSignTriggered' : 1.0
    }

class StrippingBd2KstarMuMuConf(object):
    """
    Definition of B -> MuMuK* stripping lines

    To use this in the stripping do:

    from StrippingSelections.StrippingBd2KstarMuMuTriggered import StrippingBd2KstarMuMuConf
    from StrippingSelections.StrippingBd2KstarMuMuTriggered import defaultConfig as Bd2KstarMuMuConfig
    from StrippingSelections.StrippingBd2KstarMuMuTriggered import defaultLines as Bd2KstarMuMuLines

    Bd2KstarMuMuConf = StrippingBd2KstarMuMuConf( config= Bd2KstarMuMuConfig, activeLines=Bd2KstarMuMuLines )
    lines = Bd2KstarMuMuConf.lines()
    """
   
    __configuration_keys__ = ( 
        'BMassLow'        
        ,  'BMassHighWin'    
        ,  'BDIRA'           
        ,  'BFlightCHI2'     
        ,  'BIPCHI2'         
        ,  'DaughterVertexChi2'
        ,  'BVertexCHI2'       
        ,  'IntDIRA'           
        ,  'IntFlightCHI2'     
        ,  'TrackChi2'         
        , 'TrackIPChi2'
        ,  'KstarHighMass'     
        ,  'KstarLowMass'      
        )

    __allowed_lines__ = (
        'Prescaled'
        , 'Triggered'
        , 'WideKstar'
        , 'WideKstarTriggered'
        , 'SameSign'
        , 'SameSignTriggered' 
        )
    
    def __init__(self,name="Bd2KstarMuMu",config=None, activeLines = {}):
        """
        Constructor makes the selection objects but doesn't make the lines.
        Lines are created on demand. 
        """
        checkConfig(StrippingBd2KstarMuMuConf.__configuration_keys__,config)

        for line in activeLines:
            if not line in StrippingBd2KstarMuMuConf.__allowed_lines__:
                raise Exception("Line " + line + " does not appear in allowed list") 
        
        self.name = name
        self.activeLines = activeLines
        self.Dimuon = self.__DiMuon__( config )
        self.Kstar  = self.__Kstar__( config )
        self.SameSignDimuon = self.__SameSignDiMuon__( config )
        self.WideKstar  = self.__WideKstar__( config )
    
        self.Bd = self.__Bd__( self.name,self.Kstar,self.Dimuon, config )
        self.BdWithWideKstar = self.__Bd__( self.name+"WithWideKstar",self.WideKstar,self.Dimuon, config )
        self.BdWithSameSign = self.__Bd__( self.name+"WithSameSign",self.Kstar,self.SameSignDimuon, config )
        
        self.BdTos = TOSFilter( self.name, self.Bd, "Hlt2MuTrackDecision" )
        self.BdWithWideKstarTos = TOSFilter( self.name+"WideKstar", self.BdWithWideKstar, "Hlt2MuTrackDecision" )
        self.BdWithSameSignTos = TOSFilter( self.name+"SameSign", self.BdWithSameSign, "Hlt2MuTrackDecision" )

        # Stripping lines are created on demand 
        self.Lines = {} 

        
    def __DiMuonCuts__(self,conf = None):
        """
        Return the filter string for the dimuon
        """
        _dimuonCuts = "(VFASPF(VCHI2/VDOF)< %(DaughterVertexChi2)s ) & " \
                      "(BPVDIRA> %(IntDIRA)s ) & " \
                      "(2 == NINTREE((ABSID=='mu-') & " \
                      "(TRCHI2DOF< %(TrackChi2)s ) & " \
                      "(MIPCHI2DV(PRIMARY)> %(TrackIPChi2)s )))" % conf
        return _dimuonCuts

    def __KstarCuts__(self, conf = None):
        """
        Return the filter cut string for the K*
        """
        _kstarCuts = "(VFASPF(VCHI2/VDOF)< %(DaughterVertexChi2)s ) & " \
                     "(BPVDIRA> %(IntDIRA)s ) & "\
                     "(2 == NINTREE((ISBASIC) & " \
                     "(TRCHI2DOF< %(TrackChi2)s ) & "\
                     "(MIPCHI2DV(PRIMARY)> %(TrackIPChi2)s )))" % conf
        return _kstarCuts

    def __BdCuts__(self, conf = None ):
        """
        Returns the Bd cut string
        """
        _BdCuts = "(VFASPF(VCHI2/VDOF) < %(BVertexCHI2)s ) & " \
                  "(BPVDIRA> %(BDIRA)s ) & " \
                  "(BPVVDCHI2 > %(BFlightCHI2)s ) & " \
                  "(BPVIPCHI2() < %(BIPCHI2)s )"  % conf
        return _BdCuts
        
    def __SameSignDiMuon__(self, conf = None):
        """
        CombineParticles for same sign dimuon pairs
        """
        from CommonParticles.StdVeryLooseDiMuon import StdVeryLooseDiMuon as StdDimuon

        _cuts = "(TRCHI2DOF< %(TrackChi2)s )&(MIPCHI2DV(PRIMARY)> %(TrackIPChi2)s )" % conf
        
        _dimuons = StdDimuon.clone("Combine"+self.name+"SameSignDiMuon")
        _dimuons.DecayDescriptor =  "[J/psi(1S) -> mu+ mu+]cc"
        _dimuons.DaughtersCuts = { "mu+" :  _cuts }
        _dimuons.MotherCut = self.__DiMuonCuts__(conf) + "&" + StdDimuon.MotherCut 

        _sel = WrapSelection( self.name + "SameSignDimuon" ,_dimuons )        
        return _sel
    
        
    def __DiMuon__(self, conf = None):
        """
        Filter dimuon from StdVeryLooseDiMuon
        """        
        _dimuons = DataOnDemand( Location="Phys/StdVeryLooseDiMuon")
        _filter  = FilterDesktop("Filter"+self.name + "DiMuon", Code = self.__DiMuonCuts__(conf) ) 
        _sel = Selection("Sel" + self.name + "DiMuon", RequiredSelections = [ _dimuons ] , Algorithm = _filter )
        
        return _sel 
        
    def __Kstar__(self, conf = None):
        """
        Filter Kstar from StdVeryLooseDetachedKst2Kpi
        """
        _kstar = DataOnDemand( Location="Phys/StdVeryLooseDetachedKst2Kpi" )
        _filter = FilterDesktop("Filter"+self.name + "Kstar",Code = self.__KstarCuts__(conf) )
        _sel = Selection("Sel" + self.name + "Kstar", RequiredSelections = [ _kstar ] , Algorithm = _filter )
        return _sel

    def __WideKstar__(self, conf = None):
        """
        CombineParticles for the Wide K* selection.
        To keep the overlap complete with the StdVeryLooseDetachedKst2Kpi some cuts appear twice.
        """
        from CommonParticles.StdVeryLooseDetachedKstar import StdVeryLooseDetachedKst2Kpi as StdKstar

        _cuts = "(TRCHI2DOF< %(TrackChi2)s )&(MIPCHI2DV(PRIMARY)> %(TrackIPChi2)s )" % conf
        
        _wideKstar = StdKstar.clone("Combine"+self.name+"WideKstar")
        _wideKstar.CombinationCut = "(AM < %(KstarHighMass)s *MeV) & (AM > %(KstarLowMass)s *MeV)" % conf
        _wideKstar.MotherCut = self.__KstarCuts__(conf) + "&" + StdKstar.MotherCut
        
        for d in _wideKstar.DaughtersCuts:
            _wideKstar.DaughtersCuts[d] = _cuts + "&" + StdKstar.DaughtersCuts[d]
            
        _sel = WrapSelection(self.name+"WideKstar",  _wideKstar )
        return _sel 

      
    def __Bd__(self, name = None, Kstar = None, Dimuon = None, conf = None ):
        """
        Combine particles for the signal B
        """
        _combineBd = CombineParticles("Combine"+name)
        _combineBd.DecayDescriptor = "[B0 -> K*(892)0 J/psi(1S)]cc"
        _combineBd.CombinationCut = "(AM > %(BMassLow)s *MeV) & (ADAMASS('B0') < %(BMassHighWin)s *MeV)"  % conf
        _combineBd.MotherCut = self.__BdCuts__(conf)

        _sel = Selection("Sel"+name,RequiredSelections = [Kstar,Dimuon], Algorithm = _combineBd )
        return _sel
    
    def lines(self):
        """
        Returns a line list based on the list of active lines
        """
        _linelist = [ self.line( linename ) for linename in self.activeLines ]
        return _linelist
        

    def line(self, linename = None):
        """
        Returns a single line based on a line name. The line must be declared active in the constructor.
        """
        if not linename in self.activeLines:
            raise Exception("Line " + linename + " does not appear in allowed list or is declared inactive")

        if not linename in self.Lines:
            _sel = {
                "Triggered" : self.BdTos,
                "Prescaled" : self.Bd,
                "WideKstar" : self.BdWithWideKstar,
                "WideKstarTriggered" : self.BdWithWideKstarTos,
                "SameSign" : self.BdWithSameSign,
                "SameSignTriggered" : self.BdWithSameSignTos
                }[linename]

            _HLT = "HLT_PASS('Hlt2MuTrackDecision')" if "Triggered" in linename else None
        
            self.Lines[linename] = StrippingLine( self.name+"_"+linename, HLT = _HLT, 
                                                  prescale = self.activeLines[linename],
                                                  algos = [ _sel ] )
                                             
        return self.Lines[linename]
    
    
    def selection(self):
        """
        Returns the standard B construction as a SelectionSequence
        """
        if self.sequenceBd == None:
            self.sequenceBd = SelectionSequence("Seq" + self.name, TopSelection = self.Bd)
        return self.sequenceBd



## Generic functions, added for functionality

def TOSFilter( name = None, sel = None, trigger = None ):
    """
    Function to return a selection object, filtering for TOS candidates from input selection
    """
    from Configurables import TisTosParticleTagger

    _filter = TisTosParticleTagger(name+"_TriggerTos")
    _filter.TisTosSpecs = { trigger+"%TOS" : 0 }
    _filter.NoRegex = True
    
    _sel = Selection("Sel" + name + "_TriggerTos", RequiredSelections = [ sel ], Algorithm = _filter )
    return _sel


def WrapSelection( name = None , alg = None ):
    """
    Wrap an algorithm with fixed InputLocations in a Selection object.
    @todo: Register with DoD instead. 
    """
    if hasattr( alg, 'InputLocations' ):
        _requires = []
        for loc in alg.InputLocations:
            if 'Phys/' in loc: _requires += [ DataOnDemand( Location = loc ) ]
            else : _requires += [ DataOnDemand( Location = "Phys/" + loc ) ]

        alg.InputLocations = []
        _sel = Selection( "Sel"+name , RequiredSelections = _requires, Algorithm = alg )
        return _sel
    else:
        raise Exception("Algorithm does not have any 'InputLocation'") 
