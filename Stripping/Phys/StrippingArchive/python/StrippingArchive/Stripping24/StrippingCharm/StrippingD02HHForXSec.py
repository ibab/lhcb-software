"""
Stripping lines for selection of
    [D0 -> K- pi+]cc
    [D*(2007)0 -> (D0 -> K- pi+) pi0]cc
    [D*(2010)+ -> (D0 -> K- pi+) pi+]cc
for open charm cross section measurement.
"""

__author__ = ['Alex Pearce']

__all__ = (
    'default_config',
    'StrippingD02HHForXSecConf'
)


from GaudiKernel.SystemOfUnits import MeV, GeV, mrad
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
    'NAME': 'D02HHForXSec',
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingD02HHForXSecConf',
    'STREAMS': ['Charm'],
    'CONFIG': {
        # Minimum transverse momentum all D0 daughters must satisfy
        'Daug_All_PT_MIN': 250.0*MeV,
        # Minimum best primary vertex IP chi^2 all D0 daughters must satisfy
        'Daug_All_BPVIPCHI2_MIN': 4.0,
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
        'D0_ADAMASS_WIN': 80.0*MeV,
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
        'D0_MVA_Weights': '$TMVAWEIGHTSROOT/data/D02HHForXSec_BDT_v1r0.xml',
        # Dictionary of LoKi functors defining the D0 MVA input variables
        # The keys must match those used when training the MVA
        'D0_MVA_Variables': {
            'D0_Loki_LV02': 'LV02',
            'D0_Loki_BPVLTIME': "BPVLTIME()",
            'D0_ENDVERTEX_CHI2': 'VFASPF(VCHI2/VDOF)',
            'D0_h1_PT': 'CHILD(PT, 1)',
            'D0_h2_PT': 'CHILD(PT, 2)',
            'D0_h1_ETA': 'CHILD(ETA, 1)',
            'D0_h2_ETA': 'CHILD(ETA, 2)'
        },
        # HLT filters, only process events firing triggers matching the RegEx
        'Hlt1Filter': None,
        'Hlt2Filter': None,
        # Fraction of candidates to randomly throw away before stripping
        'PrescaleD02HH': 0.0,
        'PrescaleDstar2D0Pi_D02HH': 0.0,
        'PrescaleDstar2D0Pi0Merged_D02HH': 0.0,
        'PrescaleDstar2D0Pi0Resolved_D02HH': 0.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleD02HH': 1.0,
        'PostscaleDstar2D0Pi_D02HH': 1.0,
        'PostscaleDstar2D0Pi0Merged_D02HH': 1.0,
        'PostscaleDstar2D0Pi0Resolved_D02HH': 1.0
    }
}


class StrippingD02HHForXSecConf(LineBuilder):
    """Creates LineBuilder object containing the stripping lines."""
    # Allowed configuration keys
    __configuration_keys__ = default_config['CONFIG'].keys()

    # Decay descriptors
    D02HH = [
        'D0 -> pi+ pi-',
        'D0 -> K- pi+',
        'D0 -> K+ pi-',
        'D0 -> K+ K-'
    ]
    Dstar2D0Pi = [
        'D*(2010)+ -> D0 pi+',
        'D*(2010)- -> D0 pi-'
    ]
    Dstar2D0Pi0 = [
        'D*(2007)0 -> D0 pi0'
    ]

    def __init__(self, name, config):
        """Initialise this LineBuilder instance."""
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)

        d02HH_name = '{0}D02HH'.format(name)
        dstar_name = '{0}Dstar2D0Pi_D02HH'.format(name)
        dstar0_merged_name = '{0}Dstar2D0Pi0Merged_D02HH'.format(name)
        dstar0_resolved_name = '{0}Dstar2D0Pi0Resolved_D02HH'.format(name)

        self.inPions = StdAllNoPIDsPions
        self.inSoftPions = StdAllNoPIDsPions
        self.inKaons = StdAllNoPIDsKaons
        self.inMergedPions = StdLooseMergedPi0
        self.inResolvedPions = StdLooseResolvedPi0

        self.selD02HH = self.makeD02HH(
            d02HH_name,
            inputSel=[self.inKaons, self.inPions],
            decDescriptors=self.D02HH
        )
        self.selD02HHMVA = self.makeMVASelection(
            '{0}MVASelection'.format(d02HH_name),
            self.selD02HH
        )

        self.line_D02HH = self.make_line(
            name='{0}Line'.format(d02HH_name),
            prescale=config['PrescaleD02HH'],
            postscale=config['PostscaleD02HH'],
            selection=self.selD02HHMVA,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        self.selDstar2D0Pi_D02HH = self.makeDstar2D0Pi(
            dstar_name,
            inputSel=[self.selD02HHMVA, self.inSoftPions],
            decDescriptors=self.Dstar2D0Pi
        )

        self.line_Dstar2D0Pi_D02HH = self.make_line(
            name='{0}Line'.format(dstar_name),
            prescale=config['PrescaleDstar2D0Pi_D02HH'],
            postscale=config['PostscaleDstar2D0Pi_D02HH'],
            selection=self.selDstar2D0Pi_D02HH,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        self.selDstar2D0Pi0Merged_D02HH = self.makeDstar2D0Pi0(
            dstar0_merged_name,
            inputSel=[self.selD02HHMVA, self.inMergedPions],
            decDescriptors=self.Dstar2D0Pi0
        )

        self.line_Dstar2D0Pi0Merged_D02HH = self.make_line(
            name='{0}Line'.format(dstar0_merged_name),
            prescale=config['PrescaleDstar2D0Pi0Merged_D02HH'],
            postscale=config['PostscaleDstar2D0Pi0Merged_D02HH'],
            selection=self.selDstar2D0Pi0Merged_D02HH,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        self.selDstar2D0Pi0Resolved_D02HH = self.makeDstar2D0Pi0(
            dstar0_resolved_name,
            inputSel=[self.selD02HHMVA, self.inResolvedPions],
            decDescriptors=self.Dstar2D0Pi0
        )

        self.line_Dstar2D0Pi0Resolved_D02HH = self.make_line(
            name='{0}Line'.format(dstar0_resolved_name),
            prescale=config['PrescaleDstar2D0Pi0Resolved_D02HH'],
            postscale=config['PostscaleDstar2D0Pi0Resolved_D02HH'],
            selection=self.selDstar2D0Pi0Resolved_D02HH,
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

    def makeD02HH(self, name, inputSel, decDescriptors):
        """Return a Selection instance for a D0 -> h- h+ decay.

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

        combCuts = "(ADAMASS('D0') < {0[D0_ADAMASS_WIN]})".format(self.config)

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
