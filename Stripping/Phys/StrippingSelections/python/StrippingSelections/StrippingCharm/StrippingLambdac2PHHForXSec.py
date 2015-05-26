"""
Stripping lines for selection of
    [Lambda_c+ -> p+ K- pi+]cc
    [Lambda_c+ -> p+ K- K+]cc
    [Lambda_c+ -> p+ pi- pi+]cc
    [Lambda_c+ -> p+ pi- K+]cc
and
    [Sigma_c0 -> (Lambda_c+ -> p+ K- pi+) pi-]cc
    [Sigma_c++ -> (Lambda_c+ -> p+ K- pi+) pi+]cc
for open charm cross section measurement.
"""

__author__ = ['Alex Pearce']

__all__ = (
    'default_config',
    'StrippingLambdac2PHHForXSecConf'
)

from GaudiKernel.SystemOfUnits import MeV, GeV, mm, mrad
from Configurables import CombineParticles, FilterDesktop
from StandardParticles import (
    StdAllNoPIDsProtons,
    StdAllNoPIDsKaons,
    StdAllNoPIDsPions
)
from PhysSelPython.Wrappers import Selection
from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine
from MVADictHelpers import addTMVAclassifierValue

default_config = {
    'NAME': 'Lambdac2PHHForXSec',
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingLambdac2PHHForXSecConf',
    'STREAMS': ['Charm'],
    'CONFIG': {
        # Minimum transverse momentum all Lc+ daughters must satisfy
        'Daug_All_PT_MIN': 200.0*MeV,
        # Minimum transverse momentum at least 2 Lc+ daughters must satisfy
        'Daug_2of3_PT_MIN': 400.0*MeV,
        # Minimum transverse momentum at least 1 Lc+ daughter must satisfy
        'Daug_1of3_PT_MIN': 1000.0*MeV,
        # Minimum best primary vertex IP chi^2 all Lc+ daughters must satisfy
        'Daug_All_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 2 Lc+ daughters must satisfy
        'Daug_2of3_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 1 Lc+ daughter must satisfy
        'Daug_1of3_BPVIPCHI2_MIN': 6.0,
        # Minimum Lc+ daughter momentum
        'Daug_P_MIN': 3.0*GeV,
        # Maximum Lc+ daughter momentum
        'Daug_P_MAX': 100.0*GeV,
        # Minimum Lc+ daughter pseudorapidity
        'Daug_ETA_MIN': 2.0,
        # Maximum Lc+ daughter pseudorapidity
        'Daug_ETA_MAX': 5.0,
        # Minimum Lc+ daughter proton DLLp
        'Proton_PIDpPIDpi_MIN': 10.0,
        # Minimum Lc+ daughter proton DLLp - DLLK
        'Proton_PIDpPIDK_MIN': 5.0,
        # Minimum Lc+ daughter kaon DLLK
        'K_PIDK_MIN': 5.0,
        # Maximum Lc+ daughter pion DLLK
        'Pi_PIDK_MAX': 3.0,
        # Lc+ mass window around the nominal Lc+ mass before the vertex fit
        'Comb_ADAMASS_WIN': 90.0*MeV,
        # Maximum distance of closest approach of Lc+ daughters
        'Comb_ADOCAMAX_MAX': 0.5*mm,
        # Maximum Lc+ vertex chi^2 per vertex fit DoF
        'Lambdac_VCHI2VDOF_MAX': 25.0,
        # Maximum angle between Lc+ momentum and Lc+ direction of flight
        'Lambdac_acosBPVDIRA_MAX': 35.0*mrad,
        # Primary vertex displacement requirement, that the Lc+ is some sigma
        # away from the PV and it has a minimum flight time
        'Lambdac_PVDispCut': (
            '((BPVVDCHI2 > 4.0) &'
            '(BPVLTIME() > 0.075*picosecond))'
        ),
        # Maximum delta mass value m(Sigma_c) - m(Lambda_c+) (PDG dm: 167 MeV)
        'Sigmac_AMDiff_MAX': 185.0*MeV,
        # Maximum Sigma_c vertex chi^2 per vertex DoF
        'Sigmac_VCHI2VDOF_MAX': 25.0,
        # Minimum Lc+ MVA discriminant value
        'Lambdac_MVA_MIN': -0.3,
        # Path to the Lc+ MVA weights file
        # BDT is not applied if this is the empty string or None
        'Lambdac_MVA_Weights': '$TMVAWEIGHTSROOT/data/Lambdac2PHHForXSec_BDT_v1r0.xml',
        # Dictionary of LoKi functors defining the Lc+ MVA input variables
        # The keys must match those used when training the MVA
        'Lambdac_MVA_Variables': {
            # Largest Lc+ daughter PT
            'ROOTex::Leading(Lc_h1_PT,Lc_h2_PT,Lc_p_PT)': (
                "MAXTREE(ISBASIC & HASTRACK, PT)"
            ),
            # Smallest Lc+ daughter PT
            'ROOTex::ThirdLeading(Lc_h1_PT,Lc_h2_PT,Lc_p_PT)': (
                "MINTREE(ISBASIC & HASTRACK, PT)"
            ),
            'Lc_DIRA_OWNPV': 'BPVDIRA',
            'Lc_Loki_LV02': 'LV02',
            'TMath::Log10(TMath::Abs(Lc_Loki_BPVVDCHI2))': (
                'log10(abs(BPVVDCHI2))'
            ),
            'TMath::Log10(TMath::Abs(Lc_Loki_BPVLTIME))': (
                "log10(abs(BPVLTIME()))"
            ),
            'TMath::Log10(Lc_ENDVERTEX_CHI2/Lc_ENDVERTEX_NDOF)': (
                'log10(VFASPF(VCHI2/VDOF))'
            )
        },
        # HLT filters, only process events firing triggers matching the RegEx
        'Hlt1Filter': None,
        'Hlt2Filter': None,
        # Fraction of candidates to randomly throw away before stripping
        'PrescaleLambdac2PKPi': 0.0,
        'PrescaleLambdac2PKK': 0.0,
        'PrescaleLambdac2PPiPi': 0.0,
        'PrescaleLambdac2PPiKWS': 0.0,
        'PrescaleSigmacz2LambdacPi': 0.0,
        'PrescaleSigmacpp2LambdacPi': 0.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleLambdac2PKPi': 1.0,
        'PostscaleLambdac2PKK': 1.0,
        'PostscaleLambdac2PPiPi': 1.0,
        'PostscaleLambdac2PPiKWS': -1.0,
        'PostscaleSigmacz2LambdacPi': 1.0,
        'PostscaleSigmacpp2LambdacPi': 1.0
    }
}


class StrippingLambdac2PHHForXSecConf(LineBuilder):
    """Creates LineBuilder object containing the stripping lines."""
    # Allowed configuration keys
    __configuration_keys__ = default_config['CONFIG'].keys()

    # Decay descriptors
    Lambdac2PKPi = ['[Lambda_c+ -> p+ K- pi+]cc']
    Lambdac2PKK = ['[Lambda_c+ -> p+ K- K+]cc']
    Lambdac2PPiPi = ['[Lambda_c+ -> p+ pi- pi+]cc']
    Lambdac2PPiKWS = ['[Lambda_c+ -> p+ pi- K+]cc']
    Sigmacz2LambdacPi = ['[Sigma_c0 -> Lambda_c+ pi-]cc']
    Sigmacpp2LambdacPi = ['[Sigma_c++ -> Lambda_c+ pi+]cc']

    def __init__(self, name, config):
        """Initialise this LineBuilder instance."""
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)

        # Line names
        lambdac_name = '{0}Lambdac2PKPi'.format(name)
        lambdac_pKK_name = '{0}Lambdac2PKK'.format(name)
        lambdac_ppipi_name = '{0}Lambdac2PPiPi'.format(name)
        lambdac_ppiK_name = '{0}Lambdac2PPiKWS'.format(name)
        sigmacz_name = '{0}Sigmacz2LambdacPi'.format(name)
        sigmacpp_name = '{0}Sigmacpp2LambdacPi'.format(name)

        # Build pion, kaon, and proton cut strings
        daugCuts = (
            '(PT > {0[Daug_All_PT_MIN]})'
            '& (BPVIPCHI2() > {0[Daug_All_BPVIPCHI2_MIN]})'
        ).format(self.config)
        pidFiducialCuts = (
            '(in_range({0[Daug_P_MIN]}, P, {0[Daug_P_MAX]}))'
            '& (in_range({0[Daug_ETA_MIN]}, ETA, {0[Daug_ETA_MAX]}))'
        ).format(self.config)
        protonPIDCuts = (
            pidFiducialCuts +
            '& ((PIDp - PIDpi) > {0[Proton_PIDpPIDpi_MIN]})'
            '& ((PIDp - PIDK) > {0[Proton_PIDpPIDK_MIN]})'
        ).format(self.config)
        kaonPIDCuts = (
            pidFiducialCuts +
            '& ((PIDK - PIDpi) > {0[K_PIDK_MIN]})'
        ).format(self.config)
        pionPIDCuts = (
            pidFiducialCuts +
            '& ((PIDK - PIDpi) < {0[Pi_PIDK_MAX]})'
        ).format(self.config)
        pionCuts = '{0} & {1}'.format(daugCuts, pionPIDCuts)
        kaonCuts = '{0} & {1}'.format(daugCuts, kaonPIDCuts)
        protonCuts = '{0} & {1}'.format(daugCuts, protonPIDCuts)

        # Filter StdAllNoPIDs particles
        self.inPions = Selection(
            'PionsFor{0}'.format(name),
            Algorithm=FilterDesktop(
                'FilterPionsFor{0}'.format(name),
                Code=pionCuts
            ),
            RequiredSelections=[StdAllNoPIDsPions]
        )
        self.inKaons = Selection(
            'KaonsFor{0}'.format(name),
            Algorithm=FilterDesktop(
                'FilterKaonsFor{0}'.format(name),
                Code=kaonCuts
            ),
            RequiredSelections=[StdAllNoPIDsKaons]
        )
        self.inProtons = Selection(
            'ProtonsFor{0}'.format(name),
            Algorithm=FilterDesktop(
                'FilterProtonsFor{0}'.format(name),
                Code=protonCuts
            ),
            RequiredSelections=[StdAllNoPIDsProtons]
        )
        # Don't used filtered pions for soft pions, cut would be too tight
        self.inSoftPions = StdAllNoPIDsPions

        self.selLambdac2PKPi = self.makeLambdac2PHH(
            name=lambdac_name,
            inputSel=[self.inProtons, self.inKaons, self.inPions],
            decDescriptors=self.Lambdac2PKPi
        )
        self.selLambdac2PKPiMVA = self.makeMVASelection(
            '{0}MVASelection'.format(lambdac_name),
            self.selLambdac2PKPi
        )

        self.selLambdac2PKK = self.makeLambdac2PHH(
            name=lambdac_pKK_name,
            inputSel=[self.inProtons, self.inKaons],
            decDescriptors=self.Lambdac2PKK
        )
        self.selLambdac2PKKMVA = self.makeMVASelection(
            '{0}MVASelection'.format(lambdac_pKK_name),
            self.selLambdac2PKK
        )

        self.selLambdac2PPiPi = self.makeLambdac2PHH(
            name=lambdac_ppipi_name,
            inputSel=[self.inProtons, self.inPions],
            decDescriptors=self.Lambdac2PPiPi
        )
        self.selLambdac2PPiPiMVA = self.makeMVASelection(
            '{0}MVASelection'.format(lambdac_ppipi_name),
            self.selLambdac2PPiPi
        )

        self.selLambdac2PPiKWS = self.makeLambdac2PHH(
            name=lambdac_ppiK_name,
            inputSel=[self.inProtons, self.inKaons, self.inPions],
            decDescriptors=self.Lambdac2PPiKWS
        )
        self.selLambdac2PPiKWSMVA = self.makeMVASelection(
            '{0}MVASelection'.format(lambdac_ppiK_name),
            self.selLambdac2PPiKWS
        )

        self.selSigmacz2LambdacPi = self.makeSigmac2LambdacPi(
            name=sigmacz_name,
            inputSel=[self.selLambdac2PKPiMVA, self.inSoftPions],
            decDescriptors=self.Sigmacz2LambdacPi
        )

        self.selSigmacpp2LambdacPi = self.makeSigmac2LambdacPi(
            name=sigmacpp_name,
            inputSel=[self.selLambdac2PKPiMVA, self.inSoftPions],
            decDescriptors=self.Sigmacpp2LambdacPi
        )

        self.line_Lambdac2PKPi = self.make_line(
            name='{0}Line'.format(lambdac_name),
            selection=self.selLambdac2PKPiMVA,
            prescale=config['PrescaleLambdac2PKPi'],
            postscale=config['PostscaleLambdac2PKPi'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        self.line_Lambdac2PKK = self.make_line(
            name='{0}Line'.format(lambdac_pKK_name),
            selection=self.selLambdac2PKKMVA,
            prescale=config['PrescaleLambdac2PKK'],
            postscale=config['PostscaleLambdac2PKK'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        self.line_Lambdac2PPiPi = self.make_line(
            name='{0}Line'.format(lambdac_ppipi_name),
            selection=self.selLambdac2PPiPiMVA,
            prescale=config['PrescaleLambdac2PPiPi'],
            postscale=config['PostscaleLambdac2PPiPi'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        self.line_Lambdac2PPiKWS = self.make_line(
            name='{0}Line'.format(lambdac_ppiK_name),
            selection=self.selLambdac2PPiKWSMVA,
            prescale=config['PrescaleLambdac2PPiKWS'],
            postscale=config['PostscaleLambdac2PPiKWS'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        self.line_Sigmacz2LambdacPi = self.make_line(
            name='{0}Line'.format(sigmacz_name),
            selection=self.selSigmacz2LambdacPi,
            prescale=config['PrescaleSigmacz2LambdacPi'],
            postscale=config['PostscaleSigmacz2LambdacPi'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        self.line_Sigmacpp2LambdacPi = self.make_line(
            name='{0}Line'.format(sigmacpp_name),
            selection=self.selSigmacpp2LambdacPi,
            prescale=config['PrescaleSigmacpp2LambdacPi'],
            postscale=config['PostscaleSigmacpp2LambdacPi'],
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

    def makeLambdac2PHH(self, name, inputSel, decDescriptors):
        """Return a Selection instance for a Lambda_c+ -> p+ h- h+ decay.

        Keyword arguments:
        name -- Name to give the Selection instance
        inputSel -- List of inputs passed to Selection.RequiredSelections
        decDescriptors -- List of decay descriptors for CombineParticles
        """
        lclPreambulo = [
            'from math import cos'
        ]

        combCuts = (
            "(ADAMASS('Lambda_c+') < {0[Comb_ADAMASS_WIN]})"
            '& (AMAXCHILD(PT) > {0[Daug_1of3_PT_MIN]})'
            '& (AMAXCHILD(BPVIPCHI2()) > {0[Daug_1of3_BPVIPCHI2_MIN]})'
            '& (ANUM(PT > {0[Daug_2of3_PT_MIN]}) >= 2)'
            '& (ANUM(BPVIPCHI2() > {0[Daug_2of3_BPVIPCHI2_MIN]}) >= 2)'
            "& (ACUTDOCA({0[Comb_ADOCAMAX_MAX]}, ''))"
        ).format(self.config)

        lambdacCuts = (
            '(VFASPF(VCHI2/VDOF) < {0[Lambdac_VCHI2VDOF_MAX]})'
            '& ({0[Lambdac_PVDispCut]})'
            '& (BPVDIRA > cos({0[Lambdac_acosBPVDIRA_MAX]}))'
        ).format(self.config)

        _Lambdac = CombineParticles(
            name='Combine{0}'.format(name),
            DecayDescriptors=decDescriptors,
            Preambulo=lclPreambulo,
            CombinationCut=combCuts,
            MotherCut=lambdacCuts
        )

        return Selection(name, Algorithm=_Lambdac, RequiredSelections=inputSel)

    def makeSigmac2LambdacPi(self, name, inputSel, decDescriptors):
        """Return a Selection instance for a Sigma_c -> Lambda_c+ pi decay.

        The selection is motivated in precisely the same way as a D*+ -> D0 pi+
        selection, i.e. take a Lambda_c+, attach a pion, and make some vertex
        quality requirement.
        Keyword arguments:
        name -- Name to give the Selection instance
        inputSel -- List of inputs passed to Selection.RequiredSelections
        decDescriptors -- List of decay descriptors for CombineParticles
        """
        combCuts = '((AM - AM1) < {0[Sigmac_AMDiff_MAX]})'.format(self.config)

        dstarCuts = '(VFASPF(VCHI2/VDOF) < {0[Sigmac_VCHI2VDOF_MAX]})'.format(
            self.config
        )

        _Sigmac = CombineParticles(
            name='Combine{0}'.format(name),
            DecayDescriptors=decDescriptors,
            CombinationCut=combCuts,
            MotherCut=dstarCuts
        )

        return Selection(name, Algorithm=_Sigmac, RequiredSelections=inputSel)

    def makeMVASelection(self, name, inputSel):
        # Don't apply a BDT if the weights file has not been specified
        if not self.config['Lambdac_MVA_Weights']:
            return inputSel

        cut = (
            "VALUE('LoKi::Hybrid::DictValue/{0}') > {1[Lambdac_MVA_MIN]}"
        ).format(name, self.config)
        mva = FilterDesktop('{0}Filter'.format(name), Code=cut)

        addTMVAclassifierValue(
            Component=mva,
            XMLFile=self.config['Lambdac_MVA_Weights'],
            Variables=self.config['Lambdac_MVA_Variables'],
            ToolName=name
        )

        return Selection(name, Algorithm=mva, RequiredSelections=[inputSel])
