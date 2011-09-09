#Stripping Lines for W->MuNu and Studies of their Background.
#Electroweak Group (Convenor: Tara Shears)
#Adaptation of lines (to use line builders) originally written by James Keaveney by Will Barter

# Lines needed:

# W prescaled 0.1. normal cut. stdveryloosemuons
# W prescaled 0.1. low pT window. stdloosemuons.
# W prescaled 0.1. very tight cut. stdnopids muons.
# W not prescaled. normal cut. stdloosemuons
# W not prescaled. tight cut. stdveryloosemuons
# no prescale, low pt cut, used to study background. Ensure absence of bias by only taking events that trigger min bias in HLT.

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdVeryLooseMuons, StdAllNoPIDsMuons, StdAllLooseMuons

confdict_WMu={
    
    'WMuLinePrescale_ps'    : .1
    ,  'WMuLinePrescale_hps': .05
    ,  'WMuLinePrescale'    : 1.0
    ,  'WMuLineLoosePrescale'    : 1.0
    ,  'WMuLineNoPIDsPrescale'    : 0.1
    ,  'WMuLinePostscale'   : 1.0
    ,  'mucuttight' : 20.
    ,  'mucut' : 15.
    ,  'mucut_hps' : 10. 
    ,  'mucutlow' : 5.
    ,  'trkpchi2' : 0.001
 }

default_name = "W2Mu"

class WMuConf(LineBuilder) :

    __configuration_keys__ = ('mucuttight',
                              'mucut',
                              'mucut_hps',
                              'WMuLinePrescale_hps',
                              'WMuLineLoosePrescale',
                              'WMuLineNoPIDsPrescale', 
                              'mucutlow',
                              'trkpchi2',
                              'WMuLinePrescale_ps',
                              'WMuLinePrescale',
                              'WMuLinePostscale'                           
                              )
    
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self._myname = name
        
        #Define the cuts
        _mucut='(PT>%(mucut)s*GeV)'%config
        _mucuttight = '(PT>%(mucuttight)s*GeV)'%config
        _mucutverytight = '(PT>%(mucuttight)s*GeV)&(TRPCHI2>%(trkpchi2)s)'%config
        _mucutlow='(PT>%(mucutlow)s*GeV)'%config
        _mucuthps = '(PT>%(mucut_hps)s*GeV)'%config

        # First Make the PrescaledLines


        # StdLooseMuons, Between 10 GeV and 15GeV
        # will prescale to keep rate low...
        self.selWMu_hps = makeWMu(self._myname+'WMu_hps', 
                                  _mucuthps)
     
        self.WMu_linehps = StrippingLine(self._myname+"LowPTLine",
                                             prescale = config['WMuLinePrescale_hps'],
                                             postscale = config['WMuLinePostscale'],
                                             checkPV = False,
                                             selection = self.selWMu_hps
                                             )
        self.registerLine(self.WMu_linehps)

        # Prescaled Line, StdVeryLooseMuons, Cut at 15GeV
        self.selWMu_looseps = makeWMuLoose(self._myname+'WMu_looseps', 
                                           _mucut)
     
        self.WMu_linelooseps = StrippingLine(self._myname+"Line_looseps",
                                             prescale = config['WMuLinePrescale_ps'],
                                             postscale = config['WMuLinePostscale'],
                                             checkPV = False,
                                             selection = self.selWMu_looseps
                                             )
        self.registerLine(self.WMu_linelooseps)

        # Prescaled Line, StdNoPIDsMuons, Cut at 20GeV. 
        self.selWMu_NoPIDsps = makeWMuNoPIDs(self._myname+'WMu_NoPIDsps', 
                                             _mucutverytight)
     
        self.WMu_lineNoPIDsps = StrippingLine(self._myname+"Line_NoPIDsps",
                                              prescale = config['WMuLineNoPIDsPrescale'],
                                              postscale = config['WMuLinePostscale'],
                                              checkPV = False,
                                              selection = self.selWMu_NoPIDsps
                                              )
        self.registerLine(self.WMu_lineNoPIDsps)





        # Now the tighter, non pre-scaled lines
        # StdLooseMuons, Cut at 15GeV
        self.selWMu = makeWMu(self._myname+'WMu', 
                              _mucut)
     
        self.WMu_line = StrippingLine(self._myname+"Line",
                                      prescale = config['WMuLinePrescale'],
                                      postscale = config['WMuLinePostscale'],
                                      checkPV = False,
                                      selection = self.selWMu
                                      )
        self.registerLine(self.WMu_line)

        # StdVeryLooseMuons, Cut at 20GeV
        self.selWMu_loose = makeWMuLoose(self._myname+'WMu_loose', 
                              _mucuttight)
     
        self.WMu_lineloose = StrippingLine(self._myname+"Line_loose",
                                           prescale = config['WMuLineLoosePrescale'],
                                           postscale = config['WMuLinePostscale'],
                                           checkPV = False,
                                           selection = self.selWMu_loose
                                           )
        self.registerLine(self.WMu_lineloose)


        #background line, require minbias triggers to have fired.
        self.selNoPIDsPTCut = makeNoPIDs(self._myname+'NoPIDs', 
                                         _mucutlow)  
        self.line_SingleTrackNoBias = StrippingLine(self._myname+"SingleTrackNoBias",
                                               prescale = config['WMuLinePrescale'],
                                               postscale = config['WMuLinePostscale'],
                                               checkPV = False,
                                               HLT = "HLT_PASS('Hlt1MBNoBiasDecision')",
                                               selection = self.selNoPIDsPTCut
                                               )
        self.registerLine(self.line_SingleTrackNoBias)

       
def makeWMu(name, _muCut) :
    _W = FilterDesktop(Code = _muCut)
    _stdallloosemuons = StdAllLooseMuons
    return Selection ( name,
                       Algorithm = _W,
                       RequiredSelections = [_stdallloosemuons])


def makeWMuLoose(name, _muCut) :
    _WLoose = FilterDesktop(Code = _muCut)                            
    _stdveryloosemuons = StdVeryLooseMuons
    return Selection ( name,
                       Algorithm = _WLoose,
                       RequiredSelections = [_stdveryloosemuons])


def makeWMuNoPIDs(name, _muCut) :
    _WNoPIDs = FilterDesktop(Code = _muCut)
    _AllNoPIDsmuons = StdAllNoPIDsMuons
    return Selection ( name,
                       Algorithm = _WNoPIDs,
                       RequiredSelections = [_AllNoPIDsmuons])

def makeNoPIDs(name, _muCut) :
    _NoPIDsSelection = FilterDesktop(Code = _muCut)
    _AllNoPIDsmuons = StdAllNoPIDsMuons
    return Selection ( name,
                       Algorithm = _NoPIDsSelection,
                       RequiredSelections = [_AllNoPIDsmuons])
