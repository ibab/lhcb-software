#Stripping Lines for DY->ee
#Electroweak Group (Convenor: Tara Shears)
#Adaptation of lines (to use line builders) originally written by James Keaveneyand David Ward by Will Barter

# 4 Lines - 4 standard lines cover different mass ranges.


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdTightElectrons, StdAllNoPIDsElectrons

confdict_DY2ee={
    'DY2eeLine1Prescale'    : .05 
    ,  'DY2eeLine2Prescale'   : .5 
    ,  'DY2eeLine3Prescale'   : 1.0
    ,  'DY2eeLine4Prescale'   : 1.0
    ,  'DY2eeLinePostscale'   : 1.0
    ,  'DY1MinMass' : 2.5
    ,  'DY1MaxMass' : 5.
    ,  'DY2MinMass' : 5.
    ,  'DY2MaxMass' : 10.
    ,  'DY3MinMass' : 10.
    ,  'DY3MaxMass' : 20.
    ,  'DY4MinMass' : 20.
    ,  'DY4MaxMass' : 40.
    ,  'ptcut1' : 1.
    ,  'ptcut2' : 2.
    ,  'ptcut3' : 5.
    ,  'ePID'   : 1.
    ,  'trkpchi2' : 0.0#0.001
    ,  'trkpchi2a' : 0.0#0.001
    ,  'HCalMax' : 0.05
    ,  'ECalMin' : 0.1
    ,  'PrsCalMin' : 50.
    ,  'mipdvchi2ratio' : 5.
 }

default_name = "DY2ee"

class DY2eeConf(LineBuilder) :

    __configuration_keys__ = (    'DY1MinMass',
                                  'DY1MaxMass',
                                  'DY2MinMass',
                                  'DY2MaxMass',
                                  'DY3MinMass',
                                  'DY3MaxMass',
                                  'DY4MinMass',
                                  'DY4MaxMass',
                                  'DY2eeLine1Prescale',
                                  'DY2eeLine2Prescale',
                                  'DY2eeLine3Prescale',
                                  'DY2eeLine4Prescale',
                                  'DY2eeLinePostscale',
                                  'ptcut1',
                                  'ptcut2',
                                  'ptcut3',
                                  'ePID',
                                  'trkpchi2',
                                  'trkpchi2a',
                                  'HCalMax',
                                  'ECalMin',
                                  'PrsCalMin',
                                  'mipdvchi2ratio'                           
                              )
    
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self._myname = name
        
        #Define the cuts
        _ecuta= '((PT>%(ptcut1)s*GeV)&(TRPCHI2>%(trkpchi2)s)&(MIPDV(PRIMARY)/MIPCHI2DV(PRIMARY)<%(mipdvchi2ratio)s)&(PIDe>%(ePID)s))'%config
        _ecutb= '((PT>%(ptcut2)s*GeV)&(TRPCHI2>%(trkpchi2)s)&(MIPDV(PRIMARY)/MIPCHI2DV(PRIMARY)<%(mipdvchi2ratio)s)&(PIDe>%(ePID)s))'%config
        _ecutc= '((PT>%(ptcut3)s*GeV)&(TRPCHI2>%(trkpchi2a)s)&(MIPDV(PRIMARY)/MIPCHI2DV(PRIMARY)<%(mipdvchi2ratio)s)&(PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s)&(PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s)&(PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s))'%config
        _DY1MassCut = '(MM>%(DY1MinMass)s*GeV)&(MM<%(DY1MaxMass)s*GeV)'%config
        _DY2MassCut = '(MM>%(DY2MinMass)s*GeV)&(MM<%(DY2MaxMass)s*GeV)'%config
        _DY3MassCut = '(MM>%(DY3MinMass)s*GeV)&(MM<%(DY3MaxMass)s*GeV)'%config
        _DY4MassCut = '(MM>%(DY4MinMass)s*GeV)&(MM<%(DY4MaxMass)s*GeV)'%config


        self.selDY2ee_1 = makeDY2ee_tight(self._myname+'DY2ee_1', 
                                     _DY1MassCut,
                                     _ecuta)
     
        self.DY2ee_line1 = StrippingLine(self._myname+"Line1",
                                            prescale = config['DY2eeLine1Prescale'],
                                            postscale = config['DY2eeLinePostscale'],
                                            selection = self.selDY2ee_1
                                            )
        self.registerLine(self.DY2ee_line1)



        self.selDY2ee_2 = makeDY2ee_tight(self._myname+'DY2ee_2', 
                                     _DY2MassCut,
                                     _ecutb)
     
        self.DY2ee_line2 = StrippingLine(self._myname+"Line2",
                                            prescale = config['DY2eeLine2Prescale'],
                                            postscale = config['DY2eeLinePostscale'],
                                            selection = self.selDY2ee_2
                                            )
        self.registerLine(self.DY2ee_line2)



        self.selDY2ee_3 = makeDY2ee_tight(self._myname+'DY2ee_3', 
                                     _DY3MassCut,
                                     _ecutb)
     
        self.DY2ee_line3 = StrippingLine(self._myname+"Line3",
                                            prescale = config['DY2eeLine3Prescale'],
                                            postscale = config['DY2eeLinePostscale'],
                                            selection = self.selDY2ee_3
                                            )
        self.registerLine(self.DY2ee_line3)



        self.selDY2ee_4 = makeDY2ee_NoPIDs(self._myname+'DY2ee_4', 
                                     _DY4MassCut,
                                     _ecutc)
     
        self.DY2ee_line4 = StrippingLine(self._myname+"Line4",
                                            prescale = config['DY2eeLine4Prescale'],
                                            postscale = config['DY2eeLinePostscale'],
                                            selection = self.selDY2ee_4
                                            )
        self.registerLine(self.DY2ee_line4)




def makeDY2ee_tight(name, _massCut, _ecut) :
    _DY2ee_tight = CombineParticles(DecayDescriptor = 'Z0 -> e+ e-',
                           DaughtersCuts = { 'e+' : _ecut , 
                                             'e-' : _ecut },
                           MotherCut = _massCut,
                           WriteP2PVRelations = False
                           )
    _TightElectrons = StdTightElectrons
    return Selection ( name,
                       Algorithm = _DY2ee_tight,
                       RequiredSelections = [_TightElectrons])


def makeDY2ee_NoPIDs(name, _massCutN, _ecutN) :
    _DY2ee_NoPIDs = CombineParticles(DecayDescriptor = 'Z0 -> e+ e-',
                           DaughtersCuts = { 'e+' : _ecutN , 
                                             'e-' : _ecutN },
                           MotherCut = _massCutN,
                           WriteP2PVRelations = False
                           )
    _AllNoPIDsElectrons = StdAllNoPIDsElectrons
    return Selection ( name,
                       Algorithm = _DY2ee_NoPIDs,
                       RequiredSelections = [_AllNoPIDsElectrons])
