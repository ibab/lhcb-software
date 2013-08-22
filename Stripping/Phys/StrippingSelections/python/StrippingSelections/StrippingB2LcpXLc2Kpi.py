"""
Stripping line for selection of
    B0 -> Lambda_c~- (-> p+ K- pi+) p+ pi- pi+
    B+ -> Lambda_c~- (-> p+ K- pi+) p+ pi- pi+ pi+
for proton detection asymmetry measurement.
As this is for a tag-and-probe analysis, the Lambda_c+
decays are partially reconstructed as
    Lambda_c+ -> K- pi+

Author: Alex Pearce <alex.pearce@cern.ch>
Date: 21/08/2013
"""

from GaudiKernel.SystemOfUnits import MeV
from Configurables import CombineParticles, FilterDesktop
from StandardParticles import (
    StdLooseANNKaons,
    StdLooseANNPions,
    StdLooseANNProtons
)
from PhysSelPython.Wrappers import Selection
from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine

__all__ = [
    "default_config",
    "B2LcpXLc2KpiBuilder"
]

# Dictionary of cut variables and values
default_config = {
    # Fraction of candidates to randomly throw away before stripping
    "Prescale": 1.0,
    # Fraction of candidates to randomly throw away after stripping
    "Postscale": 1.0,
    # B0 partial mass window
    "BMassMin": 2400*MeV,
    "BMassMax": 4400*MeV,
    # B0 mass minus Lambda_c partial mass window
    "BLcDeltaMassMin": 1500*MeV,
    "BLcDeltaMassMax": 3750*MeV,
    # Maximum B0 pairwise daughter distance of closest approach
    "BDOCACHI2": 50.0,
    # Minimum Sum of Lambda_c daughters' tranverse momentum
    "BPTSum": 5000.0*MeV,
    # Maximum B0 vertex fit quality per DOF
    "BVCHI2DOF": 15.0,
    # Minimum cosine of B0 pointing angle difference
    "BDIRA": 0.999,
    # Lambda_c mass window
    "LcPartialMassMin": 800.0*MeV,
    "LcPartialMassMax": 1350.0*MeV,
    # Maximum Lambda_c pairwise daughter distance of closest approach
    "LcPartialDOCACHI2": 10.0,
    # Minimum Sum of Lambda_c daughters' tranverse momentum
    "LcPartialPTSum": 1500.0*MeV,
    # Maximum Lambda_c vertex fit quality per DOF
    "LcPartialVCHI2DOF": 3.0,
    # Minimum Lambda_c flight distance fit quality per DOF
    "LcPartialFDCHI2": 20.0,
    # Minimum Lambda_c daughter momentum
    "TrackP": 2000.0*MeV,
    # Minimum final state track transverse momentum
    "TrackPT": 500.0*MeV,
    # Maximum final state track impact parameter chi^2
    "TrackIPCHI2": 20.0,
    # Minimum Kaon ANN PID
    "ProbNNk": 0.1,
    # Minimum Proton ANN PID
    "ProbNNp": 0.1,
    # Minimum Pion ANN PID
    "ProbNNpi": 0.1
}

class B2LcpXLc2KpiBuilder(LineBuilder):
    """Creates a LineBuilder object containing two B2LcX stripping lines."""
    # Decay descriptors
    bd_decay = "[B0 -> Lambda_c~- p+ pi- pi+]cc"
    bu_decay = "[B+ -> Lambda_c~- p+ pi- pi+ pi+]cc"
    lc_partial_decay = "[Lambda_c+ -> K- pi+]cc"
    # Allowed configuration keys
    __configuration_keys__ = default_config.keys()
    def __init__(self, name, config):
        """Initialise a LineBuilder instance registered with this line.

        Keyword arguments:
        name -- String passed to the LineBuilder constructor
        config -- Dictionary of cuts to apply
        """
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)

        # Final state cut strings
        track_cuts = [
            "P > {0[TrackP]}",
            "PT > {0[TrackPT]}",
            "MIPCHI2DV(PRIMARY) > {0[TrackIPCHI2]}",
        ]
        kaon_cuts = cut_string(track_cuts + [
            "PROBNNk > {0[ProbNNk]}"
        ], self.config)
        pion_cuts = cut_string(track_cuts + [
            "PROBNNpi > {0[ProbNNpi]}"
        ], self.config)
        proton_cuts = cut_string(track_cuts + [
            "PROBNNp > {0[ProbNNp]}"
        ], self.config)

        # Create final state filters
        self.kaons = Selection(
            name="KaonSelectionFor{0}".format(name),
            Algorithm=FilterDesktop(
                name="KaonFilterFor{0}".format(name),
                Code=kaon_cuts
            ),
            RequiredSelections=[StdLooseANNKaons]
        )
        self.pions = Selection(
            name="PionSelectionFor{0}".format(name),
            Algorithm=FilterDesktop(
                name="PionFilterFor{0}".format(name),
                Code=pion_cuts
            ),
            RequiredSelections=[StdLooseANNPions]
        )
        self.protons = Selection(
            name="ProtonSelectionFor{0}".format(name),
            Algorithm=FilterDesktop(
                name="ProtonFilterFor{0}".format(name),
                Code=proton_cuts
            ),
            RequiredSelections=[StdLooseANNProtons]
        )

        # Create the selections
        lc = self.make_partial_lc("LcSelectionFor{0}".format(self.name))
        bd = self.make_partial_b(
            "BdSelectionFor{0}".format(self.name),
            self.bd_decay,
            lc
        )
        bu = self.make_partial_b(
            "BuSelectionFor{0}".format(self.name),
            self.bu_decay,
            lc
        )

        # Create the stripping lines
        bd_line = self.make_line(
            "StrippingBd2LcppipiLc2pKpi{0}Line".format(name),
            bd
        )
        bu_line = self.make_line(
            "StrippingBu2LcppipipiLc2pKpi{0}Line".format(name),
            bu
        )
        # Make the lines visible to the LineBuilder
        self.registerLine(bd_line)
        self.registerLine(bu_line)

    def make_line(self, name, selection):
        """Return StrippingLine object for the line.

        The line is prescaled and postscaled by the value of the
        `Prescale` and `Postscale` config keys, respectively.
        Keyword arguments:
        name -- String to call the StrippingLine instance
        selection -- B Selection instance
        """
        return StrippingLine(
            name,
            prescale=self.config["Prescale"],
            postscale=self.config["Postscale"],
            selection=selection
        )

    def make_partial_b(self, name, decay, lc_selection):
        """Return Selection object for B decay to partial Lambda_c.

        Keyword arguments:
        name -- String to call the B Selection instance
        decay -- Decay descriptor for the decay
        lc_selection -- Lambda_c partial Selection instance
        """
        # Pre-fit cuts on combined daughters
        b_combination_cuts = cut_string([
            "AM - 10 > {0[BMassMin]}",
            "AM + 10 < {0[BMassMax]}",
            "AM - AM1 - 10 > {0[BLcDeltaMassMin]}",
            "AM - AM1 + 10 < {0[BLcDeltaMassMax]}",
            "APT > {0[BPTSum]}",
            "ADOCACHI2CUT({0[BDOCACHI2]}, '')"
        ], self.config)
        # Post-fit cuts on mother
        b_mother_cuts = cut_string([
            "M > {0[BMassMin]}",
            "M < {0[BMassMax]}",
            "VFASPF(VCHI2/VDOF) < {0[BVCHI2DOF]}",
            "BPVDIRA > {0[BDIRA]}"
        ], self.config)
        b = CombineParticles(
            name="Combine{0}".format(name),
            DecayDescriptors=[decay, self.lc_partial_decay],
            CombinationCut=b_combination_cuts,
            MotherCut=b_mother_cuts
        )
        selection = Selection(
            name=name,
            Algorithm=b,
            RequiredSelections=[
                lc_selection,
                self.protons,
                self.pions
            ]
        )
        return selection

    def make_partial_lc(self, name):
        """Return Selection object for Lambda_c+ -> K-pi+ decay.

        This is a partial reconstruction, omitting a p+.
        Keyword arguments:
        name -- String to call the Lambda_c selection instance.
        """
        # Pre-fit cuts on combined daughters
        lc_combination_cuts = cut_string([
            # Slightly larger mass window pre-fit
            "AM - 10 > {0[LcPartialMassMin]}",
            "AM + 10 < {0[LcPartialMassMax]}",
            "APT > {0[LcPartialPTSum]}",
            "ADOCACHI2CUT({0[LcPartialDOCACHI2]}, '')"
        ], self.config)
        # Post-fit cuts on mother
        lc_mother_cuts = cut_string([
            "M > {0[LcPartialMassMin]}",
            "M < {0[LcPartialMassMax]}",
            "VFASPF(VCHI2/VDOF) < {0[LcPartialVCHI2DOF]}",
            "BPVVDCHI2 > {0[LcPartialFDCHI2]}"
        ], self.config)
        lc = CombineParticles(
            name="CombinePartial{0}".format(name),
            DecayDescriptors=[self.lc_partial_decay],
            CombinationCut=lc_combination_cuts,
            MotherCut=lc_mother_cuts
        )
        selection = Selection(
            name,
            Algorithm=lc,
            RequiredSelections=[
                self.kaons,
                self.pions
            ]
        )
        return selection

def cut_string(cuts, config):
    """Return a cut string composed of cuts with values from config.

    Example usage:
        config = {"D0PT": 1000.0, "D0DOCA": 0.5}
        cuts = ["PT > {0[D0PT]}", "ADOCACHI2CUT < {0[D0DOCA]}"]
        cut_string(cuts, config)
        => (PT > 1000.0) & (D0DOCA < 0.5)"
    Keyword arguments:
    cuts -- List of cuts with str.format placeholders for values
    config -- Dictionary of cut values with keys as placeholders in `cuts`
    """
    return "({0})".format(") & (".join(cuts).format(config))

