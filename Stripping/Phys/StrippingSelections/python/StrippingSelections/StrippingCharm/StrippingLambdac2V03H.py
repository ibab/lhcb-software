"""
Stripping lines for selection of general decay topologies
    [Lambda_c+ -> (V0 -> h+ h-) h+ h+ h-]CC
In this file the stripping line for this decay is build 
    [Lambda_c+ -> (Lambda0 -> p+ pi-) pi+ pi+ pi-]CC
    [Lambda_c+ -> (Lambda0 -> p+ pi-) K+ pi+ pi-]CC
Throughout this file, 'Bachelor' refers to the children of the Lambda_c+ which is
not part of the V0 decay.
"""

__author__ = ['Andrea Merli']
__date__ = '07/04/2016'

__all__ = (
    'default_config',
    'StrippingLambdac2V03HConf'
)

from GaudiKernel.SystemOfUnits import MeV, GeV, mm, mrad
from GaudiConfUtils.ConfigurableGenerators import CombineParticles, FilterDesktop, DaVinci__N4BodyDecays
from StandardParticles import StdNoPIDsPions as InputPions
from StandardParticles import StdNoPIDsKaons as InputKaons
from StandardParticles import StdLooseLambdaLL as InputLambdasLL
from StandardParticles import StdLooseLambdaDD as InputLambdasDD

from PhysSelPython.Wrappers import Selection
from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine

default_config = {
    'NAME': 'Lambdac2V03H',
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingLambdac2V03HConf',
    'STREAMS': ['Charm'],
    'CONFIG': {
        # Minimum Lc+ bachelor momentum
        'Bach_P_MIN': 2.0*GeV,
        # Minimum L0 momentum
        'Lambda0_P_MIN': 2000*MeV,
        # Minimum L0 transverse momentum
        'Lambda0_PT_MIN': 250*MeV,
        # Minimum flight distance chi^2 of L0 from the primary vertex
        'Lambda0_FDCHI2_MIN': 49,
        # Maximum L0 vertex chi^2 per vertex fit DoF
        'Lambda0_VCHI2VDOF_MAX': 12.0,
        # Lc+ mass window around the nominal Lc+ mass before the vertex fit
        'Comb_ADAMASS_WIN': 120.0*MeV,
        # Lc+ mass window around the nominal Lc+ mass after the vertex fit
        'Lambdac_ADMASS_WIN': 90.0*MeV,
        # Maximum distance of closest approach of Lc+ children
        'Comb_ADOCAMAX_MAX': 0.5*mm,
        # Maximum Lc+ vertex chi^2 per vertex fit DoF
        'Lambdac_VCHI2VDOF_MAX': 12.0,
        # Maximum angle between Lc+ momentum and Lc+ direction of flight
        'Lambdac_acosBPVDIRA_MAX': 140.0*mrad,
        # Primary vertex displacement requirement, either that the Lc+ is some
        # sigma away from the PV, or it has a minimum flight time
        'Lambdac_PVDispCut': '(BPVVDCHI2 > 16.0)',
        # HLT filters, only process events firing triggers matching the RegEx
        'Hlt1Filter': None,
        'Hlt2Filter': None,
        # Fraction of candidates to randomly throw away before stripping
        'PrescaleLambdac2Lambda3PiLL': 1.0,
        'PrescaleLambdac2Lambda3PiDD': 1.0,
        'PrescaleLambdac2LambdaK2PiLL': 1.0,
        'PrescaleLambdac2LambdaK2PiDD': 1.0,
        # Fraction of candidates to randomly throw away after stripping
        'PostscaleLambdac2Lambda3PiLL': 1.0,
        'PostscaleLambdac2Lambda3PiDD': 1.0,
        'PostscaleLambdac2LambdaK2PiLL': 1.0,
        'PostscaleLambdac2LambdaK2PiDD': 1.0
    }
}


class StrippingLambdac2V03HConf(LineBuilder):
    """Creates LineBuilder object containing the stripping lines."""
    # Allowed configuration keys
    __configuration_keys__ = default_config['CONFIG'].keys()

    def __init__(self, name, config):
        """Initialise this LineBuilder instance."""
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)
        
        # Decay descriptors
        self.Lambdac2Lambda3Pi = ['[Lambda_c+ -> Lambda0 pi+ pi+ pi-]cc']
        self.Lambdac2LambdaK2Pi = ['[Lambda_c+ -> Lambda0 K+ pi+ pi-]cc']
        
        # Line names
        # 'LL' and 'DD' will be appended to these names for the LL and DD
        # Selection and StrippingLine instances
        self.Lambdac2Lambda3Pi_name = '{0}_Lambdac2Lambda3Pi'.format(name)
        self.Lambdac2LambdaK2Pi_name = '{0}_Lambdac2LambdaK2Pi'.format(name)
        
        # Build bachelor pion and kaon cut strings
        # Cuts MIPCHI2DV(PRIMARY)>4 & PT>250*MeV already present in the InputsParticles
        childCuts = (
            '(P > {0[Bach_P_MIN]})'
        ).format(self.config)
        
        kineticCuts = '{0}'.format(childCuts)

        # Build Lambda0 cut strings
        lambda0Cuts = (
            '(P > {0[Lambda0_P_MIN]})'
            '& (PT > {0[Lambda0_PT_MIN]})'
            '& (BPVVDCHI2 > {0[Lambda0_FDCHI2_MIN]})'
            '& (VFASPF(VCHI2/VDOF) < {0[Lambda0_VCHI2VDOF_MAX]})'
        ).format(self.config)
        # Define any additional cuts on LL/DD difference
        lambda0LLCuts = lambda0Cuts
        lambda0DDCuts = lambda0Cuts
        
        # Filter Input particles
        self.Pions = Selection(
            'PionsFor{0}'.format(name),
            Algorithm=FilterDesktop(
                 Code = kineticCuts
            ),
            RequiredSelections=[InputPions]
        )

        self.Kaons = Selection(
            'KaonsFor{0}'.format(name),
            Algorithm=FilterDesktop(
                  Code = kineticCuts
            ),
            RequiredSelections=[InputKaons]
        )
        
        # Filter Input Lambdas
        self.Lambda0LL = Selection(
            'Lambda0LLFor{0}'.format(name),
            Algorithm=FilterDesktop(
                Code=lambda0LLCuts
            ),
            RequiredSelections=[InputLambdasLL]
        )
        self.Lambda0DD = Selection(
            'Lambda0DDFor{0}'.format(name),
            Algorithm=FilterDesktop(
                Code=lambda0DDCuts
            ),
            RequiredSelections=[InputLambdasDD]
        )

        # Build selection for Lc -> L 3pi
        self.selLambdac2Lambda3Pi = self.makeLambdac2V03H(
            name=self.Lambdac2Lambda3Pi_name,
            inputSelLL=[self.Lambda0LL, self.Pions],
            inputSelDD=[self.Lambda0DD, self.Pions],
            decDescriptors=self.Lambdac2Lambda3Pi
        )

        # Build selection for Lc -> L K 2pi
        self.selLambdac2LambdaK2Pi = self.makeLambdac2V03H(
            name=self.Lambdac2LambdaK2Pi_name,
            inputSelLL=[self.Lambda0LL, self.Kaons, self.Pions],
            inputSelDD=[self.Lambda0DD, self.Kaons, self.Pions],
            decDescriptors=self.Lambdac2LambdaK2Pi
        )

        # Make line for Lc -> L 3pi
        self.line_Lambdac2Lambda3PiLL = self.make_line(
            name='{0}LLLine'.format(self.Lambdac2Lambda3Pi_name),
            selection=self.selLambdac2Lambda3Pi[0],
            prescale=config['PrescaleLambdac2Lambda3PiLL'],
            postscale=config['PostscaleLambdac2Lambda3PiLL'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )
        self.line_Lambdac2Lambda3PiDD = self.make_line(
            name='{0}DDLine'.format(self.Lambdac2Lambda3Pi_name),
            selection=self.selLambdac2Lambda3Pi[1],
            prescale=config['PrescaleLambdac2Lambda3PiDD'],
            postscale=config['PostscaleLambdac2Lambda3PiDD'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        # Make line for Lc -> L K 2pi
        self.line_Lambdac2LambdaK2PiLL = self.make_line(
            name='{0}LLLine'.format(self.Lambdac2LambdaK2Pi_name),
            selection=self.selLambdac2LambdaK2Pi[0],
            prescale=config['PrescaleLambdac2LambdaK2PiLL'],
            postscale=config['PostscaleLambdac2LambdaK2PiLL'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )
        self.line_Lambdac2LambdaK2PiDD = self.make_line(
            name='{0}DDLine'.format(self.Lambdac2LambdaK2Pi_name),
            selection=self.selLambdac2LambdaK2Pi[1],
            prescale=config['PrescaleLambdac2LambdaK2PiDD'],
            postscale=config['PostscaleLambdac2LambdaK2PiDD'],
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

    def makeLambdac2V03H(self, name, inputSelLL, inputSelDD, decDescriptors):
        """Return two Selection instances for a Lambda_c+ -> V0 h+ h+ h- decay.

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
            "& (ADOCA(1,2) < {0[Comb_ADOCAMAX_MAX]})"
            "& (ADOCA(1,3) < {0[Comb_ADOCAMAX_MAX]})"
            "& (ADOCA(1,4) < {0[Comb_ADOCAMAX_MAX]})"
        ).format(self.config)

        lambdacCuts = (
            "(VFASPF(VCHI2/VDOF) < {0[Lambdac_VCHI2VDOF_MAX]})"
            "& ({0[Lambdac_PVDispCut]})"
            "& (BPVDIRA > cos({0[Lambdac_acosBPVDIRA_MAX]}))"
            "& (ADMASS('Lambda_c+') < {0[Lambdac_ADMASS_WIN]})"
        ).format(self.config)

        comb12Cuts = (
            "(DAMASS('Lambda_c+') < {0[Comb_ADAMASS_WIN]})"
            "& (ADOCA(1,2) < {0[Comb_ADOCAMAX_MAX]})"
            ).format(self.config)

        comb123Cuts = (
            "(DAMASS('Lambda_c+') < {0[Comb_ADAMASS_WIN]})"
            "& (ADOCA(1,2) < {0[Comb_ADOCAMAX_MAX]})"
            "& (ADOCA(1,3) < {0[Comb_ADOCAMAX_MAX]})"
            ).format(self.config)

        _Lambdac = DaVinci__N4BodyDecays(
            DecayDescriptors=decDescriptors,
            Preambulo=lclPreambulo,
            CombinationCut=combCuts,
            MotherCut=lambdacCuts,
            Combination12Cut=comb12Cuts,
            Combination123Cut=comb123Cuts
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
