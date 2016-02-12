'''
Module for selecting J/psi, Psi(2S), and Upsilon 
Based on those by Jibo
'''

__author__=['Yanxi ZHANG']
__date__ = ''
__version__= '$Revision: 1.0 $'


__all__ = (
    'HeavyIonDiMuonConf',
    'default_config'
    )

default_config =  {
    'NAME'            : 'HeavyIonDiMuon',
    'WGs'             : ['ALL'],
    'STREAMS'         : ['ALL'],
    'BUILDERTYPE'     : 'HeavyIonDiMuonConf',
    'CONFIG'          : {
        "odin": ["NoBeam","Beam1","Beam2","BeamCrossing"],
        'CheckPV'    :  False,
        
        "VCHI2VDOF_max"       : 25,
        "Track_CHI2"       : 3,
        "DOCA_max"            : 0.5,
        "PTmu_min"        : 700,   # MeV
        "AM"              : 2900,   # MeV

        'Prescale'            :  1.0,
        'Postscale'           : 1.0, 

        'Hlt1Filter'          : "HLT_PASS_RE('Hlt1DiMuonHighMassDecision')",
        'Hlt2Filter'          : None,
    }
    }

from Gaudi.Configuration import *
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLooseMuons
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
#from Configurables import CombineParticles

class HeavyIonDiMuonConf(LineBuilder):
    
    __configuration_keys__ = default_config['CONFIG'].keys()

    
    def __init__(self, name, config ): 
        
        LineBuilder.__init__(self, name, config)
        self.name = name 
        self.config = config
        self.inMuons = StdAllLooseMuons

        _monCuts = ( "(VFASPF(VCHI2/VDOF)<{0[VCHI2VDOF_max]})").format(self.config)

        _dauCuts = {"mu+":("(PT>{0[PTmu_min]}) & (TRCHI2DOF<{0[Track_CHI2]})").format(self.config)}

        _comCuts = ( 
              "(AM > {0[AM]})"
              "&(ACUTDOCA({0[DOCA_max]}, ''))"
              ).format(self.config)
        self.makeJpsi = self.makeDiMuon(
              name = "Jpsi2MuMu",
              daughterCuts = _dauCuts,
              motherCuts = _monCuts,
              combCuts = _comCuts
              )

        odin = "|".join( ["(ODIN_BXTYP == LHCb.ODIN.%s)"%(odin_type,) for odin_type in self.config['odin'] if odin_type in ["NoBeam","Beam1","Beam2","BeamCrossing"]])

        self.JpsiLine = StrippingLine( name+'Jpsi2MuMuLine',
              prescale  = self.config['Prescale'],
              postscale  = self.config['Postscale'],                                                                      
              checkPV   = self.config['CheckPV'],
              HLT1       =self.config['Hlt1Filter'],
              HLT2       =self.config['Hlt2Filter'],
              algos     = [ self.makeJpsi],
              ODIN      = odin
              )
        self.registerLine( self.JpsiLine )



    def makeDiMuon( self,name, daughterCuts, motherCuts, combCuts) :
        '''create a selection using a CombineParticles'''
        diMuon = CombineParticles(
              #name               = 'Combine{0}'.format(name),
              DecayDescriptors   = ["J/psi(1S) -> mu+ mu-"],
              CombinationCut     = combCuts,
              MotherCut          = motherCuts,
              DaughtersCuts       = daughterCuts,
              )
        return Selection( name,
                          Algorithm = diMuon,
                          RequiredSelections = [ self.inMuons] )
