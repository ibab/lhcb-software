"""
Stripping line for selection of prompt

    B0 -> Lambda_c~- p+ pi- pi+
    B+ -> Lambda_c~- p+ pi- pi+ pi+
    Lambda_b0 -> Lambda_c+ pi-
    Lambda_b0 -> Lambda_c+ pi- pi+ pi-
    Sigma_c++ -> Lambda_c+ pi+
    Sigma_c0 -> Lambda_c+ pi-
    Lambda_c(2595)+ -> Lambda_c+ pi- pi+

and their charge conjugates for proton detection asymmetry measurements.
As such, the Lambda_c+ decays are partially reconstructed as

    Lambda_c+ -> K- pi+

The intention is that the yields of the modes can be measured without the
explicit reconstruction of the proton from the Lambda_c, and then measured
again after explicit reconstruction.
The ratio of the two yields is the proton reconstructione efficiency, and the
ratio of this between proton and antiprotons is the proton detection asymmetry.

Responsible: Alex Pearce <alex.pearce@cern.ch>
Date: 21/08/2014
"""
from GaudiKernel.SystemOfUnits import MeV, mm
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
    "ProtonAsymBuilder"
]

# Dictionary of cut variables and values
default_config = {
    "NAME": "ProtonAsym",
    "WGs": ["Charm"],
    "BUILDERTYPE": "ProtonAsymBuilder",
    "CONFIG": {
        # Are the candidates to be stored on microDST, as opposed to full?
        "MicroDST":  True,

        # Fraction of candidates to randomly throw away before stripping
        "Prescale": 1.0,
        # Fraction of candidates to randomly throw away after stripping
        "Postscale": 1.0,

        # Partial Lambda_c mass window, taken from TGenPhaseSpace studies
        "LcPartialMassMin": 620*MeV,
        "LcPartialMassMax": 1360*MeV,
        # Minimum sum of Lambda_c daughters' tranverse momentum
        "LcPartialPTSum": 1500.0*MeV,
        # Minimum Lambda_c flight distance fit quality per DOF
        "LcPartialFDCHI2": 50.0,
        # Maximum Lambda_c pairwise daughter distance of closest approach
        "LcPartialDOCACHI2": 20.0,
        # Maximum Lambda_c vertex fit quality per DOF
        "LcPartialVCHI2DOF": 15.0,

        # Partial Sigma_c mass window, taken from TGenPhaseSpace studies
        "ScPartialMassMin": 1000*MeV,
        "ScPartialMassMax": 1600*MeV,
        # Minimum Lambda_c vertex displacement wrt Sigma_c vertex along z
        "ScMinZ": 0.0*mm,
        # Sigma_c maximum vertex quality per DOF
        "ScVCHI2DOF": 15.0,
        # Maximum Sigma_c impact parameter chi2
        "ScBPVIPCHI2": 20.0,

        # Partial Lambda_c* mass window, taken from TGenPhaseSpace studies
        "LcstPartialMassMin": 1300*MeV,
        "LcstPartialMassMax": 1660*MeV,
        # Minimum Lambda_c vertex displacement wrt Lambda_c* vertex along z
        "LcstMinZ": 0.0*mm,
        # Lambda_c* maximum vertex quality per DOF
        "LcstVCHI2DOF": 15.0,
        # Maximum Lambda_c* impact parameter chi2
        "LcstBPVIPCHI2": 20.0,

        # Partial Lambda_b0 mass window, taken from TGenPhaseSpace studies
        "LbToLc1piPartialMassMin": 3000*MeV,
        "LbToLc3piPartialMassMin": 3500*MeV,
        "LbPartialMassMax": 4700*MeV,
        # Minimum transverse momentum of combined daughters
        "LbPTSum": 2000*MeV,
        # Lambda_b0 maximum vertex quality
        "LbVCHI2DOF": 15.0,
        # Minimum Lambda_c vertex displacement wrt Lambda_b vertex along z
        "LbMinZ": 0.0*mm,
        # Maximum Lambda_b impact parameter chi2
        "LbBPVIPCHI2": 20.0,

        # Partial B0/B+ mass window, taken from TGenPhaseSpace studies
        "BPartialMassMin": 3500*MeV,
        "BPartialMassMax": 4350*MeV,
        # Minimum transverse momentum of combined daughters
        "BPTSum": 2000*MeV,
        # B0/B+ maximum vertex quality
        "BVCHI2DOF": 15.0,
        # Minimum Lambda_c vertex displacement wrt B0/B+ vertex along z
        "BMinZ": 0.0*mm,
        # Maximum B0/B+ impact parameter chi2
        "BBPVIPCHI2": 20.0,

        # Minimum final state track momentum
        "TrackP": 3000.0*MeV,
        # Minimum final state track transverse momentum
        "TrackPT": 500.0*MeV,
        # Maximum final state track impact parameter chi^2
        "TrackIPCHI2": 20.0,
        # Minimum Kaon ANN PID
        "ProbNNk": 0.2,
        # Minimum Pion ANN PID
        "ProbNNpi": 0.2,
        # Minimum Proton ANN PID
        "ProbNNp": 0.2
    },
    "STREAMS": {
        "Calibration": [
            "StrippingBd2LcppipiLc2KpiProtonAsymLine",
            "StrippingBu2LcppipipiLc2KpiProtonAsymLine",
            "StrippingLb2LcpiLc2KpiProtonAsymLine",
            "StrippingLb2LcpipipiLc2KpiProtonAsymLine",
            "StrippingSc2LcpipLc2KpiProtonAsymLine",
            "StrippingSc2LcpimLc2KpiProtonAsymLine"
        ]
    }
}

class ProtonAsymBuilder(LineBuilder):
    """Creates a LineBuilder object containing proton asym stripping lines."""
    # Decay descriptors
    bd_decay = "[B0 -> Lambda_c~- p+ pi- pi+]cc"
    bu_decay = "[B+ -> Lambda_c~- p+ pi- pi+ pi+]cc"
    lb_1pi_decay = "[Lambda_b0 -> Lambda_c+ pi-]cc"
    lb_3pi_decay = "[Lambda_b0 -> Lambda_c+ pi- pi+ pi-]cc"
    sc_pip_decay = "[Sigma_c++ -> Lambda_c+ pi+]cc"
    sc_pim_decay = "[Sigma_c0 -> Lambda_c+ pi-]cc"
    lcst_decay = "[Lambda_c(2595)+ -> Lambda_c+ pi- pi+]cc"
    lc_partial_decay = "[Lambda_c+ -> K- pi+]cc"

    # Allowed configuration keys
    __configuration_keys__ = default_config["CONFIG"].keys()

    def __init__(self, name, config):
        """Initialise a LineBuilder instance registered with this line.

        Keyword arguments:
        name -- String passed to the LineBuilder constructor
        config -- Dictionary of cuts to apply
        """
        self.name = name
        self.config = config

        # Initialise the parent LineBuilder
        LineBuilder.__init__(self, self.name, self.config)

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

        # Lambda_c selection
        lc = self.make_partial_lc("LcSelectionFor{0}".format(self.name))

        # B+/B0 selection
        b_combination_cuts = cut_string([
            "AM - 10 > {0[BPartialMassMin]}",
            "AM + 10 < {0[BPartialMassMax]}",
            "APT > {0[BPTSum]}"
        ], self.config)
        b_mother_cuts = cut_string([
            "M > {0[BPartialMassMin]}",
            "M < {0[BPartialMassMax]}",
            "BPVIPCHI2() < {0[BBPVIPCHI2]}",
            "VFASPF(VCHI2/VDOF) < {0[BVCHI2DOF]}",
            "(CHILD(VFASPF(VZ), 1) - VFASPF(VZ)) > {0[BMinZ]}"
        ], self.config)
        bd = self.make_partial_mother(
            "BdSelectionFor{0}".format(self.name),
            self.bd_decay,
            [lc, self.pions, self.protons],
            b_combination_cuts,
            b_mother_cuts
        )
        bu = self.make_partial_mother(
            "BuSelectionFor{0}".format(self.name),
            self.bu_decay,
            [lc, self.pions, self.protons],
            b_combination_cuts,
            b_mother_cuts
        )

        # Lambda_b selections
        lb_combination_cuts = cut_string([
            "AM + 20 < {0[LbPartialMassMax]}",
            "APT > {0[LbPTSum]}"
        ], self.config)
        lb_mother_cuts = cut_string([
            "M < {0[LbPartialMassMax]}",
            "BPVIPCHI2() < {0[LbBPVIPCHI2]}",
            "VFASPF(VCHI2/VDOF) < {0[LbVCHI2DOF]}",
            "(CHILD(VFASPF(VZ), 1) - VFASPF(VZ)) > {0[LbMinZ]}"
        ], self.config)
        # 1pi and 3pi modes have different mass thresholds
        lb_1pi_combination_cuts = cut_string([
            "AM - 20 > {0[LbToLc1piPartialMassMin]}",
            lb_combination_cuts
        ], self.config)
        lb_3pi_combination_cuts = cut_string([
            "AM - 20 > {0[LbToLc3piPartialMassMin]}",
            lb_combination_cuts
        ], self.config)
        lb_1pi_mother_cuts = cut_string([
            "M > {0[LbToLc1piPartialMassMin]}",
            lb_mother_cuts
        ], self.config)
        lb_3pi_mother_cuts = cut_string([
            "M > {0[LbToLc3piPartialMassMin]}",
            lb_mother_cuts
        ], self.config)
        lb_1pi = self.make_partial_mother(
            "Lb1PiSelectionFor{0}".format(self.name),
            self.lb_1pi_decay,
            [lc, self.pions],
            lb_1pi_combination_cuts,
            lb_1pi_mother_cuts
        )
        lb_3pi = self.make_partial_mother(
            "Lb3PiSelectionFor{0}".format(self.name),
            self.lb_3pi_decay,
            [lc, self.pions],
            lb_3pi_combination_cuts,
            lb_3pi_mother_cuts
        )

        # Sigma_c selections
        sc_combination_cuts = cut_string([
            "AM - 20 > {0[ScPartialMassMin]}",
            "AM + 20 < {0[ScPartialMassMax]}"
        ], self.config)
        sc_mother_cuts = cut_string([
            "M > {0[ScPartialMassMin]}",
            "M < {0[ScPartialMassMax]}",
            "BPVIPCHI2() < {0[ScBPVIPCHI2]}",
            "VFASPF(VCHI2/VDOF) < {0[ScVCHI2DOF]}",
            "(CHILD(VFASPF(VZ), 1) - VFASPF(VZ)) > {0[ScMinZ]}"
        ], self.config)
        sc_pip = self.make_partial_mother(
            "ScPipSelectionFor{0}".format(self.name),
            self.sc_pip_decay,
            [lc, self.pions],
            sc_combination_cuts,
            sc_mother_cuts
        )
        sc_pim = self.make_partial_mother(
            "ScPimSelectionFor{0}".format(self.name),
            self.sc_pim_decay,
            [lc, self.pions],
            sc_combination_cuts,
            sc_mother_cuts
        )

        # Lambda_c* selections
        lcst_combination_cuts = cut_string([
            "AM - 20 > {0[LcstPartialMassMin]}",
            "AM + 20 < {0[LcstPartialMassMax]}"
        ], self.config)
        lcst_mother_cuts = cut_string([
            "M > {0[LcstPartialMassMin]}",
            "M < {0[LcstPartialMassMax]}",
            "BPVIPCHI2() < {0[LcstBPVIPCHI2]}",
            "VFASPF(VCHI2/VDOF) < {0[LcstVCHI2DOF]}",
            "(CHILD(VFASPF(VZ), 1) - VFASPF(VZ)) > {0[LcstMinZ]}"
        ], self.config)
        lcst = self.make_partial_mother(
            "LcstSelectionFor{0}".format(self.name),
            self.lcst_decay,
            [lc, self.pions],
            lcst_combination_cuts,
            lcst_mother_cuts
        )

        # Create the stripping lines
        bd_line = self.make_line(
            "Bd2LcppipiLc2Kpi{0}Line".format(name),
            bd
        )
        bu_line = self.make_line(
            "Bu2LcppipipiLc2Kpi{0}Line".format(name),
            bu
        )
        lb_1pi_line = self.make_line(
            "Lb2LcpiLc2Kpi{0}Line".format(name),
            lb_1pi
        )
        lb_3pi_line = self.make_line(
            "Lb2LcpipipiLc2Kpi{0}Line".format(name),
            lb_3pi
        )
        sc_pip_line = self.make_line(
            "Sc2LcpipLc2Kpi{0}Line".format(name),
            sc_pip
        )
        sc_pim_line = self.make_line(
            "Sc2LcpimLc2Kpi{0}Line".format(name),
            sc_pim
        )
        lcst_line = self.make_line(
            "Lcst2LcpipiLc2Kpi{0}Line".format(name),
            lcst
        )

        # Make the lines visible to the LineBuilder
        self.registerLine(bu_line)
        self.registerLine(bd_line)
        self.registerLine(lb_1pi_line)
        self.registerLine(lb_3pi_line)
        self.registerLine(sc_pip_line)
        self.registerLine(sc_pim_line)
        self.registerLine(lcst_line)

        # TODO if we're on MicroDST, need to save the protons
        if self.config["MicroDST"]:
            pass

    def make_line(self, name, selection):
        """Return StrippingLine object for the line.

        The line is prescaled and postscaled by the value of the
        `Prescale` and `Postscale` config keys, respectively.
        Keyword arguments:
        name -- String to call the StrippingLine instance
        selection -- Selection instance
        """
        return StrippingLine(
            name,
            prescale=self.config["Prescale"],
            postscale=self.config["Postscale"],
            selection=selection
        )

    def make_partial_mother(self, name, decay, required_selections,
                            combination_cuts, mother_cuts):
        """Return Selection object for mother decay to partial Lambda_c.

        The selection requires that the analytic solution for the missing
        proton momentum is real, i.e. that the discriminant of the quadratic
        solution is positive.
        Keyword arguments:
        name -- String to call the mother Selection instance
        decay -- Decay descriptor for the decay
        required_selections -- Required Selection instances for daughters
        combination_cuts -- Pre-fit cuts on combined daughters
        mother_cuts -- Post-fit cuts on the mother
        """
        # Define the discriminant to cut on
        preambulo = [
            "from GaudiKernel.SystemOfUnits import MeV",
            # Reference PDG 2012 mass values in MeV
            "LAMBDAC_M =  2286.46*MeV",
            "LAMBDAC_M_SQ =  LAMBDAC_M*LAMBDAC_M",
            "PROTON_M =  938.27*MeV",
            "PROTON_M_SQ =  PROTON_M*PROTON_M",
            # Flight direction of Lambda_c
            "Lc_dirx = CHILD(VFASPF(VX), 1) - VFASPF(VX)",
            "Lc_diry = CHILD(VFASPF(VY), 1) - VFASPF(VY)",
            "Lc_dirz = CHILD(VFASPF(VZ), 1) - VFASPF(VZ)",
            # Get the magnitude of flight direction vector, then normalise it
            "Lc_dir_mag = ((Lc_dirx*Lc_dirx) + (Lc_diry*Lc_diry) + (Lc_dirz*Lc_dirz))**0.5",
            "Lc_dirx /= 1.0*Lc_dir_mag",
            "Lc_diry /= 1.0*Lc_dir_mag",
            "Lc_dirz /= 1.0*Lc_dir_mag",
            # Three-momentum of the Lambda_c h1+h2 pair
            "h1h2_px = CHILD(CHILD(PX, 1), 1) + CHILD(CHILD(PX, 2), 1)",
            "h1h2_py = CHILD(CHILD(PY, 1), 1) + CHILD(CHILD(PY, 2), 1)",
            "h1h2_pz = CHILD(CHILD(PZ, 1), 1) + CHILD(CHILD(PZ, 2), 1)",
            # Longitudinal momentum of Lambda_c h1+h2 pair is the dot product of the
            # Lambda_c flight direction with the h1+h2 momentum vector
            "h1h2_pL = (Lc_dirx*h1h2_px) + (Lc_diry*h1h2_py) + (Lc_dirz*h1h2_pz)",
            # Transverse momentum of Lambda_c h1+h2 pair, which is he total h1+h2
            # momentum, minus the component along the Lambda_c direction
            "h1h2_pTx = h1h2_px - (h1h2_pL*Lc_dirx)",
            "h1h2_pTy = h1h2_py - (h1h2_pL*Lc_diry)",
            "h1h2_pTz = h1h2_pz - (h1h2_pL*Lc_dirz)",
            # Square of the magnitude of the transverse momentum of the Lambda_c h1+h2 pair
            "h1h2_pT2 = (h1h2_pTx*h1h2_pTx) + (h1h2_pTy*h1h2_pTy) + (h1h2_pTz*h1h2_pTz)",
            # Energy squared of Lambda_c h1+h2 pair
            "h1h2_E2 = (CHILD(CHILD(E, 1), 1) + CHILD(CHILD(E, 2), 1))**2",
            # This has no phyiscal meaning, its just a common term in the quadratic
            # m(Lambda_c) == m(h1+h2) as we only reconstruct Lambda_c -> h1 h2
            "delta_mass = LAMBDAC_M_SQ - (CHILD(M, 1)**2) - PROTON_M_SQ - (2*h1h2_pT2)",
            "a = ((h1h2_pL*h1h2_pL/h1h2_E2) - 1)",
            "b = delta_mass*h1h2_pL/h1h2_E2",
            "c = (delta_mass*delta_mass/(4*h1h2_E2)) - PROTON_M_SQ - h1h2_pT2",
            "discriminant = (b*b) - (4*a*c)"
        ]
        mother = CombineParticles(
            name="Combine{0}".format(name),
            DecayDescriptors=[decay, self.lc_partial_decay],
            Preambulo=preambulo,
            CombinationCut=combination_cuts,
            MotherCut=cut_string([
                "discriminant > ZERO",
                mother_cuts
            ], {})
        )
        selection = Selection(
            name=name,
            Algorithm=mother,
            RequiredSelections=required_selections
        )
        return selection

    def make_partial_lc(self, name):
        """Return Selection object for Lambda_c+ -> K- pi+ decay.

        This is a partial reconstruction, omitting a p+.
        Keyword arguments:
        name -- String to call the Lambda_c selection instance.
        """
        # Pre-fit cuts on combined daughters
        lc_combination_cuts = cut_string([
            # Slightly larger mass window pre-fit
            "AM - 20 > {0[LcPartialMassMin]}",
            "AM + 20 < {0[LcPartialMassMax]}",
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

