"""
Stripping lines for selection of
    [Lambda_c+ -> (Lambda0 -> p+ pi-) pi+]CC
    [Lambda_c+ -> (Lambda0 -> p+ pi-) K+]CC
    [Lambda_c+ -> p+ (KS0 -> pi+ pi-)]CC
Throughout this file, 'Bachelor' refers to the child of the Lambda_c+ which is
not part of the V0 decay, e.g. the proton in the [Lambda_c+ -> p+ KS0]CC decay.
"""

__author__ = ['Alex Pearce', 'Mohamad Kozeiha']

__all__ = (
    'default_config',
    'StrippingLambdac2V0HConf'
)

from GaudiKernel.SystemOfUnits import MeV, GeV, mm, mrad
from Configurables import CombineParticles, FilterDesktop
from StandardParticles import (
    StdLooseLambdaLL,
    StdLooseLambdaDD,
    StdLooseKsLL,
    StdLooseKsDD,
    StdLooseProtons,
    StdLooseKaons,
    StdLoosePions
)
from PhysSelPython.Wrappers import Selection
from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine

default_config = {
    'NAME': 'Lambdac2V0H',
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingLambdac2V0HConf',
    'STREAMS': ['Charm'],
    'CONFIG': {
        # Minimum transverse momentum all Lc+ bachelors must satisfy
        'Bach_All_PT_MIN': 750.0*MeV,
        # Minimum best primary vertex IP chi^2 all Lc+ bachelors must satisfy
        'Bach_All_BPVIPCHI2_MIN': 4.0,
        # Minimum Lc+ bachelor momentum
        'Bach_P_MIN': 2.0*GeV,
        # Maximum Lc+ bachelor momentum
        'Bach_P_MAX': 100.0*GeV,
        # Minimum Lc+ bachelor pseudorapidity
        'Bach_ETA_MIN': 2.0,
        # Maximum Lc+ bachelor pseudorapidity
        'Bach_ETA_MAX': 5.0,
        # Minimum Lc+ bachelor proton DLLp
        'Proton_PIDpPIDpi_MIN': 7.0,
        # Minimum Lc+ bachelor proton DLLp - DLLK
        'Proton_PIDpPIDK_MIN': 2.0,
        # Minimum Lc+ bachelor kaon DLLK
        'K_PIDK_MIN': 5.0,
        # Maximum Lc+ bachelor pion DLLK
        'Pi_PIDK_MAX': 3.0,
        # Minimum L0 momentum
        'Lambda0_P_MIN': 2000*MeV,
        # Minimum L0 transverse momentum
        'Lambda0_PT_MIN': 500*MeV,
        # Minimum flight distance chi^2 of L0 from the primary vertex
        'Lambda0_FDCHI2_MIN': 100,
        # Maximum L0 vertex chi^2 per vertex fit DoF
        'Lambda0_VCHI2VDOF_MAX': 10.0,
        # Minimum KS0 momentum
        'KS0_P_MIN': 2000*MeV,
        # Minimum KS0 transverse momentum
        'KS0_PT_MIN': 500*MeV,
        # Minimum flight distance chi^2 of KS0 from the primary vertex
        'KS0_FDCHI2_MIN': 100,
        # Maximum KS0 vertex chi^2 per vertex fit DoF
        'KS0_VCHI2VDOF_MAX': 10.0,
        # Lc+ mass window around the nominal Lc+ mass before the vertex fit
        'Comb_ADAMASS_WIN': 90.0*MeV,
        # Maximum distance of closest approach of Lc+ children
        'Comb_ADOCAMAX_MAX': 0.5*mm,
        # Minimum transverse momentum of Lc+ children
        'Comb_PT_MIN': 1.0*GeV,
        # Maximum Lc+ vertex chi^2 per vertex fit DoF
        'Lambdac_VCHI2VDOF_MAX': 10.0,
        # Maximum angle between Lc+ momentum and Lc+ direction of flight
        'Lambdac_acosBPVDIRA_MAX': 140.0*mrad,
        # Primary vertex displacement requirement, either that the Lc+ is some
        # sigma away from the PV, or it has a minimum flight time
        'Lambdac_PVDispCut': '(BPVVDCHI2 > 16.0)',
        # HLT filters, only process events firing triggers matching the RegEx
        'Hlt1Filter': None,
        'Hlt2Filter': None,
        # Fraction of candidates to randomly throw away before stripping
        'PrescaleLambdac2Lambda0PiLL': 1.0,
        'PrescaleLambdac2Lambda0PiDD': 1.0,
        'PrescaleLambdac2Lambda0KLL': 1.0,
        'PrescaleLambdac2Lambda0KDD': 1.0,
        'PrescaleLambdac2pKS0LL': 1.0,
        'PrescaleLambdac2pKS0DD': 1.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleLambdac2Lambda0PiLL': 1.0,
        'PostscaleLambdac2Lambda0PiDD': 1.0,
        'PostscaleLambdac2Lambda0KLL': 1.0,
        'PostscaleLambdac2Lambda0KDD': 1.0,
        'PostscaleLambdac2pKS0LL': 1.0,
        'PostscaleLambdac2pKS0DD': 1.0
    }
}


class StrippingLambdac2V0HConf(LineBuilder):
    """Creates LineBuilder object containing the stripping lines."""
    # Allowed configuration keys
    __configuration_keys__ = default_config['CONFIG'].keys()

    # Decay descriptors
    Lambdac2Lambda0Pi = ['[Lambda_c+ -> Lambda0 pi+]cc']
    Lambdac2Lambda0K = ['[Lambda_c+ -> Lambda0 K+]cc']
    Lambdac2pKS0 = ['[Lambda_c+ -> p+ KS0]cc']

    def __init__(self, name, config):
        """Initialise this LineBuilder instance."""
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)

        # Line names
        # 'LL' and 'DD' will be appended to these names for the LL and DD
        # Selection and StrippingLine instances
        Lambdac2Lambda0Pi_name = '{0}Lambdac2Lambda0Pi'.format(name)
        Lambdac2Lambda0K_name = '{0}Lambdac2Lambda0K'.format(name)
        Lambdac2pKS0_name = '{0}Lambdac2pKS0'.format(name)

        # Build bachelor pion, kaon, and proton cut strings
        childCuts = (
            '(PT > {0[Bach_All_PT_MIN]})'
            '& (BPVIPCHI2() > {0[Bach_All_BPVIPCHI2_MIN]})'
        ).format(self.config)
        pidFiducialCuts = (
            '(in_range({0[Bach_P_MIN]}, P, {0[Bach_P_MAX]}))'
            '& (in_range({0[Bach_ETA_MIN]}, ETA, {0[Bach_ETA_MAX]}))'
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
        pionCuts = '{0} & {1}'.format(childCuts, pionPIDCuts)
        kaonCuts = '{0} & {1}'.format(childCuts, kaonPIDCuts)
        protonCuts = '{0} & {1}'.format(childCuts, protonPIDCuts)

        # Build Lambda0 and KS0 cut strings
        lambda0Cuts = (
            '(P > {0[Lambda0_P_MIN]})'
            '& (PT > {0[Lambda0_PT_MIN]})'
            '& (BPVVDCHI2 > {0[Lambda0_FDCHI2_MIN]})'
            '& (VFASPF(VCHI2/VDOF) < {0[Lambda0_VCHI2VDOF_MAX]})'
        ).format(self.config)
        ks0Cuts = (
            '(P > {0[KS0_P_MIN]})'
            '& (PT > {0[KS0_PT_MIN]})'
            '& (BPVVDCHI2 > {0[KS0_FDCHI2_MIN]})'
            '& (VFASPF(VCHI2/VDOF) < {0[KS0_VCHI2VDOF_MAX]})'
        ).format(self.config)
        # Define any additional cuts on LL/DD difference
        lambda0LLCuts = lambda0Cuts
        lambda0DDCuts = lambda0Cuts
        ks0LLCuts = ks0Cuts
        ks0DDCuts = ks0Cuts

        # Filter StdLoose particles
        self.inPions = Selection(
            'PionsFor{0}'.format(name),
            Algorithm=FilterDesktop(
                'FilterPionsFor{0}'.format(name),
                Code=pionCuts
            ),
            RequiredSelections=[StdLoosePions]
        )
        self.inKaons = Selection(
            'KaonsFor{0}'.format(name),
            Algorithm=FilterDesktop(
                'FilterKaonsFor{0}'.format(name),
                Code=kaonCuts
            ),
            RequiredSelections=[StdLooseKaons]
        )
        self.inProtons = Selection(
            'ProtonsFor{0}'.format(name),
            Algorithm=FilterDesktop(
                'FilterProtonsFor{0}'.format(name),
                Code=protonCuts
            ),
            RequiredSelections=[StdLooseProtons]
        )
        self.inLambda0LL = Selection(
            'Lambda0LLFor{0}'.format(name),
            Algorithm=FilterDesktop(
                'FilterLambda0LLFor{0}'.format(name),
                Code=lambda0LLCuts
            ),
            RequiredSelections=[StdLooseLambdaLL]
        )
        self.inLambda0DD = Selection(
            'Lambda0DDFor{0}'.format(name),
            Algorithm=FilterDesktop(
                'FilterLambda0DDFor{0}'.format(name),
                Code=lambda0DDCuts
            ),
            RequiredSelections=[StdLooseLambdaDD]
        )
        self.inKS0LL = Selection(
            'KS0LLFor{0}'.format(name),
            Algorithm=FilterDesktop(
                'FilterKS0LLFor{0}'.format(name),
                Code=ks0LLCuts
            ),
            RequiredSelections=[StdLooseKsLL]
        )
        self.inKS0DD = Selection(
            'KS0DDFor{0}'.format(name),
            Algorithm=FilterDesktop(
                'FilterKS0DDFor{0}'.format(name),
                Code=ks0DDCuts
            ),
            RequiredSelections=[StdLooseKsDD]
        )

        self.selLambdac2Lambda0Pi = self.makeLambdac2V0H(
            name=Lambdac2Lambda0Pi_name,
            inputSelLL=[self.inLambda0LL, self.inPions],
            inputSelDD=[self.inLambda0DD, self.inPions],
            decDescriptors=self.Lambdac2Lambda0Pi
        )

        self.selLambdac2Lambda0K = self.makeLambdac2V0H(
            name=Lambdac2Lambda0K_name,
            inputSelLL=[self.inLambda0LL, self.inKaons],
            inputSelDD=[self.inLambda0DD, self.inKaons],
            decDescriptors=self.Lambdac2Lambda0K
        )

        self.selLambdac2pKS0 = self.makeLambdac2V0H(
            name=Lambdac2pKS0_name,
            inputSelLL=[self.inKS0LL, self.inProtons],
            inputSelDD=[self.inKS0DD, self.inProtons],
            decDescriptors=self.Lambdac2pKS0
        )

        self.line_Lambdac2Lambda0PiLL = self.make_line(
            name='{0}LLLine'.format(Lambdac2Lambda0Pi_name),
            selection=self.selLambdac2Lambda0Pi[0],
            prescale=config['PrescaleLambdac2Lambda0PiLL'],
            postscale=config['PostscaleLambdac2Lambda0PiLL'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )
        self.line_Lambdac2Lambda0PiDD = self.make_line(
            name='{0}DDLine'.format(Lambdac2Lambda0Pi_name),
            selection=self.selLambdac2Lambda0Pi[1],
            prescale=config['PrescaleLambdac2Lambda0PiDD'],
            postscale=config['PostscaleLambdac2Lambda0PiDD'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        self.line_Lambdac2Lambda0KLL = self.make_line(
            name='{0}LLLine'.format(Lambdac2Lambda0K_name),
            selection=self.selLambdac2Lambda0K[0],
            prescale=config['PrescaleLambdac2Lambda0KLL'],
            postscale=config['PostscaleLambdac2Lambda0KLL'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )
        self.line_Lambdac2Lambda0KDD = self.make_line(
            name='{0}DDLine'.format(Lambdac2Lambda0K_name),
            selection=self.selLambdac2Lambda0K[1],
            prescale=config['PrescaleLambdac2Lambda0KDD'],
            postscale=config['PostscaleLambdac2Lambda0KDD'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        self.line_Lambdac2pKS0LL = self.make_line(
            name='{0}LLLine'.format(Lambdac2pKS0_name),
            selection=self.selLambdac2pKS0[0],
            prescale=config['PrescaleLambdac2pKS0LL'],
            postscale=config['PostscaleLambdac2pKS0LL'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )
        self.line_Lambdac2pKS0DD = self.make_line(
            name='{0}DDLine'.format(Lambdac2pKS0_name),
            selection=self.selLambdac2pKS0[1],
            prescale=config['PrescaleLambdac2pKS0DD'],
            postscale=config['PostscaleLambdac2pKS0DD'],
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

    def makeLambdac2V0H(self, name, inputSelLL, inputSelDD, decDescriptors):
        """Return two Selection instances for a Lambda_c+ -> V0 h+ decay.

        The return value is a two-tuple of Selection instances as
            (LL Selection, DD Selection)
        where LL and DD are the method of reconstruction for the V0.
        Keyword arguments:
        name -- Name to give the Selection instance
        inputSelLL -- List of inputs passed to Selection.RequiredSelections
                      for the LL Selection
        inputSelDD -- List of inputs passed to Selection.RequiredSelections
                      for the DD Selection
        decDescriptors -- List of decay descriptors for CombineParticles
        """
        lclPreambulo = [
            'from math import cos'
        ]

        combCuts = (
            "(ADAMASS('Lambda_c+') < {0[Comb_ADAMASS_WIN]})"
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

        selLL = Selection(
            '{0}LL'.format(name),
            Algorithm=_Lambdac,
            RequiredSelections=inputSelLL
        )
        selDD = Selection(
            '{0}DD'.format(name),
            Algorithm=_Lambdac,
            RequiredSelections=inputSelDD
        )
        return selLL, selDD
