'''
Stripping lines for selection of
    [D+ -> K- pi+ pi+]cc
    [D+ -> K- K+ pi+]cc
    [D+ -> K- K+ K+]cc
    [D+ -> pi- pi+ pi+]cc
    [D+ -> K+ pi- pi+]cc
for open charm cross section measurement.
The three-body mass window is wide enough to select D+ and D_s+.

Adapted to current stripping framework by P. Spradlin.
'''

__author__ = ['Hamish Gordon', 'Patrick Spradlin']
__date__ = '03/09/2010'
__version__ = '$Revision: 2.0 $'

__all__ = (
    'default_config',
    'StrippingD2HHHForXSecConf'
)

from GaudiKernel.SystemOfUnits import MeV, GeV, mm, mrad
from Configurables import CombineParticles, FilterDesktop
from StandardParticles import (
    StdAllNoPIDsKaons,
    StdAllNoPIDsPions
)
from PhysSelPython.Wrappers import Selection
from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine
from MVADictHelpers import addTMVAclassifierValue

default_config = {
    'NAME': 'D2HHHForXSec',
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingD2HHHForXSecConf',
    'STREAMS': ['Charm'],
    'CONFIG': {
        # Minimum transverse momentum all D+ daughters must satisfy
        'Daug_All_PT_MIN': 200.0*MeV,
        # Minimum transverse momentum at least 2 D+ daughters must satisfy
        'Daug_2of3_PT_MIN': 400.0*MeV,
        # Minimum transverse momentum at least 1 D+ daughter must satisfy
        'Daug_1of3_PT_MIN': 400.0*MeV,
        # Minimum best primary vertex IP chi^2 all D+ daughters must satisfy
        'Daug_All_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 2 D+ daughters must satisfy
        'Daug_2of3_BPVIPCHI2_MIN': 10.0,
        # Minimum best PV IP chi^2 at least 1 D+ daughter must satisfy
        'Daug_1of3_BPVIPCHI2_MIN': 50.0,
        # Minimum D+ daughter momentum
        'Daug_P_MIN': 3.0*GeV,
        # Maximum D+ daughter momentum
        'Daug_P_MAX': 100.0*GeV,
        # Minimum D+ daughter pseudorapidity
        'Daug_ETA_MIN': 2.0,
        # Maximum D+ daughter pseudorapidity
        'Daug_ETA_MAX': 5.0,
        # Minimum D+ daughter kaon DLLK
        'K_PIDK_MIN': 5.0,
        # Maximum D+ daughter pion DLLK
        'Pi_PIDK_MAX': 3.0,
        # Minimum mass of the three-body HHH combination
        'Comb_AM_MIN': 1770.0*MeV,
        # Maximum mass of the three-body HHH combination
        'Comb_AM_MAX': 2070.0*MeV,
        # Maximum distance of closest approach of D+ daughters
        'Comb_ADOCAMAX_MAX': 0.5*mm,
        # Maximum D+ vertex chi^2 per vertex fit DoF
        'D_VCHI2VDOF_MAX': 25.0,
        # Maximum angle between D+ momentum and D+ direction of flight
        'D_acosBPVDIRA_MAX': 35.0*mrad,
        # Primary vertex displacement requirement, either that the D+ is some
        # sigma away from the PV, or it has a minimum flight time
        'D_PVDispCut': '((BPVVDCHI2 > 16.0)|(BPVLTIME() > 0.150*picosecond))',
        # Minimum D+ MVA discriminant value
        'D_MVA_MIN': -0.3,
        # Path to the D+ MVA weights file
        # BDT is not applied if this is the empty string or None
        'D_MVA_Weights': 'D2HHHForXSec_BDT_v1r0.xml',
        # Dictionary of LoKi functors defining the D+ MVA input variables
        # The keys must match those used when training the MVA
        'D_MVA_Variables': {
            # Largest D+ daughter PT
            'ROOTex::Leading(Dp_h1_PT,Dp_h2_PT,Dp_h3_PT)': (
                "MAXTREE(ISBASIC & HASTRACK, PT)"
            ),
            # Smallest D+ daughter PT
            'ROOTex::ThirdLeading(Dp_h1_PT,Dp_h2_PT,Dp_h3_PT)': (
                "MINTREE(ISBASIC & HASTRACK, PT)"
            ),
            'Dp_DIRA_OWNPV': 'BPVDIRA',
            'TMath::Log10(TMath::Abs(Dp_Loki_BPVVDCHI2))': (
                'log10(abs(BPVVDCHI2))'
            ),
            'TMath::Log10(TMath::Abs(Dp_Loki_BPVLTIME))': (
                "log10(abs(BPVLTIME()))"
            ),
            'TMath::Log10(Dp_ENDVERTEX_CHI2/Dp_ENDVERTEX_NDOF)': (
                'log10(VFASPF(VCHI2/VDOF))'
            )
        },
        # HLT filters, only process events firing triggers matching the RegEx
        'Hlt1Filter': None,
        'Hlt2Filter': None,
        # Fraction of candidates to randomly throw away before stripping
        'PrescaleD2KPP': 1.0,
        'PrescaleD2KKP': 1.0,
        'PrescaleD2KKK': -1.0,
        'PrescaleD2PPP': 1.0,
        'PrescaleD2KPPDCS': -1.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleD2KPP': 1.0,
        'PostscaleD2KKP': 1.0,
        'PostscaleD2KKK': -1.0,
        'PostscaleD2PPP': 1.0,
        'PostscaleD2KPPDCS': -1.0
    }
}


class StrippingD2HHHForXSecConf(LineBuilder):
    """Creates LineBuilder object containing the stripping lines."""
    # Allowed configuration keys
    __configuration_keys__ = default_config['CONFIG'].keys()

    # Decay descriptors
    D2KPP = ['[D+ -> K- pi+ pi+]cc']
    D2KKP = ['[D+ -> K- K+ pi+]cc']
    D2KKK = ['[D+ -> K- K+ K+]cc']
    D2PPP = ['[D+ -> pi- pi+ pi+]cc']
    D2KPPDCS = ['[D+ -> K+ pi- pi+]cc']

    def __init__(self, name, config):
        """Initialise this LineBuilder instance."""
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)

        # Line names
        d2KPP_name = '{0}D2KPP'.format(name)
        d2KKP_name = '{0}D2KKP'.format(name)
        d2KKK_name = '{0}D2KKK'.format(name)
        d2PPP_name = '{0}D2PPP'.format(name)
        d2PPK_name = '{0}D2KPPDCS'.format(name)

        self.inPions = StdAllNoPIDsPions
        self.inKaons = StdAllNoPIDsKaons

        # K- pi+ pi+
        self.selD2KPP = self.makeD2HHH(
            name=d2KPP_name,
            inputSel=[self.inPions, self.inKaons],
            decDescriptors=self.D2KPP
        )
        self.selD2KPPMVA = self.makeMVASelection(
            '{0}MVASelection'.format(d2KPP_name),
            self.selD2KPP
        )
        self.line_D2KPP = self.make_line(
            name='{0}Line'.format(d2KPP_name),
            selection=self.selD2KPPMVA,
            prescale=config['PrescaleD2KPP'],
            postscale=config['PostscaleD2KPP'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        # K- K+ pi+
        self.selD2KKP = self.makeD2HHH(
            name=d2KKP_name,
            inputSel=[self.inPions, self.inKaons],
            decDescriptors=self.D2KKP
        )
        self.selD2KKPMVA = self.makeMVASelection(
            '{0}MVASelection'.format(d2KKP_name),
            self.selD2KKP
        )
        self.line_D2KKP = self.make_line(
            name='{0}Line'.format(d2KKP_name),
            selection=self.selD2KKPMVA,
            prescale=config['PrescaleD2KKP'],
            postscale=config['PostscaleD2KKP'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        # K- K+ K-
        self.selD2KKK = self.makeD2HHH(
            name=d2KKK_name,
            inputSel=[self.inKaons],
            decDescriptors=self.D2KKK
        )
        self.selD2KKKMVA = self.makeMVASelection(
            '{0}MVASelection'.format(d2KKK_name),
            self.selD2KKK
        )
        self.line_D2KKK = self.make_line(
            name='{0}Line'.format(d2KKK_name),
            selection=self.selD2KKKMVA,
            prescale=config['PrescaleD2KKK'],
            postscale=config['PostscaleD2KKK'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        # pi- pi+ pi+
        self.selD2PPP = self.makeD2HHH(
            name=d2PPP_name,
            inputSel=[self.inPions],
            decDescriptors=self.D2PPP
        )
        self.selD2PPPMVA = self.makeMVASelection(
            '{0}MVASelection'.format(d2PPP_name),
            self.selD2PPP
        )
        self.line_D2PPP = self.make_line(
            name='{0}Line'.format(d2PPP_name),
            selection=self.selD2PPPMVA,
            prescale=config['PrescaleD2PPP'],
            postscale=config['PostscaleD2PPP'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        # K+ pi- pi+ (DCS)
        self.selD2PPK = self.makeD2HHH(
            name=d2PPK_name,
            inputSel=[self.inPions, self.inKaons],
            decDescriptors=self.D2KPPDCS
        )
        self.selD2PPKMVA = self.makeMVASelection(
            '{0}MVASelection'.format(d2PPK_name),
            self.selD2PPK
        )
        self.line_D2PPK = self.make_line(
            name='{0}Line'.format(d2PPK_name),
            selection=self.selD2PPKMVA,
            prescale=config['PrescaleD2KPPDCS'],
            postscale=config['PostscaleD2KPPDCS'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

    def make_line(self, name, selection, prescale, postscale, **kwargs):
        """Create the stripping line defined by the selection.

        Keyword arguments:
        name -- Base name for the Line
        selection -- Selection instance
        prescale -- Fraction of candidates to randomly drop before stripping
        postscale -- Fraction of candidates to randomly drop after stripping
        **kwargs -- Keyword arguments passed to StrippingLine constructor
        """
        # Only create the line with positive pre- and postscales
        # You can disable each line by setting either to a negative value
        if prescale > 0 and postscale > 0:
            line = StrippingLine(
                name,
                selection=selection,
                prescale=prescale,
                postscale=postscale,
                **kwargs
            )
            self.registerLine(line)
            return line
        else:
            return False

    def makeD2HHH(self, name, inputSel, decDescriptors):
        """Return a Selection instance for a D+ -> h- h+ h- decay.

        Keyword arguments:
        name -- Name to give the Selection instance
        inputSel -- List of inputs passed to Selection.RequiredSelections
        decDescriptors -- List of decay descriptors for CombineParticles
        """
        lclPreambulo = [
            'from math import cos'
        ]

        daugCuts = (
            '(PT > {0[Daug_All_PT_MIN]})'
            '& (BPVIPCHI2() > {0[Daug_All_BPVIPCHI2_MIN]})'
        ).format(self.config)

        pidFiducialCuts = (
            '(in_range({0[Daug_P_MIN]}, P, {0[Daug_P_MAX]}))'
            '& (in_range({0[Daug_ETA_MIN]}, ETA, {0[Daug_ETA_MAX]}))'
        ).format(self.config)

        kaonPIDCuts = (
            pidFiducialCuts +
            '& (PIDK-PIDpi > {0[K_PIDK_MIN]})'
        ).format(self.config)

        pionPIDCuts = (
            pidFiducialCuts +
            '& (PIDK-PIDpi < {0[Pi_PIDK_MAX]})'
        ).format(self.config)

        combCuts = (
            '(in_range({0[Comb_AM_MIN]}, AM, {0[Comb_AM_MAX]}))'
            '& (AMAXCHILD(PT) > {0[Daug_1of3_PT_MIN]})'
            '& (AMAXCHILD(BPVIPCHI2()) > {0[Daug_1of3_BPVIPCHI2_MIN]})'
            '& (ANUM(PT > {0[Daug_2of3_PT_MIN]}) >= 2)'
            '& (ANUM(BPVIPCHI2() > {0[Daug_2of3_BPVIPCHI2_MIN]}) >= 2)'
            "& (ACUTDOCA({0[Comb_ADOCAMAX_MAX]}, ''))"
        ).format(self.config)

        dCuts = (
            '(VFASPF(VCHI2/VDOF) < {0[D_VCHI2VDOF_MAX]})'
            '& ({0[D_PVDispCut]})'
            '& (BPVDIRA > cos({0[D_acosBPVDIRA_MAX]}))'
        ).format(self.config)

        _dplus = CombineParticles(
            name='Combine{0}'.format(name),
            DecayDescriptors=decDescriptors,
            Preambulo=lclPreambulo,
            DaughtersCuts={
                'pi+': '{0} & {1}'.format(daugCuts, pionPIDCuts),
                'K+': '{0} & {1}'.format(daugCuts, kaonPIDCuts)
            },
            CombinationCut=combCuts,
            MotherCut=dCuts
        )

        return Selection(name, Algorithm=_dplus, RequiredSelections=inputSel)

    def makeMVASelection(self, name, inputSel):
        # Don't apply a BDT if the weights file has not been specified
        if not self.config['D_MVA_Weights']:
            return inputSel

        cut = "VALUE('LoKi::Hybrid::DictValue/{0}') > {1[D_MVA_MIN]}".format(
            name, self.config
        )
        mva = FilterDesktop('{0}Filter'.format(name), Code=cut)

        addTMVAclassifierValue(
            Component=mva,
            XMLFile=self.config['D_MVA_Weights'],
            Variables=self.config['D_MVA_Variables'],
            ToolName=name
        )

        return Selection(name, Algorithm=mva, RequiredSelections=[inputSel])
