from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsElectrons

confdict_Z02ee={
    'Z02eeLinePrescale'    : 1.0 
    ,  'Z02eeLinePostscale'   : 1.0
    ,  'Z0MinMass' : 40.
    ,  'ptcut' : 10.
    ,  'trkpchi2' : 0.001
    ,  'HCalMax' : 0.05
    ,  'ECalMin' : 0.1
    ,  'PrsCalMin' : 50.
    ,  'mipdvchi2ratio' : 5.
 }

name = "Z02ee"

class Z02eeConf(LineBuilder) :

    __configuration_keys__ = ('Z0MinMass',
                              'ptcut',
                              'trkpchi2',
                              'HCalMax',
                              'ECalMin',
                              'PrsCalMin',
                              'mipdvchi2ratio',
                              'Z02eeLinePrescale',
                              'Z02eeLinePostscale'                           
                              )
    
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self._myname = name
        
        #Define the cuts
        _ecut= '((PT>%(ptcut)s*GeV)&(TRPCHI2>%(trkpchi2)s)&(MIPDV(PRIMARY)/MIPCHI2DV(PRIMARY)<%(mipdvchi2ratio)s)&(PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s)&(PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s)&(PPINFO(LHCb.ProtoParticle.CaloHcalE,0)<P*%(HCalMax)s))'%config
        _Z0MinMass = '(MM>%(Z0MinMass)s*GeV)'%config


        self.selZ02ee = makeZ02ee(self._myname+'Z02ee', 
                                     _Z0MinMass,
                                     _ecut)
     
        self.Z02ee_line = StrippingLine(self._myname+"Line",
                                            prescale = config['Z02eeLinePrescale'],
                                            postscale = config['Z02eeLinePostscale'],
                                            selection = self.selZ02ee
                                            )
        self.registerLine(self.Z02ee_line)


def makeZ02ee(name, _Z0MinMass, _ecut) :
    _Z02ee = CombineParticles(DecayDescriptor = 'Z0 -> e+ e-',
                           DaughtersCuts = { 'e+' : _ecut , 
                                             'e-' : _ecut },
                           MotherCut = _Z0MinMass,
                           WriteP2PVRelations = False
                           )
    _AllNoPIDsElectrons = StdAllNoPIDsElectrons
    return Selection ( name,
                       Algorithm = _Z02ee,
                       RequiredSelections = [_AllNoPIDsElectrons])
