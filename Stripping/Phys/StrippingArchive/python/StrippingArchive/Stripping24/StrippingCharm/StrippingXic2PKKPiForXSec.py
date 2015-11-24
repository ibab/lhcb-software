"""
Stripping lines for selection of
    [Xi_c0 -> p+ K- K- pi+]cc
for open charm cross section measurement.
"""

__author__ = ['Alex Pearce']

__all__ = (
    'default_config',
    'StrippingXic2PKKPiForXSecConf'
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
    'NAME': 'Xic2PKKPiForXSec',
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingXic2PKKPiForXSecConf',
    'STREAMS': ['Charm'],
    'CONFIG': {
        # Minimum transverse momentum all Xic0 daughters must satisfy
        'Daug_All_PT_MIN': 250.0*MeV,
        # Minimum transverse momentum at least 3 Xic0 daughters must satisfy
        'Daug_3of4_PT_MIN': 300.0*MeV,
        # Minimum transverse momentum at least 2 Xic0 daughters must satisfy
        'Daug_2of4_PT_MIN': 400.0*MeV,
        # Minimum transverse momentum at least 1 Xic0 daughter must satisfy
        'Daug_1of4_PT_MIN': 800.0*MeV,
        # Minimum best primary vertex IP chi^2 all Xic0 daughters must satisfy
        'Daug_All_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 3 Xic0 daughters must satisfy
        'Daug_3of4_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 2 Xic0 daughters must satisfy
        'Daug_2of4_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 1 Xic0 daughter must satisfy
        'Daug_1of4_BPVIPCHI2_MIN': 4.0,
        # Minimum Xic0 daughter momentum
        'Daug_P_MIN': 3.0*GeV,
        # Maximum Xic0 daughter momentum
        'Daug_P_MAX': 100.0*GeV,
        # Minimum Xic0 daughter pseudorapidity
        'Daug_ETA_MIN': 2.0,
        # Maximum Xic0 daughter pseudorapidity
        'Daug_ETA_MAX': 5.0,
        # Minimum Xic0 daughter proton DLLp
        'Proton_PIDpPIDpi_MIN': 10.0,
        # Minimum Xic0 daughter proton DLLp - DLLK
        'Proton_PIDpPIDK_MIN': 5.0,
        # Minimum Xic0 daughter kaon DLLK
        'K_PIDK_MIN': 5.0,
        # Maximum Xic0 daughter pion DLLK
        'Pi_PIDK_MAX': 3.0,
        # Xic0 mass window around the nominal Xic0 mass before the vertex fit
        'Comb_ADAMASS_WIN': 90.0*MeV,
        # Maximum distance of closest approach of Xic0 daughters
        'Comb_ADOCAMAX_MAX': 0.5*mm,
        # Maximum Xic0 vertex chi^2 per vertex fit DoF
        'Xic_VCHI2VDOF_MAX': 25.0,
        # Maximum angle between Xic0 momentum and Xic0 direction of flight
        'Xic_acosBPVDIRA_MAX': 35.0*mrad,
        # Primary vertex displacement requirement, that the Xic0 is some sigma
        # away from the PV and it has a minimum flight time
        'Xic_PVDispCut': (
            '((BPVVDCHI2 > 4.0) &'
            '(BPVLTIME() > 0.075*picosecond))'
        ),
        # Minimum Xic0 MVA discriminant value
        'Xic_MVA_MIN': -0.3,
        # Path to the Xic0 MVA weights file
        # BDT is not applied if this is the empty string or None
        'Xic_MVA_Weights': None,
        # Dictionary of LoKi functors defining the Xic0 MVA input variables
        # The keys must match those used when training the MVA
        'Xic_MVA_Variables': {
        },
        # HLT filters, only process events firing triggers matching the RegEx
        'Hlt1Filter': None,
        'Hlt2Filter': None,
        # Fraction of candidates to randomly throw away before stripping
        'PrescaleXic2PKKPi': 0.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleXic2PKKPi': 1.0
    }
}


class StrippingXic2PKKPiForXSecConf(LineBuilder):
    """Creates LineBuilder object containing the stripping lines."""
    # Allowed configuration keys
    __configuration_keys__ = default_config['CONFIG'].keys()

    # Decay descriptors
    Xic2PKKPi = ['[Xi_c0 -> p+ K- K- pi+]cc']

    def __init__(self, name, config):
        """Initialise this LineBuilder instance."""
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)

        xic_pkkpi_name = '{0}Xic2PKKPi'.format(name)

        self.inPions = StdAllNoPIDsPions
        self.inKaons = StdAllNoPIDsKaons
        self.inProtons = StdAllNoPIDsProtons

        self.selXic2PKKPi = self.makeXic2PKKPi(
            name=xic_pkkpi_name,
            inputSel=[self.inProtons, self.inKaons, self.inPions],
            decDescriptors=self.Xic2PKKPi
        )
        self.selXic2PKKPiMVA = self.makeMVASelection(
            '{0}MVASelection'.format(xic_pkkpi_name),
            self.selXic2PKKPi
        )

        self.line_Xic2PKKPi = self.make_line(
            name='{0}Line'.format(xic_pkkpi_name),
            selection=self.selXic2PKKPiMVA,
            prescale=config['PrescaleXic2PKKPi'],
            postscale=config['PostscaleXic2PKKPi'],
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

    def makeXic2PKKPi(self, name, inputSel, decDescriptors):
        """Return a Selection instance for a Xi_c0 -> p+ h- h- h+ decay.

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

        combCuts = (
            "(ADAMASS('Xi_c0') < {0[Comb_ADAMASS_WIN]})"
            '& (AMAXCHILD(PT) > {0[Daug_1of4_PT_MIN]})'
            '& (AMAXCHILD(BPVIPCHI2()) > {0[Daug_1of4_BPVIPCHI2_MIN]})'
            '& (ANUM(PT > {0[Daug_3of4_PT_MIN]}) >= 3)'
            '& (ANUM(PT > {0[Daug_2of4_PT_MIN]}) >= 2)'
            '& (ANUM(BPVIPCHI2() > {0[Daug_3of4_BPVIPCHI2_MIN]}) >= 3)'
            '& (ANUM(BPVIPCHI2() > {0[Daug_2of4_BPVIPCHI2_MIN]}) >= 2)'
            "& (ACUTDOCA({0[Comb_ADOCAMAX_MAX]}, ''))"
        ).format(self.config)

        xicCuts = (
            '(VFASPF(VCHI2/VDOF) < {0[Xic_VCHI2VDOF_MAX]})'
            '& ({0[Xic_PVDispCut]})'
            '& (BPVDIRA > cos({0[Xic_acosBPVDIRA_MAX]}))'
        ).format(self.config)

        _Xic = CombineParticles(
            name='Combine{0}'.format(name),
            DecayDescriptors=decDescriptors,
            Preambulo=lclPreambulo,
            DaughtersCuts={
                'pi+': '{0} & {1}'.format(daugCuts, pionPIDCuts),
                'K+': '{0} & {1}'.format(daugCuts, kaonPIDCuts),
                'p+': '{0} & {1}'.format(daugCuts, protonPIDCuts)
            },
            CombinationCut=combCuts,
            MotherCut=xicCuts
        )

        return Selection(name, Algorithm=_Xic, RequiredSelections=inputSel)

    def makeMVASelection(self, name, inputSel):
        # Don't apply a BDT if the weights file has not been specified
        if not self.config['Xic_MVA_Weights']:
            return inputSel

        cut = "VALUE('LoKi::Hybrid::DictValue/{0}') > {1[Xic_MVA_MIN]}".format(
            name, self.config
        )
        mva = FilterDesktop('{0}Filter'.format(name), Code=cut)

        addTMVAclassifierValue(
            Component=mva,
            XMLFile=self.config['Xic_MVA_Weights'],
            Variables=self.config['Xic_MVA_Variables'],
            ToolName=name
        )

        return Selection(name, Algorithm=mva, RequiredSelections=[inputSel])
