"""
 script to configure HLT Monitoring

 @author R. Aaij
 @date 19-06-2015
"""
# =============================================================================
__author__  = "R. Aaij roel.aaij@cern.ch"
# =============================================================================
import types
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer as Sequence

def _recurse(c,fun) :
    fun(c)
    for p in [ 'Members','Filter0','Filter1' ] :
        if not hasattr(c,p) : continue
        x = getattr(c,p)
        if list is not type(x) : x = [ x ]
        for i in x : _recurse(i,fun)

def _enableMonitoring(c) :
    if c.getType() in ['FilterDesktop','CombineParticles' ] :
        c.Monitor = True
        c.HistoProduce = True

def _disableHistograms(c,filter = lambda x : True) :
    if 'HistoProduce' in c.getDefaultProperties() and filter(c):
        c.HistoProduce = False
    for p in [ 'Members','Filter0','Filter1' ] :
        if not hasattr(c,p) : continue
        x = getattr(c,p)
        if list is not type(x) : x = [ x ]
        for i in x : _disableHistograms(i,filter)


class HltMonitoringConf(LHCbConfigurableUser):
    __used_configurables__ = []

    __slots__ = {"EnableMonitoring"         : False,
                 "EnableL0Monitor"          : True,
                 "EnableHltMonitor"         : True,
                 "EnableHlt1TrackMonitor"   : True,
                 'Hlt1TrackMonitorPrescale' : 0.005,
                 "HistogrammingLevel"       : "Line",
                 "OutputFile"               : "",
                 "ActiveHlt1Lines"          : [],
                 "ActiveHlt2Lines"          : [],
                 "MonitorSequence"          : None
                }

    def __globalMonitor(self, stage):
        from Configurables import HltGlobalMonitor
        globalMon = HltGlobalMonitor(stage + "GlobalMonitor")

        from DAQSys.Decoders import DecoderDB
        decRepLoc = DecoderDB["HltDecReportsDecoder/%sDecReportsDecoder" % stage].listOutputs()[0]
        globalMon.HltDecReports = decRepLoc
        globalMon.Stage = stage

        from HltConf.Configuration import onlinePV
        globalMon.VertexLocations = onlinePV()
        return globalMon

    def __groupLines( self, lines , mapping ) :
        import re
        groups = {}
        taken = []
        for pos in range(len(mapping)):
            (name, pattern) = mapping[pos]
            expr = re.compile(pattern)
            groups[ name ] = [ i for i in lines if expr.match(i) and i not in taken ]
            taken += groups[ name ]
        #prune empty groups
        return dict( (k,v) for k,v in groups.iteritems() if v )


    def __l0_monitoring(self, stage):

        from Configurables import HltL0GlobalMonitor
        from DAQSys.Decoders import DecoderDB
        from DAQSys.DecoderClass import decodersForBank

        l0decoder = decodersForBank( DecoderDB, 'L0DU' )
        assert len(l0decoder)
        l0decoder = l0decoder[0].setup()

        decRepLoc = DecoderDB["HltDecReportsDecoder/%sDecReportsDecoder" % stage].listOutputs()[0]
        monitor = HltL0GlobalMonitor(stage + 'L0GlobalMonitor', DecReports = decRepLoc,
                                     HltDecName = stage + "Global")

        if self.getProp("EnableL0Monitor"):
            return [l0decoder, monitor]
        else:
            return []

    def __hlt1_monitoring(self, lines1):
        ## and tell the monitoring what it should expect..
        # the keys are the Labels for the Histograms in the GUI
        # the values are the Pattern Rules to for the Decisions contributing
        from Configurables import HltGlobalMonitor, HltL0GlobalMonitor

        l0Mon = self.__l0_monitoring("Hlt1")

        ## Global monitor
        globalMon = self.__globalMonitor("Hlt1GlobalMonitor")
        globalMon.DecToGroup = self.__groupLines( [ i.decision() for i in lines1 ],
                                                  [ ("L0"         , "Hlt1L0.*Decision"),
                                                    ("LumiBeamGas", "Hlt1(Lumi|BeamGas).*Decision"),
                                                    ("SingleMuon" , "Hlt1(Single|Track)Muon.*Decision"),
                                                    ("DiMuon"     , "Hlt1DiMuon.*Decision"),
                                                    ("TrackMVA"   , "Hlt1(Two)?TrackMVADecision"),
                                                    ("ECAL"       , "Hlt1.*(Electron|Photon).*Decision"),
                                                    ("CEP"        , "Hlt1CEP.*Decision"),
                                                    ("Beauty"     , "Hlt1B2.*Decision"),
                                                    ("Commissioning", "Hlt1(ODIN.*|Tell1Error|Incident)Decision"),
                                                    ("MinBias"    , "Hlt1MB.*Decision"),
                                                    ("Calibration", "Hlt1CalibTracking.*"),
                                                    ("Global"     , ".*Global.*"),
                                                    ("Other"      , ".*") # add a 'catch all' term to pick up all remaining decisions...
                                                  ]
                                                  )

        ## Mass monitor
        from Configurables import HltMassMonitor
        from DAQSys.Decoders import DecoderDB
        massMon = HltMassMonitor("Hlt1MassMonitor")
        massMon.DecReportsLocation = DecoderDB["HltDecReportsDecoder/Hlt1DecReportsDecoder"].listOutputs()[0]
        massMon.SelReportsLocation = DecoderDB["HltSelReportsDecoder/Hlt1SelReportsDecoder"].listOutputs()[0]
        massMon.Decisions  = {"Jpsi"      : "Hlt1DiMuonHighMassDecision",
                              "JpsiAlign" : "Hlt1CalibMuonAlignJpsiDecision",
                              "D0->Kpi"   : 'Hlt1CalibTrackingKPiDetachedDecision',
                              "D0->KK"    : 'Hlt1CalibTrackingKKDecision',
                              "D0->pipi"  : "Hlt1CalibTrackingPiPiDecision",
                              "phi->KK"   : "Hlt1IncPhiDecision"}
        massMon.DecisionStructure = {"Jpsi" : [105.658,105.658]}
        massMon.Histograms = {"Jpsi"      : [ 3010, 3190, 90 ],
                              "JpsiAlign" : [ 3010, 3190, 90 ],
                              "D0->Kpi"   : [ 1815, 1915, 50 ],
                              "D0->KK"    : [ 1815, 1915, 50 ],
                              "D0->pipi"  : [ 1815, 1915, 50 ],
                              "phi->KK"   : [80,1000.,1040.]}

        # Setup the track monitoring
        from Configurables        import Hlt1TrackMonitor
        import HltTracking
        from HltTracking.HltSharedTracking import MinimalVelo, VeloTTTracking, HltHPTTracking
        from Configurables import DeterministicPrescaler
        trackMon = Hlt1TrackMonitor("Hlt1TrackMonitor")
        trackMon.VeloTrackLocation = MinimalVelo.outputSelection()
        trackMon.VeloTTTrackLocation = VeloTTTracking.outputSelection()
        trackMon.ForwardTrackLocation = HltHPTTracking.outputSelection()

        # This is not so nice but currently unavoidable
        from HltTracking.Hlt1TrackNames import Hlt1TrackLoc
        from HltTracking.HltTrackNames import HltDefaultFitSuffix
        trackMon.FittedTrackLocation = Hlt1TrackLoc["FitTrack"]

        prescaler = DeterministicPrescaler("Hlt1TrackMonitorPrescaler", AcceptFraction = self.getProp("Hlt1TrackMonitorPrescale") )
        trackMonSeq = Sequence('Hlt1TrackMonitorSequence',
                               Members = [ prescaler ] + HltHPTTracking.members() + [ trackMon ])

        # Sequence
        monSeq = Sequence("Hlt1MonitorSequence", IgnoreFilterPassed = True)
        if l0Mon:
            monSeq.Members = l0Mon
        monSeq.Members += [globalMon, massMon, trackMonSeq]
        return monSeq

    def __hlt2_monitoring(self, lines2):

        l0Mon = self.__l0_monitoring("Hlt2")

        from Configurables import HltGlobalMonitor
        globalMon = self.__globalMonitor("Hlt2GlobalMonitor")
        globalMon.DecToGroup = self.__groupLines( [ i.decision() for i in lines2 ],
                                 [("B2HH",          "Hlt2B2K{,2}Pi{,2}.*Decision"),           
                                  ("B2Kpi0",        "Hlt2B2K0?[pP]i0?.*Decision"),         
                                  ("Bc2JpsiX",      "Hlt2Bc2JpsiX.*Decision"),       
                                  ("CaloTest",      "Hlt2CaloTest.*Decision"),       
                                  ("CcDiHadron",    "Hlt2CcDiHadron.*Decision"),     
                                  ("CharmHad",      "Hlt2CharmHad.*Decision"),       
                                  ("Commissioning", "Hlt2Commissioning.*Decision"),  
                                  ("DPS",           "Hlt2DPS.*Decision"),            
                                  ("DiMuon",        "Hlt2DiMuon.*Decision"),         
                                  ("DisplVertices", "Hlt2DisplVertices.*Decision"),  
                                  ("EW",            "Hlt2EW.*Decision"),             
                                  ("LowMult",       "Hlt2LowMult.*Decision"),        
                                  ("PID",           "Hlt2PID.*Decision"),            
                                  ("Phi",           "Hlt2Phi.*Decision"),            
                                  ("Radiative",     "Hlt2Radiative.*Decision"),      
                                  ("RareCharm",     "Hlt2RareCharm.*Decision"),      
                                  ("RareStrange",   "Hlt2RareStrange.*Decision"),    
                                  ("RecoTest",      "Hlt2RecoTest.*Decision"),       
                                  ("SingleMuon",    "Hlt2SingleMuon.*Decision"),     
                                  ("Topo",          "Hlt2Topo.*Decision"),           
                                  ("TrackEff",      "Hlt2TrackEff.*Decision"),       
                                  ("TrackEffDiMuon","Hlt2TrackEffDiMuon.*Decision"), 
                                  ("TriMuon",       "Hlt2TriMuon.*Decision"),        
                                  ("XcMuXForTau",   "Hlt2XcMuXForTau.*Decision"),
                                  ("Global",        ".*Global.*"),
                                  ("Other",         ".*") # add a 'catch all' term to pick up all remaining decisions...
                                 ]
                                 )

        from Configurables import HltMassMonitor
        massMon = HltMassMonitor("Hlt2MassMonitor")
        from DAQSys.Decoders import DecoderDB
        massMon.DecReportsLocation = DecoderDB["HltDecReportsDecoder/Hlt2DecReportsDecoder"].listOutputs()[0]
        massMon.SelReportsLocation = DecoderDB["HltSelReportsDecoder/Hlt2SelReportsDecoder"].listOutputs()[0]
        massMon.Decisions  = {"Jpsi"                : "Hlt1DiMuonJPsiDecision",
                              "JpsiRefit"           : "Hlt2JPsiReFitPVsTurboDecision",
                              "Psi2S"               : "Hlt2DiMuonPsi2STurboDecision",
                              "D+->Kpipi"           : "Hlt2CharmHadDpToKmPipPipTurboDecision",
                              "Ds+->KKpi"           : "Hlt2CharmHadDspToKmKpPipTurboDecision",
                              "Lambdac->pKpi"       : "Hlt2CharmHadLcpToPpKmPipTurboDecision",
                              "Omega->Lambda(LL)K"  : "Hlt2CharmHadOmm2LamKm_LLLDecision",
                              "Omega->Lambda(DD)K"  : "Hlt2CharmHadOmm2LamKm_DDLDecision",
                              "Xi->Lambda(LL)pi"    : "Hlt2CharmHadXim2LambPim_LLLDecision",
                              "Xi->Lambda(DD)pi"    : "Hlt2CharmHadXim2LambPim_DDLDecision",
                              "D*->(D0->KK)pi"      : "Hlt2CharmHadDstp2D0Pip_D02KmKpTurboDecision",
                              "D*->(D0->Kpi)pi"     : "Hlt2CharmHadDstp2D0Pip_D02KmPipTurboDecision",
                              "D*->(D0->Kpipipi)pi" : "Hlt2CharmHadDstp2D0Pip_D02KmPimPipPipTurboDecision",
                              "Xc->(D0->Kpi)pi"     : "Hlt2CharmHadSpec_D0ToKPi_PiTurboDecision",
                              "Xc->(D+->Kpipi)pi"   : "Hlt2CharmHadSpec_DpPiTurboDecision",
                              "D0->Kpi"             : "Hlt2RareCharmD02KPiDecision",
                              "phi->KK"             : "Hlt2IncPhiDecision"
                              }

        massMon.Histograms = {"Jpsi"                : [3005, 3186, 50],
                              "JpsiRefit"           : [3005, 3186, 50],
                              "Psi2S"               : [3600, 3770, 50],
                              "D+->Kpipi"           : [100,1820.,1920.],
                              "Ds+->KKpi"           : [100, 1920., 2020.],
                              "Lambdac->pKpi"       : [100, 2235., 2335.],
                              "Omega->Lambda(LL)K"  : [65,1640.,1705.],
                              "Omega->Lambda(DD)K"  : [65,1640.,1705.],
                              "Xi->Lambda(LL)pi"    : [65,1290.,1355.],
                              "Xi->Lambda(DD)pi"    : [65,1290.,1355.],
                              "D*->(D0->KK)pi"      : [100.,1990,2040.],
                              "D*->(D0->Kpi)pi"     : [100,1990.,2040.],
                              "D*->(D0->Kpipipi)pi" : [100,1990.,2040.],
                              "Xc->(D0->Kpi)pi"     : [200,1975.,2975.],
                              "Xc->(D+->Kpipi)pi"   : [200,1995.,2995.],
                              "D0->Kpi"             : [100,1815.,1915.],
                              "phi->KK"             : [80,1000.,1040.]
                              }
                              

        monSeq = Sequence("Hlt2MonitorSequence", IgnoreFilterPassed = True,
                          Members = l0Mon + [globalMon, massMon])
        return monSeq

    def __configureOutput(self):
        histoFile = self.getProp("OutputFile")
        if not histoFile:
            return

        from Configurables import HistogramPersistencySvc, RootHistCnv__PersSvc
        HistogramPersistencySvc().OutputFile = histoFile
        RootHistCnv__PersSvc().OutputFile = histoFile

    def configureHltMonitoring(self, lines1, lines2):
        """
        HLT Monitoring configuration
        """
        ## Only do things here that need to know the list of instantiated HLT
        ## lines. This function is called from the postConfigAction of
        ## HltConf.Configuration.

        monSeq = Sequence("HltMonitorSequence", IgnoreFilterPassed = True)
        if lines1:
            hlt1Mon = self.__hlt1_monitoring(lines1)
            monSeq.Members += [hlt1Mon]
        if lines2:
            hlt2Mon = self.__hlt2_monitoring(lines2)
            monSeq.Members += [hlt2Mon]

        ## Tell the monitoring what it should expect..
        # the keys are the Labels for the Histograms in the GUI
        # the values are the Pattern Rules to for the Decisions contributing
        from HltLine.HltLine     import hlt1Lines, hlt2Lines
        if   self.getProp('HistogrammingLevel') == 'None' :
            for i in hlt1Lines()+hlt2Lines() : _disableHistograms( i.configurable() )
        elif self.getProp('HistogrammingLevel') == 'Line' :
            for i in hlt1Lines()+hlt2Lines() : _disableHistograms( i.configurable(), lambda x: x.getType()!='Hlt::Line' )
        elif self.getProp('HistogrammingLevel') == 'NotLine' :
            for i in hlt1Lines()+hlt2Lines() : _disableHistograms( i.configurable(), lambda x: x.getType()=='Hlt::Line' )
        if self.getProp('EnableMonitoring') :
            for i in hlt1Lines()+hlt2Lines() : _recurse( i.configurable(),_enableMonitoring )

    def __apply_configuration__(self):
        ## Only do things here that do not need to know the list of instantiated
        ## HLT lines.
        if not self.getProp("MonitorSequence"):
            raise RuntimeException("HltMonitoringConf: MonitorSequence property must be set.")
        self.__configureOutput()
