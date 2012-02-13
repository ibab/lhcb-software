#Stripping Lines for DY->MuMu
#Electroweak Group (Convenor: Tara Shears)
#Adaptation of lines (to use line builders) originally written by James Keaveney by Will Barter

# 8 Lines - 4 standard lines cover different mass ranges. The additional 4 place HLT requirements, in an attempt to reduce the level of prescale needed at low mass. 


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLooseMuons

confdict_DY2MuMu={
    'DY2MuMu1Line_psPrescale'    : .02
    ,  'DY2MuMu1Line_HltPrescale': .2 
    ,  'DY2MuMu1Line_HltaPrescale': 1.0 
    ,  'DY2MuMu1LinePostscale'   : 1.0
    ,  'DY2MuMu2LinePrescale'    : .05 
    ,  'DY2MuMu2Line_HltPrescale': .4
    ,  'DY2MuMu2Line_HltaPrescale': 1.0
    ,  'DY2MuMu2LinePostscale'   : 1.0
    ,  'DY2MuMu3LinePrescale'    : 1.0
    ,  'DY2MuMu3LinePostscale'   : 1.0
    ,  'DY2MuMu4LinePrescale'    : 1.0
    ,  'DY2MuMu4LinePostscale'   : 1.0
    ,  'DY1MinMass' : 2.5
    ,  'DY1jpsiexclow' : 3.0
    ,  'DY1jpsiexchigh' : 3.2
    ,  'DY1MaxMass' : 5.
    ,  'DY2MinMass' : 5.
    ,  'DY2MaxMass' : 10.
    ,  'DY3MinMass' : 10.
    ,  'DY3MaxMass' : 20.
    ,  'DY4MinMass' : 20.
    ,  'DY4MaxMass' : 40.
    ,  'mupt1' : 800.
    ,  'mupt2' : 1.
    ,  'mutrkpchi2' : 0.001
    ,  'muipdvchi2ratio' : 5.
    ,  'mupidreq' : -3.
 }

default_name = "DY2MuMu"

class DY2MuMuConf(LineBuilder) :

    __configuration_keys__ = ('DY1MinMass',
                              'DY1jpsiexclow',
                              'DY1jpsiexchigh',
                              'DY1MaxMass',
                              'DY2MinMass',
                              'DY2MaxMass',
                              'DY3MinMass',
                              'DY3MaxMass',
                              'DY4MinMass',
                              'DY4MaxMass',
                              'mupt1',
                              'mupt2',
                              'mutrkpchi2',
                              'muipdvchi2ratio',
                              'mupidreq',
                              'DY2MuMu1Line_psPrescale',
                              'DY2MuMu1Line_HltPrescale',
                              'DY2MuMu1Line_HltaPrescale',
                              'DY2MuMu1LinePostscale',
                              'DY2MuMu2LinePrescale',
                              'DY2MuMu2Line_HltPrescale',
                              'DY2MuMu2Line_HltaPrescale',
                              'DY2MuMu2LinePostscale',
                              'DY2MuMu3LinePrescale',
                              'DY2MuMu3LinePostscale',
                              'DY2MuMu4LinePrescale',
                              'DY2MuMu4LinePostscale',
                              )

    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self._myname = name

        #Define the cuts
        _mucut1= '(PT>%(mupt1)s*MeV)&(TRPCHI2>%(mutrkpchi2)s)&(MIPDV(PRIMARY)/MIPCHI2DV(PRIMARY)<%(muipdvchi2ratio)s)&((PIDmu-PIDpi)>%(mupidreq)s)'%config
        _mucut2= '(PT>%(mupt2)s*GeV)&(TRPCHI2>%(mutrkpchi2)s)&(MIPDV(PRIMARY)/MIPCHI2DV(PRIMARY)<%(muipdvchi2ratio)s)'%config

        _DY1MassCut = '((MM>%(DY1MinMass)s*GeV)&(MM<%(DY1jpsiexclow)s*GeV))|((MM>%(DY1jpsiexchigh)s*GeV)&(MM<%(DY1MaxMass)s*GeV))'%config
        _DY1MassCutHlt = '((MM>%(DY1MinMass)s*GeV)&(MM<%(DY1jpsiexclow)s*GeV))|((MM>%(DY1jpsiexchigh)s*GeV))'%config
        _DY2MassCut = '(MM>%(DY2MinMass)s*GeV)&(MM<%(DY2MaxMass)s*GeV)'%config
        _DY2MassCutHlt = '(MM>%(DY2MinMass)s*GeV)'%config
        _DY3MassCut = '(MM>%(DY3MinMass)s*GeV)&(MM<%(DY3MaxMass)s*GeV)'%config
        _DY4MassCut = '(MM>%(DY4MinMass)s*GeV)&(MM<%(DY4MaxMass)s*GeV)'%config


        self.selDY2MuMu1 = makeDY2MuMu1(self._myname+'DYDecay1_ps',
                                     _DY1MassCut,
                                     _mucut1)

        self.selDY2MuMu1_Hlt = makeDY2MuMu1(self._myname+'DYDecay1_Hlt',
                                     _DY1MassCutHlt,
                                     _mucut1)

        self.DY2MuMu_line1_ps = StrippingLine(self._myname+"Line1_ps",
                                            prescale = config['DY2MuMu1Line_psPrescale'],
                                            postscale = config['DY2MuMu1LinePostscale'],
                                            selection = self.selDY2MuMu1
                                            )

        self.DY2MuMu_line1_Hlt = StrippingLine(self._myname+"Line1_Hlt",
                                            prescale = config['DY2MuMu1Line_HltPrescale'],
                                            postscale = config['DY2MuMu1LinePostscale'],
                                            HLT = "HLT_PASS_RE('Hlt2DiMuonDY.*Decision')",
                                            selection = self.selDY2MuMu1_Hlt
                                            )

        self.DY2MuMu_line1_Hlta = StrippingLine(self._myname+"Line1_Hlta",
                                            prescale = config['DY2MuMu1Line_HltaPrescale'],
                                            postscale = config['DY2MuMu1LinePostscale'],
                                            HLT = "HLT_PASS_RE('Hlt2(MuonFromHLT1|PassThrough).*Decision')",
                                            selection = self.selDY2MuMu1_Hlt
                                            )

        self.registerLine(self.DY2MuMu_line1_ps)

        self.registerLine(self.DY2MuMu_line1_Hlt)

        self.registerLine(self.DY2MuMu_line1_Hlta)


        self.selDY2MuMu2 = makeDY2MuMu2(self._myname+'DYDecay2',
                                     _DY2MassCut,
                                     _mucut2)

        self.selDY2MuMu2_Hlt = makeDY2MuMu2(self._myname+'DYDecay2_Hlt',
                                     _DY2MassCutHlt,
                                     _mucut1)

        self.DY2MuMu_line2 = StrippingLine(self._myname+"Line2",
                                            prescale = config['DY2MuMu2LinePrescale'],
                                            postscale = config['DY2MuMu2LinePostscale'],
                                            selection = self.selDY2MuMu2
                                            )

        self.DY2MuMu_line2_Hlt = StrippingLine(self._myname+"Line2_Hlt",
                                            prescale = config['DY2MuMu2Line_HltPrescale'],
                                            postscale = config['DY2MuMu2LinePostscale'],
                                            HLT = "HLT_PASS_RE('Hlt2DiMuonDY.*Decision')",
                                            selection = self.selDY2MuMu2_Hlt
                                            )

        self.DY2MuMu_line2_Hlta = StrippingLine(self._myname+"Line2_Hlta",
                                            prescale = config['DY2MuMu2Line_HltaPrescale'],
                                            postscale = config['DY2MuMu2LinePostscale'],
                                            HLT = "HLT_PASS_RE('Hlt2(MuonFromHLT1|PassThrough).*Decision')",
                                            selection = self.selDY2MuMu2_Hlt
                                            )


        self.registerLine(self.DY2MuMu_line2)

        self.registerLine(self.DY2MuMu_line2_Hlt)

        self.registerLine(self.DY2MuMu_line2_Hlta)



        self.selDY2MuMu3 = makeDY2MuMu3(self._myname+'DY2MuMuDecay3',
                                     _DY3MassCut,
                                     _mucut2)

        self.DY2MuMu_line3 = StrippingLine(self._myname+"Line3",
                                            prescale = config['DY2MuMu3LinePrescale'],
                                            postscale = config['DY2MuMu3LinePostscale'],
                                            selection = self.selDY2MuMu3
                                            )

        self.registerLine(self.DY2MuMu_line3)


        self.selDY2MuMu4 = makeDY2MuMu4(self._myname+'DY2MuMuDecay4',
                                     _DY4MassCut,
                                     _mucut2)

        self.DY2MuMu_line4 = StrippingLine(self._myname+"Line4",
                                            prescale = config['DY2MuMu4LinePrescale'],
                                            postscale = config['DY2MuMu4LinePostscale'],
                                            selection = self.selDY2MuMu4
                                            )

        self.registerLine(self.DY2MuMu_line4)



def makeDY2MuMu1(name, _DY1MassCut, _mucut1) :
    _DY1= CombineParticles(DecayDescriptor = 'Z0 -> mu+ mu-',
                           DaughtersCuts = { 'mu+' : _mucut1 ,
                                             'mu-' : _mucut1 },
                           MotherCut = _DY1MassCut,
                           WriteP2PVRelations = False
                           )
    _stdallloosemuons = StdAllLooseMuons
    return Selection ( name,
                       Algorithm = _DY1,
                       RequiredSelections = [_stdallloosemuons])


def makeDY2MuMu2(name, _DY2MassCut, _mucut2) :
    _DY2 = CombineParticles(DecayDescriptor = 'Z0 -> mu+ mu-',
                           DaughtersCuts = { 'mu+' : _mucut2 ,
                                             'mu-' : _mucut2 },
                           MotherCut = _DY2MassCut,
                           WriteP2PVRelations = False
                           )
    _stdallloosemuons = StdAllLooseMuons
    return Selection ( name,
                       Algorithm = _DY2,
                       RequiredSelections = [_stdallloosemuons])


def makeDY2MuMu3(name, _DY3MassCut, _mucut2) :
    _DY3 = CombineParticles(DecayDescriptor = 'Z0 -> mu+ mu-',
                           DaughtersCuts = { 'mu+' : _mucut2 ,
                                             'mu-' : _mucut2 },
                           MotherCut = _DY3MassCut,
                           WriteP2PVRelations = False
                           )
    _stdallloosemuons = StdAllLooseMuons
    return Selection ( name,
                       Algorithm = _DY3,
                       RequiredSelections = [_stdallloosemuons])


def makeDY2MuMu4(name, _DY4MassCut, _mucut2) :
    _DY4 = CombineParticles(DecayDescriptor = 'Z0 -> mu+ mu-',
                           DaughtersCuts = { 'mu+' : _mucut2 ,
                                             'mu-' : _mucut2 },
                           MotherCut = _DY4MassCut,
                           WriteP2PVRelations = False
                           )
    _stdallloosemuons = StdAllLooseMuons
    return Selection ( name,
                       Algorithm = _DY4,
                       RequiredSelections = [_stdallloosemuons])
