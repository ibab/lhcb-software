# Stripping lines for b (di-)jet analyses.
# 19/01/2014
# C. Potterat, V. Coco, M. Rangel

__all__ = ('JetsConf',
           'default_config')

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import MeV, GeV, mm
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from Configurables import CombineParticles
from JetAccessories.JetMaker_Config import JetMakerConf
from StandardParticles import StdJets, StdAllNoPIDsPions
from Configurables import LoKi__VoidFilter, FilterInTrees
from Configurables import TopologicalTagging
from Configurables import LoKi__VertexFitter

#
# Define the default configuration.
default_config = {

    'NAME'        : 'Jets',
    'BUILDERTYPE' : 'JetsConf',
    'STREAMS'     : [ 'EW' ] ,
    'WGs'         : [ 'QEE' ],

    # Prescale for the calibration line.
    'CONFIG':{
        "PRESCALE" : {"MB"         : 1.0,
                      "DIFF"       : 1.0,
                      "bJetPT15"   : 0.005,
                      "bJetPT50"   : 0.1,
                      "bJetPT90"   : 1.0,
                      "dibJetT6A"  : 0.05,
                      "dibJetT6PS" : 1.0,
                      "_3jets_Pt7_3sv" : 1.0,
                      "_4jets_Pt5_3sv" : 1.0,
                      "_4jets_Pt5_0sv_Prescaled" : 0.01
                      },

        # HLT properties.
        "HLT"   : {"LINETOPO"       : "Hlt2Topo", #Line to use for TOPO
                   "LINEMB"         : "Hlt1MBNoBiasDecision"} , # Line to use for MB
        # Track properties.
        "TRK"   : {"MAX_MULT"       : 2500,       # Multiplicity.
                   "MIN_P"          : 5*GeV,     # Momentum.
                   "MIN_PT"         : 500*MeV,   # Transverse momentum.
                   "MIN_MIPCHI2DV"  : 16,        # Impact parameter chi-squared.
                   "MAX_GHP"        : 0.4,
                   "MAX_PROBNNGHOST": 0.7},      # Ghost probability.
        # Secondary vertex properties.
        "SVR"   : {"MAX_CHI2DOCA"   : 8,         # Chi-squared of closest approach.
                   "MIN_BPVDIRA"    : 0,         # Cos of angle between P and flight.
                   "MIN_BPVVDCHI2"  : 100,       # Chi-squared distance with PV.
                   "MAX_CHI2"       : 8,         # Chi-squared.
                   "MAX_M"          : 7*GeV,     # Combined mass.
                   "MIN_SUM_PT"     : 2*GeV},    # Scalar sum of transverse momenta.
        # Fully reconstructed jet p roperties.
        "JET"   : {"JEC"            : False,     # If no STDJETS, apply JEC.
                   "R"              : 0.5,       # If no STDJETS, set jet radius.
                   "MIN_PT"         :  5*GeV,    # Transverse momentum.
                   "EXTLOW_PT"      : 7*GeV,    # Transverse momentum.
                   "VERYLOW_PT"     : 15*GeV,   # Transverse momentum.
                   "LOW_PT"         : 17*GeV,   # Transverse momentum.
                   "MEDIUM_PT"      : 50*GeV,   # Transverse momentum.
                   "HIGH_PT"        : 90*GeV},   # Transverse momentum.
        "DIJET" : {"MAX_COSDPHI"    : -0.8}      # Cos of transverse angle.
    },

}
# Define the class for the di-jet stripping.
class JetsConf(LineBuilder):

    # Configuration keys.
    __configuration_keys__ = default_config['CONFIG'].keys()

    # Initializer.
    def __init__(self, name, config) :

        # Initialize the stripping.
        LineBuilder.__init__(self, name, config)
        self._config = config
        self._name = name

        # Preselection strings for HLT and FILTER.
        hlt     = "HLT_PASS_RE('" + self._config["HLT"]["LINETOPO"] + ".*Decision')"
        hltMB   = "HLT_PASS('"+self._config["HLT"]["LINEMB"]+"')"
        fltDIFF = {"Code" : "(recSummary(LHCb.RecSummary.nPVs, 'Rec/Vertex/Primary')<2)"
                            "& (recSummaryTrack(LHCb.RecSummary.nBackTracks, TrBACKWARD) < 1)",
                   'Preambulo' : ['from LoKiTracks.decorators import *',
                                  'from LoKiCore.functions    import *']}

        # Select the particles.
        trks     = self._create_trks([StdAllNoPIDsPions])
        svrs     = self._create_svrs([trks])
        mptJets  = self._filter_hPTJets([StdJets],str(self._config["JET"]["MIN_PT"]) )
        hptJets  = self._filter_hPTJets([StdJets],str(self._config["JET"]["VERYLOW_PT"]) )
        bJets    = self._create_bJets(mptJets,svrs)
        #one jet has 60% of a TOPO object LHCbID, with PT > 15, 50, 90
        fbJets0   = self._filter_bJets(bJets,0,str(self._config["JET"]["VERYLOW_PT"]))
        fbJets1   = self._filter_bJets(bJets,0,str(self._config["JET"]["MEDIUM_PT"]))
        fbJets2   = self._filter_bJets(bJets,0,str(self._config["JET"]["HIGH_PT"]))
        dijets_T6_A  = self._create_dijets([hptJets, bJets], cut2=0, PT = str(self._config["JET"]["LOW_PT"]))
        #one jet has 60% of a TOPO object LHCbID, the other has both trk of one 2-trk SV
        dijets_T6_PS = self._create_dijets([bJets],   cut1=0,  cut3=15, PT = str(self._config["JET"]["LOW_PT"]))
        #
        #cut1 and cut2 act on the TopoObject tagging for jet1 and jet2:
        #    cut1 apply to jet1 and cut2 apply to jet2, or cut2 apply to jet1 and cut1
        #    apply to jet2
        #cut3 and cut4 act on the  created SV tagging for jet1 and jet2
        #
        # cutX == 0  no tagging
        # cutX == 1  at least  60% of the LHCbID to the "tagging" object are
        # continas in the jet
        # cutX == 11 TOS (or all tracks in the jet)
        #
        #The three following lines are designed for the 4b analyses (Contact: Stephane Tourneur):
        _3jets_Pt7_3sv = self._create_3jets([mptJets, bJets], minPT = str(self._config["JET"]["EXTLOW_PT"]), Nsvtag="2.5")
        _4jets_Pt5_3sv = self._create_4jets([mptJets, bJets], minPT = str(self._config["JET"]["MIN_PT"]), Nsvtag="2.5")
        _4jets_Pt5_0sv = self._create_4jets([mptJets, bJets], minPT = str(self._config["JET"]["MIN_PT"]), Nsvtag="-0.5")

        line_jetDIFF = StrippingLine(
            name + "Diffractive",
            prescale = self._config["PRESCALE"]["DIFF"],
            HLT2 = hlt,
            FILTER = fltDIFF,
            selection = hptJets,
            RequiredRawEvents = ["Calo"]
        )
        line_jetMB = StrippingLine(
            name + "MB",
            prescale = self._config["PRESCALE"]["MB"],
            HLT1 = hltMB,
            selection = hptJets,
            RequiredRawEvents = ["Calo"]
        )
        line_jetPT0 = StrippingLine(
            name + "bJetPT15",
            prescale = self._config["PRESCALE"]["bJetPT15"],
            HLT2 = hlt,
            selection = fbJets0,
            RequiredRawEvents = ["Calo"]
            )
        line_jetPT1 = StrippingLine(
            name + "bJetPT50",
            prescale = self._config["PRESCALE"]["bJetPT50"],
            HLT2 = hlt,
            selection = fbJets1,
            RequiredRawEvents = ["Calo"]
            )
        line_jetPT2 = StrippingLine(
            name + "bJetPT90",
            prescale = self._config["PRESCALE"]["bJetPT90"],
            HLT2 = hlt,
            selection = fbJets2,
            RequiredRawEvents = ["Calo"]
            )
        line_Dijets_T6A = StrippingLine(
            name + "DiJet20_T6A",
            prescale = self._config["PRESCALE"]["dibJetT6A"],
            HLT2 = hlt,
            selection = dijets_T6_A,
            RequiredRawEvents = ["Calo"]
            )
        line_Dijets_T6PS = StrippingLine(
            name + "DiJet20_T6PS",
            prescale = self._config["PRESCALE"]["dibJetT6PS"],
            HLT2 = hlt,
            selection = dijets_T6_PS,
            RequiredRawEvents = ["Calo"]
            )
        line__3jets_Pt7_3sv  = StrippingLine(
            name + "_3jets_Pt7_3sv",
            prescale = self._config["PRESCALE"]["_3jets_Pt7_3sv"],
            HLT2 = hlt,
            selection = _3jets_Pt7_3sv,
            RequiredRawEvents = ["Calo"]
            )
        line__4jets_Pt5_3sv  = StrippingLine(
            name + "_4jets_Pt5_3sv",
            prescale = self._config["PRESCALE"]["_4jets_Pt5_3sv"],
            HLT2 = hlt,
            selection = _4jets_Pt5_3sv,
            RequiredRawEvents = ["Calo"]
            )
        line__4jets_Pt5_0sv_Prescaled  = StrippingLine(
            name + "_4jets_Pt5_0sv_Prescaled",
            prescale = self._config["PRESCALE"]["_4jets_Pt5_0sv_Prescaled"],
            HLT2 = hlt,
            selection = _4jets_Pt5_0sv,
            RequiredRawEvents = ["Calo"]
            )

        self.registerLine(line_jetMB)
        self.registerLine(line_jetDIFF)
        self.registerLine(line_jetPT0)
        self.registerLine(line_jetPT1)
        self.registerLine(line_jetPT2)
        self.registerLine(line_Dijets_T6A)
        self.registerLine(line_Dijets_T6PS)
        self.registerLine(line__3jets_Pt7_3sv)
        self.registerLine(line__4jets_Pt5_3sv)
        self.registerLine(line__4jets_Pt5_0sv_Prescaled)


    def _filter_hPTJets(self, inputs, ptlim):
        code = ("(PT > "+(ptlim)+")")
        jets = FilterDesktop(Code = code)
        return Selection(self._name + str(ptlim).split("*")[0]+"GeVJets",
                         Algorithm = jets,
                         RequiredSelections = inputs)

    def _create_bJets(self, inputs, test):
        bjets = TopologicalTagging(self._name+'taggingJet')
        bjets.ParticleAbsPID = 98
        bjets.SVLocation = test.outputLocation()
        bjets.TriggerLine = self._config["HLT"]["LINETOPO"] + ".*Decision"
        ## Fix Hlt split configuration
        config_split_HLT(bjets)

        imergedSel = MergedSelection(self._name+"merged", RequiredSelections=[inputs,test])
        return Selection(self._name + "bJets",
                         Algorithm = bjets,
                         RequiredSelections = [imergedSel])


    def _filter_bJets(self, _in2, Type=0, minPT = 30*GeV):
        _filter = FilterDesktop(Code="(ABSID=='CELLjet') & (PT> "+(minPT)+") & (PINFO(9990,-1)>"+str(Type)+")")
        return Selection ( self._name+"_bJetFilter"+str(Type)+"PT"+str(minPT).split("*")[0],
                           Algorithm          = _filter ,
                           RequiredSelections =  [_in2]
                       )
   # Create the tracks.
    def _create_trks(self, inputs):
        code = ("(PT > %s)"
                % self._config["TRK"]["MIN_PT"]
                + " & (P > %s)"
                % self._config["TRK"]["MIN_P"]
                + " & (MIPCHI2DV(PRIMARY) > %s)"
                % self._config["TRK"]["MIN_MIPCHI2DV"]
                + " & (TRGHP < %s)"
                % self._config["TRK"]["MAX_GHP"]
                + " & (PPINFO(705,-1) < %s)"
                % self._config["TRK"]["MAX_PROBNNGHOST"])
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
            "2ndVertexJetsStrippingLine",
            DecayDescriptors = ["D0 -> pi- pi+", "D0 -> pi- pi-",
                                "D0 -> pi+ pi+"],
            CombinationCut = cmb_cuts,
            MotherCut = mom_cuts
            )
        return Selection(self._name + "SvrsSelection",
                         Algorithm = svrs,
                         RequiredSelections = inputs)

    # Create the di-jets.
    def _create_dijets(self, inputs,cut1=-1, cut2=-1, cut3 = -1, cut4 = -1, PT = 25*GeV,  LeadingPT = 0):
        pre = ["COSDPHI = cos(ACHILD(PHI,1) - ACHILD(PHI,2))"]
        cmb_cuts = "(abs(ACHILD(BPV(VZ),1)-ACHILD(BPV(VZ),2))<1e-6)  & (COSDPHI < %s) " % self._config["DIJET"]["MAX_COSDPHI"]
        label = ""
        if(cut1 >-1 or cut2 > -1):
            cmb_cuts += "& ((  (ACHILD(PINFO(9990,0),1)>"+str(cut1)+") &  (ACHILD(PINFO(9990,0),2)>"+str(cut2)+"))  |    ((ACHILD(PINFO(9990,0),2)>"+str(cut1)+") &  (ACHILD(PINFO(9990,0),1)>"+str(cut2)+"))) "
            label += "T"
        if(cut3 >-1 or cut4 > -1):
            cmb_cuts += "& ((  (ACHILD(PINFO(9991,0),1)>"+str(cut3)+") &  (ACHILD(PINFO(9991,0),2)>"+str(cut4)+"))  |    ((ACHILD(PINFO(9991,0),2)>"+str(cut3)+") &  (ACHILD(PINFO(9991,0),1)>"+str(cut4)+"))) "
            label += "PS"
        if(cut3 >-1 and cut1 > -1):
            cmb_cuts += "& ((  (ACHILD(PINFO(9990,0),1)>"+str(cut1)+") &  (ACHILD(PINFO(9991,0),2)>"+str(cut3)+"))  |    ((ACHILD(PINFO(9990,0),2)>"+str(cut3)+") &  (ACHILD(PINFO(9991,0),1)>"+str(cut1)+"))) "
            label += "TPS"
        dijets = CombineParticles(
            "DiJetCreatation" + str(PT) + str(LeadingPT) + str(cut1) + str(cut2) + str(cut3) + str(cut4),
            ParticleCombiners = {"" : "LoKi::VertexFitter"},
            DecayDescriptor = "H_10 -> CELLjet CELLjet",
            DaughtersCuts = {"CELLjet" : "(PT > "+str(PT)+")"},
            #                             % self._config["JET"]["MIN_PT"]},
            Preambulo = pre,
            CombinationCut = cmb_cuts,
            MotherCut = "INTREE((ABSID=='CELLjet')&(PT>"+str(LeadingPT)+"))")
        dijets.addTool( LoKi__VertexFitter, name="LoKi::VertexFitter" )
        vfitter = getattr ( dijets , "LoKi::VertexFitter" )
        vfitter.Jets = ""

        return Selection(self._name + label + "PT" +str(PT).split('*')[0]+"LePT"+ str(LeadingPT).split('*')[0]+ "Selection",
                                         Algorithm = dijets,
                                         RequiredSelections = inputs)

    def _create_3jets(self, inputs, minPT = 5*GeV, LeadingPT = 0*GeV, Nsvtag = 0):

        cmb_cuts = "(abs(ACHILD(BPV(VZ),1)-ACHILD(BPV(VZ),2))<1*mm) & (abs(ACHILD(BPV(VZ),1)-ACHILD(BPV(VZ),3))<1*mm)"
        _3jets = CombineParticles(
            "3JetCreatation" + str(minPT) + str(LeadingPT) + str(Nsvtag),
            ParticleCombiners = {"" : "LoKi::VertexFitter"},
            DecayDescriptor = "H_10 -> CELLjet CELLjet CELLjet",
            CombinationCut = cmb_cuts,
            DaughtersCuts = {"CELLjet" : "(PT > "+str(minPT)+")"},
            MotherCut = "((INTREE((ABSID=='CELLjet')&(PT>"+str(LeadingPT)+")))&(INTREE((ABSID=='CELLjet')&(PINFO(9990,0)>0)))&("+str(Nsvtag)+" < NINTREE((ABSID=='CELLjet')&(PINFO(9991,0) > 5 ))))")
        _3jets.addTool( LoKi__VertexFitter, name="LoKi::VertexFitter" )
        vfitter = getattr ( _3jets , "LoKi::VertexFitter" )
        vfitter.Jets = ""


        return Selection(self._name + "3jets_PTmin" +str(minPT).split('*')[0]+"LeadPT"+ str(LeadingPT).split('*')[0]+ "Nsv" + str(Nsvtag) +"Selection", Algorithm = _3jets, RequiredSelections = inputs)

    def _create_4jets(self, inputs, minPT = 5*GeV, LeadingPT = 0*GeV, Nsvtag = 0):

        cmb_cuts = "(abs(ACHILD(BPV(VZ),1)-ACHILD(BPV(VZ),2))<1*mm) & (abs(ACHILD(BPV(VZ),1)-ACHILD(BPV(VZ),3))<1*mm) & (abs(ACHILD(BPV(VZ),1)-ACHILD(BPV(VZ),4))<1*mm)"
        _4jets = CombineParticles(
            "4JetCreatation" + str(minPT) + str(LeadingPT) + str(Nsvtag),
            ParticleCombiners = {"" : "LoKi::VertexFitter"},
            DecayDescriptor = "H_10 -> CELLjet CELLjet CELLjet CELLjet",
            CombinationCut = cmb_cuts,
            DaughtersCuts = {"CELLjet" : "(PT > "+str(minPT)+")"},
            MotherCut = "((INTREE((ABSID=='CELLjet')&(PT>"+str(LeadingPT)+")))&(INTREE((ABSID=='CELLjet')&(PINFO(9990,0)>0)))&("+str(Nsvtag)+" < NINTREE((ABSID=='CELLjet')&(PINFO(9991,0) > 5 ))))")
        _4jets.addTool( LoKi__VertexFitter, name="LoKi::VertexFitter" )
        vfitter = getattr ( _4jets , "LoKi::VertexFitter" )
        vfitter.Jets = ""


        return Selection(self._name + "4jets_PTmin" +str(minPT).split('*')[0]+"LeadPT"+ str(LeadingPT).split('*')[0]+ "Nsv" + str(Nsvtag) +"Selection", Algorithm = _4jets, RequiredSelections = inputs)

def config_split_HLT(tool):
    from DAQSys.Decoders import DecoderDB
    from Configurables import TriggerTisTos
    tool.addTool(TriggerTisTos, "TriggerTisTos")
    tool.TriggerTisTos.HltDecReportsLocation = DecoderDB["HltDecReportsDecoder/Hlt2DecReportsDecoder"].listOutputs()[0]
    tool.TriggerTisTos.HltSelReportsLocation = DecoderDB["HltSelReportsDecoder/Hlt2SelReportsDecoder"].listOutputs()[0]

