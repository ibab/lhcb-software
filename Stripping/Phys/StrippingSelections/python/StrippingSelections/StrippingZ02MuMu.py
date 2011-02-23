#Stripping Lines for Z->MuMu
#Electroweak Group (Convenor: Tara Shears)
#Adaptation of lines (to use line builders) originally written by James Keaveney by Will Barter

# Lines needed:
# Z02MuMu. stdloosemuons
# Z02MuMu. stdveryloosemuons
# Z02MuMu. stdnopids muons. 

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


confdict_Z02MuMu={
    'Z02MuMuLinePrescale'    : 1.0 
    ,  'Z02MuMuLinePostscale'   : 1.0
    ,  'Z0MinMass' : 40.
    ,  'mucut' : 15.                                   
 }

name = "Z02MuMu"

class Z02MuMuConf(LineBuilder) :

    __configuration_keys__ = ('Z0MinMass',
                              'mucut',
                              'Z02MuMuLinePrescale',
                              'Z02MuMuLinePostscale'                           
                              )
    
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self._myname = name
        
        #Define the cuts
        _mucut= '(PT>%(mucut)s*GeV)'%config
        _Z0MinMass = '(MM>%(Z0MinMass)s*GeV)'%config


        self.selZ02MuMu = makeZ02MuMu(self._myname+'Z02MuMu', 
                                     _Z0MinMass,
                                     _mucut)
     
        self.Z02MuMu_line = StrippingLine(self._myname+"Z02MuMuLine",
                                          prescale = config['Z02MuMuLinePrescale'],
                                          postscale = config['Z02MuMuLinePostscale'],
                                          checkPV = False,
                                          selection = self.selZ02MuMu
                                          )
        self.registerLine(self.Z02MuMu_line)
       
        self.selZ02MuMuloose = makeZ02MuMuloose(self._myname+'Z02MuMuLoose', 
                                     _Z0MinMass,
                                     _mucut)
     
        self.Z02MuMu_lineloose = StrippingLine(self._myname+"Z02MuMuLooseLine",
                                               prescale = config['Z02MuMuLinePrescale'],
                                               postscale = config['Z02MuMuLinePostscale'],
                                               checkPV = False,
                                               selection = self.selZ02MuMuloose
                                               )
        self.registerLine(self.Z02MuMu_lineloose)


        self.selZ02MuMuNoPIDs = makeZ02MuMuNoPIDs(self._myname+'Z02MuMuNoPIDs', 
                                     _Z0MinMass,
                                     _mucut)
     
        self.Z02MuMu_lineNoPIDs = StrippingLine(self._myname+"Z02MuMuNoPIDsLine",
                                                prescale = config['Z02MuMuLinePrescale'],
                                                postscale = config['Z02MuMuLinePostscale'],
                                                checkPV = False,
                                                selection = self.selZ02MuMuNoPIDs
                                            )
        self.registerLine(self.Z02MuMu_lineNoPIDs)


def makeZ02MuMu(name, _Z0MinMass, _mucut) :
    _Z0 = CombineParticles(DecayDescriptor = 'Z0 -> mu+ mu-',
                           DaughtersCuts = { 'mu+' : _mucut , 
                                             'mu-' : _mucut },
                           MotherCut = _Z0MinMass,
                           WriteP2PVRelations = False
                           )
    _stdloosemuons = DataOnDemand(Location = "Phys/StdLooseMuons")
    return Selection ( name,
                       Algorithm = _Z0,
                       RequiredSelections = [_stdloosemuons])


def makeZ02MuMuloose(name, _Z0MinMass, _mucut) :
    _Z0loose = CombineParticles(DecayDescriptor = 'Z0 -> mu+ mu-',
                           DaughtersCuts = { 'mu+' : _mucut , 
                                             'mu-' : _mucut },
                           MotherCut = _Z0MinMass,
                           WriteP2PVRelations = False
                           )
    _stdveryloosemuons = DataOnDemand(Location = "Phys/StdVeryLooseMuons")
    return Selection ( name,
                       Algorithm = _Z0loose,
                       RequiredSelections = [_stdveryloosemuons])


def makeZ02MuMuNoPIDs(name, _Z0MinMass, _mucut) :
    _Z0NoPIDs = CombineParticles(DecayDescriptor = 'Z0 -> mu+ mu-',
                           DaughtersCuts = { 'mu+' : _mucut , 
                                             'mu-' : _mucut },
                           MotherCut = _Z0MinMass,
                           WriteP2PVRelations = False
                           )
    _NoPIDsMuons = DataOnDemand(Location = "Phys/StdNoPIDsMuons")
    return Selection ( name,
                       Algorithm = _Z0NoPIDs,
                       RequiredSelections = [_NoPIDsMuons])
