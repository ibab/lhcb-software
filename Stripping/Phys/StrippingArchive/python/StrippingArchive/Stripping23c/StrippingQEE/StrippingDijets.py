# Stripping lines for b and c di-jet analyses.
# 19/01/2014
# P. Ilten and M. Williams

__all__ = ('DijetsConf',
           'default_config')

from Gaudi.Configuration import *
from Configurables import TisTosParticleTagger
from GaudiKernel.SystemOfUnits import MeV, GeV, mm
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from JetAccessories.JetMaker_Config import JetMakerConf
from Configurables import LoKi__FastJetMaker, LoKi__JetMaker
from StandardParticles import StdLoosePhotons, StdAllNoPIDsPions, StdJets

# Define the default configuration.

default_config = {
    'NAME'        : 'Dijets',
    'BUILDERTYPE' : 'DijetsConf',
    'STREAMS'     : ['EW'],
    'WGs'         : ['QEE'],
    'CONFIG'      : { 
        # Prescale for the calibration line.
        "PRESCALE" : 0.03,
        # HLT properties.
        "HLT"   : {"LINE" : "Hlt1TrackMuon"},   # Line to use.
        # Track properties.
        "TRK"   : {"MAX_MULT"      : 250,       # Multiplicity.
                   "MIN_P"         : 5*GeV,     # Momentum.
                   "MIN_PT"        : 500*MeV,   # Transverse momentum.
                   "MIN_MIPCHI2DV" : 16,        # Impact parameter chi-squared.
                   "MAX_GHP"       : 0.4},      # Ghost probability.
        # Muon properties.
        "MUO"   : {"MIN_PT"        : 2*GeV,     # Transverse momentum.
                   "MIN_MIPCHI2DV" : 16},       # Impact parameter chi-squared.
        # Secondary vertex properties.
        "SVR"   : {"MAX_CHI2DOCA"  : 8,         # Chi-squared of closest approach.
                   "MIN_BPVDIRA"   : 0,         # Cos of angle between P and flight.
                   "MIN_BPVVDCHI2" : 100,       # Chi-squared distance with PV.
                   "MAX_CHI2"      : 8,         # Chi-squared.
                   "MAX_M"         : 7*GeV,     # Combined mass.
                   "MIN_SUM_PT"    : 2*GeV},    # Scalar sum of transverse momenta.
        # Fully reconstructed jet properties.
        "JET"   : {"STDJETS"       : False,     # Flag to use StdJets container.
                   "PF"            : True,      # Flag to use particle flow.
                   "JEC"           : False,     # If no STDJETS, apply JEC.
                   "R"             : 0.7,       # If no STDJETS, set jet radius.
                   "MIN_PT"        : 19*GeV},   # Transverse momentum.
        # Pair of secondary vertices properties.
        "DISVR" : {"MAX_COSDPHI"   : 0,         # Cos of transverse angle.
                   "MAX_COSDALPHA" : 0.99,      # Cos of opening angle.
                   "MIN_M"         : 2*GeV,     # Combined mass.
                   "MIN_SUM_PT"    : 10*GeV},   # Scalar sum of transverse momenta.
        # Pair of jets properties.
        "DIJET" : {"MAX_COSDPHI"   : -0.8}      # Cos of transverse angle.
    },
 }

# Define the class for the di-jet stripping.
class DijetsConf(LineBuilder):

    # Configuration keys.
    __configuration_keys__ = default_config['CONFIG'].keys()
    
    # Initializer.
    def __init__(self, name, config) :

        # Initialize the stripping.
        LineBuilder.__init__(self, name, config)
        self._config = config
        self._name = name

        # Preselection strings for HLT and FILTER.
        hlt = "HLT_PASS_RE('" + self._config["HLT"]["LINE"] + ".*Decision')"
        flt = {"Code" : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)"
               " < %s)" % self._config["TRK"]["MAX_MULT"],
               'Preambulo' : ["from LoKiTracks.decorators import *",
                              'from LoKiCore.functions    import *']}

        # Select the particles.
        trks   = self._create_trks([StdAllNoPIDsPions])
        svrs   = self._create_svrs([trks])
        jets   = (self._create_jets() if config["JET"]["STDJETS"] is False
                  else StdJets)
        disvrs = self._create_disvrs([svrs])
        dijets = self._create_dijets([disvrs, jets])
        
        # The di-jet line.
        line_dijets = StrippingLine(
            name + "Line",
            prescale = 1.0,
            RequiredRawEvents = ["Calo"],
            # HLT = hlt,
            HLT1 = hlt,
            FILTER = flt,
            selection = dijets
            )
        self.registerLine(line_dijets)

        # The pre-scaled di-jet line.
        line_prescaled_dijets = StrippingLine(
            name + "PrescaledLine",
            prescale = self._config["PRESCALE"],
            RequiredRawEvents = ["Calo"],
            # HLT = hlt,
            HLT1 = hlt,
            FILTER = flt,
            selection =  self._create_dijets([jets], "Prescaled")
            )
        self.registerLine(line_prescaled_dijets)

    # Create the tracks.
    def _create_trks(self, inputs):
        code = ("(PT > %s)"
                % self._config["TRK"]["MIN_PT"]
                + " & (P > %s)"
                % self._config["TRK"]["MIN_P"]
                + " & (MIPCHI2DV(PRIMARY) > %s)"
                % self._config["TRK"]["MIN_MIPCHI2DV"]
                + " & (TRGHP < %s)"
                % self._config["TRK"]["MAX_GHP"])
        trks = FilterDesktop(Code = code)
        return Selection(self._name + "TrksSelection",
                         Algorithm = trks,
                         RequiredSelections = inputs)

    # Create the svrs.
    def _create_svrs(self, inputs):
        cmb_cuts = ("(ADOCACHI2CUT(%s,''))"
                    % self._config["SVR"]["MAX_CHI2DOCA"]
                    + " & (AM < %s)"
                    % self._config["SVR"]["MAX_M"]
                    + " & (ASUM(PT) > %s)"
                    % self._config["SVR"]["MIN_SUM_PT"])
        mom_cuts = ("(BPVDIRA > %s)"
                    % self._config["SVR"]["MIN_BPVDIRA"]
                    + " & (BPVVDCHI2 > %s)"
                    % self._config["SVR"]["MIN_BPVVDCHI2"]
                    + " & (VFASPF(VCHI2) < %s)"
                    % self._config["SVR"]["MAX_CHI2"])
        svrs = CombineParticles(
            DecayDescriptors = ["D0 -> pi- pi+", "D0 -> pi- pi-",
                                "D0 -> pi+ pi+"],
            CombinationCut = cmb_cuts,
            MotherCut = mom_cuts
            )
        return Selection(self._name + "SvrsSelection",
                         Algorithm = svrs,
                         RequiredSelections = inputs)

    # Create the jets.
    def _create_jets(self):
        # Use particle flow.
        if self._config["JET"]["PF"]:
            pfps = DataOnDemand(Location = "Phys/PFParticles/Particles")
            jets = JetMakerConf(
                self._name + "StdJets",
                R = self._config["JET"]["R"],
                JetEnergyCorrection = self._config["JET"]["JEC"]).algorithms[0]
            return Selection(self._name + "JetsSelection",
                             Algorithm = jets,
                             RequiredSelections = [pfps])
        # Just use pions and photons.
        else:
            jets =  LoKi__JetMaker(
                self._name + "JetsMaker",
                JetMaker = 'LoKi::FastJetMaker',
                JetID = False,
                Associate2Vertex = True,
                ApplyJEC = self._config["JET"]["JEC"],
                Inputs = ['Phys/StdLoosePhotons/Particles',
                          'Phys/StdAllNoPIDsPions/Particles']
                )
            jets.addTool(LoKi__FastJetMaker)
            jets.LoKi__FastJetMaker.Type = 2                             
            jets.LoKi__FastJetMaker.RParameter = self._config["JET"]["R"]
            jets.LoKi__FastJetMaker.PtMin = self._config["JET"]["MIN_PT"]
            jets.LoKi__FastJetMaker.Recombination = 0                    
            return Selection(self._name + "JetsSelection" ,
                             Algorithm = jets,
                             RequiredSelections = [StdAllNoPIDsPions, 
                                                   StdLoosePhotons])

    # Create the di-svrs.
    def _create_disvrs(self, inputs):
        pre = ["D1 = ACHILD(BPVVD,1)",
               "DZ1 = ACHILD(BPVVDZ,1)",
               "DR1 = ACHILD(BPVVDR,1)",
               "SIN1 = DR1/D1",
               "COS1 = DZ1/D1",
               "SVX1 = ACHILD(VFASPF(VX),1)",
               "SVY1 = ACHILD(VFASPF(VY),1)",
               "D2 = ACHILD(BPVVD,2)",
               "DZ2 = ACHILD(BPVVDZ,2)",
               "DR2 = ACHILD(BPVVDR,2)",
               "SIN2 = DR2/D2",
               "COS2 = DZ2/D2",
               "SVX2 = ACHILD(VFASPF(VX),2)",
               "SVY2 = ACHILD(VFASPF(VY),2)",
               "DSVX = (SVX1-SVX2)",
               "DSVY = (SVY1-SVY2)",
               "COSDPHI = (DR1**2 + DR2**2 - DSVX**2 - DSVY**2)/(2*DR1*DR2)",
               "COSDALPHA = SIN1*SIN2*COSDPHI + COS1*COS2"]
        cmb_cuts = ("(COSDALPHA < %s)"
                    % self._config["DISVR"]["MAX_COSDALPHA"]
                    + " & (COSDPHI < %s)"
                    % self._config["DISVR"]["MAX_COSDPHI"]
                    + " & (AM > %s)"
                    % self._config["DISVR"]["MIN_M"]
                    + " & (ASUM(SUMTREE(PT,(ISBASIC),0.0)) > %s)"
                    % self._config["DISVR"]["MIN_SUM_PT"])
        mom_cuts = ("INTREE(HASMUON & ISMUON"
                    + " & (PT > %s)"
                    % self._config["MUO"]["MIN_PT"]
                    + " & (MIPCHI2DV(PRIMARY) > %s))"
                    % self._config["MUO"]["MIN_MIPCHI2DV"])
        disvrs = CombineParticles(
            DecayDescriptors = ["B0 -> D0 D0"],
            Preambulo = pre,
            CombinationCut = cmb_cuts,
            MotherCut = mom_cuts
            )
        return Selection(self._name + "DisvrsSelection",
                         Algorithm = disvrs,
                         RequiredSelections = inputs)

    # Create the di-jets.
    def _create_dijets(self, inputs, label = ""):
        pre = ["COSDPHI = cos(ACHILD(PHI,1) - ACHILD(PHI,2))"]
        cmb_cuts = "COSDPHI < %s" % self._config["DIJET"]["MAX_COSDPHI"]
        dijets = CombineParticles(
            ParticleCombiners = {"" : "LoKi::VertexFitter"},
            DecayDescriptor = "H_10 -> CELLjet CELLjet",
            DaughtersCuts = {"CELLjet" : "PT > %s"
                             % self._config["JET"]["MIN_PT"]},
            Preambulo = pre,
            CombinationCut = cmb_cuts,
            MotherCut = "ALL"
            )
        return Selection(self._name + label + "Selection",
                         Algorithm = dijets,
                         RequiredSelections = inputs)
