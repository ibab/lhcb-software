"""
Stripping lines for selection of
    [D_s+ -> pi+ (phi(1020) -> K- K+)]cc
    [D*_s+ -> (D_s+ -> pi+ (phi(1020) -> K- K+)) gamma]cc
for open charm cross section measurement.
The pi+phi(1020) mass window is wide enough to select D+ and D_s+.

Adapted to current stripping framework by P. Spradlin.
"""

__author__ = ['Conor Fitzpatrick', 'Patrick Spradlin']
__date__ = '03/09/2010'
__version__ = '$Revision: 2.0 $'

__all__ = (
    'default_config',
    'StrippingD2PhiPiForXSecConf'
)

from GaudiKernel.SystemOfUnits import MeV, GeV, mrad
from Configurables import CombineParticles, FilterDesktop
from StandardParticles import (
    StdAllNoPIDsKaons,
    StdAllNoPIDsPions,
    StdLooseAllPhotons
)
from PhysSelPython.Wrappers import Selection
from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine
from MVADictHelpers import addTMVAclassifierValue

default_config = {
    'NAME': 'D2PhiPiForXSec',
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingD2PhiPiForXSecConf',
    'STREAMS': ['Charm'],
    'CONFIG': {
        # Minimum transverse momentum all D_s+ and phi daughters must satisfy
        'Daug_All_PT_MIN': 200.0*MeV,
        # Minimum best primary vertex IP chi^2 all D_s+ and phi daughters must
        # satisfy
        'Daug_All_BPVIPCHI2_MIN': 4.0,
        # Minimum phi daughter momentum
        'Daug_P_MIN': 3.0*GeV,
        # Maximum phi daughter momentum
        'Daug_P_MAX': 100.0*GeV,
        # Minimum phi daughter pseudorapidity
        'Daug_ETA_MIN': 2.0,
        # Maximum phi daughter pseudorapidity
        'Daug_ETA_MAX': 5.0,
        # Minimum phi daughter kaon DLLK
        'K_PIDK_MIN': 5.0,
        # Maximum D_s+ daughter pion DLLK
        'Pi_PIDK_MAX': 3.0,
        # Phi mass window around the nominal phi mass before the vertex fit
        'Comb_Phi_ADAMASS_WIN': 20*MeV,
        # Minimum mass of the two-body pi+phi(1020) combination
        'D_AM_MIN': 1770.0*MeV,
        # Maximum mass of the two-body pi+phi(1020) combination
        'D_AM_MAX': 2070.0*MeV,
        # Maximum D+ vertex chi^2 per vertex fit DoF
        'D_VCHI2VDOF_MAX': 25.0,
        # Maximum angle between D+ momentum and D+ direction of flight
        'D_acosBPVDIRA_MAX': 35.0*mrad,
        # Primary vertex displacement requirement, either that the D+ is some
        # sigma away from the PV, or it has a minimum flight time
        'D_PVDispCut': '((BPVVDCHI2 > 16.0)|(BPVLTIME() > 0.150*picosecond))',
        # Minimum photon confidence level
        'Photon_CL_MIN': 0.25,
        # Maximum delta mass value m(D*_s+) - m(D_s+) (PDG dm: 143 MeV)
        'Dsstar_AMDiff_MAX': 170*MeV,
        # Minimum D+ MVA discriminant value
        'D_MVA_MIN': -0.3,
        # Path to the D+ MVA weights file
        # BDT is not applied if this is the empty string or None
        'D_MVA_Weights': 'D2PhiPiForXSec_BDT_v1r0.xml',
        # Dictionary of LoKi functors defining the D+ MVA input variables
        # The keys must match those used when training the MVA
        'D_MVA_Variables': {
            # Largest D+ and phi daughter PT
            'ROOTex::Leading(phi_h1_PT,phi_h2_PT,Ds_pi_PT)': (
                "MAXTREE(ISBASIC & HASTRACK, PT)"
            ),
            # Smallest D+ and phi daughter PT
            'ROOTex::ThirdLeading(phi_h1_PT,phi_h2_PT,Ds_pi_PT)': (
                "MINTREE(ISBASIC & HASTRACK, PT)"
            ),
            'Ds_DIRA_OWNPV': 'BPVDIRA',
            'Ds_Loki_LV02': 'LV02',
            'TMath::Log10(TMath::Abs(Ds_Loki_BPVLTIME))': (
                "log10(abs(BPVLTIME()))"
            ),
            'TMath::Log10(TMath::Abs(Ds_Loki_BPVVDCHI2))': (
                'log10(abs(BPVVDCHI2))'
            ),
            'TMath::Log10(Ds_ENDVERTEX_CHI2/Ds_ENDVERTEX_NDOF)': (
                'log10(VFASPF(VCHI2/VDOF))'
            )
        },
        # HLT filters, only process events firing triggers matching the RegEx
        'Hlt1Filter': None,
        'Hlt2Filter': None,
        # Fraction of candidates to randomly throw away before stripping
        'PrescaleD2PhiPi': 1.0,
        'PrescaleDsstar2DsGamma': 1.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleD2PhiPi': 1.0,
        'PostscaleDsstar2DsGamma': 1.0
    }
}


class StrippingD2PhiPiForXSecConf(LineBuilder):
    """Creates LineBuilder object containing the stripping lines."""
    # Allowed configuration keys
    __configuration_keys__ = default_config['CONFIG'].keys()

    # Decay descriptors
    Phi2KK = ['phi(1020) -> K+ K-']
    D2PhiPi = ['[D_s+ -> pi+ phi(1020)]cc']
    Dsstar2DsGamma = ['[D*_s+ -> D_s+ gamma]cc']

    def __init__(self, name, config):
        """Initialise this LineBuilder instance."""
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)

        phi2KK_name = '{0}Phi2KK'.format(name)
        d2PhiPi_name = '{0}D2PhiPi'.format(name)
        dsstar2DsGamma_name = '{0}Dsstar2DsGamma'.format(name)

        self.inPions = StdAllNoPIDsPions
        self.inKaons = StdAllNoPIDsKaons
        self.inPhotons = StdLooseAllPhotons

        self.selPhi2KK = self.makePhi2KK(
            name=phi2KK_name,
            inputSel=[self.inKaons],
            decDescriptors=self.Phi2KK
        )

        self.selD2PhiPi = self.makeD2PhiPi(
            name=d2PhiPi_name,
            inputSel=[self.selPhi2KK, self.inPions],
            decDescriptors=self.D2PhiPi
        )
        self.selD2PhiPiMVA = self.makeMVASelection(
            '{0}MVASelection'.format(d2PhiPi_name),
            self.selD2PhiPi
        )

        self.selDsstar2Dsgamma = self.makeDsstar2DsGamma(
            name=dsstar2DsGamma_name,
            inputSel=[self.selD2PhiPiMVA, self.inPhotons],
            decDescriptors=self.Dsstar2DsGamma
        )

        self.line_D2PhiPi = self.make_line(
            name=d2PhiPi_name + 'Line',
            selection=self.selD2PhiPiMVA,
            prescale=config['PrescaleD2PhiPi'],
            postscale=config['PostscaleD2PhiPi'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        self.line_Dsstar2DsGamma = self.make_line(
            name=dsstar2DsGamma_name + 'Line',
            selection=self.selDsstar2Dsgamma,
            prescale=config['PrescaleDsstar2DsGamma'],
            postscale=config['PostscaleDsstar2DsGamma'],
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

    def makePhi2KK(self, name, inputSel, decDescriptors):
        """Return a Selection instance for a phi(1020) -> K- K+ decay.

        Keyword arguments:
        name -- Name to give the Selection instance
        inputSel -- List of inputs passed to Selection.RequiredSelections
        decDescriptors -- List of decay descriptors for CombineParticles
        """
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

        combCuts = "(ADAMASS('phi(1020)') < {0[Comb_Phi_ADAMASS_WIN]})".format(
            self.config
        )

        phiCuts = 'ALL'

        _phi = CombineParticles(
            name='Combine{0}'.format(name),
            DecayDescriptors=decDescriptors,
            DaughtersCuts={
                'K+': '{0} & {1}'.format(daugCuts, kaonPIDCuts)
            },
            CombinationCut=combCuts,
            MotherCut=phiCuts
        )

        return Selection(name, Algorithm=_phi, RequiredSelections=inputSel)

    def makeD2PhiPi(self, name, inputSel, decDescriptors):
        """Return a Selection instance for a D_(s)+ -> pi+ phi(1020) decay.

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

        combCuts = '(in_range({0[D_AM_MIN]}, AM, {0[D_AM_MAX]}))'.format(
            self.config
        )

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
                'pi+': '{0} & {1}'.format(daugCuts, pionPIDCuts)
            },
            CombinationCut=combCuts,
            MotherCut=dCuts,
        )

        return Selection(name, Algorithm=_dplus, RequiredSelections=inputSel)

    def makeDsstar2DsGamma(self, name, inputSel, decDescriptors):
        """Return a Selection instance for a D*_s+ -> D_s+ gamma decay.

        Keyword arguments:
        name -- Name to give the Selection instance
        inputSel -- List of inputs passed to Selection.RequiredSelections
        decDescriptors -- List of decay descriptors for CombineParticles
        """
        photonCuts = '(CL > {0[Photon_CL_MIN]})'.format(self.config)

        combCuts = '((AM - AM1) < {0[Dsstar_AMDiff_MAX]})'.format(self.config)

        dsstarCuts = '(ALL)'.format(self.config)

        _dsstar = CombineParticles(
            name='Combine{0}'.format(name),
            DecayDescriptors=decDescriptors,
            DaughtersCuts={
                'gamma': photonCuts
            },
            CombinationCut=combCuts,
            MotherCut=dsstarCuts,
        )

        return Selection(name, Algorithm=_dsstar, RequiredSelections=inputSel)

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
