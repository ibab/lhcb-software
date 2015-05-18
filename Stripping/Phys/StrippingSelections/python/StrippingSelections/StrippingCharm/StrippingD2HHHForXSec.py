"""
Stripping lines for selection of
    [D+ -> K- pi+ pi+]cc
    [D+ -> K- K+ pi+]cc
    [D+ -> K- K+ K+]cc
    [D+ -> pi- pi+ pi+]cc
    [D+ -> K+ pi- pi+]cc
and the same D_s+ modes for open charm cross section measurement.
The three-body [h+ h- h+]cc combination is made with a mass window wide enough
to select D+ and D_s+, and then this selection is filtered in to a D+ selection
around the nominal D+ mass and a D_s+ selection around the nominal D_s+ mass.
"""

__author__ = ['Alex Pearce']

__all__ = (
    'default_config',
    'StrippingD2HHHForXSecConf'
)

from GaudiKernel.SystemOfUnits import MeV, GeV, mm, mrad
from Configurables import CombineParticles, FilterDesktop, SubstitutePID
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
        # Minimum transverse momentum all D daughters must satisfy
        'Daug_All_PT_MIN': 200.0*MeV,
        # Minimum transverse momentum at least 2 D daughters must satisfy
        'Daug_2of3_PT_MIN': 400.0*MeV,
        # Minimum transverse momentum at least 1 D daughter must satisfy
        'Daug_1of3_PT_MIN': 400.0*MeV,
        # Minimum best primary vertex IP chi^2 all D daughters must satisfy
        'Daug_All_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 2 D daughters must satisfy
        'Daug_2of3_BPVIPCHI2_MIN': 10.0,
        # Minimum best PV IP chi^2 at least 1 D daughter must satisfy
        'Daug_1of3_BPVIPCHI2_MIN': 50.0,
        # Minimum D daughter momentum
        'Daug_P_MIN': 3.0*GeV,
        # Maximum D daughter momentum
        'Daug_P_MAX': 100.0*GeV,
        # Minimum D daughter pseudorapidity
        'Daug_ETA_MIN': 2.0,
        # Maximum D daughter pseudorapidity
        'Daug_ETA_MAX': 5.0,
        # Minimum D daughter kaon DLLK
        'K_PIDK_MIN': 5.0,
        # Maximum D daughter pion DLLK
        'Pi_PIDK_MAX': 3.0,
        # D+ mass window around the nominal D+ mass after the vertex fit
        # Lower bound of this, -10 MeV, is used before the hhh combination
        # vertex fit
        'Dp_ADAMASS_WIN': 80.0*MeV,
        # D_s+ mass window around the nominal D+ mass after the vertex fit
        # Upper bound of this, +10 MeV, is used before the hhh combination
        # vertex fit
        'Ds_ADAMASS_WIN': 80.0*MeV,
        # Maximum distance of closest approach of D daughters
        'Comb_ADOCAMAX_MAX': 0.5*mm,
        # Maximum D vertex chi^2 per vertex fit DoF
        'D_VCHI2VDOF_MAX': 25.0,
        # Maximum angle between D momentum and D direction of flight
        'D_acosBPVDIRA_MAX': 35.0*mrad,
        # Primary vertex displacement requirement, either that the D is some
        # sigma away from the PV, or it has a minimum flight time
        'D_PVDispCut': '((BPVVDCHI2 > 16.0)|(BPVLTIME() > 0.150*picosecond))',
        # Minimum D+ and D_s+ MVA discriminant value
        'Dp_MVA_MIN': -0.3,
        'Ds_MVA_MIN': -0.3,
        # Path to the D+ and D_s+ MVA weights files
        # BDT is not applied if this is the empty string or None
        'Dp_MVA_Weights': '$TMVAWEIGHTSROOT/data/D2HHHForXSec_BDT_v1r0.xml',
        'Ds_MVA_Weights': '$TMVAWEIGHTSROOT/data/D2HHHForXSec_BDT_v1r0.xml',
        # Dictionary of LoKi functors defining the D MVA input variables
        # The keys must match those used when training the MVA
        # Same input variables are used for both D+ and D_s+
        'D_MVA_Variables': {
            # Largest D daughter PT
            'ROOTex::Leading(Dp_h1_PT,Dp_h2_PT,Dp_h3_PT)': (
                "MAXTREE(ISBASIC & HASTRACK, PT)"
            ),
            # Smallest D daughter PT
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
        'PrescaleDp2KPP': 0.0,
        'PrescaleDp2KKP': 0.0,
        'PrescaleDp2KKK': 0.0,
        'PrescaleDp2PPP': 0.0,
        'PrescaleDp2KPPDCS': 0.0,
        'PrescaleDs2KPP': 0.0,
        'PrescaleDs2KKP': 0.0,
        'PrescaleDs2KKK': 0.0,
        'PrescaleDs2PPP': 0.0,
        'PrescaleDs2KPPDCS': 0.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleDp2KPP': 1.0,
        'PostscaleDp2KKP': 1.0,
        'PostscaleDp2KKK': -1.0,
        'PostscaleDp2PPP': 1.0,
        'PostscaleDp2KPPDCS': -1.0,
        'PostscaleDs2KPP': 1.0,
        'PostscaleDs2KKP': 1.0,
        'PostscaleDs2KKK': -1.0,
        'PostscaleDs2PPP': 1.0,
        'PostscaleDs2KPPDCS': -1.0
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

        # HHH combination names
        d2KPP_name = '{0}D2KPP'.format(name)
        d2KKP_name = '{0}D2KKP'.format(name)
        d2KKK_name = '{0}D2KKK'.format(name)
        d2PPP_name = '{0}D2PPP'.format(name)
        d2PPK_name = '{0}D2KPPDCS'.format(name)
        # D+ line names
        dp2KPP_name = '{0}Dp2KPP'.format(name)
        dp2KKP_name = '{0}Dp2KKP'.format(name)
        dp2KKK_name = '{0}Dp2KKK'.format(name)
        dp2PPP_name = '{0}Dp2PPP'.format(name)
        dp2PPK_name = '{0}Dp2KPPDCS'.format(name)
        # D_s+ line names
        ds2KPP_name = '{0}Ds2KPP'.format(name)
        ds2KKP_name = '{0}Ds2KKP'.format(name)
        ds2KKK_name = '{0}Ds2KKK'.format(name)
        ds2PPP_name = '{0}Ds2PPP'.format(name)
        ds2PPK_name = '{0}Ds2KPPDCS'.format(name)

        # Build pion and kaon cut strings
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
        pionCuts = '{0} & {1}'.format(daugCuts, pionPIDCuts)
        kaonCuts = '{0} & {1}'.format(daugCuts, kaonPIDCuts)

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

        # K- pi+ pi+
        self.selD2KPP = self.makeD2HHH(
            name=d2KPP_name,
            inputSel=[self.inKaons, self.inPions],
            decDescriptors=self.D2KPP
        )
        self.selDp2KPP, self.selDs2KPP = self.splitHHH(
            d2KPP_name,
            self.selD2KPP
        )
        self.selDp2KPPMVA = self.makeMVASelection(
            '{0}MVASelection'.format(dp2KPP_name),
            self.selDp2KPP,
            self.config['Dp_MVA_Weights'],
            self.config['Dp_MVA_MIN']
        )
        self.selDs2KPPMVA = self.makeMVASelection(
            '{0}MVASelection'.format(ds2KPP_name),
            self.selDs2KPP,
            self.config['Ds_MVA_Weights'],
            self.config['Ds_MVA_MIN']
        )
        self.line_Dp2KPP = self.make_line(
            name='{0}Line'.format(dp2KPP_name),
            selection=self.selDp2KPPMVA,
            prescale=config['PrescaleDp2KPP'],
            postscale=config['PostscaleDp2KPP'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )
        self.line_Ds2KPP = self.make_line(
            name='{0}Line'.format(ds2KPP_name),
            selection=self.selDs2KPPMVA,
            prescale=config['PrescaleDs2KPP'],
            postscale=config['PostscaleDs2KPP'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        # K- K+ pi+
        self.selD2KKP = self.makeD2HHH(
            name=d2KKP_name,
            inputSel=[self.inKaons, self.inPions],
            decDescriptors=self.D2KKP
        )
        self.selDp2KKP, self.selDs2KKP = self.splitHHH(
            d2KKP_name,
            self.selD2KKP
        )
        self.selDp2KKPMVA = self.makeMVASelection(
            '{0}MVASelection'.format(dp2KKP_name),
            self.selDp2KKP,
            self.config['Dp_MVA_Weights'],
            self.config['Dp_MVA_MIN']
        )
        self.selDs2KKPMVA = self.makeMVASelection(
            '{0}MVASelection'.format(ds2KKP_name),
            self.selDs2KKP,
            self.config['Ds_MVA_Weights'],
            self.config['Ds_MVA_MIN']
        )
        self.line_Dp2KKP = self.make_line(
            name='{0}Line'.format(dp2KKP_name),
            selection=self.selDp2KKPMVA,
            prescale=config['PrescaleDp2KKP'],
            postscale=config['PostscaleDp2KKP'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )
        self.line_Ds2KKP = self.make_line(
            name='{0}Line'.format(ds2KKP_name),
            selection=self.selDs2KKPMVA,
            prescale=config['PrescaleDs2KKP'],
            postscale=config['PostscaleDs2KKP'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        # K- K+ K-
        self.selD2KKK = self.makeD2HHH(
            name=d2KKK_name,
            inputSel=[self.inKaons],
            decDescriptors=self.D2KKK
        )
        self.selDp2KKK, self.selDs2KKK = self.splitHHH(
            d2KKK_name,
            self.selD2KKK
        )
        self.selDp2KKKMVA = self.makeMVASelection(
            '{0}MVASelection'.format(dp2KKK_name),
            self.selDp2KKK,
            self.config['Dp_MVA_Weights'],
            self.config['Dp_MVA_MIN']
        )
        self.selDs2KKKMVA = self.makeMVASelection(
            '{0}MVASelection'.format(ds2KKK_name),
            self.selDs2KKK,
            self.config['Ds_MVA_Weights'],
            self.config['Ds_MVA_MIN']
        )
        self.line_Dp2KKK = self.make_line(
            name='{0}Line'.format(dp2KKK_name),
            selection=self.selDp2KKKMVA,
            prescale=config['PrescaleDp2KKK'],
            postscale=config['PostscaleDp2KKK'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )
        self.line_Ds2KKK = self.make_line(
            name='{0}Line'.format(ds2KKK_name),
            selection=self.selDs2KKKMVA,
            prescale=config['PrescaleDs2KKK'],
            postscale=config['PostscaleDs2KKK'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        # pi- pi+ pi+
        self.selD2PPP = self.makeD2HHH(
            name=d2PPP_name,
            inputSel=[self.inPions],
            decDescriptors=self.D2PPP
        )
        self.selDp2PPP, self.selDs2PPP = self.splitHHH(
            d2PPP_name,
            self.selD2PPP
        )
        self.selDp2PPPMVA = self.makeMVASelection(
            '{0}MVASelection'.format(dp2PPP_name),
            self.selDp2PPP,
            self.config['Dp_MVA_Weights'],
            self.config['Dp_MVA_MIN']
        )
        self.selDs2PPPMVA = self.makeMVASelection(
            '{0}MVASelection'.format(ds2PPP_name),
            self.selDs2PPP,
            self.config['Ds_MVA_Weights'],
            self.config['Ds_MVA_MIN']
        )
        self.line_Dp2PPP = self.make_line(
            name='{0}Line'.format(dp2PPP_name),
            selection=self.selDp2PPPMVA,
            prescale=config['PrescaleDp2PPP'],
            postscale=config['PostscaleDp2PPP'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )
        self.line_Ds2PPP = self.make_line(
            name='{0}Line'.format(ds2PPP_name),
            selection=self.selDs2PPPMVA,
            prescale=config['PrescaleDs2PPP'],
            postscale=config['PostscaleDs2PPP'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

        # K+ pi- pi+ (DCS)
        self.selD2PPK = self.makeD2HHH(
            name=d2PPK_name,
            inputSel=[self.inKaons, self.inPions],
            decDescriptors=self.D2KPPDCS
        )
        self.selDp2PPK, self.selDs2PPK = self.splitHHH(
            d2PPK_name,
            self.selD2PPK
        )
        self.selDp2PPKMVA = self.makeMVASelection(
            '{0}MVASelection'.format(dp2PPK_name),
            self.selDp2PPK,
            self.config['Dp_MVA_Weights'],
            self.config['Dp_MVA_MIN']
        )
        self.selDs2PPKMVA = self.makeMVASelection(
            '{0}MVASelection'.format(ds2PPK_name),
            self.selDs2PPK,
            self.config['Ds_MVA_Weights'],
            self.config['Ds_MVA_MIN']
        )
        self.line_Dp2PPK = self.make_line(
            name='{0}Line'.format(dp2PPK_name),
            selection=self.selDp2PPKMVA,
            prescale=config['PrescaleDp2KPPDCS'],
            postscale=config['PostscaleDp2KPPDCS'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )
        self.line_Ds2PPK = self.make_line(
            name='{0}Line'.format(ds2PPK_name),
            selection=self.selDs2PPKMVA,
            prescale=config['PrescaleDs2KPPDCS'],
            postscale=config['PostscaleDs2KPPDCS'],
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

        # Add 10 MeV either side of the ADAMASS checks to account for the
        # poorer mass resolution before the vertex fit
        combCuts = (
            '('
            "(ADAMASS('D+') < ({0[Dp_ADAMASS_WIN]} + 10))"
            "| (ADAMASS('D_s+') < ({0[Ds_ADAMASS_WIN]} + 10))"
            ')'
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
            CombinationCut=combCuts,
            MotherCut=dCuts
        )

        return Selection(name, Algorithm=_dplus, RequiredSelections=inputSel)

    def splitHHH(self, name, hhhSelection):
        """Split the input h-h+h+ Selection in to a D+ and D_s+ selection.

        Returns a two-tuple as (D+ Selection, D_s+ Selection).
        Keyword arguments:
        hhhSelection -- A single Selection instance; the output of makeD2HHH
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
            RequiredSelections=[hhhSelection]
        )
        dsSel = Selection(
            'SelFilteredDs{0}'.format(name),
            Algorithm=dsFilter,
            RequiredSelections=[hhhSelection]
        )

        # The HHH selection is labelled as a D+, so rename the candidates in
        # the D_s+ selection as such
        dsSubPID = SubstitutePID(
            name='SubPidDs{0}'.format(name),
            Code="DECTREE('[D+ -> X- X+ X+]CC')",
            Substitutions={
                'D+ -> X- X+ X+': 'D_s+',
                'D- -> X+ X- X-': 'D_s-'

            },
            MaxChi2PerDoF=-1
        )
        dsSubPIDSel = Selection(
            'SubPIDDsSel{0}'.format(name),
            Algorithm=dsSubPID,
            RequiredSelections=[dsSel]
        )

        return dpSel, dsSubPIDSel

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
