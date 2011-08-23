#Stripping Lines for W->eNu.
#Electroweak Group (Convenor: Tara Shears)
#Adaptation of lines (to use line builders) originally written by James Keaveney and David Ward by Will Barter


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsElectrons

confdict_We={
    'WeLinePrescale'    : 1.0 
    ,  'WeLinePostscale'   : 1.0
    ,  'WeLinePrescale_ps'    : .1 
    ,  'ptcut' : 20.
    ,  'ptcut_ps' : 15.
    ,  'trkpchi2' : 0.0
    ,  'HCalMax' : 0.05
    ,  'ECalMin' : 0.1
    ,  'PrsCalMin' : 50.
    ,  'trkghostprob' : 0.6
 }

name = "We"

class WeConf(LineBuilder) :

    __configuration_keys__ = ('ptcut',
                              'ptcut_ps',
                              'trkpchi2',
                              'HCalMax',
                              'ECalMin',
                              'PrsCalMin',
                              'trkghostprob',
                              'WeLinePrescale',
                              'WeLinePrescale_ps',
                              'WeLinePostscale'                           
                              )
    
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self._myname = name
        
        #Define the cuts
        _ecut= '((PT>%(ptcut)s*GeV)&(TRPCHI2>%(trkpchi2)s)&(TRGHOSTPROB<%(trkghostprob)s)&(PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s)&(PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s)&(PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s))'%config
        _ecut_ps= '((PT>%(ptcut_ps)s*GeV)&(TRPCHI2>%(trkpchi2)s)&(PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s)&(PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s)&(PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s))'%config

        
        self.selWe = makeWe(self._myname+'We', 
                               _ecut)
        
        self.We_line = StrippingLine(self._myname+"Line",
                                     prescale = config['WeLinePrescale'],
                                     postscale = config['WeLinePostscale'],
                                     selection = self.selWe
                                     )
        self.registerLine(self.We_line)


        self.selWe_ps = makeWe(self._myname+'We_ps', 
                               _ecut_ps)
        
        self.We_line_ps = StrippingLine(self._myname+"Line_ps",
                                     prescale = config['WeLinePrescale_ps'],
                                     postscale = config['WeLinePostscale'],
                                     selection = self.selWe_ps
                                     )
        self.registerLine(self.We_line_ps)


def makeWe(name, eCut) :
    _We = FilterDesktop(Code = eCut)
    _AllNoPIDsElectrons = StdAllNoPIDsElectrons
    return Selection ( name,
                       Algorithm = _We,
                       RequiredSelections = [_AllNoPIDsElectrons])
