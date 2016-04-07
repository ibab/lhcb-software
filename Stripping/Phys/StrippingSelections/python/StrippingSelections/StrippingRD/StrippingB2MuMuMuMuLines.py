'''
Module for construction of B-->MuMuMuMu and D-->MuMuMuMu stripping selections and
B-->KS0(mumu)KS0(mumu) (DetachedDimuons) stripping selections and
and lines.
'''

__author__ = ['Diego Martinez Santos', 'Johannes Albrecht', 'Konstantinos A. Petridis', 'Alexander Baranov']
__date__ = '20/10/2010'
__version__ = '$Revision: 1.0 $'
__all__ = ('B2MuMuMuMuLinesConf', 'makeB2MuMuMuMu', 'default_config')


default_config = {
    'NAME' : 'B2MuMuMuMu',
    'WGs' : ['RD'],
    'BUILDERTYPE' : 'B2MuMuMuMuLinesConf',
    'CONFIG'      : {
        'B2MuMuMuMuLinePrescale'    : 1,
        'B2MuMuMuMuLinePostscale'   : 1,
        'B23MuPiLinePrescale'    : 1,
        'B23MuPiLinePostscale'   : 1,
        'D2MuMuMuMuLinePrescale'    : 1,
        'D2MuMuMuMuLinePostscale'   : 1,
        'B2TwoDetachedDimuonLinePrescale'  : 1,
        'B2TwoDetachedDimuonLinePostscale' : 1,
        'B2JpsiKmumuLinePrescale'  : 1,
        'B2JpsiKmumuLinePostscale' : 1,
        'B2JpsiPhimumuLinePrescale'  : 1,
        'B2JpsiPhimumuLinePostscale' : 1,
        'B2DetachedDimuonAndJpsiLinePrescale' : 1,
        'B2DetachedDimuonAndJpsiLinePostscale': 1,
        'DetachedDiMuons': {
            'AMAXDOCA_MAX'  : '0.5*mm',
            'ASUMPT_MIN'    : '1000*MeV',
            'VCHI2DOF_MAX'  : 16,
            'BPVVDCHI2_MIN' : 16,
        },
        'B2DetachedDiMuons': {
            'SUMPT_MIN'        : '2000*MeV',
            'VCHI2DOF_MAX'     : 6,
            'BPVIPCHI2_MAX'    : 16,
            'BPVVDCHI2_MIN'    : 50,
            'BPVDIRA_MIN'      : 0.0,
            'MASS_MIN'         : {'B':'4600*MeV'},
            'MASS_MAX'         : {'B':'6000*MeV'}
        },
    },
    'STREAMS' : ['Dimuon'],
}



from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from Beauty2Charm_LoKiCuts import LoKiCuts
from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays as Combine4Particles

#from StrippingSelections.Utils import checkConfig

class B2MuMuMuMuLinesConf(LineBuilder) :
    """
    Builder of:
       ...


    Usage:
    >>> config = { .... }
    >>> bsConf = Bs2MuMuLinesConf('PrescaledBs2MuMuTest',config)
    >>> bsLines = bsConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.


    Exports as instance data members:
    selDefault     : nominal Bs2mumu stripping line
    selLoose       : loose Bs2MuMu stripping line to understand systematics
    defaultLine    : Stripping line made from selDefault
    looseLine      : Stripping line made from selLoose
    lines          : lsit of lines:  [ defaultLine, looseLine ]

    Exports as class data member:
    Bs2MuMuLinesConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = (
        'B2MuMuMuMuLinePrescale',
        'B2MuMuMuMuLinePostscale',
        'B23MuPiLinePrescale',
        'B23MuPiLinePostscale',
        'D2MuMuMuMuLinePrescale',
        'D2MuMuMuMuLinePostscale',
        'B2TwoDetachedDimuonLinePrescale',
        'B2TwoDetachedDimuonLinePostscale',
        'B2JpsiKmumuLinePrescale',
        'B2JpsiKmumuLinePostscale',
        'B2JpsiPhimumuLinePrescale',
        'B2JpsiPhimumuLinePostscale',
        'DetachedDiMuons',
        'B2DetachedDiMuons',
        'B2DetachedDimuonAndJpsiLinePrescale',
        'B2DetachedDimuonAndJpsiLinePostscale',
    )

    def __init__(self, name, config):
        self.name = name
        #self.__confdict__ = default_config['CONFIG']
        self.__confdict__ = config

        LineBuilder.__init__(self, name, config)
        #checkConfig(B2MuMuMuMuLinesConf.__configuration_keys__,config)

        self.BDaughtersCuts = "(TRCHI2DOF < 2.5 ) & (MIPCHI2DV(PRIMARY)> 9.)"
        self.BMotherCuts = """
            (VFASPF(VCHI2/VDOF)<9)
            & (BPVDIRA > 0)
            & (BPVVDCHI2>100)
            & (BPVIPCHI2()< 25)
        """


        # Generaly used input particles
        self.inMuons = DataOnDemand(Location="Phys/StdLooseMuons/Particles")
        self.inKaons = DataOnDemand(Location="Phys/StdAllNoPIDsKaons/Particles")
        self.inPions = DataOnDemand(Location="Phys/StdAllNoPIDsPions/Particles")
        self.inDetachedDimuons = makeDetachedDimuons(
            name +'DetachedDimuons',
            config['DetachedDiMuons'],
            inputSel=[self.inMuons]
        )

        # Generaly used selections
        Jpsi_name = name + 'Jpsi'
        Phi_name = name + 'Phi'
        self.selJpsi = makeJpsi(self, Jpsi_name, inputSel=[self.inMuons])
        self.selJpsiWide = makeJpsiWide(self, Jpsi_name+"Wide", inputSel=[self.inMuons])
        self.selPhi = makePhi(self, Phi_name, inputSel=[self.inKaons])


        # Stripping lines
        default_name = name + "B24Mu"
        self.selDefault = makeDefault(self, default_name, inputSel=[self.inMuons])
        self.defaultLine = StrippingLine(
            default_name + "Line",
            prescale=config['B2MuMuMuMuLinePrescale'],
            postscale=config['B2MuMuMuMuLinePostscale'],
            algos=[self.selDefault]
        )
        self.registerLine(self.defaultLine)


        b23mupi_name = name + "B23MuPi"
        self.selB23mupi = makeB23mupi(self, b23mupi_name, inputSel=[self.inMuons, self.inPions])
        self.b23mupiLine = StrippingLine(
            b23mupi_name + "Line",
            prescale=config['B23MuPiLinePrescale'],
            postscale=config['B23MuPiLinePostscale'],
            algos=[self.selB23mupi]
        )
        self.registerLine(self.b23mupiLine)


        D_name = name + 'D24Mu'
        self.selD2MuMuMuMu = makeD2MuMuMuMu(D_name, inputSel=[self.inMuons])
        self.D2MuMuMuMuLine = StrippingLine(
            D_name + "Line",
            prescale=config['D2MuMuMuMuLinePrescale'],
            postscale=config['D2MuMuMuMuLinePostscale'],
            algos=[self.selD2MuMuMuMu]
        )
        self.registerLine(self.D2MuMuMuMuLine)



        B2DetachedDimuons_name = name +'B2DetachedDimuons'
        self.selB2TwoDetachedDimuons = makeB2TwoDetachedDimuons(
            B2DetachedDimuons_name,
            config['B2DetachedDiMuons'],
            inputSel=[self.inDetachedDimuons]
        )
        self.B2TwoDetachedDimuonsLine = StrippingLine(
            B2DetachedDimuons_name+"Line",
            prescale=config['B2TwoDetachedDimuonLinePrescale'],
            postscale=config['B2TwoDetachedDimuonLinePostscale'],
            algos=[self.selB2TwoDetachedDimuons]
        )
        self.registerLine(self.B2TwoDetachedDimuonsLine)



        B2JpsiKmumu_name = name + 'B2JpsiKmumu'
        self.selB2JpsiKmumu = makeB2JpsiKmumu(
            self,
            B2JpsiKmumu_name,
            inputSel=[self.selJpsi, self.inMuons,self.inKaons]
        )
        self.B2JpsiKmumuLine = StrippingLine(
            B2JpsiKmumu_name+"Line",
            prescale=config['B2JpsiKmumuLinePrescale'],
            postscale=config['B2JpsiKmumuLinePostscale'],
            algos=[self.selB2JpsiKmumu]
        )
        self.registerLine(self.B2JpsiKmumuLine)



        B2JpsiPhimumu_name = name + 'B2JpsiPhimumu'
        self.selB2JpsiPhimumu = makeB2JpsiPhimumu(
            self,
            B2JpsiPhimumu_name,
            inputSel=[self.selJpsi, self.selPhi, self.inMuons]
        )
        self.B2JpsiPhimumuLine = StrippingLine(
            B2JpsiPhimumu_name+"Line",
            prescale=config['B2JpsiPhimumuLinePrescale'],
            postscale=config['B2JpsiPhimumuLinePostscale'],
            algos=[self.selB2JpsiPhimumu]
        )
        self.registerLine(self.B2JpsiPhimumuLine)



        B2DetachedDimuonAndJpsi_name = name + 'B2DetachedDimuonAndJpsi'
        self.selB2DetachedDimuonAndJpsi = makeB2DetachedDimuonAndJpsi(
            B2DetachedDimuonAndJpsi_name,
            config['B2DetachedDiMuons'],
            inputSel=[self.selJpsiWide, self.inDetachedDimuons]
        )
        self.B2DetachedDimuonAndJpsiLine = StrippingLine(
            B2DetachedDimuonAndJpsi_name+"Line",
            prescale=config['B2DetachedDimuonAndJpsiLinePrescale'],
            postscale=config['B2DetachedDimuonAndJpsiLinePostscale'],
            algos=[self.selB2DetachedDimuonAndJpsi]
        )
        self.registerLine(self.B2DetachedDimuonAndJpsiLine)


def makeDefault(self,name,inputSel) :
    """
    B --> 4 mu selection
    should become     inclusive bb-->4 mu selection  ??
    """
    Detached4mu = Combine4Particles(
        DecayDescriptor="B_s0 -> mu+ mu- mu+ mu-",
        DaughtersCuts={ "mu+" : self.BDaughtersCuts},
        Combination12Cut="(AMAXDOCA('')<0.3*mm)",
        Combination123Cut="(AMAXDOCA('')<0.3*mm)",
        CombinationCut="(AM>4000*MeV) "\
                         "& (AMAXDOCA('')<0.3*mm)",
        MotherCut=self.BMotherCuts + " & (M>4000*MeV) "
    )

    return Selection(
        name,
        Algorithm=Detached4mu,
        RequiredSelections=inputSel
    )


def makeB23mupi(self,name,inputSel) :
    """
    B --> mu mu mu pi selection. pi = not-strong-selected muon
    """
    Detached3mupi = Combine4Particles(
        DecayDescriptor="B_s0 -> mu+ mu- mu+ pi-",
        DaughtersCuts={"mu+" : self.BDaughtersCuts, "pi+": self.BDaughtersCuts},
        Combination12Cut="(AMAXDOCA('')<0.3*mm)",
        Combination123Cut="(AMAXDOCA('')<0.3*mm)",
        CombinationCut="(AM>4000*MeV) "\
                         "& (AMAXDOCA('')<0.3*mm)",
        MotherCut=self.BMotherCuts + " & (M>4000*MeV) "
    )

    return Selection(
        name,
        Algorithm=Detached3mupi,
        RequiredSelections=inputSel
    )


def makeD2MuMuMuMu(name,inputSel) :
    """
    D --> 4 mu selection
    """
    D2MuMuMuMu = Combine4Particles(
        DecayDescriptor = "D0 -> mu+ mu- mu+ mu-",
        DaughtersCuts = {
            "mu+" : "(TRCHI2DOF < 3.0 ) "\
            " & (MIPCHI2DV(PRIMARY)> 4.)"\
            " & (TRGHOSTPROB<0.3) "\
            " & (P> 3000.*MeV)"
        },
        Combination12Cut = "(AMAXDOCA('')<0.2*mm)",
        Combination123Cut = "(AMAXDOCA('')<0.2*mm)",
        CombinationCut =  "(ADAMASS('D0')<300*MeV) "\
                          "& (AMAXDOCA('')<0.2*mm) ",
        MotherCut = "(VFASPF(VCHI2/VDOF)<12.) "\
			      "& (BPVVDZ > 0.) " \
			      "& (MIPCHI2DV(PRIMARY) < 25. )"
    )

    return Selection(
        name,
        Algorithm=D2MuMuMuMu,
        RequiredSelections=inputSel
    )


def makeDetachedDimuons(name,config,inputSel):
    """
    KS0->mumu selection
    """
    comboCuts = LoKiCuts(['ASUMPT','AMAXDOCA'],config).code()
    momCuts   = LoKiCuts(['VCHI2DOF','BPVVDCHI2'],config).code()
    KS2MuMu = CombineParticles("Combine"+name)
    KS2MuMu.DecayDescriptor='KS0 -> mu+ mu-'
    KS2MuMu.CombinationCut=comboCuts
    KS2MuMu.MotherCut=momCuts

    return Selection(
        name,
        Algorithm=KS2MuMu,
        RequiredSelections=inputSel
    )


def makeB2TwoDetachedDimuons(name,config,inputSel) :
    """
    B --> KS0 KS0 --> 4mu selection
    """
    # define cuts on B object
    wm = ['in_range(%s,AM,%s)' % (config['MASS_MIN']['B'],
                                  config['MASS_MAX']['B'])]
    wm = '('+('|'.join(wm))+')'
    comboCuts = [LoKiCuts(['SUMPT'],config).code(),wm]
    comboCuts = LoKiCuts.combine(comboCuts)
    momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVIPCHI2','BPVDIRA'],
                       config).code()
    B2KSKS = CombineParticles("Combine"+name)
    B2KSKS.DecayDescriptor = 'B0 -> KS0 KS0'
    B2KSKS.CombinationCut = comboCuts
    B2KSKS.MotherCut = momCuts

    return Selection(
        name,
        Algorithm=B2KSKS,
        RequiredSelections=inputSel
    )


def makeJpsi(self,name,inputSel):
    """
    Jpsi->mu+ mu- selection
    """
    JpsiMuMu = CombineParticles("Combine"+name)
    JpsiMuMu.DecayDescriptor='[J/psi(1S) -> mu+ mu-]cc'
    JpsiMuMu.CombinationCut ="(ADAMASS('J/psi(1S)') < 200*MeV)"\
        "& (AMAXDOCA('')<0.3*mm)"
    JpsiMuMu.MotherCut = "(ADMASS('J/psi(1S)') < 200*MeV)"\
        " & (BPVDIRA > 0) "
    JpsiMuMu.DaughtersCuts = { "mu+" : self.BDaughtersCuts}

    return Selection(
        name,
        Algorithm=JpsiMuMu,
        RequiredSelections=inputSel
    )


def makeJpsiWide(self,name,inputSel):
    """
    Jpsi->mu+ mu- selection not in Jpsi mass interval
    """
    JpsiMuMu = CombineParticles("Combine"+name)
    JpsiMuMu.DecayDescriptor='[J/psi(1S) -> mu+ mu-]cc'
    # JpsiMuMu.CombinationCut ="(ADAMASS('J/psi(1S)') > 200*MeV)"\
    #     "& (AMAXDOCA('')<0.3*mm)"
    JpsiMuMu.CombinationCut = "(AMAXDOCA('')<0.3*mm)"
    JpsiMuMu.MotherCut = "(ADMASS('J/psi(1S)') > 200*MeV)"\
        " & (BPVDIRA > 0) "
    JpsiMuMu.DaughtersCuts = { "mu+" : self.BDaughtersCuts}

    return Selection(
        name,
        Algorithm=JpsiMuMu,
        RequiredSelections=inputSel
    )


def makePhi(self,name,inputSel):
    """
    phi->K+ K- selection
    """
    Phimumu = CombineParticles("Combine"+name)
    Phimumu.DecayDescriptor = '[phi(1020) -> K+ K-]cc'
    Phimumu.CombinationCut = "(ADAMASS('phi(1020)')<200*MeV)"\
        "& (AMAXDOCA('')<0.3*mm)"
    Phimumu.MotherCut = "(ADMASS('phi(1020)') < 200*MeV)"\
        " & (BPVDIRA > 0) "
    Phimumu.DaughtersCuts = { "K+" : self.BDaughtersCuts}

    return Selection(
        name,
        Algorithm=Phimumu,
        RequiredSelections=inputSel
    )


def makeB2JpsiKmumu(self,name,inputSel):
    """
    B+ -> (Jpsi->mu+ mu-) K+ mu+ mu- selection
    """
    B2JpsiKmumu = Combine4Particles(
        DecayDescriptor="[B+ -> J/psi(1S) K+ mu+ mu-]cc",
        DaughtersCuts={
            "mu+" : self.BDaughtersCuts,
            "K+" : self.BDaughtersCuts
        },
        Combination12Cut="(AMAXDOCA('')<0.3*mm)",
        Combination123Cut="(AMAXDOCA('')<0.3*mm)",
        CombinationCut="(ADAMASS('B+')<1000*MeV)"\
                       "& (AMAXDOCA('')<0.3*mm)",
        MotherCut=self.BMotherCuts + " & (ADMASS('B+')<1000*MeV) "
    )

    return Selection(
        name,
        Algorithm=B2JpsiKmumu,
        RequiredSelections=inputSel
    )


def makeB2JpsiPhimumu(self,name,inputSel):
    """
    Bs -> (Jpsi->mu+ mu-) (phi -> K+ K-) mu+ mu- selection
    """
    B2JpsiPhimumu = Combine4Particles(
        DecayDescriptor="[B_s0 -> J/psi(1S) phi(1020) mu+ mu-]cc",
        DaughtersCuts={ "mu+" : self.BDaughtersCuts },
        Combination12Cut="(AMAXDOCA('')<0.3*mm)",
        Combination123Cut="(AMAXDOCA('')<0.3*mm)",
        CombinationCut="(ADAMASS('B_s0')<1000*MeV)"\
                       "& (AMAXDOCA('')<0.3*mm)",
        MotherCut=self.BMotherCuts + " & (ADMASS('B_s0')<1000*MeV) "
    )

    return Selection(
        name,
        Algorithm=B2JpsiPhimumu,
        RequiredSelections=inputSel
    )


def makeB2DetachedDimuonAndJpsi(name,config,inputSel) :
    """
    B --> J/psi KS0 --> 4mu selection
    """
    # define cuts on B object
    wm = ['in_range(%s,AM,%s)' % (config['MASS_MIN']['B'],
                                  config['MASS_MAX']['B'])]
    wm = '('+('|'.join(wm))+')'
    comboCuts = [LoKiCuts(['SUMPT'],config).code(),wm]
    comboCuts = LoKiCuts.combine(comboCuts)
    momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVIPCHI2','BPVDIRA'], config).code()

    B2JpsiKS = CombineParticles("Combine"+name)
    B2JpsiKS.DecayDescriptor = 'B0 -> J/psi(1S) KS0'
    B2JpsiKS.CombinationCut = comboCuts
    B2JpsiKS.MotherCut = momCuts

    return Selection(
        name,
        Algorithm=B2JpsiKS,
        RequiredSelections=inputSel
    )
