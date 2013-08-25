"""
QEE group
Configuration for Stripping lines for analyses with 3, 4 jets and 1 or 2 topo B
"""

__author__ = ['Stephane Tourneur']
__date__   = '17/08/2013'

__all__ = ( 'BJetsLinesConf' )

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import GeV
from LHCbKernel.Configuration import DEBUG, VERBOSE

from PhysSelPython.Wrappers import AutomaticData, Selection, DataOnDemand, PassThroughSelection, EventSelection
from StrippingConf.StrippingLine import StrippingLine, bindMembers
from StrippingUtils.Utils import LineBuilder

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from GaudiConfUtils.ConfigurableGenerators import GaudiSequencer as GaudiSequenceroid
from Configurables import GaudiSequencer
from Configurables import LoKi__HDRFilter  as HltFilter
from Configurables import LoKi__ODINFilter as ODINFilter
from Configurables import LoKi__VoidFilter

from Configurables import LoKi__FastJetMaker,LoKi__JetMaker

# constants

default_config = {
    "1B2jetlow" : { "PTmin" : 8*GeV ,  "prescale" : 0.005 },
    "1B2jethigh" : { "PTmin" : 14*GeV ,  "prescale" : 0.1 },
    "1B4jet" : { "PTmin" : 16*GeV ,  "prescale" : 1 },
    "2B3jet" : { "PTmin" : 11*GeV ,  "prescale" : 1 },
    "2B4jet" : { "PTmin" : 8*GeV ,  "prescale" : 1 },
    "my2TOPO" : {"ANGLE_MIN":2/57., "M_MIN":0, "DPHI_MIN":0}
    }

class BJetsLinesConf(LineBuilder):

    __configuration_keys__ = (
        "1B2jetlow",
        "1B2jethigh",
        "1B4jet",
        "2B3jet",
        "2B4jet",
        "my2TOPO"
        )

    __confdict__={}
        

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)
        self.__confdict__= config
        self.name = name
                
        self.LongPions = self._getLongPions(name)
        Jets = self._createJets(name)
        sel_2topo = self._2topo(name)
        
        _2B3jetsel = Selection( name+"HighPT3Jets",
                                      Algorithm = self._3jets(name, str(self.__confdict__["2B3jet"]["PTmin"])),
                                      RequiredSelections = [sel_2topo, Jets] )
        _2B3jetLine = StrippingLine( name+"2B3jetLine" ,
                                              HLT = "HLT_PASS_RE('Hlt2.*Topo.*Decision')",
                                              prescale = self.__confdict__["2B3jet"]["prescale"] ,
                                              selection = _2B3jetsel )

        _2B4jetsel = Selection( name+"HighPT4Jets",
                                      Algorithm = self._4jets(name, str(self.__confdict__["2B4jet"]["PTmin"])),
                                      RequiredSelections = [ sel_2topo, Jets] )

        _2B4jetLine = StrippingLine( name+"2B4jetLine" ,
                                              HLT = "HLT_PASS_RE('Hlt2.*Topo.*Decision')",
                                              prescale = self.__confdict__["2B4jet"]["prescale"] ,
                                              selection = _2B4jetsel )

        _1B2jetlowsel = Selection( name+"1B2jetlow",
                                      Algorithm = self._2jets(name, str(self.__confdict__["1B2jetlow"]["PTmin"])),
                                      RequiredSelections = [ Jets] )
        _1B2jetlowLine = StrippingLine( name+"1B2jetlowLinePS" ,
                                              HLT = "HLT_PASS_RE('Hlt2.*Topo.*Decision')",
                                              prescale = self.__confdict__["1B2jetlow"]["prescale"] ,
                                              selection = _1B2jetlowsel )

        _1B2jethighsel = Selection( name+"1B2jethigh",
                                      Algorithm = self._3jets(name, str(self.__confdict__["1B2jethigh"]["PTmin"])),
                                      RequiredSelections = [ Jets] )
        _1B2jethighLine = StrippingLine( name+"1B2jethighLinePS" ,
                                              HLT = "HLT_PASS_RE('Hlt2.*Topo.*Decision')",
                                              prescale = self.__confdict__["1B2jethigh"]["prescale"] ,
                                              selection = _1B2jethighsel )

        _1B4jetsel = Selection( name+"1B4jet",
                                      Algorithm = self._4jets(name, str(self.__confdict__["1B4jet"]["PTmin"])),
                                      RequiredSelections = [ Jets] )
        _1B4jetLine = StrippingLine( name+"1B4jetLine" ,
                                              HLT = "HLT_PASS_RE('Hlt2.*Topo.*Decision')",
                                              prescale = self.__confdict__["1B4jet"]["prescale"] ,
                                              selection = _1B4jetsel )

        self.registerLine( _2B4jetLine )
        self.registerLine( _2B3jetLine )
        self.registerLine(_1B4jetLine  )
        self.registerLine(_1B2jethighLine  )
        self.registerLine(_1B2jetlowLine  )
        
    def _getLongPions( self , name ):
        """
        Select only the StdAllNoPIDSPions which comes from the same PV than the Topo candidate
        """
        _stdAllPions = DataOnDemand( Location = "Phys/StdAllNoPIDsPions/Particles" )
        _code = "(PT > 200. *MeV)"
        _selectPions = FilterDesktop( Code = _code )

        return Selection( name+"HighPTPions" ,
                          Algorithm = _selectPions,
                          RequiredSelections = [_stdAllPions] )
    

    def _createJets( self,  name ):
        _stdAllPions = DataOnDemand( Location = "Phys/StdAllNoPIDsPions/Particles" )
        _stdLoosePhotons = DataOnDemand( Location = "Phys/StdLoosePhotons/Particles" )
        #_stdNoPIDsDownPions = DataOnDemand( Location = "Phys/StdNoPIDsDownPions/Particles" )
        
        _jetAlgo =  LoKi__JetMaker ( name+"JetMaker" )
        _jetAlgo.JetMaker = 'LoKi__FastJetMaker'
        _jetAlgo.JetID = False
        _jetAlgo.ApplyJEC = False #If True, error
        _jetAlgo.Associate2Vertex = True ## Inputs are selected from the good PV before hands
        _jetAlgo.addTool ( LoKi__FastJetMaker )
        _jetAlgo.Inputs = ['Phys/StdLoosePhotons/Particles','Phys/BjetsHighPTPions/Particles']#'Phys/StdNoPIDsDownPions/Particles',
        _jetTool = getattr ( _jetAlgo , 'LoKi__FastJetMaker' )
        _jetTool.Type = 2 ## anti-kt
        _jetTool.RParameter = 0.5
        _jetTool.PtMin = self.__confdict__["2B4jet"]["PTmin"]
        _jetTool.Recombination = 0
        return Selection( name+"Jets" ,
                          Algorithm = _jetAlgo,
                          RequiredSelections = [_stdLoosePhotons, self.LongPions])#, _stdNoPIDsDownPions] )
        
    def _2jets( self, name, ptcut ):
        HighPT2Jets = CombineParticles()
        HighPT2Jets.ParticleCombiners = { "" : "LoKi::VertexFitter" }
        HighPT2Jets.IgnoreP2PVFromInputLocations = True
        HighPT2Jets.DecayDescriptor = "H_10 -> CELLjet CELLjet"
        HighPT2Jets.DaughtersCuts = { "CELLjet" : "PT > "+ptcut+"*MeV" }
        HighPT2Jets.CombinationCut = "(ADOCACHI2CUT(50, ''))"
        HighPT2Jets.MotherCut = "VFASPF(VCHI2PDOF) < 20"
        return HighPT2Jets

    def _3jets( self, name, ptcut ):
        HighPT3Jets = CombineParticles()
        HighPT3Jets.ParticleCombiners = { "" : "LoKi::VertexFitter" }
        HighPT3Jets.IgnoreP2PVFromInputLocations = True
        HighPT3Jets.DecayDescriptor = "H_10 -> CELLjet CELLjet CELLjet"
        HighPT3Jets.DaughtersCuts = { "CELLjet" : "PT > "+ptcut+"*MeV" }
        HighPT3Jets.CombinationCut = "(ADOCACHI2CUT(50, ''))"
        HighPT3Jets.MotherCut = "VFASPF(VCHI2PDOF) < 20"
        return HighPT3Jets

    def _4jets( self, name, ptcut ):
        HighPT4Jets = CombineParticles()
        HighPT4Jets.ParticleCombiners = { "" : "LoKi::VertexFitter" }
        HighPT4Jets.IgnoreP2PVFromInputLocations = True
        HighPT4Jets.DecayDescriptor = "H_10 -> CELLjet CELLjet CELLjet CELLjet"
        HighPT4Jets.DaughtersCuts = { "CELLjet" : "PT > "+ptcut+"*MeV" }
        HighPT4Jets.CombinationCut = "(ADOCACHI2CUT(50, ''))"
        HighPT4Jets.MotherCut = "VFASPF(VCHI2PDOF) < 20"
        return HighPT4Jets

    # Same code as the double Topo stripping line
    def _2topo( self, name): 
        from Configurables import DoubleTopoTool as DT
        code = "ACCEPT('DoubleTopoTool/DoubleTopoLine_DT')"
        alg = LoKi__VoidFilter('DoubleTopoLineFilter',Code=code)
        sel = EventSelection('DoubleTopoEventSel',Algorithm=alg)
        dt = DT('DoubleTopoLine_DT')
        dt.minAngle = self.__confdict__["my2TOPO"]["ANGLE_MIN"]
        dt.minMass = self.__confdict__["my2TOPO"]["M_MIN"]
        dt.minDPhi = self.__confdict__["my2TOPO"]["DPHI_MIN"]
        return sel
