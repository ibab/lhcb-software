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
from PhysSelPython.Wrappers import Selection, DataOnDemand, AutomaticData
from StrippingConf.StrippingLine import bindMembers
from StrippingSelections.Utils import checkConfig


__all__ = ("StrippingBd2KstarMuMuConf_JD")


defaultConfig = {
    'BMassLow'           : 5050          # MeV, low mass cut
    ,  'BMassHighWin'       :  500       # MeV, high mass window
    ,  'BDIRA'              : 0.9998     # dimensionless (0.9999?)
    ,  'BFlightCHI2'        : 100        # dimensionless
    ,  'BIPCHI2'            : 64         # dimensionless 
    ,  'DaughterVertexChi2' : 20         # dimensionless
    ,  'BVertexCHI2'        : 20         # dimensionless
    ,  'IntDIRA'            : -0.9       # dimensionless
    ,  'IntFlightCHI2'      : 9          # dimensionless
    ,  'TrackChi2'          : 10         # dimensionless
    ,  'TrackIPChi2'        : 4          # dimensionless
    ,  'KstarHighMass'      : 2500.      # MeV
    ,  'KstarLowMass'       : 500.       # MeV
    ,  'noFD_BIPCHI2'            :   16       # dimensionless
    ,  'noFD_BVertexCHI2'        :   12       # dimensionless (9?)
    ,  'noFD_DaughterVertexChi2' :   12       # dimensionless (9?)
    ,  'noFD_DaughterDIRA'       :   -0.9     # dimensionless
    ,  'noFD_TrackChi2'          :    5       # dimensionless
    ,  'noFD_TrackIPChi2'        :    9       # dimensionless
    }
    
defaultLines = {
    'Signal' : 1.0,
    'SignalTriggered' : 1.0,
    'WideKstarTriggered' : 1.0,
    'SameSign' : 1.0,
    'SameSignTriggered' : 1.0,
    'Signal_noFD' : 1.0,
    'SignalTriggered_noFD' : 1.0,
    'WideKstarTriggered_noFD' : 1.0,
    'SameSign_noFD' : 1.0,
    'SameSignTriggered_noFD' : 1.0
    }

class StrippingBd2KstarMuMuConf_JD(object):
    """
    Definition of B -> MuMuK* stripping lines

    To use this in the stripping do:

    from StrippingSelections.StrippingBd2KstarMuMuTriggered_JD import StrippingBd2KstarMuMuConf_JD
    from StrippingSelections.StrippingBd2KstarMuMuTriggered_JD import defaultConfig as Bd2KstarMuMuConfig
    from StrippingSelections.StrippingBd2KstarMuMuTriggered_JD import defaultLines as Bd2KstarMuMuLines

    Bd2KstarMuMuConf = StrippingBd2KstarMuMuConf_JD( config= Bd2KstarMuMuConfig, activeLines=Bd2KstarMuMuLines )
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
        ,  'TrackIPChi2'
        ,  'KstarHighMass'     
        ,  'KstarLowMass'      
        ,  'noFD_BIPCHI2'
        ,  'noFD_BVertexCHI2'
        ,  'noFD_DaughterVertexChi2'
        ,  'noFD_DaughterDIRA'
        ,  'noFD_TrackChi2'
        ,  'noFD_TrackIPChi2'
        )
    
    __allowed_lines__ = (
        'Signal'
        , 'SignalTriggered'
        , 'WideKstar'
        , 'WideKstarTriggered'
        , 'SameSign'
        , 'SameSignTriggered' 
        , 'Signal_noFD'
        , 'SignalTriggered_noFD'
        , 'WideKstarTriggered_noFD'
        , 'SameSign_noFD'
        , 'SameSignTriggered_noFD'
        )
    
    def __init__(self,name="Bd2KstarMuMu",config=None, activeLines = {}):
        """
        Constructor makes the selection objects but doesn't make the lines.
        Lines are created on demand. 
        """
        checkConfig(StrippingBd2KstarMuMuConf_JD.__configuration_keys__,config)

        for line in activeLines:
            if not line in StrippingBd2KstarMuMuConf_JD.__allowed_lines__:
                raise Exception("Line " + line + " does not appear in allowed list") 
        
        self.name = name
        self.activeLines = activeLines

        self.Dimuon = self.__DiMuon__( self.name, self.__DiMuonCuts__(config) ) # filter StdVeryLooseDiMuon with DiMuon cuts
        self.Dimuon_noFD = self.__DiMuon__( self.name + "_noFD", self.__DiMuonCuts_noFD__(config) ) # filter StdVeryLooseDiMuon with DiMuon_noFD cuts
        
        self.Kstar  = self.__Kstar__( self.name, self.__KstarCuts__(config) ) # filter StdVeryLooseDetachedKst2Kpi with Kstar cuts
        self.Kstar_noFD  = self.__Kstar__( self.name + "_noFD", self.__KstarCuts_noFD__(config) ) # filter StdVeryLooseDetachedKst2Kpi with Kstar_noFD cuts

        self.SameSignDiMuons = self.__BuildSameSignDiMuon__(config)

        self.SameSignDimuon = self.__SameSignDiMuon__( self.name, self.SameSignDiMuons, self.__DiMuonCuts__(config), config )
        self.SameSignDimuon_noFD = self.__SameSignDiMuon__( self.name + "_noFD", self.SameSignDiMuons, self.__DiMuonCuts_noFD__(config), config )

        self.WideKstars = self.__BuildWideKstar__(config)
     
        self.WideKstar = self.__WideKstar__( self.name, self.WideKstars, self.__KstarCuts__(config), config ) # build and filter WideKstar
        self.WideKstar_noFD = self.__WideKstar__( self.name + "_noFD", self.WideKstars, self.__KstarCuts_noFD__(config), config ) # build and filter WideKstar (no FD)
    
        self.Bd = self.__Bd__( self.name, self.Kstar, self.Dimuon, self.__BdCuts__(config), config )
        self.Bd_noFD = self.__Bd__( self.name + "_noFD", self.Kstar_noFD, self.Dimuon_noFD, self.__BdCuts_noFD__(config), config )
        
        self.BdWithWideKstar = self.__Bd__( self.name + "WithWideKstar", self.WideKstar, self.Dimuon, self.__BdCuts__(config), config )
        self.BdWithWideKstar_noFD = self.__Bd__( self.name + "WithWideKstar_noFD",self.WideKstar_noFD, self.Dimuon_noFD, self.__BdCuts_noFD__(config), config )
        
        self.BdWithSameSign = self.__Bd__( self.name+"WithSameSign", self.Kstar, self.SameSignDimuon, self.__BdCuts__(config), config )
        self.BdWithSameSign_noFD = self.__Bd__( self.name+"WithSameSign_noFD", self.Kstar_noFD, self.SameSignDimuon_noFD, self.__BdCuts_noFD__(config), config )

        #self.TriggerList = ["Hlt2MuTrackDecision"]
        self.TriggerList = ["Hlt2MuTrackDecision", "Hlt2TopoOSTF3BodyDecision"]

        self.BdTos = TOSFilter( self.name, self.Bd, self.TriggerList )
        self.BdTos_noFD = TOSFilter( self.name + "_noFD", self.Bd_noFD, self.TriggerList )
        
        self.BdWithWideKstarTos = TOSFilter( self.name+"WideKstar", self.BdWithWideKstar, self.TriggerList )
        self.BdWithWideKstarTos_noFD = TOSFilter( self.name+"WideKstar_noFD", self.BdWithWideKstar_noFD, self.TriggerList )
        
        self.BdWithSameSignTos = TOSFilter( self.name+"SameSign", self.BdWithSameSign, self.TriggerList )
        self.BdWithSameSignTos_noFD = TOSFilter( self.name+"SameSign_noFD", self.BdWithSameSign_noFD, self.TriggerList )

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

    def __DiMuonCuts_noFD__(self,conf = None):
        """
        Return the filter string for the dimuon
        """
        _dimuonCuts = "(VFASPF(VCHI2/VDOF)< %(noFD_DaughterVertexChi2)s ) & " \
                      "(BPVDIRA> %(noFD_DaughterDIRA)s ) & " \
                      "(2 == NINTREE((ABSID=='mu-') & " \
                      "(TRCHI2DOF< %(noFD_TrackChi2)s ) & " \
                      "(MIPCHI2DV(PRIMARY)> %(noFD_TrackIPChi2)s )))" % conf
        
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

    def __KstarCuts_noFD__(self, conf = None):
        """
        Return the filter cut string for the K*
        """
        _kstarCuts = "(VFASPF(VCHI2/VDOF)< %(noFD_DaughterVertexChi2)s ) & " \
                     "(BPVDIRA> %(noFD_DaughterDIRA)s ) & "\
                     "(2 == NINTREE((ISBASIC) & " \
                     "(TRCHI2DOF< %(noFD_TrackChi2)s ) & "\
                     "(MIPCHI2DV(PRIMARY)> %(noFD_TrackIPChi2)s )))" % conf
        
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

    def __BdCuts_noFD__(self, conf = None ):
        """
        Returns the Bd cut string
        """
        _BdCuts = "(VFASPF(VCHI2/VDOF) < %(noFD_BVertexCHI2)s ) & " \
                  "(BPVIPCHI2() < %(noFD_BIPCHI2)s )"  % conf
        return _BdCuts
        
    def __BuildSameSignDiMuon__(self, conf = None):
        """
        CombineParticles for same sign dimuon pairs
        """
        from CommonParticles.StdVeryLooseDiMuon import StdVeryLooseDiMuon as StdDimuon

        _cuts = "( (TRCHI2DOF < %(TrackChi2)s ) & (MIPCHI2DV(PRIMARY) > %(TrackIPChi2)s ) )"\
                "| ( (TRCHI2DOF < %(noFD_TrackChi2)s ) & (MIPCHI2DV(PRIMARY) > %(noFD_TrackIPChi2)s ) )" % conf
        
        _dimuons = StdDimuon.clone("Combine"+self.name+"SameSignDiMuon")
        _dimuons.DecayDescriptor =  "[J/psi(1S) -> mu+ mu+]cc"
        _dimuons.DaughtersCuts = { "mu+" :  _cuts }
        _dimuons.MotherCut = "(" + self.__DiMuonCuts__(conf) + " | " + self.__DiMuonCuts_noFD__(conf) + ") & " + StdDimuon.MotherCut 

        _sel = WrapSelection( "SelBuild" + self.name + "SameSignDiMuon", _dimuons )        

        return _sel

    def __SameSignDiMuon__(self, Name = None, DiMuons = None, DiMuonCuts = None, conf = None):
        """
        Filter SameSignDiMuon from __BuildSameSignDiMuon__
        """

        if(Name == None):
            Name = self.name

        _filter  = FilterDesktop("Filter" + Name + "SameSignDiMuon", Code = DiMuonCuts ) 
        _sel = Selection("Sel" + Name + "SameSignDiMuon", RequiredSelections = [ DiMuons ], Algorithm = _filter)
        
        return _sel 
    
    def __DiMuon__(self, Name = None, DiMuonCuts = None):
        """
        Filter dimuon from StdVeryLooseDiMuon
        """

        if(Name == None):
            Name = self.name
        
        _dimuons = DataOnDemand( Location="Phys/StdVeryLooseDiMuon")
        _filter  = FilterDesktop("Filter" + Name + "Dimuon", Code = DiMuonCuts ) 
        _sel = Selection("Sel" + Name + "DiMuon", RequiredSelections = [ _dimuons ] , Algorithm = _filter )
        
        return _sel 

    def __Kstar__(self, Name = None, KstarCuts = None):
        """
        Filter Kstar from StdVeryLooseDetachedKst2Kpi
        """
        if(Name == None):
            Name = self.name
            
        _kstar = DataOnDemand( Location="Phys/StdVeryLooseDetachedKst2Kpi" )
        _filter = FilterDesktop("Filter" + Name + "Kstar", Code = KstarCuts )
        _sel = Selection("Sel" + Name + "Kstar", RequiredSelections = [ _kstar ] , Algorithm = _filter )
        return _sel

    def __BuildWideKstar__(self, conf = None):
        """
        CombineParticles for the Wide K* selection.
        To keep the overlap complete with the StdVeryLooseDetachedKst2Kpi some cuts appear twice.
        """
        from CommonParticles.StdVeryLooseDetachedKstar import StdVeryLooseDetachedKst2Kpi as StdKstar

        _cuts = "( (TRCHI2DOF< %(TrackChi2)s ) & (MIPCHI2DV(PRIMARY)> %(TrackIPChi2)s) )"\
                " | ( (TRCHI2DOF< %(noFD_TrackChi2)s ) & (MIPCHI2DV(PRIMARY)> %(noFD_TrackIPChi2)s) )" % conf
        
        _wideKstar = StdKstar.clone("Combine"+self.name+"WideKstar")
        _wideKstar.CombinationCut = "(AM < %(KstarHighMass)s *MeV) & (AM > %(KstarLowMass)s *MeV)" % conf
        _wideKstar.MotherCut = self.__KstarCuts__(conf) + "&" + StdKstar.MotherCut
        
        for d in _wideKstar.DaughtersCuts:
            _wideKstar.DaughtersCuts[d] = _cuts + "&" + StdKstar.DaughtersCuts[d]
            
        _sel = WrapSelection("SelBuild" + self.name + "WideKstar", _wideKstar)
        
        return _sel 

    def __WideKstar__(self, Name = None, WideKstars = None, KstarCuts = None, conf = None):
        """
        Filter Kstar from __BuildWideKstar__
        """

        if(Name == None):
            Name = self.name

        _filter  = FilterDesktop("Filter" + Name + "WideKstar", Code = KstarCuts ) 
        _sel = Selection("Sel" + Name + "WideKstar", RequiredSelections = [ WideKstars ], Algorithm = _filter)
        
        return _sel
    
    def __Bd__(self, Name = None, Kstar = None, Dimuon = None, BdCuts = None, conf = None):
        """
        Combine particles for the signal B
        """

        if(Name == None):
            Name = self.name

        _combineBd = CombineParticles("Combine" + Name)
        _combineBd.DecayDescriptor = "[B0 -> K*(892)0 J/psi(1S)]cc"
        _combineBd.CombinationCut = "(AM > %(BMassLow)s *MeV) & (ADAMASS('B0') < %(BMassHighWin)s *MeV)"  % conf
        _combineBd.MotherCut = BdCuts

        _sel = Selection("Sel" + Name, RequiredSelections = [Kstar,Dimuon], Algorithm = _combineBd)
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
                "SignalTriggered" : self.BdTos,
                "Signal" : self.Bd,
                "WideKstar" : self.BdWithWideKstar,
                "WideKstarTriggered" : self.BdWithWideKstarTos,
                "SameSign" : self.BdWithSameSign,
                "SameSignTriggered" : self.BdWithSameSignTos,
                "SignalTriggered_noFD" : self.BdTos_noFD,
                "Signal_noFD" : self.Bd_noFD,
                "WideKstar_noFD" : self.BdWithWideKstar_noFD,
                "WideKstarTriggered_noFD" : self.BdWithWideKstarTos_noFD,
                "SameSign_noFD" : self.BdWithSameSign_noFD,
                "SameSignTriggered_noFD" : self.BdWithSameSignTos_noFD
                }[linename]

            _HLT = "HLT_PASS('Hlt2MuTrackDecision') | HLT_PASS('Hlt2TopoOSTF3BodyDecision')" if "Triggered" in linename else None
        
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

def TOSFilter( name = None, sel = None, Triggers = None ):
    """
    Function to return a selection object, filtering for TOS candidates from input selection
    """
    from Configurables import TisTosParticleTagger

    _filter = TisTosParticleTagger(name+"_TriggerTos")
    #_filter.TisTosSpecs = { Triggers+"%TOS" : 0 }
    #_filter.NoRegex = True
    
    SpecsMap = {}
    for Trigger in Triggers:
        SpecsMap[Trigger+"%TOS"] = 0
    
    _filter.TisTosSpecs = SpecsMap
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
        _sel = Selection( "Sel" + name , RequiredSelections = _requires, Algorithm = alg )
        return _sel
    else:
        raise Exception("Algorithm does not have any 'InputLocation'") 
