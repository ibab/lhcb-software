__author__ = 'Patrick Koppenburg'
__date__ = '03/09/2011'
__version__ = '$Revision: 1.1 $'

"""
Bd->K*MuMu Lifetime biased selection 
"""
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from StrippingSelections.Utils import checkConfig
from PhysSelPython.Wrappers import Selection, DataOnDemand
from Configurables import FilterDesktop, CombineParticles

__all__ = ("StrippingBd2KstarMuMuLTConf")


defaultConfig = { 
    'BMassMedWin'         : 300        # MeV, high mass window
    ,  'IntVertexCHI2Tight'  : 9          # adimentional
    ,  'TrackChi2'           : 5          # adimentional
    ,  'BasicIPChi2'         : 9          # adimentional
    ,  'KpiPT'               : 300        # MeV
    ,  'SimpleDiMuonPT'      : 1000       # MeV
    ,  'SimpleBdLT'          : 0.001      # ns = 1 ps
    }

class StrippingBd2KstarMuMuLTConf(object):
    """
    Definition of B->MuMuK* stripping with a hard lifetime cut
    
    To use this in the stripping do:
    
    from StrippingSelections.StrippingBd2KstarMuMuLT import StrippingBd2KstarMuMuLTConf
    from StrippingSelections.StrippingBd2KstarMuMuLT import defaultConfig
    
    Bd2KstarMuMuLTConf = StrippingBd2KstarMuMuLTConf( config=defaultConfig )
    lines = Bd2KstarMuMuLTConf.lines()

    author : P. Koppenburg
    data   : 03/09/2011
    """

    __configuration_keys__ = ( 
        'BMassMedWin'     
        ,  'IntVertexCHI2Tight' 
        ,  'TrackChi2'          
        ,  'BasicIPChi2'        
        ,  'KpiPT'              
        ,  'SimpleDiMuonPT'     
        ,  'SimpleBdLT'         
        )


    def __init__(self,name="Bd2KstarMuMuLT",config=None):
        """
        Constructor makes the selection objects but doesn't make the lines
        """
        checkConfig(StrippingBd2KstarMuMuLTConf.__configuration_keys__,config)

        self.name = name
        self.DiMuon = self.__simplestDiMuon__( config )
        self.LTBd = self.__simplestBd__( config )

        self.simplestLine = None


####################################################################################################
#
# Simplest line based on LT
#
    def __simplestDiMuon__( self, conf = None ):
        """
        Very simple line based on LT 
        
        @author P. Koppenburg
        @date 25/2/2010
        """
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from StrippingConf.StrippingLine import StrippingLine

	_muons =  DataOnDemand(Location = 'Phys/StdVeryLooseDiMuon')

        _diMu = FilterDesktop("FilterForSimpleBd2KstarMuMuLT")
        _diMu.Code = """(PT > %(SimpleDiMuonPT)s *MeV) &
                        ((VFASPF ( VCHI2/VDOF) ) < %(IntVertexCHI2Tight)s ) &
                        (NINTREE( (ISBASIC) & (MIPCHI2DV(PRIMARY) > %(BasicIPChi2)s) & (TRCHI2DOF < %(TrackChi2)s ))==2) &
                        (BPVVDZ > 0)""" % conf
        _sd = Selection("SelFilterForSimpleBd2KstarMuMuLT",
                       Algorithm = _diMu,
                       RequiredSelections = [ _muons ] )

        return _sd
        
    def __simplestBd__( self, conf = None ):
        """
        Very simple line based on LT cut
        
        @author P. Koppenburg
        @date 25/2/2010, retuned 03/09/2010
        """

        KC = "((VFASPF ( VCHI2/VDOF) ) < %(IntVertexCHI2Tight)s ) & " \
             "(NINTREE( (ISBASIC) & (MIPCHI2DV(PRIMARY) > %(BasicIPChi2)s ) & " \
                                   "(TRCHI2DOF < %(TrackChi2)s ) & " \
                                   "(PT > %(KpiPT)s ))==2 ) & " \
                                   "(BPVVDZ > 0)" % conf

        MC = "(VFASPF(VCHI2/VDOF) < %(IntVertexCHI2Tight)s ) &" \
             "( BPVLTIME() > %(SimpleBdLT)s ) & (ADMASS('B0') < %(BMassMedWin)s *MeV)"  % conf
        CC = "(ADAMASS('B0') < %(BMassMedWin)s *MeV)" % conf

        _kstar = DataOnDemand( Location="Phys/StdVeryLooseDetachedKst2Kpi" )
        _comb = CombineParticles("SimpleBd2KstarMuMuLT",
                                 DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0 ]cc" ,
                                 DaughtersCuts = { "K*(892)0" : KC } ,
                                 CombinationCut = CC ,
                                 MotherCut = MC )
        return Selection("Sel"+self.name, RequiredSelections = [ self.DiMuon, _kstar ], Algorithm = _comb )
    

    def lines( self ):
        """
        Creates and returns the line list for the default stripping
        
        @author P. Koppenburg
        @date 03/09/2010
        """
        from StrippingConf.StrippingLine import StrippingLine

        if self.simplestLine == None :
            self.simplestLine = StrippingLine ( self.name,
                                                prescale = 1,
                                                algos = [ self.LTBd ] )

        return [ self.simplestLine ]

    def selection(self):
        """
        Returns the standard B construction as a SelectionSequence
        """
        if self.sequenceLTBd == None:
            self.sequenceLTBd = SelectionSequence("Seq" + self.name, TopSelection = self.LTBd)
        return self.sequenceLTBd
        
