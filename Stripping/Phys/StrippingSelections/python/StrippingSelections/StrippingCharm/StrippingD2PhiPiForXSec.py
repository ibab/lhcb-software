"""
Stripping lines for selection of
    [D+ -> pi+ (phi(1020) -> K- K+)]cc
    [D_s+ -> pi+ (phi(1020) -> K- K+)]cc
    [D*_s+ -> (D_s+ -> pi+ (phi(1020) -> K- K+)) gamma]cc
for open charm cross section measurement.
The pi+phi(1020) mass window is wide enough to select D+ and D_s+.
The two-body [phi(1020) pi+]cc combination is made with a mass window wide
enough to select D+ and D_s+, and then this selection is filtered in to a D+
selection around the nominal D+ mass and a D_s+ selection around the nominal
D_s+ mass.
"""

__author__ = ['Alex Pearce']

__all__ = (
    'default_config',
    'StrippingD2PhiPiForXSecConf'
)

from GaudiKernel.SystemOfUnits import MeV, GeV, mrad
from Configurables import CombineParticles, FilterDesktop, SubstitutePID
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
        # D+ mass window around the nominal D+ mass after the vertex fit
        # Lower bound of this, -10 MeV, is used before the hhh combination
        # vertex fit
        'Dp_ADAMASS_WIN': 80.0*MeV,
        # D_s+ mass window around the nominal D+ mass after the vertex fit
        # Upper bound of this, +10 MeV, is used before the hhh combination
        # vertex fit
        'Ds_ADAMASS_WIN': 80.0*MeV,
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
        # Minimum D+ and D_s+ MVA discriminant value
        'Dp_MVA_MIN': -0.3,
        'Ds_MVA_MIN': -0.3,
        # Path to the D+ and D_s+ MVA weights files
        # BDT is not applied if this is the empty string or None
        'Dp_MVA_Weights': '$TMVAWEIGHTSROOT/data/D2PhiPiForXSec_BDT_v1r0.xml',
        'Ds_MVA_Weights': '$TMVAWEIGHTSROOT/data/D2PhiPiForXSec_BDT_v1r0.xml',
        # Dictionary of LoKi functors defining the D+ MVA input variables
        # The keys must match those used when training the MVA
        # Same input variables are used for both D+ and D_s+
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
        'PrescaleDp2PhiPi': 0.0,
        'PrescaleDs2PhiPi': 0.0,
        'PrescaleDsstar2DsGamma': 0.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleDp2PhiPi': 1.0,
        'PostscaleDs2PhiPi': 1.0,
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
        dp2PhiPi_name = '{0}Dp2PhiPi'.format(name)
        ds2PhiPi_name = '{0}Ds2PhiPi'.format(name)
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
        self.selDp2PhiPi, self.selDs2PhiPi = self.splitPhiPi(
            d2PhiPi_name,
            self.selD2PhiPi
        )
        self.selDp2PhiPiMVA = self.makeMVASelection(
            '{0}MVASelection'.format(dp2PhiPi_name),
            self.selDp2PhiPi,
            self.config['Dp_MVA_Weights'],
            self.config['Dp_MVA_MIN']
        )
        self.selDs2PhiPiMVA = self.makeMVASelection(
            '{0}MVASelection'.format(ds2PhiPi_name),
            self.selDs2PhiPi,
            self.config['Ds_MVA_Weights'],
            self.config['Ds_MVA_MIN']
        )

        self.selDsstar2Dsgamma = self.makeDsstar2DsGamma(
            name=dsstar2DsGamma_name,
            inputSel=[self.selDs2PhiPiMVA, self.inPhotons],
            decDescriptors=self.Dsstar2DsGamma
        )

        self.line_Dp2PhiPi = self.make_line(
            name=dp2PhiPi_name + 'Line',
            selection=self.selDp2PhiPiMVA,
            prescale=config['PrescaleDp2PhiPi'],
            postscale=config['PostscaleDp2PhiPi'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )
        self.line_Ds2PhiPi = self.make_line(
            name=ds2PhiPi_name + 'Line',
            selection=self.selDs2PhiPiMVA,
            prescale=config['PrescaleDs2PhiPi'],
            postscale=config['PostscaleDs2PhiPi'],
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

        # Be within either the D+ or D_s+ mass window
        combCuts = (
            '('
            "(ADAMASS('D+') < ({0[Dp_ADAMASS_WIN]} + 10))"
            "| (ADAMASS('D_s+') < ({0[Ds_ADAMASS_WIN]} + 10))"
            ')'
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

    def splitPhiPi(self, name, phiPiSelection):
        """Split the input phi pi+ Selection in to a D+ and D_s+ selection.

        Returns a two-tuple as (D+ Selection, D_s+ Selection).
        Keyword arguments:
        phiPiSelection -- A single Selection instance; output of makeD2PhiPi
        """
        dpFilter = FilterDesktop(
            'FilterDp{0}'.format(name),
            Code="(ADMASS('D+') < {0[Dp_ADAMASS_WIN]})".format(self.config)
        )
        dsFilter = FilterDesktop(
            'FilterDs{0}'.format(name),
            Code="(ADMASS('D_s+') < {0[Ds_ADAMASS_WIN]})".format(self.config)
        )

        dpSel = Selection(
            'SelFilteredDp{0}'.format(name),
            Algorithm=dpFilter,
            RequiredSelections=[phiPiSelection]
        )
        dsSel = Selection(
            'SelFilteredDs{0}'.format(name),
            Algorithm=dsFilter,
            RequiredSelections=[phiPiSelection]
        )

        # The PhiPi selection is labelled as a D_s+, so rename the candidates
        # in the Dp selection as such
        dpSubPID = SubstitutePID(
            name='SubPidDp{0}'.format(name),
            Code="DECTREE('[D_s+ -> X0 X+]CC')",
            Substitutions={
                'D_s+ -> X0 X+': 'D+',
                'D_s- -> X0 X-': 'D-'

            },
            MaxChi2PerDoF=-1
        )
        dpSubPIDSel = Selection(
            'SubPIDDpSel{0}'.format(name),
            Algorithm=dpSubPID,
            RequiredSelections=[dpSel]
        )

        return dpSubPIDSel, dsSel

    def makeMVASelection(self, name, inputSel, weights_file, cut_value):
        # Don't apply a BDT if the weights file has not been specified
        if not weights_file:
            return inputSel

        cut = "VALUE('LoKi::Hybrid::DictValue/{0}') > {1}".format(
            name, cut_value
        )
        mva = FilterDesktop('{0}Filter'.format(name), Code=cut)

        addTMVAclassifierValue(
            Component=mva,
            XMLFile=weights_file,
            Variables=self.config['D_MVA_Variables'],
            ToolName=name
        )

        return Selection(name, Algorithm=mva, RequiredSelections=[inputSel])
