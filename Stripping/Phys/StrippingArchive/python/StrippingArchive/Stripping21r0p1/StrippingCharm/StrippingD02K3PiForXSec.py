"""
Stripping lines for selection of
    [D0 -> K- pi+ pi- pi+]cc
    [D*(2007)0 -> (D0 -> K- pi+ pi- pi+) pi0]cc
    [D*(2010)+ -> (D0 -> K- pi+ pi- pi+) pi+]cc
for open charm cross section measurement.
"""

__author__ = ['Alex Pearce']

__all__ = (
    'default_config',
    'StrippingD02K3PiForXSecConf'
)

from GaudiKernel.SystemOfUnits import MeV, GeV, mm, mrad
from Configurables import CombineParticles, FilterDesktop
from StandardParticles import (
    StdAllNoPIDsKaons,
    StdAllNoPIDsPions,
    StdLooseMergedPi0,
    StdLooseResolvedPi0
)
from PhysSelPython.Wrappers import Selection
from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine
from MVADictHelpers import addTMVAclassifierValue

default_config = {
    'NAME': 'D02K3PiForXSec',
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingD02K3PiForXSecConf',
    'STREAMS': ['Charm'],
    'CONFIG': {
        # Minimum transverse momentum all D0 daughters must satisfy
        'Daug_All_PT_MIN': 200.0*MeV,
        # Minimum transverse momentum at least 3 D0 daughters must satisfy
        'Daug_3of4_PT_MIN': 300.0*MeV,
        # Minimum transverse momentum at least 2 D0 daughters must satisfy
        'Daug_2of4_PT_MIN': 350.0*MeV,
        # Minimum transverse momentum at least 1 D0 daughter must satisfy
        'Daug_1of4_PT_MIN': 400.0*MeV,
        # Minimum best primary vertex IP chi^2 all D0 daughters must satisfy
        'Daug_All_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 3 D0 daughters must satisfy
        'Daug_3of4_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 2 D0 daughters must satisfy
        'Daug_2of4_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 1 D0 daughter must satisfy
        'Daug_1of4_BPVIPCHI2_MIN': 4.0,
        # Minimum D0 daughter momentum
        'Daug_P_MIN': 3.0*GeV,
        # Maximum D0 daughter momentum
        'Daug_P_MAX': 100.0*GeV,
        # Minimum D0 daughter pseudorapidity
        'Daug_ETA_MIN': 2.0,
        # Maximum D0 daughter pseudorapidity
        'Daug_ETA_MAX': 5.0,
        # Minimum D0 daughter kaon DLLK
        'K_PIDK_MIN': 5.0,
        # Maximum D0 daughter pion DLLK
        'Pi_PIDK_MAX': 3.0,
        # D0 mass window around the nominal D0 mass before the vertex fit
        'Comb_ADAMASS_WIN': 80.0*MeV,
        # Maximum distance of closest approach of D0 daughters
        'Comb_ADOCAMAX_MAX': 0.5*mm,
        # Maximum D0 vertex chi^2 per vertex fit DoF
        'D0_VCHI2VDOF_MAX': 25.0,
        # Maximum angle between D0 momentum and D0 direction of flight
        'D0_acosBPVDIRA_MAX': 35.0*mrad,
        # Primary vertex displacement requirement, either that the D0 is some
        # sigma away from the PV, or it has a minimum flight time
        'D0_PVDispCut': '((BPVVDCHI2 > 16.0)|(BPVLTIME() > 0.150*picosecond))',
        # Maximum delta mass value m(D*{0,+}) - m(D0)
        'Dstar_AMDiff_MAX': 160.0*MeV,
        # Maximum D*+ vertex chi^2 per vertex DoF (_not_ applied to D*0)
        'Dstar_VCHI2VDOF_MAX': 25.0,
        # Minimum D0 MVA discriminant value
        'D0_MVA_MIN': -0.3,
        # Path to the D0 MVA weights file
        # BDT is not applied if this is the empty string or None
        'D0_MVA_Weights': '$TMVAWEIGHTSROOT/data/D02K3PiForXSec_BDT_v1r0.xml',
        # Dictionary of LoKi functors defining the D0 MVA input variables
        # The keys must match those used when training the MVA
        'D0_MVA_Variables': {
            'D0_Loki_LV02': 'LV02',
            'D0_Loki_BPVLTIME': "BPVLTIME()",
            'D0_Loki_DOCAMAX': "DOCAMAX",
            'D0_DIRA_OWNPV': 'BPVDIRA',
            'D0_ENDVERTEX_CHI2': 'VFASPF(VCHI2/VDOF)',
            'D0_h1_PT': 'CHILD(PT, 1)',
            'D0_h2_PT': 'CHILD(PT, 2)',
            'D0_h3_PT': 'CHILD(PT, 3)',
            'D0_h4_PT': 'CHILD(PT, 4)',
            'D0_h1_ETA': 'CHILD(ETA, 1)',
            'D0_h2_ETA': 'CHILD(ETA, 2)',
            'D0_h3_ETA': 'CHILD(ETA, 3)',
            'D0_h4_ETA': 'CHILD(ETA, 4)'
        },
        # HLT filters, only process events firing triggers matching the RegEx
        'Hlt1Filter': None,
        'Hlt2Filter': None,
        # Fraction of candidates to randomly throw away before stripping
        'PrescaleD02K3Pi': 0.0,
        'PrescaleDstar2D0Pi_D02K3Pi': 0.0,
        'PrescaleDstar2D0Pi0Merged_D02K3Pi': 0.0,
        'PrescaleDstar2D0Pi0Resolved_D02K3Pi': 0.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleD02K3Pi': 1.0,
        'PostscaleDstar2D0Pi_D02K3Pi': 1.0,
        'PostscaleDstar2D0Pi0Merged_D02K3Pi': 1.0,
        'PostscaleDstar2D0Pi0Resolved_D02K3Pi': 1.0
    }
}


class StrippingD02K3PiForXSecConf(LineBuilder):
    """Creates LineBuilder object containing the stripping lines."""
    # Allowed configuration keys
    __configuration_keys__ = default_config['CONFIG'].keys()

    # Decay descriptors
    D02K3Pi = ['[D0 -> K- pi+ pi- pi+]cc']
    Dstar2D0Pi = ['[D*(2010)+ -> D0 pi+]cc']
    Dstar2D0Pi0 = ['D*(2007)0 -> D0 pi0']

    def __init__(self, name, config):
        """Initialise this LineBuilder instance."""
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)

        d02K3Pi_name = '{0}D02K3Pi'.format(name)
        dstar_name = '{0}Dstar2D0Pi_D02K3Pi'.format(name)
        dstar0_merged_name = '{0}Dstar2D0Pi0Merged_D02K3Pi'.format(name)
        dstar0_resolved_name = '{0}Dstar2D0Pi0Resolved_D02K3Pi'.format(name)

        self.inPions = StdAllNoPIDsPions
        self.inSoftPions = StdAllNoPIDsPions
        self.inKaons = StdAllNoPIDsKaons
        self.inMergedPions = StdLooseMergedPi0
        self.inResolvedPions = StdLooseResolvedPi0

        self.selD02K3Pi = self.makeD02K3Pi(
            d02K3Pi_name,
            inputSel=[self.inKaons, self.inPions],
            decDescriptors=self.D02K3Pi
        )
        self.selD02K3PiMVA = self.makeMVASelection(
            '{0}MVASelection'.format(d02K3Pi_name),
            self.selD02K3Pi
        )

        self.line_D02K3Pi = self.make_line(
            name=d02K3Pi_name + 'Line',
            selection=self.selD02K3PiMVA,
            prescale=config['PrescaleD02K3Pi'],
            postscale=config['PostscaleD02K3Pi'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        self.selDstar2D0Pi_D02K3Pi = self.makeDstar2D0Pi(
            dstar_name,
            inputSel=[self.selD02K3PiMVA, self.inSoftPions],
            decDescriptors=self.Dstar2D0Pi
        )

        self.line_Dstar2D0Pi_D02K3Pi = self.make_line(
            name=dstar_name + 'Line',
            prescale=config['PrescaleDstar2D0Pi_D02K3Pi'],
            postscale=config['PostscaleDstar2D0Pi_D02K3Pi'],
            selection=self.selDstar2D0Pi_D02K3Pi,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        self.selDstar2D0Pi0Merged_D02K3Pi = self.makeDstar2D0Pi0(
            dstar0_merged_name,
            inputSel=[self.selD02K3PiMVA, self.inMergedPions],
            decDescriptors=self.Dstar2D0Pi0
        )

        self.line_Dstar2D0Pi0Merged_D02K3Pi = self.make_line(
            name='{0}Line'.format(dstar0_merged_name),
            prescale=config['PrescaleDstar2D0Pi0Merged_D02K3Pi'],
            postscale=config['PostscaleDstar2D0Pi0Merged_D02K3Pi'],
            selection=self.selDstar2D0Pi0Merged_D02K3Pi,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        self.selDstar2D0Pi0Resolved_D02K3Pi = self.makeDstar2D0Pi0(
            dstar0_resolved_name,
            inputSel=[self.selD02K3PiMVA, self.inResolvedPions],
            decDescriptors=self.Dstar2D0Pi0
        )

        self.line_Dstar2D0Pi0Resolved_D02K3Pi = self.make_line(
            name='{0}Line'.format(dstar0_resolved_name),
            prescale=config['PrescaleDstar2D0Pi0Resolved_D02K3Pi'],
            postscale=config['PostscaleDstar2D0Pi0Resolved_D02K3Pi'],
            selection=self.selDstar2D0Pi0Resolved_D02K3Pi,
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

    def makeD02K3Pi(self, name, inputSel, decDescriptors):
        """Return a Selection instance for a D0 -> h- h+ h- h+ decay.

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

        pionPIDCuts = (
            pidFiducialCuts +
            '& (PIDK-PIDpi < {0[Pi_PIDK_MAX]})'
        ).format(self.config)

        kaonPIDCuts = (
            pidFiducialCuts +
            '& (PIDK-PIDpi > {0[K_PIDK_MIN]})'
        ).format(self.config)

        combCuts = (
            "(ADAMASS('D0') < {0[Comb_ADAMASS_WIN]})"
            '& (AMAXCHILD(PT) > {0[Daug_1of4_PT_MIN]})'
            '& (AMAXCHILD(BPVIPCHI2()) > {0[Daug_1of4_BPVIPCHI2_MIN]})'
            '& (ANUM(PT > {0[Daug_3of4_PT_MIN]}) >= 3)'
            '& (ANUM(PT > {0[Daug_2of4_PT_MIN]}) >= 2)'
            '& (ANUM(BPVIPCHI2() > {0[Daug_3of4_BPVIPCHI2_MIN]}) >= 3)'
            '& (ANUM(BPVIPCHI2() > {0[Daug_2of4_BPVIPCHI2_MIN]}) >= 2)'
            "& (ACUTDOCA({0[Comb_ADOCAMAX_MAX]}, ''))"
        ).format(self.config)

        d0Cuts = (
            '(VFASPF(VCHI2/VDOF) < {0[D0_VCHI2VDOF_MAX]})'
            '& ({0[D0_PVDispCut]})'
            '& (BPVDIRA > cos({0[D0_acosBPVDIRA_MAX]}))'
        ).format(self.config)

        _D0 = CombineParticles(
            name='Combine{0}'.format(name),
            DecayDescriptors=decDescriptors,
            Preambulo=lclPreambulo,
            DaughtersCuts={
                'pi+': '{0} & {1}'.format(daugCuts, pionPIDCuts),
                'K+': '{0} & {1}'.format(daugCuts, kaonPIDCuts)
            },
            CombinationCut=combCuts,
            MotherCut=d0Cuts
        )

        return Selection(name, Algorithm=_D0, RequiredSelections=inputSel)

    def makeDstar2D0Pi(self, name, inputSel, decDescriptors):
        """Return a Selection instance for a D*+ -> D0 pi+ decay.

        Keyword arguments:
        name -- Name to give the Selection instance
        inputSel -- List of inputs passed to Selection.RequiredSelections
        decDescriptors -- List of decay descriptors for CombineParticles
        """
        combCuts = '((AM - AM1) < {0[Dstar_AMDiff_MAX]})'.format(self.config)

        dstarCuts = '(VFASPF(VCHI2/VDOF) < {0[Dstar_VCHI2VDOF_MAX]})'.format(
            self.config
        )

        _Dstar = CombineParticles(
            name='Combine{0}'.format(name),
            DecayDescriptors=decDescriptors,
            CombinationCut=combCuts,
            MotherCut=dstarCuts
        )

        return Selection(name, Algorithm=_Dstar, RequiredSelections=inputSel)

    def makeDstar2D0Pi0(self, name, inputSel, decDescriptors):
        """Return a Selection instance for a D*0 -> D0 pi0 decay.

        Keyword arguments:
        name -- Name to give the Selection instance
        inputSel -- List of inputs passed to Selection.RequiredSelections
        decDescriptors -- List of decay descriptors for CombineParticles
        """
        combCuts = '((AM - AM1) < {0[Dstar_AMDiff_MAX]})'.format(self.config)

        dstarCuts = '(ALL)'.format(self.config)

        _Dstar = CombineParticles(
            name='Combine{0}'.format(name),
            DecayDescriptors=decDescriptors,
            CombinationCut=combCuts,
            MotherCut=dstarCuts
        )

        return Selection(name, Algorithm=_Dstar, RequiredSelections=inputSel)

    def makeMVASelection(self, name, inputSel):
        # Don't apply a BDT if the weights file has not been specified
        if not self.config['D0_MVA_Weights']:
            return inputSel

        cut = "VALUE('LoKi::Hybrid::DictValue/{0}') > {1[D0_MVA_MIN]}".format(
            name, self.config
        )
        mva = FilterDesktop('{0}Filter'.format(name), Code=cut)

        addTMVAclassifierValue(
            Component=mva,
            XMLFile=self.config['D0_MVA_Weights'],
            Variables=self.config['D0_MVA_Variables'],
            ToolName=name
        )

        return Selection(name, Algorithm=mva, RequiredSelections=[inputSel])
