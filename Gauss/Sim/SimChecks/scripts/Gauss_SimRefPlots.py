#################################################################################
#  Script to monitor the output of different Gauss versions. It takes in input 
#  the Gauss root and log files of the two samples to be compared 
#  Usage:
#  python Gauss_SimRefPlots.py --help
#  Example: 
#  python Gauss_SimRefPlots.py -v -r GaussHistos_REF_30000000.root  -l  Gauss_REF.log -s GaussHistos_30000000.root -f Gauss.log 
#################################################################################

from ROOT import TFile, TCanvas, TH1D, TH2D
from ROOT import gDirectory, gPad, gStyle
from optparse import OptionParser
import re, sys, os, shutil

#################################################################################
def grepPattern(P,L):
  result = None
  resultobject = re.search( P , L )
  if ( resultobject != None ):
    result = resultobject.group(1)
  return result

#################################################################################
class SimulationLogFile:
  def __init__(self,N):
    self.fileName = N
    self.GaussVersion = None
    self.PythiaVersion = None
    self.GeantVersion = None
    self.DDDBVersion = None
    self.SIMCONDVersion = None
    self.EventType = None
# VELO 
    self.VeloMCHitsEvent = None
    self.VeloPileUpMCHits = None
# Muon
    self.MuonMCHitsR1M1 = None
    self.MuonMCHitsR1M2 = None
    self.MuonMCHitsR1M3 = None
    self.MuonMCHitsR1M4 = None
    self.MuonMCHitsR1M5 = None
    self.MuonMCHitsR2M1 = None
    self.MuonMCHitsR2M2 = None
    self.MuonMCHitsR2M3 = None
    self.MuonMCHitsR2M4 = None
    self.MuonMCHitsR2M5 = None
    self.MuonMCHitsR3M1 = None
    self.MuonMCHitsR3M2 = None
    self.MuonMCHitsR3M3 = None
    self.MuonMCHitsR3M4 = None
    self.MuonMCHitsR3M5 = None
    self.MuonMCHitsR4M1 = None
    self.MuonMCHitsR4M2 = None
    self.MuonMCHitsR4M3 = None
    self.MuonMCHitsR4M4 = None
    self.MuonMCHitsR4M5 = None
    self.MuonMCHitsAllRM1 = None
    self.MuonMCHitsAllRM2 = None
    self.MuonMCHitsAllRM3 = None
    self.MuonMCHitsAllRM4 = None
    self.MuonMCHitsAllRM5 = None
# RICH 
    self.RichInvalidFlag = None
    self.RichMCHitsR1 = None
    self.RichInvalidRadiatorHitsR1 = None
    self.RichGasQuartzCKHitsR1 = None
    self.RichHPDQuartzCKHitsR1 = None
    self.RichNitrogenCKHitsR1 = None
    self.RichMCHitsR2 = None
    self.RichInvalidRadiatorHitsR2 = None
    self.RichGasQuartzCKHitsR2 = None
    self.RichHPDQuartzCKHitsR2 = None
    self.RichNitrogenCKHitsR2 = None
    self.RichSignalCKMCHitsAero = None
    self.RichScatteredHitsAero = None
    self.RichParticleLessMCHitsAero = None
    self.RichSignalCKMCHitsC4F10 = None
    self.RichScatteredHitsC4F10 = None
    self.RichParticleLessMCHitsC4F10 = None
    self.RichSignalCKMCHitsCF4 = None
    self.RichScatteredHitsCF4 = None
    self.RichParticleLessMCHitsCF4 = None
# General hit info
    self.MCParticlesMean = None
    self.MCParticlesRMS = None
    self.MCHitsTTMean = None
    self.MCHitsTTRMS = None
    self.MCHitsITMean = None
    self.MCHitsITRMS = None
    self.MCHitsOTMean = None
    self.MCHitsOTRMS = None
    self.MCHitsSPDMean = None
    self.MCHitsSPDRMS = None
    self.MCHitsPRSMean = None
    self.MCHitsPRSRMS = None
    self.MCHitsECALMean = None
    self.MCHitsECALRMS = None
    self.MCHitsHCALMean = None
    self.MCHitsHCALRMS = None
# ST
    self.ITHitsPerEvent = None
    self.ITMeanBetaGamma = None
    self.ITMPVDepCharge = None
    self.ITHalfSaWidth = None
    self.TTHitsPerEvent = None
    self.TTMeanBetaGamma = None
    self.TTMPVDepCharge = None
    self.TTHalfSaWidth = None
    self.OTHitsPerEvent = None
    self.OTMeanBetaGamma = None
    self.OTMPVDepCharge = None
    self.OTHalfSaWidth = None
# Processing Time
    self.TotEntries = None
    self.TimeSpeedFactor = None
    self.TimeEventLoop = None
    self.TimeEventLoopTOT = None
    self.TimeGauss = None
    self.TimeGaussTOT = None
    self.TimeGenerator = None
    self.TimeGeneratorTOT = None
    self.TimeSimulation = None
    self.TimeSimulationTOT = None
    self.TimeGaussSim = None
    self.TimeGaussSimTOT = None
    self.TimeGigaStore = None
    self.TimeGigaStoreTOT = None
    self.TimeGeo = None
    self.TimeGeoTOT = None
    self.TimeGenerationToSimulation = None
    self.TimeGenerationToSimulationTOT = None
    self.TimeGiGaFlushAlgorithm = None
    self.TimeGiGaFlushAlgorithmTOT = None
    self.TimeSimulationToMCTruth = None
    self.TimeSimulationToMCTruthTOT = None
    self.TimeDetectorsHits = None
    self.TimeDetectorsHitsTOT = None
    self.TimeSimMonitor = None
    self.TimeSimMonitorTOT = None
# Errors
    self.ExcepCount = 0
    self.ErrCount = 0
    self.WarCount = 0
    self.G4Count = 0


  def computeQuantities(self):
    outExceptions = file("Exceptions.log", 'w')
    outErrors = file("Errors.log", 'w')
    outWarnings = file("Warnings.log", 'w')
    outG4Exceptions = file("G4Exceptions.log", 'w')

    f = open(self.fileName)
    for line in f:
      if ( self.EventType == None ):
        self.EventType = grepPattern('Requested to generate EventType (\S+)',line)
      if ( self.GaussVersion == None ):
        self.GaussVersion = grepPattern( 'Welcome to Gauss version (\S+)' , line )
      if ( self.PythiaVersion == None ):
        self.PythiaVersion = grepPattern( 'This is PYTHIA version (\S+)' , line )
      if ( self.GeantVersion == None ):
        self.GeantVersion = grepPattern( 'Geant4 version Name: (\S+)' , line )
      if ( self.DDDBVersion == None ):
        self.DDDBVersion = grepPattern( 'DDDB   * INFO Using TAG *(\S+)' , line )
      if ( self.SIMCONDVersion == None ):
        self.SIMCONDVersion = grepPattern( 'SIMCOND   * INFO Using TAG *(\S+)' , line )
# VELO
      if ( self.VeloMCHitsEvent == None ):
        self.VeloMCHitsEvent = grepPattern( 'VeloGaussMoni *INFO *\|* Number of MCHits/Event: *(\S+)' , line )
      if ( self.VeloPileUpMCHits == None ):
        self.VeloPileUpMCHits = grepPattern( 'VeloGaussMoni *INFO *\|* Number of PileUpMCHits/Event: *(\S+)' , line )
# MUON
      if ( self.MuonMCHitsR1M1 == None ):
        self.MuonMCHitsR1M1 = grepPattern( 'MuonHitChecker * INFO *(\S+) .* *R1' , line )
      if ( self.MuonMCHitsR1M2 == None ):
        self.MuonMCHitsR1M2 = grepPattern( 'MuonHitChecker * INFO *\S+ *(\S+) .* *R1' , line )
      if ( self.MuonMCHitsR1M3 == None ):
        self.MuonMCHitsR1M3 = grepPattern( 'MuonHitChecker * INFO *\S+ *\S+ *(\S+) .* *R1' , line )
      if ( self.MuonMCHitsR1M4 == None ):
        self.MuonMCHitsR1M4 = grepPattern( 'MuonHitChecker * INFO *\S+ *\S+ *\S+ *(\S+) .* *R1' , line )
      if ( self.MuonMCHitsR1M5 == None ):
        self.MuonMCHitsR1M5 = grepPattern( 'MuonHitChecker * INFO *\S+ *\S+ *\S+ *\S+ *(\S+) .* *R1' , line )

      if ( self.MuonMCHitsR2M1 == None ):
        self.MuonMCHitsR2M1 = grepPattern( 'MuonHitChecker * INFO *(\S+) .* *R2' , line )
      if ( self.MuonMCHitsR2M2 == None ):
        self.MuonMCHitsR2M2 = grepPattern( 'MuonHitChecker * INFO *\S+ *(\S+)  .* *R2' , line )
      if ( self.MuonMCHitsR2M3 == None ):
        self.MuonMCHitsR2M3 = grepPattern( 'MuonHitChecker * INFO *\S+ *\S+ *(\S+)  .* *R2' , line )
      if ( self.MuonMCHitsR2M4 == None ):
        self.MuonMCHitsR2M4 = grepPattern( 'MuonHitChecker * INFO *\S+ *\S+ *\S+ *(\S+)  .* *R2' , line )
      if ( self.MuonMCHitsR2M5 == None ):
        self.MuonMCHitsR2M5 = grepPattern( 'MuonHitChecker * INFO *\S+ *\S+ *\S+ *\S+ *(\S+)  .* *R2' , line )

      if ( self.MuonMCHitsR3M1 == None ):
        self.MuonMCHitsR3M1 = grepPattern( 'MuonHitChecker * INFO *(\S+) .* *R3' , line )
      if ( self.MuonMCHitsR3M2 == None ):
        self.MuonMCHitsR3M2 = grepPattern( 'MuonHitChecker * INFO *\S+ *(\S+)  .* *R3' , line )
      if ( self.MuonMCHitsR3M3 == None ):
        self.MuonMCHitsR3M3 = grepPattern( 'MuonHitChecker * INFO *\S+ *\S+ *(\S+)  .* *R3' , line )
      if ( self.MuonMCHitsR3M4 == None ):
        self.MuonMCHitsR3M4 = grepPattern( 'MuonHitChecker * INFO *\S+ *\S+ *\S+ *(\S+)  .* *R3' , line )
      if ( self.MuonMCHitsR3M5 == None ):
        self.MuonMCHitsR3M5 = grepPattern( 'MuonHitChecker * INFO *\S+ *\S+ *\S+ *\S+ *(\S+)  .* *R3' , line )

      if ( self.MuonMCHitsR4M1 == None ):
        self.MuonMCHitsR4M1 = grepPattern( 'MuonHitChecker * INFO *(\S+) .* *R4' , line )
      if ( self.MuonMCHitsR4M2 == None ):
        self.MuonMCHitsR4M2 = grepPattern( 'MuonHitChecker * INFO *\S+ *(\S+)  .* *R4' , line )
      if ( self.MuonMCHitsR4M3 == None ):
        self.MuonMCHitsR4M3 = grepPattern( 'MuonHitChecker * INFO *\S+ *\S+ *(\S+)  .* *R4' , line )
      if ( self.MuonMCHitsR4M4 == None ):
        self.MuonMCHitsR4M4 = grepPattern( 'MuonHitChecker * INFO *\S+ *\S+ *\S+ *(\S+)  .* *R4' , line )
      if ( self.MuonMCHitsR4M5 == None ):
        self.MuonMCHitsR4M5 = grepPattern( 'MuonHitChecker * INFO *\S+ *\S+ *\S+ *\S+ *(\S+)  .* *R4' , line )

      if ( self.MuonMCHitsAllRM1 == None ):
        self.MuonMCHitsAllRM1 = grepPattern( 'MuonHitChecker * INFO *(\S+) .* *allR' , line )
      if ( self.MuonMCHitsAllRM2 == None ):
        self.MuonMCHitsAllRM2 = grepPattern( 'MuonHitChecker * INFO *\S+ *(\S+)  .* *allR' , line )
      if ( self.MuonMCHitsAllRM3 == None ):
        self.MuonMCHitsAllRM3 = grepPattern( 'MuonHitChecker * INFO *\S+ *\S+ *(\S+)  .* *allR' , line )
      if ( self.MuonMCHitsAllRM4 == None ):
        self.MuonMCHitsAllRM4 = grepPattern( 'MuonHitChecker * INFO *\S+ *\S+ *\S+ *(\S+)  .* *allR' , line )
      if ( self.MuonMCHitsAllRM5 == None ):
        self.MuonMCHitsAllRM5 = grepPattern( 'MuonHitChecker * INFO *\S+ *\S+ *\S+ *\S+ *(\S+)  .* *allR' , line )
# RICH
      if ( self.RichInvalidFlag == None ):
        self.RichInvalidFlag = grepPattern( 'Invalid RICH flags * = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichMCHitsR1 == None ):
        self.RichMCHitsR1 = grepPattern( 'MCRichHits * : Rich1 = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichInvalidRadiatorHitsR1 == None ):
        self.RichInvalidRadiatorHitsR1 = grepPattern( 'Invalid radiator hits * : Rich1 = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichGasQuartzCKHitsR1 == None ):
        self.RichGasQuartzCKHitsR1 = grepPattern( 'Gas Quartz CK hits *: Rich1 = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichHPDQuartzCKHitsR1 == None ):
        self.RichHPDQuartzCKHitsR1 = grepPattern( 'HPD Quartz CK hits *: Rich1 = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichNitrogenCKHitsR1 == None ):
        self.RichNitrogenCKHitsR1 = grepPattern( 'Nitrogen CK hits *: Rich1 = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichMCHitsR2 == None ):
        self.RichMCHitsR2 = grepPattern( 'MCRichHits *: Rich1 = .* Rich2 = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichInvalidRadiatorHitsR2 == None ):
        self.RichInvalidRadiatorHitsR2 = grepPattern( 'Invalid radiator hits *: Rich1 = .* Rich2 = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichGasQuartzCKHitsR2 == None ):
        self.RichGasQuartzCKHitsR2 = grepPattern( 'Gas Quartz CK hits *: Rich1 =  .* Rich2 = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichHPDQuartzCKHitsR2 == None ):
        self.RichHPDQuartzCKHitsR2 = grepPattern( 'HPD Quartz CK hits *: Rich1 = .* Rich2 = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichNitrogenCKHitsR2 == None ):
        self.RichNitrogenCKHitsR2 = grepPattern( 'Nitrogen CK hits *: Rich1 = .* Rich2 = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichSignalCKMCHitsAero == None ):
        self.RichSignalCKMCHitsAero = grepPattern( 'Signal CK MCRichHits *: Aero  = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichScatteredHitsAero == None ):
        self.RichScatteredHitsAero = grepPattern( 'Rayleigh scattered hits *: Aero  = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichParticleLessMCHitsAero == None ):
        self.RichParticleLessMCHitsAero = grepPattern( 'MCParticle-less hits *: Aero  = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichSignalCKMCHitsC4F10 == None ):
        self.RichSignalCKMCHitsC4F10 = grepPattern( 'Signal CK MCRichHits *: Aero  =  .* C4F10 = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichScatteredHitsC4F10 == None ):
        self.RichScatteredHitsC4F10 = grepPattern( 'Rayleigh scattered hits *: Aero  =  .* C4F10 = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichParticleLessMCHitsC4F10 == None ):
        self.RichParticleLessMCHitsC4F10 = grepPattern( 'MCParticle-less hits *: Aero  =  .* C4F10 = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichSignalCKMCHitsCF4 == None ):
        self.RichSignalCKMCHitsCF4 = grepPattern( 'Signal CK MCRichHits *: Aero  = .* CF4 = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichScatteredHitsCF4  == None ):
        self.RichScatteredHitsCF4  = grepPattern( 'Rayleigh scattered hits *: Aero  =  .* CF4 = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
      if ( self.RichParticleLessMCHitsCF4 == None ):
        self.RichParticleLessMCHitsCF4 = grepPattern( 'MCParticle-less hits *: Aero  =  .* CF4 = *(\d+.\d+ *\D+ *\d+.\d+)' , line )
# General Hits info
      if ( self.MCParticlesMean == None ):
        self.MCParticlesMean = grepPattern( 'INFO *\"\#MCParticles\" *\| *\S+ *\| *\S+ *\| *(\S+)' , line )
      if ( self.MCParticlesRMS == None ):
        self.MCParticlesRMS = grepPattern( 'INFO *\"\#MCParticles\"  *\| *\S+ *\| *\S+ *\| *\S+ *\| *(\S+)' , line )
      if ( self.MCHitsTTMean == None ):
        self.MCHitsTTMean = grepPattern( 'INFO *\"\#TT MCHits\" *\| *\S+ *\| *\S+ *\| *(\S+)' , line )
      if ( self.MCHitsTTRMS == None ):
        self.MCHitsTTRMS = grepPattern( 'INFO *\"\#TT MCHits\"  *\| *\S+ *\| *\S+ *\| *\S+ *\| *(\S+)' , line )
      if ( self.MCHitsITMean == None ):
        self.MCHitsITMean = grepPattern( 'INFO *\"\#IT MCHits\" *\| *\S+ *\| *\S+ *\| *(\S+)' , line )
      if ( self.MCHitsITRMS == None ):
        self.MCHitsITRMS = grepPattern( 'INFO *\"\#IT MCHits\"  *\| *\S+ *\| *\S+ *\| *\S+ *\| *(\S+)' , line )
      if ( self.MCHitsOTMean == None ):
        self.MCHitsOTMean = grepPattern( 'INFO *\"\#OT MCHits\" *\| *\S+ *\| *\S+ *\| *(\S+)' , line )
      if ( self.MCHitsOTRMS == None ):
        self.MCHitsOTRMS = grepPattern( 'INFO *\"\#OT MCHits\"  *\| *\S+ *\| *\S+ *\| *\S+ *\| *(\S+)' , line )
      if ( self.MCHitsSPDMean == None ):
        self.MCHitsSPDMean = grepPattern( 'INFO *\"\#Spd MCHits\" *\| *\S+ *\| *\S+ *\| *(\S+)' , line )
      if ( self.MCHitsSPDRMS == None ):
        self.MCHitsSPDRMS = grepPattern( 'INFO *\"\#Spd MCHits\"  *\| *\S+ *\| *\S+ *\| *\S+ *\| *(\S+)' , line )
      if ( self.MCHitsPRSMean == None ):
        self.MCHitsPRSMean = grepPattern( 'INFO *\"\#Prs MCHits\" *\| *\S+ *\| *\S+ *\| *(\S+)' , line )
      if ( self.MCHitsPRSRMS == None ):
        self.MCHitsPRSRMS = grepPattern( 'INFO *\"\#Prs MCHits\"  *\| *\S+ *\| *\S+ *\| *\S+ *\| *(\S+)' , line )
      if ( self.MCHitsECALMean == None ):
        self.MCHitsECALMean = grepPattern( 'INFO *\"\#Ecal MCHits\" *\| *\S+ *\| *\S+ *\| *(\S+)' , line )
      if ( self.MCHitsECALRMS == None ):
        self.MCHitsECALRMS = grepPattern( 'INFO *\"\#Ecal MCHits\"  *\| *\S+ *\| *\S+ *\| *\S+ *\| *(\S+)' , line )
      if ( self.MCHitsHCALMean == None ):
        self.MCHitsHCALMean = grepPattern( 'INFO *\"\#Hcal MCHits\" *\| *\S+ *\| *\S+ *\| *(\S+)' , line )
      if ( self.MCHitsHCALRMS == None ):
        self.MCHitsHCALRMS = grepPattern( 'INFO *\"\#Hcal MCHits\"  *\| *\S+ *\| *\S+ *\| *\S+ *\| *(\S+)' , line )
## 15.01.2009 New ST info added from next release:
      if ( self.ITHitsPerEvent == None ):
        self.ITHitsPerEvent = grepPattern( 'ITHitMonitor * INFO \#hits per event: *(\S+)' , line )
      if ( self.ITMeanBetaGamma == None ):
        self.ITMeanBetaGamma = grepPattern( 'ITHitMonitor * INFO Mean beta \* gamma: *(\S+)' , line )
      if ( self.ITMPVDepCharge == None ):
        self.ITMPVDepCharge = grepPattern( 'ITHitMonitor * INFO Most Probable deposited charge: *(\S+)' , line )
      if ( self.ITHalfSaWidth == None ):
        self.ITHalfSaWidth = grepPattern( 'ITHitMonitor * INFO Half Sample width *(\S+)' , line )
      if ( self.TTHitsPerEvent == None ):
        self.TTHitsPerEvent = grepPattern( 'TTHitMonitor * INFO \#hits per event: *(\S+)' , line )
      if ( self.TTMeanBetaGamma == None ):
        self.TTMeanBetaGamma = grepPattern( 'TTHitMonitor * INFO Mean beta \* gamma: *(\S+)' , line )
      if ( self.TTMPVDepCharge == None ):
        self.TTMPVDepCharge = grepPattern( 'TTHitMonitor * INFO Most Probable deposited charge: *(\S+)' , line )
      if ( self.TTHalfSaWidth == None ):
        self.TTHalfSaWidth = grepPattern( 'TTHitMonitor * INFO Half Sample width *(\S+)' , line )
      if ( self.OTHitsPerEvent == None ):
        self.OTHitsPerEvent = grepPattern( 'OTHitMonitor * INFO \#hits per event: *(\S+)' , line )
      if ( self.OTMeanBetaGamma == None ):
        self.OTMeanBetaGamma = grepPattern( 'OTHitMonitor * INFO Mean beta \* gamma: *(\S+)' , line )
      if ( self.OTMPVDepCharge == None ):
        self.OTMPVDepCharge = grepPattern( 'OTHitMonitor * INFO Most Probable deposited charge: *(\S+)' , line )
      if ( self.OTHalfSaWidth == None ):
        self.OTHalfSaWidth = grepPattern( 'OTHitMonitor * INFO Half Sample width *(\S+)' , line )
# Processing Time
      if ( self.TimeSpeedFactor == None ):
        self.TimeSpeedFactor = grepPattern( 'TimingAuditor.* INFO This machine has a speed about *(\S+)' , line )
      if ( self.TotEntries == None ):
        self.TotEntries = grepPattern( 'TimingAuditor.* INFO EVENT LOOP *\| *\S+ *\| *\S+ *\| *\S+ *\S+ *\| *(\S+)' , line )
      if ( self.TimeEventLoop == None ):
        self.TimeEventLoop = grepPattern( 'TimingAuditor.* INFO EVENT LOOP *\| *\S+ *\| *(\S+)' , line )
      if ( self.TimeEventLoopTOT == None ):
        self.TimeEventLoopTOT = grepPattern( 'TimingAuditor.* INFO EVENT LOOP *\| .*\| *(\S+)' , line )
      if ( self.TimeGauss == None ):
        self.TimeGauss = grepPattern( 'TimingAuditor.* INFO * Gauss *\| *\S+ *\| *(\S+)' , line )
      if ( self.TimeGaussTOT == None ):
        self.TimeGaussTOT = grepPattern( 'TimingAuditor.* INFO * Gauss *\| .*\| *(\S+)' , line )
      if ( self.TimeGenerator == None ):
        self.TimeGenerator = grepPattern( 'TimingAuditor.* INFO * Generator *\| *\S+ *\| *(\S+)' , line )
      if ( self.TimeGeneratorTOT == None ):
        self.TimeGeneratorTOT = grepPattern( 'TimingAuditor.* INFO * Generator *\| .*\| *(\S+)' , line )
      if ( self.TimeSimulation == None ):
        self.TimeSimulation = grepPattern( 'TimingAuditor.* INFO * Simulation *\| *\S+ *\| *(\S+)' , line )
      if ( self.TimeSimulationTOT == None ):
        self.TimeSimulationTOT = grepPattern( 'TimingAuditor.* INFO * Simulation *\| .*\| *(\S+)' , line )
      if ( self.TimeGaussSim == None ):
        self.TimeGaussSim = grepPattern( 'TimingAuditor.* INFO * GaussSim *\| *\S+ *\| *(\S+)' , line )
      if ( self.TimeGaussSimTOT == None ):
        self.TimeGaussSimTOT = grepPattern( 'TimingAuditor.* INFO * GaussSim *\| .*\| *(\S+)' , line )
      if ( self.TimeGigaStore == None ):
        self.TimeGigaStore = grepPattern( 'TimingAuditor.* INFO * GiGaStore *\| *\S+ *\| *(\S+)' , line )
      if ( self.TimeGigaStoreTOT == None ):
        self.TimeGigaStoreTOT = grepPattern( 'TimingAuditor.* INFO * GiGaStore *\| .*\| *(\S+)' , line )
      if ( self.TimeGeo == None ):
        self.TimeGeo = grepPattern( 'TimingAuditor.* INFO * Geo *\| *\S+ *\| *(\S+)' , line )
      if ( self.TimeGeoTOT == None ):
        self.TimeGeoTOT = grepPattern( 'TimingAuditor.* INFO * Geo *\| .*\| *(\S+)' , line )
      if ( self.TimeGenerationToSimulation == None ):
        self.TimeGenerationToSimulation = grepPattern( 'TimingAuditor.* INFO * GenerationToSimulation *\| *\S+ *\| *(\S+)' , line )
      if ( self.TimeGenerationToSimulationTOT == None ):
        self.TimeGenerationToSimulationTOT = grepPattern( 'TimingAuditor.* INFO * GenerationToSimulation *\| .*\| *(\S+)' , line )
      if ( self.TimeGiGaFlushAlgorithm == None ):
        self.TimeGiGaFlushAlgorithm = grepPattern( 'TimingAuditor.* INFO * GiGaFlushAlgorithm *\| *\S+ *\| *(\S+)' , line )
      if ( self.TimeGiGaFlushAlgorithmTOT == None ):
        self.TimeGiGaFlushAlgorithmTOT = grepPattern( 'TimingAuditor.*INFO * GiGaFlushAlgorithm *\| .*\| *(\S+)' , line )
      if ( self.TimeSimulationToMCTruth == None ):
        self.TimeSimulationToMCTruth = grepPattern( 'TimingAuditor.* INFO * SimulationToMCTruth *\| *\S+ *\| *(\S+)' , line )
      if ( self.TimeSimulationToMCTruthTOT == None ):
        self.TimeSimulationToMCTruthTOT = grepPattern( 'TimingAuditor.* INFO * SimulationToMCTruth *\| .*\| *(\S+)' , line )
      if ( self.TimeDetectorsHits == None ):
        self.TimeDetectorsHits = grepPattern( 'TimingAuditor.* INFO * DetectorsHits *\| *\S+ *\| *(\S+)' , line )
      if ( self.TimeDetectorsHitsTOT == None ):
        self.TimeDetectorsHitsTOT = grepPattern( 'TimingAuditor.* INFO * DetectorsHits *\| .*\| *(\S+)' , line )
      if ( self.TimeSimMonitor  == None ):
        self.TimeSimMonitor  = grepPattern( 'TimingAuditor.* INFO * SimMonitor *\| *\S+ *\| *(\S+)' , line )
      if ( self.TimeSimMonitorTOT == None ):
        self.TimeSimMonitorTOT = grepPattern( 'TimingAuditor.* INFO * SimMonitor *\| .*\| *(\S+)' , line )
# Exceptions/Errors/Warnings: write error output in Log file
      if re.search("EXCEPTION*", line.upper()):
        print >> outExceptions, line  
        self.ExcepCount +=1
      if re.search("ERROR*", line.upper()):
        print >> outErrors, line  
        self.ErrCount +=1
      if re.search("WARNING*", line.upper()):
        print >> outWarnings, line
        self.WarCount +=1
      if re.search("G4EXCEPTION*", line.upper()):
        print >> outG4Exceptions, line  
        self.G4Count +=1
    
    f.close()
    outExceptions.close()
    outErrors.close()
    outWarnings.close()
    outG4Exceptions.close()

    
  def eventType(self):
    return self.EventType
  def gaussVersion(self):
    return self.GaussVersion
  def pythiaVersion(self):
    return self.PythiaVersion
  def geantVersion(self):
    return self.GeantVersion
  def dddbVersion(self):
    return self.DDDBVersion
  def simcondVersion(self):
    return self.SIMCONDVersion
# VELO
  def veloMCHitsEvent(self):
    return self.VeloMCHitsEvent
  def veloPileUpMCHits(self):
    return self.VeloPileUpMCHits
# MUON
  def muonMCHitsR1M1(self):
    return self.MuonMCHitsR1M1
  def muonMCHitsR1M2(self):
    return self.MuonMCHitsR1M2
  def muonMCHitsR1M3(self):
    return self.MuonMCHitsR1M3
  def muonMCHitsR1M4(self):
    return self.MuonMCHitsR1M4
  def muonMCHitsR1M5(self):
    return self.MuonMCHitsR1M5

  def muonMCHitsR2M1(self):
    return self.MuonMCHitsR2M1
  def muonMCHitsR2M2(self):
    return self.MuonMCHitsR2M2
  def muonMCHitsR2M3(self):
    return self.MuonMCHitsR2M3
  def muonMCHitsR2M4(self):
    return self.MuonMCHitsR2M4
  def muonMCHitsR2M5(self):
    return self.MuonMCHitsR2M5

  def muonMCHitsR3M1(self):
    return self.MuonMCHitsR3M1
  def muonMCHitsR3M2(self):
    return self.MuonMCHitsR3M2
  def muonMCHitsR3M3(self):
    return self.MuonMCHitsR3M3
  def muonMCHitsR3M4(self):
    return self.MuonMCHitsR3M4
  def muonMCHitsR3M5(self):
    return self.MuonMCHitsR3M5

  def muonMCHitsR4M1(self):
    return self.MuonMCHitsR4M1
  def muonMCHitsR4M2(self):
    return self.MuonMCHitsR4M2
  def muonMCHitsR4M3(self):
    return self.MuonMCHitsR4M3
  def muonMCHitsR4M4(self):
    return self.MuonMCHitsR4M4
  def muonMCHitsR4M5(self):
    return self.MuonMCHitsR4M5

  def muonMCHitsAllRM1(self):
    return self.MuonMCHitsAllRM1
  def muonMCHitsAllRM2(self):
    return self.MuonMCHitsAllRM2
  def muonMCHitsAllRM3(self):
    return self.MuonMCHitsAllRM3
  def muonMCHitsAllRM4(self):
    return self.MuonMCHitsAllRM4
  def muonMCHitsAllRM5(self):
    return self.MuonMCHitsAllRM5
# RICH  
  def richInvalidFlag(self):
    return self.RichInvalidFlag
  def richMCHitsR1(self):
    return self.RichMCHitsR1
  def richInvalidRadiatorHitsR1(self):
    return self.RichInvalidRadiatorHitsR1
  def richGasQuartzCKHitsR1(self):
    return self.RichGasQuartzCKHitsR1
  def richHPDQuartzCKHitsR1(self):
    return self.RichHPDQuartzCKHitsR1
  def richNitrogenCKHitsR1(self):
    return self.RichNitrogenCKHitsR1
  def richMCHitsR2(self):
    return self.RichMCHitsR2
  def richInvalidRadiatorHitsR2(self):
    return self.RichInvalidRadiatorHitsR2
  def richGasQuartzCKHitsR2(self):
    return self.RichGasQuartzCKHitsR2
  def richHPDQuartzCKHitsR2(self):
    return self.RichHPDQuartzCKHitsR2
  def richNitrogenCKHitsR2(self):
    return self.RichNitrogenCKHitsR2
  def richSignalCKMCHitsAero(self):
    return self.RichSignalCKMCHitsAero
  def richScatteredHitsAero(self):
    return self.RichScatteredHitsAero
  def richParticleLessMCHitsAero(self):
    return self.RichParticleLessMCHitsAero
  def richSignalCKMCHitsC4F10(self):
    return self.RichSignalCKMCHitsC4F10
  def richScatteredHitsC4F10(self):
    return self.RichScatteredHitsC4F10
  def richParticleLessMCHitsC4F10(self):
    return self.RichParticleLessMCHitsC4F10
  def richSignalCKMCHitsCF4(self):
    return self.RichSignalCKMCHitsCF4
  def richScatteredHitsCF4(self):
    return self.RichScatteredHitsCF4
  def richParticleLessMCHitsCF4(self):
    return self.RichParticleLessMCHitsCF4
# General Hits info
  def mcParticlesMean(self):
    return self.MCParticlesMean
  def mcParticlesRMS(self):
    return self.MCParticlesRMS
  def mcHitsTTMean(self):
    return self.MCHitsTTMean
  def mcHitsTTRMS(self):
    return self.MCHitsTTRMS
  def mcHitsITMean(self):
    return self.MCHitsITMean
  def mcHitsITRMS(self):
    return self.MCHitsITRMS
  def mcHitsOTMean(self):
    return self.MCHitsOTMean
  def mcHitsOTRMS(self):
    return self.MCHitsOTRMS
  def mcHitsSPDMean(self):
    return self.MCHitsSPDMean
  def mcHitsSPDRMS(self):
    return self.MCHitsSPDRMS
  def mcHitsPRSMean(self):
    return self.MCHitsPRSMean
  def mcHitsPRSRMS(self):
    return self.MCHitsPRSRMS
  def mcHitsECALMean(self):
    return self.MCHitsECALMean
  def mcHitsECALRMS(self):
    return self.MCHitsECALRMS
  def mcHitsHCALMean(self):
    return self.MCHitsHCALMean
  def mcHitsHCALRMS(self):
    return self.MCHitsHCALRMS
# ST
  def itHitsPerEvent(self):
    return self.ITHitsPerEvent
  def itMeanBetaGamma(self):
    return self.ITMeanBetaGamma
  def itMPVDepCharge(self):
    return self.ITMPVDepCharge
  def itHalfSaWidth(self):
    return self.ITHalfSaWidth
  def ttHitsPerEvent(self):
    return self.TTHitsPerEvent
  def ttMeanBetaGamma(self):
    return self.TTMeanBetaGamma
  def ttMPVDepCharge(self):
    return self.TTMPVDepCharge
  def ttHalfSaWidth(self):
    return self.TTHalfSaWidth
  def otHitsPerEvent(self):
    return self.OTHitsPerEvent
  def otMeanBetaGamma(self):
    return self.OTMeanBetaGamma
  def otMPVDepCharge(self):
    return self.OTMPVDepCharge
  def otHalfSaWidth(self):
    return self.OTHalfSaWidth
# Processing Time
  def timeSpeedFactor(self):
    return self.TimeSpeedFactor
  def totEntries(self):
    return self.TotEntries
  def timeEventLoop(self):
    if (self.TimeEventLoop==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeEventLoop)*float(self.TimeSpeedFactor))
  def timeEventLoopTOT(self):
    if (self.TimeEventLoopTOT==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeEventLoopTOT)*float(self.TimeSpeedFactor))
  def timeGauss(self):
    if (self.TimeGauss==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeGauss)*float(self.TimeSpeedFactor))
  def timeGaussTOT(self):
    if (self.TimeGaussTOT==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeGaussTOT)*float(self.TimeSpeedFactor))
  def timeGenerator(self):
    if (self.TimeGenerator==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeGenerator)*float(self.TimeSpeedFactor))
  def timeGeneratorTOT(self):
    if (self.TimeGeneratorTOT==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeGeneratorTOT)*float(self.TimeSpeedFactor))
  def timeSimulation(self):
    if (self.TimeSimulation==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeSimulation)*float(self.TimeSpeedFactor))
  def timeSimulationTOT(self):
    if (self.TimeSimulationTOT==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeSimulationTOT)*float(self.TimeSpeedFactor))
  def timeGaussSim(self):
    if (self.TimeGaussSim==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeGaussSim)*float(self.TimeSpeedFactor))
  def timeGaussSimTOT(self):
    if (self.TimeGaussSimTOT==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeGaussSimTOT)*float(self.TimeSpeedFactor))
  def timeGigaStore(self):
    if (self.TimeGigaStore==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeGigaStore)*float(self.TimeSpeedFactor))
  def timeGigaStoreTOT(self):
    if (self.TimeGigaStoreTOT==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeGigaStoreTOT)*float(self.TimeSpeedFactor))
  def timeGeo(self):
    if (self.TimeGeo==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeGeo)*float(self.TimeSpeedFactor))
  def timeGeoTOT(self):
    if (self.TimeGeoTOT==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeGeoTOT)*float(self.TimeSpeedFactor))
  def timeGenerationToSimulation(self):
    if (self.TimeGenerationToSimulation==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeGenerationToSimulation)*float(self.TimeSpeedFactor))
  def timeGenerationToSimulationTOT(self):
    if (self.TimeGenerationToSimulationTOT==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeGenerationToSimulationTOT)*float(self.TimeSpeedFactor))
  def timeGiGaFlushAlgorithm(self):
    if (self.TimeGiGaFlushAlgorithm==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeGiGaFlushAlgorithm)*float(self.TimeSpeedFactor))
  def timeGiGaFlushAlgorithmTOT(self):
    if (self.TimeGiGaFlushAlgorithmTOT==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeGiGaFlushAlgorithmTOT)*float(self.TimeSpeedFactor))
  def timeSimulationToMCTruth(self):
    if (self.TimeSimulationToMCTruth==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeSimulationToMCTruth)*float(self.TimeSpeedFactor))
  def timeSimulationToMCTruthTOT(self):
    if (self.TimeSimulationToMCTruthTOT==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeSimulationToMCTruthTOT)*float(self.TimeSpeedFactor))
  def timeDetectorsHits(self):
    if (self.TimeDetectorsHits==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeDetectorsHits)*float(self.TimeSpeedFactor))
  def timeDetectorsHitsTOT(self):
    if (self.TimeDetectorsHitsTOT==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeDetectorsHitsTOT)*float(self.TimeSpeedFactor))
  def timeSimMonitor(self):
    if (self.TimeSimMonitor==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeSimMonitor)*float(self.TimeSpeedFactor))
  def timeSimMonitorTOT(self):
    if (self.TimeSimMonitorTOT==None or self.TimeSpeedFactor==None):
      return 0
    return float(float(self.TimeSimMonitorTOT)*float(self.TimeSpeedFactor))
# Excptions/Errors/Warnings
  def excepCount(self):
    return self.ExcepCount
  def errCount(self):
    return self.ErrCount
  def warCount(self):
    return self.WarCount
  def g4Count(self):
    return self.G4Count


     
#################################################################################  
class SimulationHisto:
  def __init__(self,c,RH,OH,XT,YT,HT,FN,LS):
    self.canvas = c
    self.referenceHisto = RH
    self.compHisto = OH
    self.XTitle = XT 
    self.YTitle = YT
    self.HistTitle = HT
    self.FileName = FN
    self.LogScale = LS
    
  def title(self):
    return self.HistTitle
  def refFileName(self):
    return "Reference"+self.FileName+".eps"
  def fileName(self):
    return self.FileName+".eps"
  def compFileName(self):
    return "Comp"+self.FileName+".gif"    
    
  def plot(self):
    ERef = self.referenceHisto.GetEntries()
    EComp = self.compHisto.GetEntries()

    if(ERef==0 or EComp==0):
      return 0

    self.referenceHisto.GetXaxis().SetTitle( self.XTitle) 
    self.referenceHisto.GetXaxis().SetLabelSize( 0.025 ) 
    self.referenceHisto.GetYaxis().SetTitle( self.YTitle ) 
    self.referenceHisto.GetYaxis().SetLabelSize( 0.025 ) 
    self.referenceHisto.SetMarkerColor(2)
    self.referenceHisto.SetLineWidth(2)
    self.referenceHisto.SetLineColor(2)
    self.referenceHisto.SetTitle(self.HistTitle)
    if self.LogScale:
      gPad.SetLogy()
    else:
      gPad.SetLogy(0)

    gStyle.SetStatTextColor(2)
    gStyle.SetStatY(0.8)
    self.referenceHisto.DrawNormalized('histo') 
    self.canvas.Update()
#    self.canvas.Print(self.refFileName())  

    self.compHisto.GetXaxis().SetTitle( self.XTitle) 
    self.compHisto.GetXaxis().SetLabelSize( 0.025 ) 
    self.compHisto.GetYaxis().SetTitle( self.YTitle ) 
    self.compHisto.GetYaxis().SetLabelSize( 0.025 ) 
    self.compHisto.SetMarkerColor(4)
    self.compHisto.SetLineWidth(2)
    self.compHisto.SetLineColor(4)
    self.compHisto.SetTitle(self.HistTitle)
    if self.LogScale:
      gPad.SetLogy()
    else:
      gPad.SetLogy(0)
    gStyle.SetStatTextColor(4)
    gStyle.SetStatY(1.)
    self.compHisto.DrawNormalized('histo') 
    self.canvas.Update()
#    self.canvas.Print(self.fileName())            


    if (self.compHisto.IsA().InheritsFrom( "TH2" )):
      self.canvas.Clear()
      self.canvas.Divide(1,2)
      self.canvas.cd(1)
      gStyle.SetStatTextColor(4)
      gStyle.SetStatY(1.)
      self.compHisto.SetMarkerColor(4)
      self.compHisto.DrawNormalized('histo')
      self.canvas.Update()
      self.canvas.cd(2)
      gStyle.SetStatTextColor(2)
      gStyle.SetStatY(1.)
      self.referenceHisto.SetMarkerColor(2)
      self.referenceHisto.DrawNormalized('histo')
      self.canvas.Update()
      self.canvas.Print(self.compFileName())
      self.canvas.Divide(1,1)
      self.canvas.Clear()
    else:
# plot first the Ymax histo to adjust the scale
      maxCompH = self.compHisto.GetMaximum()
      nEntriesCompH = self.compHisto.GetEntries()
      maxCompH = maxCompH / nEntriesCompH
      maxRefH = self.referenceHisto.GetMaximum()
      nEntriesRefH = self.referenceHisto.GetEntries()
      maxRefH = maxRefH / nEntriesRefH
      if (maxCompH > maxRefH ):
        gStyle.SetStatTextColor(4)
        gStyle.SetStatY(1.)
        self.compHisto.DrawNormalized('histo')
        self.canvas.Update()
        gStyle.SetStatTextColor(2)
        gStyle.SetStatY(.8)
        self.referenceHisto.DrawNormalized('histosames')
        self.canvas.Update()
        self.canvas.Print(self.compFileName())
      else:
        gStyle.SetStatTextColor(2)
        gStyle.SetStatY(.8)
        self.referenceHisto.DrawNormalized('histo')
        self.canvas.Update()
        gStyle.SetStatTextColor(4)
        gStyle.SetStatY(1.)
        self.compHisto.DrawNormalized('histosames')
        self.canvas.Update()
        self.canvas.Print(self.compFileName())
      
    return 1


  def kstest(self,kopt):
    ks = self.referenceHisto.KolmogorovTest( self.compHisto, kopt) 
    return ks

  def entries(self):
    EntRef = self.referenceHisto.GetEntries()
    EntComp = self.compHisto.GetEntries()
    if(EntRef <30 or EntComp <30):
       entpass = False
    else:
       entpass = True
    return entpass
 
#####################################################################
class SimulationWebPage:
  def __init__(self,N):
    self.name = N
# Tables
    self.listOfQuantities = []

    self.basicQuantities = dict()
    self.basicRefQuantities = dict()
    self.tabDet = dict()
    self.basicQuantities2 = dict()
    self.basicRefQuantities2 = dict()
    self.basicQuantities3 = dict()
    self.basicRefQuantities3 = dict()
    self.basicQuantities4 = dict()
    self.basicRefQuantities4 = dict()
    self.basicQuantities5 = dict()
    self.basicRefQuantities5 = dict()

# Plots
    self.listOfPlots = []
    self.refPlots = dict()
    self.plots = dict()
    self.supPlots = dict()
    self.subDet = dict()
    self.bkgcolPlots = dict()

  def setVersions(self,V,RV,P,PRV,GE,GERV,DV,DRV,SV,SRV):
    self.version = V
    if V == None:
      self.version = 'v31r1'
    self.refVersion = RV
    if RV == None:
      self.refVersion = 'v26r1'
    self.pythiaVersion = P 
    if P == None:
      self.pythiaVersion = '6.416'
    self.refPythiaVersion = PRV
    if PRV == None:
      self.refPythiaVersion = '6.416'
    self.geantVersion = GE
    if GE == None:
      self.geantVersion = '3333'
    self.refGeantVersion = GERV
    if GERV == None:
      self.refGeantVersion = '333'
    self.dddbVersion = DV
    if DV == None:
      self.dddbVersion = 'head-20081002'
    self.refdddbVersion = DRV
    if DRV == None:
      self.refdddbVersion = 'head-20081002'
    self.simcondVersion = SV
    if SV == None:
      self.simcondVersion = 'head-20081002'
    self.refsimcondVersion = DRV
    if SRV == None:
      self.refsimcondVersion = 'head-20081002'


  def setEventType(self,E):
    self.eventType = E
  def setTotEvents(self,NE):
    self.totEvents = NE
  def setTotEventsRef(self,NER):
    self.totEventsRef = NER
  def addTable3Col(self,N,R,C,DetTab):
    self.listOfQuantities.append(N)
    self.tabDet[N] = DetTab
    self.basicQuantities[N]=C
    self.basicRefQuantities[N]=R
  def addTable5Col(self,N,R,C,R2,C2,DetTab):
    self.listOfQuantities.append(N)
    self.tabDet[N] = DetTab
    self.basicQuantities[N]=C
    self.basicRefQuantities[N]=R
    self.basicQuantities2[N]=C2
    self.basicRefQuantities2[N]=R2
  def addTable7Col(self,N,R,C,R2,C2,R3,C3,DetTab):
    self.listOfQuantities.append(N)
    self.tabDet[N] = DetTab
    self.basicQuantities[N]=C
    self.basicRefQuantities[N]=R
    self.basicQuantities2[N]=C2
    self.basicRefQuantities2[N]=R2
    self.basicQuantities3[N]=C3
    self.basicRefQuantities3[N]=R3
  def addTable11Col(self,N,R,R2,R3,R4,R5,C,C2,C3,C4,C5,DetTab):
    self.listOfQuantities.append(N)
    self.tabDet[N] = DetTab
    self.basicQuantities[N]=C
    self.basicRefQuantities[N]=R
    self.basicQuantities2[N]=C2
    self.basicRefQuantities2[N]=R2
    self.basicQuantities3[N]=C3
    self.basicRefQuantities3[N]=R3
    self.basicQuantities4[N]=C4
    self.basicRefQuantities4[N]=R4
    self.basicQuantities5[N]=C5
    self.basicRefQuantities5[N]=R5


  def addPlot(self,Plot,Detector,kopt):
    self.listOfPlots.append(Plot.title())
    self.subDet[Plot.title()] = Detector
    self.supPlots[Plot.title()] = Plot.compFileName()
# Kolmogorov test
    ksr = Plot.kstest(kopt)
    print "ksr test result", ksr
    if ( ksr >= 0.2 ):
      kstestcol = "green"
    elif ( ksr < 0.2 and ksr >= 0.05):
      kstestcol = "orange"
    elif ( ksr < 0.05 and ksr > 0.0):  
      kstestcol = "red"
    else:
      kstestcol = "grey"
# Min Entries test 
    EPass = Plot.entries()
    if (EPass == False or int(self.totEvents) < 50 or int(self.totEventsRef) < 50 ):
      kstestcol = "blue"
    self.bkgcolPlots[Plot.title()] = kstestcol
      


  def create(self):
    f = open(self.name,'w')
    f.write("<HTML>\n")
    f.write("<HEAD>\n")
    f.write("<link rel=\"stylesheet\" href=\"http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/css/lhcb.css\" type=\"text/css\" media=\"screen\">\n")
    f.write("<link rel=\"stylesheet\" href=\"http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/gauss/css/css.css\" type=\"text/css\" media=\"screen\">\n")  
    f.write("<link rel=\"stylesheet\" href=\"css.css\" type=\"text/css\" media=\"screen\">\n")      
    f.write("<title>GAUSS Simulation Plots</title>\n")
    f.write("</HEAD>\n")
    f.write("<BODY>\n")
    f.write("<div class=\"ctitle\">\n")
    f.write("<TABLE id=\"pagetitle\">\n")
    f.write("<TBODY>\n")
    f.write("<TR>\n")
    f.write("<TD class=iconspace>\n")
    f.write("<A href=\"http://cern.ch/lhcb-comp\">\n")
    f.write("<IMG id=lhcblogo  src=\"http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/images/lhcbcomputinglogo.gif\" >\n")
    f.write("</A>\n")
    f.write("</TD>\n")
    f.write("<TD vAlign=middle align=center>\n")
    f.write("<H1><a href=\"http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/gauss\" >Simulation plots for Gauss version " + self.version + "</a></H1></TD>\n")
    f.write("</TR>\n")
    f.write("</TBODY>\n")
    f.write("</TABLE>\n")    
    f.write("</div>\n")
    f.write("<div class=pagebody>\n")
    f.write("<div id=manifest>\n")
    f.write("<p>Comparison of Gauss " + self.version + " (Geant " + self.geantVersion + ", "+ self.totEvents +" events) with " + self.refVersion + " (Geant " + self.refGeantVersion + ", "+ self.totEventsRef +" events) " )
    f.write("for event type " + self.eventType + "</p>\n")
    f.write("</div>\n")
    f.write("<H2>Version numbers</H2>\n")
    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell style=background-color:red >\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("</div>\n")
    f.write("</TR>\n")
#    
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("Gauss\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write(self.version+"\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write(self.refVersion+"\n")
    f.write("</TD>\n")
    f.write("</TR>\n")
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("Pythia\n") 
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write(self.pythiaVersion+"\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write(self.refPythiaVersion+"\n")
    f.write("</TD>\n")
    f.write("</TR>\n")
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("Geant\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write(self.geantVersion+"\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write(self.refGeantVersion+"\n")
    f.write("</TD>\n")
    f.write("</TR>\n")
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("DDDB\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write(self.dddbVersion+"\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write(self.refdddbVersion+"\n")
    f.write("</TD>\n")
    f.write("</TR>\n")
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("SIMCOND\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write(self.simcondVersion+"\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write(self.refsimcondVersion+"\n")
    f.write("</TD>\n")
    f.write("</TR>\n")
    f.write("</TABLE>\n")
    

    f.write("<H2>Subdetectors MCHits info</H2>\n")
    
# Velo Quantities

    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("Av. #\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell style=background-color:red>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("</div>\n")
    f.write("</TR>\n")   
    
    for nameOfQuantity in self.listOfQuantities:
      if (self.tabDet[nameOfQuantity]=="Velo"):
       f.write("<TR>\n")
       f.write("<TD>\n")
       f.write(nameOfQuantity + "\n")
       f.write("</TD>\n")
       f.write("<TD>\n")
       if self.basicQuantities[nameOfQuantity]!=None:
         f.write(self.basicQuantities[nameOfQuantity]+"\n")
       f.write("</TD>\n")
       f.write("<TD>\n")
       if self.basicRefQuantities[nameOfQuantity]!=None:
         f.write(self.basicRefQuantities[nameOfQuantity]+"\n")
       f.write("</TD>\n")
       f.write("</TR>\n")
    f.write("</TABLE>\n")

    f.write("<br>")

# Tracker Quantities

    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("Av. #\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell style=background-color:red>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("</div>\n")
    f.write("</TR>\n")   
    

    for nameOfQuantity in self.listOfQuantities:
      if (self.tabDet[nameOfQuantity]=="Tracker"):
        f.write("<TR>\n")
        f.write("<TD>\n")
        f.write(nameOfQuantity + "\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities[nameOfQuantity]!=None:
          f.write(self.basicQuantities[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities[nameOfQuantity]!=None:
          f.write(self.basicRefQuantities[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("</TR>\n")
    f.write("</TABLE>\n")

    f.write("<br>")

# Muon Quantities 

    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("Av. #\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("<TD colspan=5>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD colspan=5>\n")
    f.write("<div class=firstcell style=background-color:red>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("</TR>\n")   

    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("M1\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("M2\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("M3\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("M4\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("M5\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("M1\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("M2\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("M3\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("M4\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("M5\n")
    f.write("</TD>\n")
    f.write("</TR>\n")   

    for nameOfQuantity in self.listOfQuantities:
      if (self.tabDet[nameOfQuantity]=="Muon"):
        f.write("<TR>\n")
        f.write("<TD>\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        f.write(nameOfQuantity + "\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities[nameOfQuantity]!=None:
          f.write(self.basicQuantities[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities2[nameOfQuantity]!=None:
          f.write(self.basicQuantities2[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities3[nameOfQuantity]!=None:
          f.write(self.basicQuantities3[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities4[nameOfQuantity]!=None:
          f.write(self.basicQuantities4[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities5[nameOfQuantity]!=None:
          f.write(self.basicQuantities5[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities[nameOfQuantity]!=None:
          f.write(self.basicRefQuantities[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities2[nameOfQuantity]!=None:
          f.write(self.basicRefQuantities2[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities3[nameOfQuantity]!=None:
          f.write(self.basicRefQuantities3[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities4[nameOfQuantity]!=None:
          f.write(self.basicRefQuantities4[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities5[nameOfQuantity]!=None:
          f.write(self.basicRefQuantities5[nameOfQuantity]+"\n")
        f.write("</TD>\n")

        f.write("</TR>\n")


    f.write("</TABLE>\n")
    f.write("<br>")

# Rich Quantities 

    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("Av. #\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell style=background-color:red>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("</div>\n")
    f.write("</TR>\n")   
    
    for nameOfQuantity in self.listOfQuantities:
      if (self.tabDet[nameOfQuantity]=="RICH1"):
        f.write("<TR>\n")
        f.write("<TD>\n")
        f.write(nameOfQuantity + "\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities[nameOfQuantity]!=None:
          f.write(self.basicQuantities[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities[nameOfQuantity]!=None:
          f.write(self.basicRefQuantities[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("</TR>\n")
    f.write("</TABLE>\n")

    f.write("<br>")

    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("Av. # \n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell style=background-color:red>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell style=background-color:red>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("</TR>\n")

    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("RICH 1\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("RICH 1\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("RICH 2\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("RICH 2\n")
    f.write("</TD>\n")
    f.write("</TR>\n")

    for nameOfQuantity in self.listOfQuantities:
      if (self.tabDet[nameOfQuantity]=="RICH2"):
        f.write("<TR>\n")
        f.write("<TD>\n")
        f.write(nameOfQuantity + "\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities[nameOfQuantity]!=None:
          f.write(self.basicQuantities[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities[nameOfQuantity]!=None:
          f.write(self.basicRefQuantities[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities2[nameOfQuantity]!=None:
          f.write(self.basicQuantities2[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities2[nameOfQuantity]!=None:
          f.write(self.basicRefQuantities2[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("</TR>\n")
    f.write("</TABLE>\n")

    f.write("<br>")

    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("Av. #\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell style=background-color:red>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell style=background-color:red>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("</div>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell style=background-color:red>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("</TR>\n")   

    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("Aero\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("Aero\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("C4F10\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("C4F10\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("CF4\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("CF4\n")
    f.write("</TD>\n")
    f.write("</TR>\n")   
    
    for nameOfQuantity in self.listOfQuantities:
      if (self.tabDet[nameOfQuantity]=="RICH3"):
        f.write("<TR>\n")
        f.write("<TD>\n")
        f.write(nameOfQuantity + "\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities[nameOfQuantity]!=None:
          f.write(self.basicQuantities[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities[nameOfQuantity]!=None:
          f.write(self.basicRefQuantities[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities2[nameOfQuantity]!=None:
          f.write(self.basicQuantities2[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities2[nameOfQuantity]!=None:
          f.write(self.basicRefQuantities2[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities3[nameOfQuantity]!=None:
          f.write(self.basicQuantities3[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities3[nameOfQuantity]!=None:
          f.write(self.basicRefQuantities3[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("</TR>\n")


    f.write("</TABLE>\n")


    f.write("<br>")

# General Quantities

    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell style=background-color:red>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell style=background-color:red>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("</div>\n")
    f.write("</TR>\n")   

    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("Mean\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("Mean\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("RMS\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("RMS\n")
    f.write("</TD>\n")
    f.write("</TR>\n")   

    for nameOfQuantity in self.listOfQuantities:
      if (self.tabDet[nameOfQuantity]=="Gene"):
        f.write("<TR>\n")
        f.write("<TD>\n")
        f.write(nameOfQuantity + "\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities[nameOfQuantity]!=None:
          f.write(self.basicQuantities[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities[nameOfQuantity]!=None:
          f.write(self.basicRefQuantities[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities2[nameOfQuantity]!=None:
          f.write(self.basicQuantities2[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities2[nameOfQuantity]!=None:
          f.write(self.basicRefQuantities2[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("</TR>\n")
    f.write("</TABLE>\n")

    f.write("<br>")

# Timing Quantities

    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell style=background-color:red>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell style=background-color:red>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("</div>\n")
    f.write("</TR>\n")   

    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("(2.8 GHz Xeon speed)\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("Clock (ms)\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("Clock(ms)\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("Total(s)\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("Total(s)\n")
    f.write("</TD>\n")
    f.write("</TR>\n")   

    for nameOfQuantity in self.listOfQuantities:
      if (self.tabDet[nameOfQuantity]=="Time"):
        f.write("<TR>\n")
        f.write("<TD>\n")
        f.write(nameOfQuantity + "\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities[nameOfQuantity]!=None:
          f.write("%.3f"%self.basicQuantities[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities[nameOfQuantity]!=None:
          f.write("%.3f"%self.basicRefQuantities[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities2[nameOfQuantity]!=None:
          f.write("%.3f"%self.basicQuantities2[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities2[nameOfQuantity]!=None:
          f.write("%.3f"%self.basicRefQuantities2[nameOfQuantity]+"\n")
        f.write("</TD>\n")
        f.write("</TR>\n")
    f.write("</TABLE>\n")

    f.write("<br>")

# Errors/Warnings

    f.write("<H2>Geant4 Errors / Warnings </H2>\n")

    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell style=background-color:red>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("</div>\n")
    f.write("</TR>\n")   

    for nameOfQuantity in self.listOfQuantities:
      if (self.tabDet[nameOfQuantity]=="Err"):
        f.write("<TR>\n")
        f.write("<TD>\n")
        f.write(nameOfQuantity + "\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicQuantities[nameOfQuantity]!=None:
          if nameOfQuantity=="Exceptions":
            f.write("<a href=Exceptions.log>" + str(self.basicQuantities[nameOfQuantity]) + "</a>\n")
          if nameOfQuantity=="Errors":
            f.write("<a href=Errors.log>" + str(self.basicQuantities[nameOfQuantity]) + "</a>\n")
          if nameOfQuantity=="Warnings":
            f.write("<a href=Warnings.log>" + str(self.basicQuantities[nameOfQuantity]) + "</a>\n")
          if nameOfQuantity=="G4 Exceptions":
            f.write("<a href=G4Exceptions.log>" + str(self.basicQuantities[nameOfQuantity]) + "</a>\n")
        f.write("</TD>\n")
        f.write("<TD>\n")
        if self.basicRefQuantities[nameOfQuantity]!=None:
          if nameOfQuantity=="Exceptions":
            f.write("<a href=Exceptions_Ref.log>" + str(self.basicRefQuantities[nameOfQuantity]) + "</a>\n")
          if nameOfQuantity=="Errors":
            f.write("<a href=Errors_Ref.log>" + str(self.basicRefQuantities[nameOfQuantity]) + "</a>\n")
          if nameOfQuantity=="Warnings":
            f.write("<a href=Warnings_Ref.log>" + str(self.basicRefQuantities[nameOfQuantity]) + "</a>\n")
          if nameOfQuantity=="G4 Exceptions":
            f.write("<a href=G4Exceptions_Ref.log>" + str(self.basicRefQuantities[nameOfQuantity]) + "</a>\n")
        f.write("</TD>\n")
        f.write("</TR>\n")

    f.write("</TABLE>\n")
    f.write("<br>")
    

    f.write("<H2>Simulation Monitoring Plots</H2>\n")
    f.write("The color background of the following plots indicates the result of the Kolmogorov test for the comparison of the current and reference versions; in case one (or both) of the compared plots has low statistics the blue background is used (K-Test unreliable). The grey background indicates that the returned Kolmogorov probability is zero. This happens when the compared histograms do not have the same binning edges or channels (in this case also the normalization fails) or when the probability is so low that the value is rounded to zero.") 
    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("K-Test gt 0.2")
    f.write("</TD>\n")
    f.write("<TD style=background-color:green>\n")
    f.write("</TD>\n")
    f.write("</TR>\n")
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("K-Test gt 0.05 and lt 0.2")
    f.write("</TD>\n")
    f.write("<TD style=background-color:orange>\n")
    f.write("</TD>\n")
    f.write("</TR>\n")
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("K-Test lt 0.05")
    f.write("</TD>\n")
    f.write("<TD style=background-color:red>\n")
    f.write("</TD>\n")
    f.write("</TR>\n")
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("K-Test failed")
    f.write("</TD>\n")
    f.write("<TD style=background-color:grey>\n")
    f.write("</TD>\n")
    f.write("</TR>\n")

    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("Insufficient statistics")
    f.write("</TD>\n")
    f.write("<TD style=background-color:blue>\n")
    f.write("</TD>\n")
    f.write("</TR>\n")
    f.write("<TABLE>\n") 


# MC Truth Plots
    f.write("<H2>MC Truth Plots</H2>\n")

    
    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")
    f.write("Comparison Current-Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("</div>\n")
    f.write("</TR>\n")   


    for nameOfPlot in self.listOfPlots:

      if (self.subDet[nameOfPlot]=="MC"):
         f.write("<TR>\n")
         f.write("<TD>\n")
         f.write(nameOfPlot + "\n")
         f.write("</TD>\n")
         f.write("<TD style=background-color:"+self.bkgcolPlots[nameOfPlot]+">\n")
         f.write("<A href=\""+self.supPlots[nameOfPlot]+"\"><img src=\""+self.supPlots[nameOfPlot]+"\" width=60 height=40/></A>\n")
         f.write("</TD>\n")
         f.write("</TR>\n")
    f.write("</TABLE>\n")    

    f.write("<br>")

# Velo Plots

    f.write("<H2>Velo Plots</H2>\n")

    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Comparison Current-Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("</div>\n")
    f.write("</TR>\n")   

    for nameOfPlot in self.listOfPlots:

      if (self.subDet[nameOfPlot]=="Velo"):
         f.write("<TR>\n")
         f.write("<TD>\n")
         f.write(nameOfPlot + "\n")
         f.write("</TD>\n")
         f.write("<TD style=background-color:"+self.bkgcolPlots[nameOfPlot]+">\n")
         f.write("<A href=\""+self.supPlots[nameOfPlot]+"\"><img src=\""+self.supPlots[nameOfPlot]+"\" width=60 height=40/></A>\n")
         f.write("</TD>\n")
         f.write("</TR>\n")
    f.write("</TABLE>\n")    

    f.write("<br>")

# TT Plots

    f.write("<H2>TT Plots</H2>\n")

    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Comparison Current-Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("</div>\n")
    f.write("</TR>\n")   

    for nameOfPlot in self.listOfPlots:

      if (self.subDet[nameOfPlot]=="TT"):
         f.write("<TR>\n")
         f.write("<TD>\n")
         f.write(nameOfPlot + "\n")
         f.write("</TD>\n")
         f.write("<TD style=background-color:"+self.bkgcolPlots[nameOfPlot]+">\n")
         f.write("<A href=\""+self.supPlots[nameOfPlot]+"\"><img src=\""+self.supPlots[nameOfPlot]+"\" width=60 height=40/></A>\n")
         f.write("</TD>\n")
         f.write("</TR>\n")
    f.write("</TABLE>\n")    

    f.write("<br>")

# IT Plots

    f.write("<H2>IT Plots</H2>\n")
    
    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Comparison Current-Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("</TR>\n")   


    for nameOfPlot in self.listOfPlots:

      if (self.subDet[nameOfPlot]=="IT"):
         f.write("<TR>\n")
         f.write("<TD>\n")
         f.write(nameOfPlot + "\n")
         f.write("</TD>\n")
         f.write("<TD style=background-color:"+self.bkgcolPlots[nameOfPlot]+">\n")
         f.write("<A href=\""+self.supPlots[nameOfPlot]+"\"><img src=\""+self.supPlots[nameOfPlot]+"\" width=60 height=40/></A>\n")
         f.write("</TD>\n")
         f.write("</TR>\n")
    f.write("</TABLE>\n")    

    f.write("<br>")

# OT Plots

    f.write("<H2>OT Plots</H2>\n")
    
    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Comparison Current-Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("</TR>\n")   

    for nameOfPlot in self.listOfPlots:

      if (self.subDet[nameOfPlot]=="OT"):
         f.write("<TR>\n")
         f.write("<TD>\n")
         f.write(nameOfPlot + "\n")
         f.write("</TD>\n")
         f.write("<TD style=background-color:"+self.bkgcolPlots[nameOfPlot]+">\n")
         f.write("<A href=\""+self.supPlots[nameOfPlot]+"\"><img src=\""+self.supPlots[nameOfPlot]+"\" width=60 height=40/></A>\n")
         f.write("</TD>\n")
         f.write("</TR>\n")
    f.write("</TABLE>\n")    

    f.write("<br>")

# ECAL Plots

    f.write("<H2>ECAL Plots</H2>\n")
    
    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Comparison Current-Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("</TR>\n")   

    for nameOfPlot in self.listOfPlots:

      if (self.subDet[nameOfPlot]=="ECAL"):
         f.write("<TR>\n")
         f.write("<TD>\n")
         f.write(nameOfPlot + "\n")
         f.write("</TD>\n")
         f.write("<TD style=background-color:"+self.bkgcolPlots[nameOfPlot]+">\n")
         f.write("<A href=\""+self.supPlots[nameOfPlot]+"\"><img src=\""+self.supPlots[nameOfPlot]+"\" width=60 height=40/></A>\n")
         f.write("</TD>\n")
         f.write("</TR>\n")
    f.write("</TABLE>\n")    

    f.write("<br>")

# HCAL Plots

    f.write("<H2>HCAL Plots</H2>\n")
    
    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Comparison Current-Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("</TR>\n")   

    for nameOfPlot in self.listOfPlots:

      if (self.subDet[nameOfPlot]=="HCAL"):
         f.write("<TR>\n")
         f.write("<TD>\n")
         f.write(nameOfPlot + "\n")
         f.write("</TD>\n")
         f.write("<TD style=background-color:"+self.bkgcolPlots[nameOfPlot]+">\n")
         f.write("<A href=\""+self.supPlots[nameOfPlot]+"\"><img src=\""+self.supPlots[nameOfPlot]+"\" width=60 height=40/></A>\n")
         f.write("</TD>\n")
         f.write("</TR>\n")
    f.write("</TABLE>\n")    

    f.write("<br>")

# PRS Plots

    f.write("<H2>PRS Plots</H2>\n")
    
    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Comparison Current-Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("</TR>\n")   

    for nameOfPlot in self.listOfPlots:

      if (self.subDet[nameOfPlot]=="PRS"):
         f.write("<TR>\n")
         f.write("<TD>\n")
         f.write(nameOfPlot + "\n")
         f.write("</TD>\n")
         f.write("<TD style=background-color:"+self.bkgcolPlots[nameOfPlot]+">\n")
         f.write("<A href=\""+self.supPlots[nameOfPlot]+"\"><img src=\""+self.supPlots[nameOfPlot]+"\" width=60 height=40/></A>\n")
         f.write("</TD>\n")
         f.write("</TR>\n")
    f.write("</TABLE>\n")    

    f.write("<br>")

# SPD Plots

    f.write("<H2>SPD Plots</H2>\n")
    
    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Comparison Current-Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("</TR>\n")   

    for nameOfPlot in self.listOfPlots:

      if (self.subDet[nameOfPlot]=="SPD"):
         f.write("<TR>\n")
         f.write("<TD>\n")
         f.write(nameOfPlot + "\n")
         f.write("</TD>\n")
         f.write("<TD style=background-color:"+self.bkgcolPlots[nameOfPlot]+">\n")
         f.write("<A href=\""+self.supPlots[nameOfPlot]+"\"><img src=\""+self.supPlots[nameOfPlot]+"\" width=60 height=40/></A>\n")
         f.write("</TD>\n")
         f.write("</TR>\n")
    f.write("</TABLE>\n")    

    f.write("<br>")

# RICH Plots

    f.write("<H2>RICH Plots</H2>\n")
    
    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Comparison Current-Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("</TR>\n")   

    for nameOfPlot in self.listOfPlots:

      if (self.subDet[nameOfPlot]=="RICH"):
         f.write("<TR>\n")
         f.write("<TD>\n")
         f.write(nameOfPlot + "\n")
         f.write("</TD>\n")
         f.write("<TD style=background-color:"+self.bkgcolPlots[nameOfPlot]+">\n")
         f.write("<A href=\""+self.supPlots[nameOfPlot]+"\"><img src=\""+self.supPlots[nameOfPlot]+"\" width=60 height=40/></A>\n")
         f.write("</TD>\n")
         f.write("</TR>\n")
    f.write("</TABLE>\n")    

    f.write("<br>")

# MUON Plots

    f.write("<H2>MUON Plots</H2>\n")
    
    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Comparison Current-Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("</TD>\n")
    f.write("</TR>\n")   

    for nameOfPlot in self.listOfPlots:

      if (self.subDet[nameOfPlot]=="MUON"):
         f.write("<TR>\n")
         f.write("<TD>\n")
         f.write(nameOfPlot + "\n")
         f.write("</TD>\n")
         f.write("<TD style=background-color:"+self.bkgcolPlots[nameOfPlot]+">\n")
         f.write("<A href=\""+self.supPlots[nameOfPlot]+"\"><img src=\""+self.supPlots[nameOfPlot]+"\" width=60 height=40/></A>\n")
         f.write("</TD>\n")
         f.write("</TR>\n")
    f.write("</TABLE>\n")    

    f.write("<br>")

    f.write("</BODY>\n")
    f.write("</HTML>\n")
    f.close()
  def install(self):
    dstDir = "/afs/cern.ch/lhcb/software/releases/DOC/gauss/simulation/validation/"+"Gauss_"+self.eventType + "_" + self.version+"_"+self.refVersion+"_Geant_"+self.geantVersion+"_"+self.refGeantVersion
    if os.path.exists(dstDir):
      for f in os.listdir(dstDir):
        os.remove( dstDir+"/"+f)
      os.rmdir( dstDir )
    os.mkdir( dstDir )
    shutil.move( self.name , dstDir )
    shutil.move( "Exceptions_Ref.log" , dstDir )
    shutil.move( "Errors_Ref.log" , dstDir )
    shutil.move( "Warnings_Ref.log" , dstDir )
    shutil.move( "G4Exceptions_Ref.log" , dstDir )
    shutil.move( "Exceptions.log" , dstDir )
    shutil.move( "Errors.log" , dstDir )
    shutil.move( "Warnings.log" , dstDir )
    shutil.move( "G4Exceptions.log" , dstDir )
    os.remove("reduced.log")
    os.remove("reduced_ref.log")
    for nameofPlot , supPlots in self.supPlots.iteritems():
      shutil.move( self.supPlots[nameofPlot] , dstDir )

  def addToIndex(self):
    dstDir = "Gauss_"+self.eventType + "_" + self.version+"_"+self.refVersion+"_Geant_"+self.geantVersion+"_"+self.refGeantVersion
    sumDir = "/afs/cern.ch/lhcb/software/releases/DOC/gauss/simulation/validation/"
    shutil.copy(sumDir+"index.html", "index.html")
    f = open("index.html",'r')
    g = open("newindex.html",'w') 
    for line in f:
      g.write(line)
      if line=="</TR> <!-- end of table -->\n":
        g.write( "<TR>\n" )
        g.write( "<TD>\n" ) 
        g.write( "<A href=\""+dstDir+"/simpage.html\">"+self.eventType+"</A>\n")
        g.write( "</TD>\n<TD>\n")
        g.write( self.version + "\n </TD>\n<TD>\n" + self.geantVersion + "\n" )
        g.write( "</TD>\n<TD>" + self.refVersion + "\n</TD>\n<TD>\n" + self.refGeantVersion + "\n" )
        g.write( "</TD>\n</TR>" )
    f.close()        
    g.close()
    os.remove( "index.html" ) 
    os.rename( "newindex.html","index.html")
    shutil.move( "index.html" , sumDir )      

#####################################################################
# Read command line options
def main():
  usage = "usage: %prog [options]"
  parser = OptionParser(usage)
  parser.add_option( "-r" , "--refhisto" , action="store", type="string" , 
    dest="referenceHistoName" , help="Histogram reference file" ) 
  parser.add_option( "-l" , "--reflog" , action="store" , type="string" ,
    dest="referenceLogfileName" , help="Reference log file" ) 
  parser.add_option( "-s" , "--histo" , action="store", type="string" , 
    dest="HistoName" , help="Histogram file to compare" ) 
  parser.add_option( "-f" , "--log" , action="store" , type="string" ,
    dest="LogfileName" , help="Log file to compare" ) 
  parser.add_option( "-v" , "--verbose" , action="store_true" , dest="verbose" )  
  parser.add_option( "-a" , "--add-to-index" , action="store_true" , dest="addToIndex" ) 
  parser.add_option( "-i" , "--install" , action="store_true" , dest="install" )
  parser.add_option( "-k" , "--kolmogorov" , action="store", type="string" , dest="kstopt"  , default ="", help="Kolmogorov test options [DEFAULT= ]")
      
  (options, args) = parser.parse_args()
  
  webPage = SimulationWebPage("simpage.html")
  
  #####################################################################
  if options.verbose:
    print "Read reference log file: " + options.referenceLogfileName
    print "Reducing ref log file dimensions... " 
  os.system("grep -i -e DEBUG -e GigaGetEventAlg -e GigaGetHitsAlg -e 'INFO Evt' -e 'barcode equal' -e 'Illegal (negative)' -e 'The hit collection' -e 'Hits> is not found' -e '<Rich1TopHC> is not found' -v "+ options.referenceLogfileName +"> reduced_ref.log")
  if options.verbose:
    print "Reduced ref log file produced - start processing info..." 
  # ReferenceLog = SimulationLogFile( options.referenceLogfileName ) 
  ReferenceLog = SimulationLogFile( "reduced_ref.log" ) 
  ReferenceLog.computeQuantities()
  shutil.move( "Exceptions.log" ,"Exceptions_Ref.log")
  shutil.move( "Errors.log" ,"Errors_Ref.log")
  shutil.move( "Warnings.log" ,"Warnings_Ref.log")
  shutil.move( "G4Exceptions.log" ,"G4Exceptions_Ref.log")

  if options.verbose:
    print "Read log file: " + options.LogfileName
    print "Reducing log file dimensions... "
  os.system("grep -i -e DEBUG -e GigaGetEventAlg -e GigaGetHitsAlg -e 'INFO Evt' -e 'barcode equal' -e 'Illegal (negative)' -e 'The hit collection' -e 'Hits> is not found' -e '<Rich1TopHC> is not found' -v "+ options.LogfileName +"> reduced.log")
  if options.verbose:
    print "Reduced log file produced - start processing info..." 
  # TheLog = SimulationLogFile( options.LogfileName ) 
  TheLog = SimulationLogFile( "reduced.log" ) 
  TheLog.computeQuantities()
  
  if ReferenceLog.eventType() != TheLog.eventType():
    print "ERROR: the two files are of different event types: " + ReferenceLog.eventType() + " / " + TheLog.eventType()
  print "Comparison for event type " + ReferenceLog.eventType() 
  print "between versions " + ReferenceLog.gaussVersion() + " and " + TheLog.gaussVersion()
    
  if options.verbose:    
    # add Geant info
    print "Reference Geant version = " , ReferenceLog.geantVersion()
    print "Reference DDDB version = " , ReferenceLog.dddbVersion()
    print "Reference SIMCOND version = " , ReferenceLog.simcondVersion()
    # Velo monitoring info
    print "Reference Velo MCHits / Event = " , ReferenceLog.veloMCHitsEvent()
    print "Reference Velo PileUp MCHits = " , ReferenceLog.veloPileUpMCHits()
    # Muon monitoring info
    print "Reference Muon MCHits R1M1 = " , ReferenceLog.muonMCHitsR1M1()
    print "Reference Muon MCHits R1M2 = " , ReferenceLog.muonMCHitsR1M2()
    print "Reference Muon MCHits R1M3 = " , ReferenceLog.muonMCHitsR1M3()
    print "Reference Muon MCHits R1M4 = " , ReferenceLog.muonMCHitsR1M4()
    print "Reference Muon MCHits R1M5 = " , ReferenceLog.muonMCHitsR1M5()

    print "Reference Muon MCHits R2M1 = " , ReferenceLog.muonMCHitsR2M1()
    print "Reference Muon MCHits R2M2 = " , ReferenceLog.muonMCHitsR2M2()
    print "Reference Muon MCHits R2M3 = " , ReferenceLog.muonMCHitsR2M3()
    print "Reference Muon MCHits R2M4 = " , ReferenceLog.muonMCHitsR2M4()
    print "Reference Muon MCHits R2M5 = " , ReferenceLog.muonMCHitsR2M5()

    print "Reference Muon MCHits R3M1 = " , ReferenceLog.muonMCHitsR3M1()
    print "Reference Muon MCHits R3M2 = " , ReferenceLog.muonMCHitsR3M2()
    print "Reference Muon MCHits R3M3 = " , ReferenceLog.muonMCHitsR3M3()
    print "Reference Muon MCHits R3M4 = " , ReferenceLog.muonMCHitsR3M4()
    print "Reference Muon MCHits R3M5 = " , ReferenceLog.muonMCHitsR3M5()

    print "Reference Muon MCHits R4M1 = " , ReferenceLog.muonMCHitsR4M1()
    print "Reference Muon MCHits R4M2 = " , ReferenceLog.muonMCHitsR4M2()
    print "Reference Muon MCHits R4M3 = " , ReferenceLog.muonMCHitsR4M3()
    print "Reference Muon MCHits R4M4 = " , ReferenceLog.muonMCHitsR4M4()
    print "Reference Muon MCHits R4M5 = " , ReferenceLog.muonMCHitsR4M5()

    print "Reference Muon MCHits AllR M1 = " , ReferenceLog.muonMCHitsAllRM1()
    print "Reference Muon MCHits AllR M2 = " , ReferenceLog.muonMCHitsAllRM2()
    print "Reference Muon MCHits AllR M3 = " , ReferenceLog.muonMCHitsAllRM3()
    print "Reference Muon MCHits AllR M4 = " , ReferenceLog.muonMCHitsAllRM4()
    print "Reference Muon MCHits AllR M5 = " , ReferenceLog.muonMCHitsAllRM5()

    
    # RICH monitoring info
    print "Reference Av. # Invalid RICH flags = " , ReferenceLog.richInvalidFlag()
    print "Reference Av. # MCRichHits Rich1= " , ReferenceLog.richMCHitsR1()
    print "Reference Av. # Invalid radiator hits Rich1 = " , ReferenceLog.richInvalidRadiatorHitsR1()
    print "Reference Av. # Gas Quartz CK hits Rich1 = " , ReferenceLog.richGasQuartzCKHitsR1()
    print "Reference Av. # HPD Quartz CK hits Rich1 = " , ReferenceLog.richHPDQuartzCKHitsR1()
    print "Reference Av. # Nitrogen CK hits Rich1 = " , ReferenceLog.richNitrogenCKHitsR1()
    print "Reference Av. # MCRichHits Rich2= " , ReferenceLog.richMCHitsR2()
    print "Reference Av. # Invalid radiator hits Rich2 = " , ReferenceLog.richInvalidRadiatorHitsR2()
    print "Reference Av. # Gas Quartz CK hits Rich2 = " , ReferenceLog.richGasQuartzCKHitsR2()
    print "Reference Av. # HPD Quartz CK hits Rich2 = " , ReferenceLog.richHPDQuartzCKHitsR2()
    print "Reference Av. # Nitrogen CK hits Rich2 = " , ReferenceLog.richNitrogenCKHitsR2()
    print "Reference Av. # RICH Signal CK Hits Aero= " , ReferenceLog.richSignalCKMCHitsAero()
    print "Reference Av. # RICH Scattered Hits Aero = " , ReferenceLog.richScatteredHitsAero()
    print "Reference Av. # RICH MCParticle-less Hits Aero = " , ReferenceLog.richParticleLessMCHitsAero()
    print "Reference Av. # RICH Signal CK Hits C4F10 = " , ReferenceLog.richSignalCKMCHitsC4F10()
    print "Reference Av. # RICH Scattered Hits C4F10 = " , ReferenceLog.richScatteredHitsC4F10()
    print "Reference Av. # RICH MCParticle-less Hits C4F10 = " , ReferenceLog.richParticleLessMCHitsC4F10()
    print "Reference Av. # RICH Signal CK Hits CF4 = " , ReferenceLog.richSignalCKMCHitsCF4()
    print "Reference Av. # RICH Scattered Hits CF4 = " , ReferenceLog.richScatteredHitsCF4()
    print "Reference Av. # RICH MCParticle-less Hits CF4 = " , ReferenceLog.richParticleLessMCHitsCF4()
    # General monitoring info
    print "Reference MCParticles mean = " , ReferenceLog.mcParticlesMean()
    print "Reference MCParticles RMS = " , ReferenceLog.mcParticlesRMS()
    print "Reference MCHits TT mean = " , ReferenceLog.mcHitsTTMean()
    print "Reference MCHits TT RMS = " , ReferenceLog.mcHitsTTRMS()
    print "Reference MCHits IT mean = " , ReferenceLog.mcHitsITMean()
    print "Reference MCHits IT RMS = " , ReferenceLog.mcHitsITRMS()
    print "Reference MCHits OT mean = " , ReferenceLog.mcHitsOTMean()
    print "Reference MCHits OT RMS = " , ReferenceLog.mcHitsOTRMS()
    print "Reference MCHits SPD mean = " , ReferenceLog.mcHitsSPDMean()
    print "Reference MCHits SPD RMS = " , ReferenceLog.mcHitsSPDRMS()
    print "Reference MCHits PRS mean = " , ReferenceLog.mcHitsPRSMean()
    print "Reference MCHits PRS RMS = " , ReferenceLog.mcHitsPRSRMS()
    print "Reference MCHits ECAL mean = " , ReferenceLog.mcHitsECALMean()
    print "Reference MCHits ECAL RMS = " , ReferenceLog.mcHitsECALRMS()
    print "Reference MCHits HCAL mean = " , ReferenceLog.mcHitsHCALMean()
    print "Reference MCHits HCAL RMS = " , ReferenceLog.mcHitsHCALRMS()
    # ST monitoring info
    print "Reference IT Hits per Event = " , ReferenceLog.itHitsPerEvent()
    print "Reference IT Mean beta * gamma = " , ReferenceLog.itMeanBetaGamma()
    print "Reference IT Most Probable deposited charge = " , ReferenceLog.itMPVDepCharge()
    print "Reference IT Half Sample Width = " , ReferenceLog.itHalfSaWidth()
    print "Reference TT Hits per Event = " , ReferenceLog.ttHitsPerEvent()
    print "Reference TT Mean beta * gamma = " , ReferenceLog.ttMeanBetaGamma()
    print "Reference TT Most Probable deposited charge = " , ReferenceLog.ttMPVDepCharge()
    print "Reference TT Half Sample Width = " , ReferenceLog.ttHalfSaWidth()
    print "Reference OT Hits per Event = " , ReferenceLog.otHitsPerEvent()
    print "Reference OT Mean beta * gamma = " , ReferenceLog.otMeanBetaGamma()
    print "Reference OT Most Probable deposited charge = " , ReferenceLog.otMPVDepCharge()
    print "Reference OT Half Sample Width = " , ReferenceLog.otHalfSaWidth()
    # Error monitoring info
    print "Warnings = " , ReferenceLog.warCount()
    print "Errors = " , ReferenceLog.errCount()
    print "Exceptions = " , ReferenceLog.excepCount()
    print "G4 Exceptions = " , ReferenceLog.g4Count()
    # Processing Time
    print "Reference Speed Factor = " , ReferenceLog.timeSpeedFactor()
    print "Reference Tot Entries = " , ReferenceLog.totEntries()
    print "Reference Time Event Loop = " , ReferenceLog.timeEventLoop()
    print "Reference Time Event Loop TOT= " , ReferenceLog.timeEventLoopTOT()
    print "Reference Time Gauss = " , ReferenceLog.timeGauss()
    print "Reference Time Gauss TOT= " , ReferenceLog.timeGaussTOT()
    print "Reference Time Generator = " , ReferenceLog.timeGenerator()
    print "Reference Time Generator TOT= " , ReferenceLog.timeGeneratorTOT()
    print "Reference Time Simulation = " , ReferenceLog.timeSimulation()
    print "Reference Time Simulation TOT= " , ReferenceLog.timeSimulationTOT()
    print "Reference Time GaussSim = " , ReferenceLog.timeGaussSim()
    print "Reference Time GaussSim TOT= " , ReferenceLog.timeGaussSimTOT()
    print "Reference Time GigaStore = " , ReferenceLog.timeGigaStore()
    print "Reference Time GigaStore TOT= " , ReferenceLog.timeGigaStoreTOT()
    print "Reference Time Geo = " , ReferenceLog.timeGeo()
    print "Reference Time Geo TOT= " , ReferenceLog.timeGeoTOT()
    print "Reference Time GenerationToSimulation = " , ReferenceLog.timeGenerationToSimulation()
    print "Reference Time GenerationToSimulation TOT= " , ReferenceLog.timeGenerationToSimulationTOT()
    print "Reference Time GiGaFlushAlgorithm = " , ReferenceLog.timeGiGaFlushAlgorithm()
    print "Reference Time GiGaFlushAlgorithm TOT = " , ReferenceLog.timeGiGaFlushAlgorithmTOT()
    print "Reference Time SimulationToMCTruth = " , ReferenceLog.timeSimulationToMCTruth()
    print "Reference Time SimulationToMCTruth TOT = " , ReferenceLog.timeSimulationToMCTruthTOT()
    print "Reference Time DetectorsHits = " , ReferenceLog.timeDetectorsHits()
    print "Reference Time DetectorsHitsTOT = " , ReferenceLog.timeDetectorsHitsTOT()
    print "Reference Time SimMonitor = " , ReferenceLog.timeSimMonitor()
    print "Reference Time SimMonitorTOT = " , ReferenceLog.timeSimMonitorTOT()


    print "Event type = " , TheLog.eventType() 
    # add Geant info
    print "Geant version= " , TheLog.geantVersion()
    print "DDDB version= " , TheLog.dddbVersion()
    print "SIMCOND version= " , TheLog.simcondVersion()
    # Velo monitoring info
    print "Velo MCHits / Event= " , TheLog.veloMCHitsEvent()
    print "Velo PileUp MCHits= " , TheLog.veloPileUpMCHits()
    # Muon monitoring info
    print "Muon MCHits R1M1 = " , TheLog.muonMCHitsR1M1()
    print "Muon MCHits R1M2 = " , TheLog.muonMCHitsR1M2()
    print "Muon MCHits R1M3 = " , TheLog.muonMCHitsR1M3()
    print "Muon MCHits R1M4 = " , TheLog.muonMCHitsR1M4()
    print "Muon MCHits R1M5 = " , TheLog.muonMCHitsR1M5()

    print "Muon MCHits R2M1 = " , TheLog.muonMCHitsR2M1()
    print "Muon MCHits R2M2 = " , TheLog.muonMCHitsR2M2()
    print "Muon MCHits R2M3 = " , TheLog.muonMCHitsR2M3()
    print "Muon MCHits R2M4 = " , TheLog.muonMCHitsR2M4()
    print "Muon MCHits R2M5 = " , TheLog.muonMCHitsR2M5()

    print "Muon MCHits R3M1 = " , TheLog.muonMCHitsR3M1()
    print "Muon MCHits R3M2 = " , TheLog.muonMCHitsR3M2()
    print "Muon MCHits R3M3 = " , TheLog.muonMCHitsR3M3()
    print "Muon MCHits R3M4 = " , TheLog.muonMCHitsR3M4()
    print "Muon MCHits R3M5 = " , TheLog.muonMCHitsR3M5()

    print "Muon MCHits R4M1 = " , TheLog.muonMCHitsR4M1()
    print "Muon MCHits R4M2 = " , TheLog.muonMCHitsR4M2()
    print "Muon MCHits R4M3 = " , TheLog.muonMCHitsR4M3()
    print "Muon MCHits R4M4 = " , TheLog.muonMCHitsR4M4()
    print "Muon MCHits R4M5 = " , TheLog.muonMCHitsR4M5()

    print "Muon MCHits AllR M1 = " , TheLog.muonMCHitsAllRM1()
    print "Muon MCHits AllR M2 = " , TheLog.muonMCHitsAllRM2()
    print "Muon MCHits AllR M3 = " , TheLog.muonMCHitsAllRM3()
    print "Muon MCHits AllR M4 = " , TheLog.muonMCHitsAllRM4()
    print "Muon MCHits AllR M5 = " , TheLog.muonMCHitsAllRM5()
    # Rich monitoring info
    print "Av. # Invalid RICH flags = " , TheLog.richInvalidFlag()
    print "Av. # RICH MCHits Rich1= " , TheLog.richMCHitsR1()
    print "Av. # Invalid radiator hits Rich1 = " , TheLog.richInvalidRadiatorHitsR1()
    print "Av. # Gas Quartz CK hits Rich1 = " , TheLog.richGasQuartzCKHitsR1()
    print "Av. # HPD Quartz CK hits Rich1 = " , TheLog.richHPDQuartzCKHitsR1()
    print "Av. # Nitrogen CK hits Rich1 = " , TheLog.richNitrogenCKHitsR1()
    print "Av. # RICH MCHits Rich2= " , TheLog.richMCHitsR2()
    print "Av. # Invalid radiator hits Rich2 = " , TheLog.richInvalidRadiatorHitsR2()
    print "Av. # Gas Quartz CK hits Rich2 = " , TheLog.richGasQuartzCKHitsR2()
    print "Av. # HPD Quartz CK hits Rich2 = " , TheLog.richHPDQuartzCKHitsR2()
    print "Av. # Nitrogen CK hits Rich2 = " , TheLog.richNitrogenCKHitsR2()
    print "Av. # RICH Signal CK Hits Aero= " , TheLog.richSignalCKMCHitsAero()
    print "Av. # RICH Scattered Hits Aero = " , TheLog.richScatteredHitsAero()
    print "Av. # RICH MCParticle-less Hits Aero= " , TheLog.richParticleLessMCHitsAero()
    print "Av. # RICH Signal CK Hits C4F10 = " , TheLog.richSignalCKMCHitsC4F10()
    print "Av. # RICH Scattered Hits C4F10 = " , TheLog.richScatteredHitsC4F10()
    print "Av. # RICH MCParticle-less Hits C4F10 = " , TheLog.richParticleLessMCHitsC4F10()
    print "Av. # RICH Signal CK Hits CF4 = " , TheLog.richSignalCKMCHitsCF4()
    print "Av. # RICH Scattered Hits CF4 = " , TheLog.richScatteredHitsCF4()
    print "Av. # RICH MCParticle-less Hits CF4 = " , TheLog.richParticleLessMCHitsCF4()
    # General monitoring info
    print "MCParticles mean = " , TheLog.mcParticlesMean()
    print "MCParticles RMS = " , TheLog.mcParticlesRMS()
    print "MCHits TT mean = " , TheLog.mcHitsTTMean()
    print "MCHits TT RMS = " , TheLog.mcHitsTTRMS()
    print "MCHits IT mean = " , TheLog.mcHitsITMean()
    print "MCHits IT RMS = " , TheLog.mcHitsITRMS()
    print "MCHits OT mean = " , TheLog.mcHitsOTMean()
    print "MCHits OT RMS = " , TheLog.mcHitsOTRMS()
    print "MCHits SPD mean = " , TheLog.mcHitsSPDMean()
    print "MCHits SPD RMS = " , TheLog.mcHitsSPDRMS()
    print "MCHits PRS mean = " , TheLog.mcHitsPRSMean()
    print "MCHits PRS RMS = " , TheLog.mcHitsPRSRMS()
    print "MCHits ECAL mean = " , TheLog.mcHitsECALMean()
    print "MCHits ECAL RMS = " , TheLog.mcHitsECALRMS()
    print "MCHits HCAL mean = " , TheLog.mcHitsHCALMean()
    print "MCHits HCAL RMS = " , TheLog.mcHitsHCALRMS()
    print "IT Hits per Event = " , TheLog.itHitsPerEvent()
    print "IT Mean beta * gamma = " , TheLog.itMeanBetaGamma()
    print "IT Most Probable deposited charge = " , TheLog.itMPVDepCharge()
    print "IT Half Sample Width = " , TheLog.itHalfSaWidth()    
    print "TT Hits per Event = " , TheLog.ttHitsPerEvent()
    print "TT Mean beta * gamma = " , TheLog.ttMeanBetaGamma()
    print "TT Most Probable deposited charge = " , TheLog.ttMPVDepCharge()
    print "TT Half Sample Width = " , TheLog.ttHalfSaWidth()
    print "OT Hits per Event = " , TheLog.otHitsPerEvent()
    print "OT Mean beta * gamma = " , TheLog.otMeanBetaGamma()
    print "OT Most Probable deposited charge = " , TheLog.otMPVDepCharge()
    print "OT Half Sample Width = " , TheLog.otHalfSaWidth()
    # Errors
    print "Warnings = " , TheLog.warCount()
    print "Errors = " , TheLog.errCount()
    print "Exceptions = " , TheLog.excepCount()
    print "G4 Exceptions = " , TheLog.g4Count()
    # Processing Time
    print "Time Speed Factor = " , TheLog.timeSpeedFactor()
    print "Tot Entries = " , TheLog.totEntries()
    print "Time Event Loop = " , TheLog.timeEventLoop()
    print "Time Event Loop TOT= " , TheLog.timeEventLoopTOT()
    print "Time Gauss = " , TheLog.timeGauss()
    print "Time Gauss TOT= " , TheLog.timeGaussTOT()
    print "Time Generator = " , TheLog.timeGenerator()
    print "Time Generator TOT= " , TheLog.timeGeneratorTOT()
    print "Time Simulation = " , TheLog.timeSimulation()
    print "Time Simulation TOT= " , TheLog.timeSimulationTOT()
    print "Time GaussSim = " , TheLog.timeGaussSim()
    print "Time GausSim TOT= " , TheLog.timeGaussSim()
    print "Time GigaStore = " , TheLog.timeGigaStore()
    print "Time GigaStore TOT= " , TheLog.timeGigaStore()
    print "Time GaussSim = " , TheLog.timeGaussSim()
    print "Time GaussSim TOT= " , TheLog.timeGaussSimTOT()
    print "Time GigaStore = " , TheLog.timeGigaStore()
    print "Time GigaStore TOT= " , TheLog.timeGigaStoreTOT()
    print "Time Geo = " , TheLog.timeGeo()
    print "Time Geo TOT= " , TheLog.timeGeoTOT()
    print "Time GenerationToSimulation = " , TheLog.timeGenerationToSimulation()
    print "Time GenerationToSimulationTOT= " , TheLog.timeGenerationToSimulationTOT()
    print "Time GiGaFlushAlgorithm = " , TheLog.timeGiGaFlushAlgorithm()
    print "Time GiGaFlushAlgorithm = " , TheLog.timeGiGaFlushAlgorithmTOT()
    print "Time SimulationToMCTruth = " , TheLog.timeSimulationToMCTruth()
    print "Time SimulationToMCTruth TOT = " , TheLog.timeSimulationToMCTruthTOT()
    print "Time DetectorsHits = " , TheLog.timeDetectorsHits()
    print "Time DetectorsHitsTOT = " , TheLog.timeDetectorsHitsTOT()
    print "Time SimMonitor = " , TheLog.timeSimMonitor()
    print "Time SimMonitorTOT = " , TheLog.timeSimMonitorTOT()



  
  webPage.setVersions( TheLog.gaussVersion() , ReferenceLog.gaussVersion() , TheLog.pythiaVersion() , ReferenceLog.pythiaVersion() ,  TheLog.geantVersion() ,  ReferenceLog.geantVersion(), TheLog.dddbVersion() , ReferenceLog.dddbVersion() , TheLog.simcondVersion() , ReferenceLog.simcondVersion()   )   
  webPage.setEventType( TheLog.eventType() )
  webPage.setTotEvents( TheLog.totEntries())
  webPage.setTotEventsRef( ReferenceLog.totEntries())

# Table VELO 
  webPage.addTable3Col("Velo MCHits / Event" , ReferenceLog.veloMCHitsEvent() , TheLog.veloMCHitsEvent() , "Velo")
  webPage.addTable3Col("Velo Pile Up MCHits" , ReferenceLog.veloPileUpMCHits() , TheLog.veloPileUpMCHits(), "Velo")                

# Table Tracker
  webPage.addTable3Col("IT hits per event" , ReferenceLog.itHitsPerEvent() , TheLog.itHitsPerEvent() , "Tracker")                
  webPage.addTable3Col("IT Mean beta * gamma" , ReferenceLog.itMeanBetaGamma() , TheLog.itMeanBetaGamma() , "Tracker")                
  webPage.addTable3Col("IT Most Probable deposited charge" , ReferenceLog.itMPVDepCharge() , TheLog.itMPVDepCharge() , "Tracker")                
  webPage.addTable3Col("IT Half Sample Width" , ReferenceLog.itHalfSaWidth() , TheLog.itHalfSaWidth() , "Tracker")                
  webPage.addTable3Col("TT hits per event" , ReferenceLog.ttHitsPerEvent() , TheLog.ttHitsPerEvent() , "Tracker")                
  webPage.addTable3Col("TT Mean beta * gamma" , ReferenceLog.ttMeanBetaGamma() , TheLog.ttMeanBetaGamma() , "Tracker")                
  webPage.addTable3Col("TT Most Probable deposited charge" , ReferenceLog.ttMPVDepCharge() , TheLog.ttMPVDepCharge() , "Tracker")                
  webPage.addTable3Col("TT Half Sample Width" , ReferenceLog.ttHalfSaWidth() , TheLog.ttHalfSaWidth() , "Tracker")                
  webPage.addTable3Col("OT hits per event" , ReferenceLog.otHitsPerEvent() , TheLog.otHitsPerEvent() , "Tracker" )                
  webPage.addTable3Col("OT Mean beta * gamma" , ReferenceLog.otMeanBetaGamma() , TheLog.otMeanBetaGamma() , "Tracker" )                
  webPage.addTable3Col("OT Most Probable deposited charge" , ReferenceLog.otMPVDepCharge() , TheLog.otMPVDepCharge() , "Tracker" )
  webPage.addTable3Col("OT Half Sample Width" , ReferenceLog.otHalfSaWidth() , TheLog.otHalfSaWidth() , "Tracker")                

# Table Muons
  webPage.addTable11Col(" R1" , ReferenceLog.muonMCHitsR1M1(), ReferenceLog.muonMCHitsR1M2(), ReferenceLog.muonMCHitsR1M3() , ReferenceLog.muonMCHitsR1M4(), ReferenceLog.muonMCHitsR1M5() , TheLog.muonMCHitsR1M1() ,  TheLog.muonMCHitsR1M2() ,  TheLog.muonMCHitsR1M3() ,  TheLog.muonMCHitsR1M4() ,  TheLog.muonMCHitsR1M5() , "Muon")                
  webPage.addTable11Col(" R2" , ReferenceLog.muonMCHitsR2M1(), ReferenceLog.muonMCHitsR2M2(), ReferenceLog.muonMCHitsR2M3() , ReferenceLog.muonMCHitsR2M4(), ReferenceLog.muonMCHitsR2M5() , TheLog.muonMCHitsR2M1() ,  TheLog.muonMCHitsR2M2() ,  TheLog.muonMCHitsR2M3() ,  TheLog.muonMCHitsR2M4() ,  TheLog.muonMCHitsR2M5() , "Muon")                
  webPage.addTable11Col(" R3" , ReferenceLog.muonMCHitsR3M1(), ReferenceLog.muonMCHitsR3M2(), ReferenceLog.muonMCHitsR3M3() , ReferenceLog.muonMCHitsR3M4(), ReferenceLog.muonMCHitsR3M5() , TheLog.muonMCHitsR3M1() ,  TheLog.muonMCHitsR3M2() ,  TheLog.muonMCHitsR3M3() ,  TheLog.muonMCHitsR3M4() ,  TheLog.muonMCHitsR3M5() , "Muon")                
  webPage.addTable11Col(" R4" , ReferenceLog.muonMCHitsR4M1(), ReferenceLog.muonMCHitsR4M2(), ReferenceLog.muonMCHitsR4M3() , ReferenceLog.muonMCHitsR4M4(), ReferenceLog.muonMCHitsR4M5() , TheLog.muonMCHitsR4M1() ,  TheLog.muonMCHitsR4M2() ,  TheLog.muonMCHitsR4M3() ,  TheLog.muonMCHitsR4M4() ,  TheLog.muonMCHitsR4M5() , "Muon")                
  webPage.addTable11Col("all R" , ReferenceLog.muonMCHitsAllRM1(), ReferenceLog.muonMCHitsAllRM2(), ReferenceLog.muonMCHitsAllRM3() , ReferenceLog.muonMCHitsAllRM4(), ReferenceLog.muonMCHitsAllRM5() , TheLog.muonMCHitsAllRM1() ,  TheLog.muonMCHitsAllRM2() ,  TheLog.muonMCHitsAllRM3() ,  TheLog.muonMCHitsAllRM4() ,  TheLog.muonMCHitsAllRM5() , "Muon")                

# Table Rich - 1
  webPage.addTable3Col("Invalid RICH flags " , ReferenceLog.richInvalidFlag() , TheLog.richInvalidFlag(), "RICH1" )


# Table Rich - 2
  webPage.addTable5Col("MCRichHits " , ReferenceLog.richMCHitsR1() , TheLog.richMCHitsR1() , ReferenceLog.richMCHitsR2() , TheLog.richMCHitsR2() , "RICH2")
  webPage.addTable5Col("Invalid radiator hits " , ReferenceLog.richInvalidRadiatorHitsR1() , TheLog.richInvalidRadiatorHitsR1() , ReferenceLog.richInvalidRadiatorHitsR2() , TheLog.richInvalidRadiatorHitsR2() , "RICH2" )                
  webPage.addTable5Col("Gas Quartz CK hits " , ReferenceLog.richGasQuartzCKHitsR1() , TheLog.richGasQuartzCKHitsR1() , ReferenceLog.richGasQuartzCKHitsR2() , TheLog.richGasQuartzCKHitsR2() , "RICH2")                
  webPage.addTable5Col("HPD Quartz CK hits " , ReferenceLog.richHPDQuartzCKHitsR1() , TheLog.richHPDQuartzCKHitsR1() , ReferenceLog.richHPDQuartzCKHitsR2() , TheLog.richHPDQuartzCKHitsR2() , "RICH2")                
  webPage.addTable5Col("Nitrogen hits " , ReferenceLog.richNitrogenCKHitsR1() , TheLog.richNitrogenCKHitsR1() , ReferenceLog.richNitrogenCKHitsR2() , TheLog.richNitrogenCKHitsR2() , "RICH2")                

# Table Rich - 3
  webPage.addTable7Col("Signal CK MCRichHits" , ReferenceLog.richSignalCKMCHitsAero() , TheLog.richSignalCKMCHitsAero() , ReferenceLog.richSignalCKMCHitsC4F10() , TheLog.richSignalCKMCHitsC4F10() , ReferenceLog.richSignalCKMCHitsCF4() , TheLog.richSignalCKMCHitsCF4() , "RICH3")
  webPage.addTable7Col("Scattered hits" , ReferenceLog.richScatteredHitsAero() , TheLog.richScatteredHitsAero() ,  ReferenceLog.richScatteredHitsC4F10() , TheLog.richScatteredHitsC4F10(), ReferenceLog.richScatteredHitsCF4() , TheLog.richScatteredHitsCF4() , "RICH3")
  webPage.addTable7Col("MCParticle-less hits" , ReferenceLog.richParticleLessMCHitsAero() , TheLog.richParticleLessMCHitsAero() , ReferenceLog.richParticleLessMCHitsC4F10() , TheLog.richParticleLessMCHitsC4F10() , ReferenceLog.richParticleLessMCHitsCF4() , TheLog.richParticleLessMCHitsCF4() , "RICH3")
  
# General info table
  webPage.addTable5Col("MC Particles" , ReferenceLog.mcParticlesMean() , TheLog.mcParticlesMean(),ReferenceLog.mcParticlesRMS() , TheLog.mcParticlesRMS() , "Gene")
  webPage.addTable5Col("TT MCHits" , ReferenceLog.mcHitsTTMean() , TheLog.mcHitsTTMean() ,ReferenceLog.mcHitsTTRMS() , TheLog.mcHitsTTRMS() , "Gene")
  webPage.addTable5Col("IT MCHits" , ReferenceLog.mcHitsITMean() , TheLog.mcHitsITMean() ,ReferenceLog.mcHitsITRMS() , TheLog.mcHitsITRMS() , "Gene")
  webPage.addTable5Col("OT MCHits" , ReferenceLog.mcHitsOTMean() , TheLog.mcHitsOTMean() ,ReferenceLog.mcHitsOTRMS() , TheLog.mcHitsOTRMS() , "Gene")
  webPage.addTable5Col("Spd MCHits" , ReferenceLog.mcHitsSPDMean() , TheLog.mcHitsSPDMean() ,ReferenceLog.mcHitsSPDRMS() , TheLog.mcHitsSPDRMS() , "Gene")
  webPage.addTable5Col("Prs MCHits" , ReferenceLog.mcHitsPRSMean() , TheLog.mcHitsPRSMean() ,ReferenceLog.mcHitsPRSRMS() , TheLog.mcHitsPRSRMS() , "Gene")
  webPage.addTable5Col("Ecal MCHits" , ReferenceLog.mcHitsECALMean() , TheLog.mcHitsECALMean() ,ReferenceLog.mcHitsECALRMS() , TheLog.mcHitsECALRMS() , "Gene")
  webPage.addTable5Col("Hcal MCHits" , ReferenceLog.mcHitsHCALMean() , TheLog.mcHitsHCALMean() ,ReferenceLog.mcHitsHCALRMS() , TheLog.mcHitsHCALRMS() , "Gene")


# Timing table
  webPage.addTable5Col("Event Loop" , ReferenceLog.timeEventLoop() , TheLog.timeEventLoop() , ReferenceLog.timeEventLoopTOT() ,  TheLog.timeEventLoopTOT() , "Time")
  webPage.addTable5Col("Gauss" , ReferenceLog.timeGauss() , TheLog.timeGauss() , ReferenceLog.timeGaussTOT() ,  TheLog.timeGaussTOT() , "Time")
  webPage.addTable5Col("Generator" , ReferenceLog.timeGenerator() , TheLog.timeGenerator() , ReferenceLog.timeGeneratorTOT() ,  TheLog.timeGeneratorTOT() , "Time")
  webPage.addTable5Col("Simulation" , ReferenceLog.timeSimulation() , TheLog.timeSimulation() , ReferenceLog.timeSimulationTOT() ,  TheLog.timeSimulationTOT() , "Time")
  webPage.addTable5Col("GaussSim" , ReferenceLog.timeGaussSim() , TheLog.timeGaussSim() , ReferenceLog.timeGaussSimTOT() ,  TheLog.timeGaussSimTOT() , "Time")
  webPage.addTable5Col("GigaStore" , ReferenceLog.timeGigaStore() , TheLog.timeGigaStore() , ReferenceLog.timeGigaStoreTOT() ,  TheLog.timeGigaStoreTOT() , "Time" )
  webPage.addTable5Col("Geo" , ReferenceLog.timeGeo() , TheLog.timeGeo() , ReferenceLog.timeGeoTOT() ,  TheLog.timeGeoTOT() , "Time" )
  webPage.addTable5Col("GenerationToSimulation" , ReferenceLog.timeGenerationToSimulation() , TheLog.timeGenerationToSimulation() , ReferenceLog.timeGenerationToSimulationTOT() ,  TheLog.timeGenerationToSimulationTOT() , "Time" )
  webPage.addTable5Col("GiGaFlushAlgorithm" , ReferenceLog.timeGiGaFlushAlgorithm() , TheLog.timeGiGaFlushAlgorithm() , ReferenceLog.timeGiGaFlushAlgorithmTOT() ,  TheLog.timeGiGaFlushAlgorithmTOT() , "Time")
  webPage.addTable5Col("SimulationToMCTruth" , ReferenceLog.timeSimulationToMCTruth() , TheLog.timeSimulationToMCTruth() , ReferenceLog.timeSimulationToMCTruthTOT() ,  TheLog.timeSimulationToMCTruthTOT() , "Time")
  webPage.addTable5Col("DetectorsHits" , ReferenceLog.timeDetectorsHits() , TheLog.timeDetectorsHits() , ReferenceLog.timeDetectorsHitsTOT() ,  TheLog.timeDetectorsHitsTOT() , "Time")
  webPage.addTable5Col("SimMonitor" , ReferenceLog.timeSimMonitor() , TheLog.timeSimMonitor() , ReferenceLog.timeSimMonitorTOT() ,  TheLog.timeSimMonitorTOT() , "Time")

# Errors table
  webPage.addTable3Col("Exceptions" , ReferenceLog.excepCount() , TheLog.excepCount() , "Err")                
  webPage.addTable3Col("Errors" , ReferenceLog.errCount() , TheLog.errCount() , "Err")                
  webPage.addTable3Col("Warnings" , ReferenceLog.warCount() , TheLog.warCount() , "Err")                
  webPage.addTable3Col("G4 Exceptions" , ReferenceLog.g4Count() , TheLog.g4Count() , "Err")                


  ##### Ref Histos    
  if options.verbose:
    print "Read reference histogram file: " + options.referenceHistoName 
  refFile = TFile( options.referenceHistoName )

  # if Partcle Gun sample -> switch off some histos
  PGun=False
  etype = TheLog.eventType()
  if( etype[0]=="5"):
    PGun=True
    print "Running monitoring on PGun", etype
  
# MC truth monitoring histos
  ZOrigPartREF = gDirectory.Get( 'MCTruthMonitor/102' ) 
  MomPartREF = gDirectory.Get( 'MCTruthMonitor/103' ) 
  MomPrimPartREF = gDirectory.Get( 'MCTruthMonitor/104' ) 
  MomProtPartREF = gDirectory.Get( 'MCTruthMonitor/105' ) 
  VertexTypeREF = gDirectory.Get( 'MCTruthMonitor/201' ) 
  ZPosVertREF = gDirectory.Get( 'MCTruthMonitor/202' ) 
  ZPosVertZoomREF = gDirectory.Get( 'MCTruthMonitor/203' ) 
  TimeVertREF = gDirectory.Get( 'MCTruthMonitor/204' ) 

# VELO Simulation monitoring histos
  VeloTOFREF = gDirectory.Get( 'Velo/VeloGaussMoni/TOF' ) 
  VeloTOFPUREF = gDirectory.Get( 'Velo/VeloGaussMoni/TOFPU' ) 
  VeloeDepSiREF = gDirectory.Get( 'Velo/VeloGaussMoni/eDepSi' )
  VeloeDepSiPUREF = gDirectory.Get( 'Velo/VeloGaussMoni/eDepSiPU' )
  VeloeMCPartREF = gDirectory.Get( 'Velo/VeloGaussMoni/eMCPart' )
  VeloeMCPartPUREF = gDirectory.Get( 'Velo/VeloGaussMoni/eMCPartPU' )
  VeloNumHitsPUREF = gDirectory.Get( 'Velo/VeloGaussMoni/nMCPUHits' )
  VeloEntryZXREF = gDirectory.Get( 'Velo/VeloGaussMoni/entryZX' )
  VeloEntryXYREF = gDirectory.Get( 'Velo/VeloGaussMoni/entryXY' )

# TT Simulation monitoring histos
  TTTOF100REF = gDirectory.Get( 'TTHitMonitor/100' ) 
  TTNumHitsREF = gDirectory.Get( 'TTHitMonitor/1' ) 
  TTPMagREF = gDirectory.Get( 'TTHitMonitor/4' ) 
  TTloss300REF = gDirectory.Get( 'TTHitMonitor/300' ) 
  TTpathlengthREF = gDirectory.Get( 'TTHitMonitor/path length' )
  TTXvsYREF = gDirectory.Get( 'TTHitMonitor/200' ) 

# IT Simulation monitoring histos
  ITTOF100REF = gDirectory.Get( 'ITHitMonitor/100' ) 
  ITNumHitsREF = gDirectory.Get( 'ITHitMonitor/1' ) 
  ITPMagREF = gDirectory.Get( 'ITHitMonitor/4' ) 
  ITloss300REF = gDirectory.Get( 'ITHitMonitor/300' ) 
  ITpathlengthREF = gDirectory.Get( 'ITHitMonitor/path length' ) 
  ITXvsYREF = gDirectory.Get( 'ITHitMonitor/200' ) 

# OT Simulation monitoring histos
  OTTOF100REF = gDirectory.Get( 'OTHitMonitor/100' ) 
  OTNumHitsREF = gDirectory.Get( 'OTHitMonitor/1' ) 
  OTPMagREF = gDirectory.Get( 'OTHitMonitor/4' ) 
  OTloss300REF = gDirectory.Get( 'OTHitMonitor/300' ) 
  OTpathlengthREF = gDirectory.Get( 'OTHitMonitor/path length' )
  OTXvsYREF = gDirectory.Get( 'OTHitMonitor/200' ) 

# 13.01.2009: FROM THE NEXT RELEASE the ECAL/HCAL/PRS/SPD histos will have the correct IDs, now swap "inner" with "outer" 
# ECAL Simulation monitoring histos
  ECALSubhitsREF = gDirectory.Get( 'EcalMonitor/11' ) 
  ECALInnerSubhitsREF = gDirectory.Get( 'EcalMonitor/113' ) 
  ECALMiddleSubhitsREF = gDirectory.Get( 'EcalMonitor/112' ) 
  ECALOuterSubhitsREF = gDirectory.Get( 'EcalMonitor/111' ) 
  ECALEnWeightSubhitsREF = gDirectory.Get( 'EcalMonitor/12' ) 
  ECALInnerEnWeightSubhitsREF = gDirectory.Get( 'EcalMonitor/123' ) 
  ECALMiddleEnWeightSubhitsREF = gDirectory.Get( 'EcalMonitor/122' )
  ECALOuterEnWeightSubhitsREF = gDirectory.Get( 'EcalMonitor/121' ) 
  ECALAccEnREF = gDirectory.Get( 'EcalMonitor/13' ) 
  ECALInnerAccEnREF = gDirectory.Get( 'EcalMonitor/133' ) 
  ECALMiddleAccEnREF = gDirectory.Get( 'EcalMonitor/132' )
  ECALOuterAccEnREF = gDirectory.Get( 'EcalMonitor/131' ) 
  ECALNumSubhitsREF = gDirectory.Get( 'EcalMonitor/14' )
  ECALNumSubhitsBCm1REF = gDirectory.Get( 'EcalMonitor/140' )
  ECALNumSubhitsBC0REF = gDirectory.Get( 'EcalMonitor/141' )
  ECALNumSubhitsBCp1REF = gDirectory.Get( 'EcalMonitor/142' )

  # HCAL Simulation monitoring histos
  HCALSubhitsREF = gDirectory.Get( 'HcalMonitor/11' ) 
  HCALInnerSubhitsREF = gDirectory.Get( 'HcalMonitor/112' ) 
  HCALOuterSubhitsREF = gDirectory.Get( 'HcalMonitor/111' ) 
  HCALEnWeightSubhitsREF = gDirectory.Get( 'HcalMonitor/12' ) 
  HCALInnerEnWeightSubhitsREF = gDirectory.Get( 'HcalMonitor/122' ) 
  HCALOuterEnWeightSubhitsREF = gDirectory.Get( 'HcalMonitor/121' )
  HCALAccEnREF = gDirectory.Get( 'HcalMonitor/13' ) 
  HCALInnerAccEnREF = gDirectory.Get( 'HcalMonitor/132' ) 
  HCALOuterAccEnREF = gDirectory.Get( 'HcalMonitor/131' )
  HCALNumSubhitsREF = gDirectory.Get( 'HcalMonitor/14' )
  HCALNumSubhitsBCm1REF = gDirectory.Get( 'HcalMonitor/140' )
  HCALNumSubhitsBC0REF = gDirectory.Get( 'HcalMonitor/141' )
  HCALNumSubhitsBCp1REF = gDirectory.Get( 'HcalMonitor/142' )

  # PRS Simulation monitoring histos
  PRSSubhitsREF = gDirectory.Get( 'PrsMonitor/11' ) 
  PRSInnerSubhitsREF = gDirectory.Get( 'PrsMonitor/113' ) 
  PRSMiddleSubhitsREF = gDirectory.Get( 'PrsMonitor/112' ) 
  PRSOuterSubhitsREF = gDirectory.Get( 'PrsMonitor/111' ) 
  PRSEnWeightSubhitsREF = gDirectory.Get( 'PrsMonitor/12' ) 
  PRSInnerEnWeightSubhitsREF = gDirectory.Get( 'PrsMonitor/123' ) 
  PRSMiddleEnWeightSubhitsREF = gDirectory.Get( 'PrsMonitor/122' )
  PRSOuterEnWeightSubhitsREF = gDirectory.Get( 'PrsMonitor/121' )
  PRSAccEnREF = gDirectory.Get( 'PrsMonitor/13' ) 
  PRSInnerAccEnREF = gDirectory.Get( 'PrsMonitor/133' ) 
  PRSMiddleAccEnREF = gDirectory.Get( 'PrsMonitor/132' )
  PRSOuterAccEnREF = gDirectory.Get( 'PrsMonitor/131' )
  PRSNumSubhitsREF = gDirectory.Get( 'PrsMonitor/14' )
  PRSNumSubhitsBCm1REF = gDirectory.Get( 'PrsMonitor/140' )
  PRSNumSubhitsBC0REF = gDirectory.Get( 'PrsMonitor/141' )
  PRSNumSubhitsBCp1REF = gDirectory.Get( 'PrsMonitor/142' )

  # SPD Simulation monitoring histos
  SPDSubhitsREF = gDirectory.Get( 'SpdMonitor/11' ) 
  SPDInnerSubhitsREF = gDirectory.Get( 'SpdMonitor/113' ) 
  SPDMiddleSubhitsREF = gDirectory.Get( 'SpdMonitor/112' ) 
  SPDOuterSubhitsREF = gDirectory.Get( 'SpdMonitor/111' ) 
  SPDEnWeightSubhitsREF = gDirectory.Get( 'SpdMonitor/12' ) 
  SPDInnerEnWeightSubhitsREF = gDirectory.Get( 'SpdMonitor/123' ) 
  SPDMiddleEnWeightSubhitsREF = gDirectory.Get( 'SpdMonitor/122' )
  SPDOuterEnWeightSubhitsREF = gDirectory.Get( 'SpdMonitor/121' )
  SPDAccEnREF = gDirectory.Get( 'SpdMonitor/13' ) 
  SPDInnerAccEnREF = gDirectory.Get( 'SpdMonitor/133' ) 
  SPDMiddleAccEnREF = gDirectory.Get( 'SpdMonitor/132' )
  SPDOuterAccEnREF = gDirectory.Get( 'SpdMonitor/131' )
  SPDNumSubhitsREF = gDirectory.Get( 'SpdMonitor/14' )
  SPDNumSubhitsBCm1REF = gDirectory.Get( 'SpdMonitor/140' )
  SPDNumSubhitsBC0REF = gDirectory.Get( 'SpdMonitor/141' )
  SPDNumSubhitsBCp1REF = gDirectory.Get( 'SpdMonitor/142' )

# RICH Simulation monitoring histos

  RICHSatHitsAeroR1REF = gDirectory.Get( 'RICHG4HISTOSET2/129' )
  RICHSatHitsC4F10R1REF = gDirectory.Get( 'RICHG4HISTOSET2/159' )
  RICHSatHitsCF4R1REF = gDirectory.Get( 'RICHG4HISTOSET2/179' )
  RICHMomPartR1REF = gDirectory.Get( 'RICHG4HISTOSET2/501' )
  RICHMomPartR2REF = gDirectory.Get( 'RICHG4HISTOSET2/511' )

  RICHTvsMomAeroREF = gDirectory.Get( 'RICHG4HISTOSET2/600' )
  RICHTvsMomC4F10REF = gDirectory.Get( 'RICHG4HISTOSET2/610' )
  RICHTvsMomCF4REF = gDirectory.Get( 'RICHG4HISTOSET2/620' )

# MUON Simulation monitoring histos

  MU1000REF = gDirectory.Get( 'MuonHitChecker/1000' )
  MU1001REF = gDirectory.Get( 'MuonHitChecker/1001' )
  MU1002REF = gDirectory.Get( 'MuonHitChecker/1002' )
  MU1003REF = gDirectory.Get( 'MuonHitChecker/1003' )
  MU1004REF = gDirectory.Get( 'MuonHitChecker/1004' )
  MU1005REF = gDirectory.Get( 'MuonHitChecker/1005' )
  MU1006REF = gDirectory.Get( 'MuonHitChecker/1006' )
  MU1007REF = gDirectory.Get( 'MuonHitChecker/1007' )
  MU1008REF = gDirectory.Get( 'MuonHitChecker/1008' )
  MU1009REF = gDirectory.Get( 'MuonHitChecker/1009' )
  MU1010REF = gDirectory.Get( 'MuonHitChecker/1010' )
  MU1011REF = gDirectory.Get( 'MuonHitChecker/1011' )
  MU1012REF = gDirectory.Get( 'MuonHitChecker/1012' )
  MU1013REF = gDirectory.Get( 'MuonHitChecker/1013' )
  MU1014REF = gDirectory.Get( 'MuonHitChecker/1014' )
  MU1015REF = gDirectory.Get( 'MuonHitChecker/1015' )
  MU1016REF = gDirectory.Get( 'MuonHitChecker/1016' )
  MU1017REF = gDirectory.Get( 'MuonHitChecker/1017' )
  MU1018REF = gDirectory.Get( 'MuonHitChecker/1018' )
  MU1019REF = gDirectory.Get( 'MuonHitChecker/1019' )

  MU2000REF = gDirectory.Get( 'MuonHitChecker/2000' )
  MU2001REF = gDirectory.Get( 'MuonHitChecker/2001' )
  MU2002REF = gDirectory.Get( 'MuonHitChecker/2002' )
  MU2003REF = gDirectory.Get( 'MuonHitChecker/2003' )
  MU2004REF = gDirectory.Get( 'MuonHitChecker/2004' )
  MU2005REF = gDirectory.Get( 'MuonHitChecker/2005' )
  MU2006REF = gDirectory.Get( 'MuonHitChecker/2006' )
  MU2007REF = gDirectory.Get( 'MuonHitChecker/2007' )
  MU2008REF = gDirectory.Get( 'MuonHitChecker/2008' )
  MU2009REF = gDirectory.Get( 'MuonHitChecker/2009' )
  MU2010REF = gDirectory.Get( 'MuonHitChecker/2010' )
  MU2011REF = gDirectory.Get( 'MuonHitChecker/2011' )
  MU2012REF = gDirectory.Get( 'MuonHitChecker/2012' )
  MU2013REF = gDirectory.Get( 'MuonHitChecker/2013' )
  MU2014REF = gDirectory.Get( 'MuonHitChecker/2014' )
  MU2015REF = gDirectory.Get( 'MuonHitChecker/2015' )
  MU2016REF = gDirectory.Get( 'MuonHitChecker/2016' )
  MU2017REF = gDirectory.Get( 'MuonHitChecker/2017' )
  MU2018REF = gDirectory.Get( 'MuonHitChecker/2018' )
  MU2019REF = gDirectory.Get( 'MuonHitChecker/2019' )



  #####  Histos    
  if options.verbose:
    print "Compare with histogram file: " + options.HistoName
  aFile = TFile( options.HistoName ) 

  # MC truth monitoring histos
  ZOrigPart = gDirectory.Get( 'MCTruthMonitor/102' ) 
  MomPart = gDirectory.Get( 'MCTruthMonitor/103' ) 
  MomPrimPart = gDirectory.Get( 'MCTruthMonitor/104' ) 
  MomProtPart = gDirectory.Get( 'MCTruthMonitor/105' ) 
  VertexType = gDirectory.Get( 'MCTruthMonitor/201' ) 
  ZPosVert = gDirectory.Get( 'MCTruthMonitor/202' ) 
  ZPosVertZoom = gDirectory.Get( 'MCTruthMonitor/203' ) 
  TimeVert = gDirectory.Get( 'MCTruthMonitor/204' ) 

  # VELO Simulation monitoring histos
  VeloTOF = gDirectory.Get( 'Velo/VeloGaussMoni/TOF' ) 
  VeloTOFPU = gDirectory.Get( 'Velo/VeloGaussMoni/TOFPU' ) 
  VeloeDepSi = gDirectory.Get( 'Velo/VeloGaussMoni/eDepSi' )
  VeloeDepSiPU = gDirectory.Get( 'Velo/VeloGaussMoni/eDepSiPU' )
  VeloeMCPart = gDirectory.Get( 'Velo/VeloGaussMoni/eMCPart' )
  VeloeMCPartPU = gDirectory.Get( 'Velo/VeloGaussMoni/eMCPartPU' )
  VeloNumHitsPU = gDirectory.Get( 'Velo/VeloGaussMoni/nMCPUHits' )
  VeloEntryZX = gDirectory.Get( 'Velo/VeloGaussMoni/entryZX' )
  VeloEntryXY = gDirectory.Get( 'Velo/VeloGaussMoni/entryXY' )

  # TT Simulation monitoring histos
  TTTOF100 = gDirectory.Get( 'TTHitMonitor/100' )
  TTNumHits = gDirectory.Get( 'TTHitMonitor/1' )
  TTPMag = gDirectory.Get( 'TTHitMonitor/4' )
  TTloss300 = gDirectory.Get( 'TTHitMonitor/300' ) 
  TTpathlength = gDirectory.Get( 'TTHitMonitor/path length' ) 
  TTXvsY = gDirectory.Get( 'TTHitMonitor/200' )
  
# IT Simulation monitoring histos
  ITTOF100 = gDirectory.Get( 'ITHitMonitor/100' ) 
  ITNumHits = gDirectory.Get( 'ITHitMonitor/1' ) 
  ITPMag = gDirectory.Get( 'ITHitMonitor/4' ) 
  ITloss300 = gDirectory.Get( 'ITHitMonitor/300' ) 
  ITpathlength = gDirectory.Get( 'ITHitMonitor/path length' ) 
  ITXvsY = gDirectory.Get( 'ITHitMonitor/200' ) 

# OT Simulation monitoring histos
  OTTOF100 = gDirectory.Get( 'OTHitMonitor/100' ) 
  OTTOF101 = gDirectory.Get( 'OTHitMonitor/101' )
  OTTOF102 = gDirectory.Get( 'OTHitMonitor/102' )
  OTNumHits = gDirectory.Get( 'OTHitMonitor/1' ) 
  OTPMag = gDirectory.Get( 'OTHitMonitor/4' ) 
  OTloss300 = gDirectory.Get( 'OTHitMonitor/300' ) 
  OTloss301 = gDirectory.Get( 'OTHitMonitor/301' ) 
  OTloss302 = gDirectory.Get( 'OTHitMonitor/302' ) 
  OTpathlength = gDirectory.Get( 'OTHitMonitor/path length' ) 
  OTXvsY = gDirectory.Get( 'OTHitMonitor/200' )

# 13.01.2009: FROM THE NEXT RELEASE the ECAL/HCAL/PRS/SPD histos will have the correct IDs, now swap "inner" with "outer" 
# ECAL Simulation monitoring histos

  ECALSubhits = gDirectory.Get( 'EcalMonitor/11' ) 
  ECALInnerSubhits = gDirectory.Get( 'EcalMonitor/113' ) 
  ECALMiddleSubhits = gDirectory.Get( 'EcalMonitor/112' ) 
  ECALOuterSubhits = gDirectory.Get( 'EcalMonitor/111' ) 
  ECALEnWeightSubhits = gDirectory.Get( 'EcalMonitor/12' ) 
  ECALInnerEnWeightSubhits = gDirectory.Get( 'EcalMonitor/123' ) 
  ECALMiddleEnWeightSubhits = gDirectory.Get( 'EcalMonitor/122' )
  ECALOuterEnWeightSubhits = gDirectory.Get( 'EcalMonitor/121' ) 
  ECALAccEn = gDirectory.Get( 'EcalMonitor/13' ) 
  ECALInnerAccEn = gDirectory.Get( 'EcalMonitor/133' ) 
  ECALMiddleAccEn = gDirectory.Get( 'EcalMonitor/132' )
  ECALOuterAccEn = gDirectory.Get( 'EcalMonitor/131' ) 
  ECALNumSubhits = gDirectory.Get( 'EcalMonitor/14' )
  ECALNumSubhitsBCm1 = gDirectory.Get( 'EcalMonitor/140' )
  ECALNumSubhitsBC0 = gDirectory.Get( 'EcalMonitor/141' )
  ECALNumSubhitsBCp1 = gDirectory.Get( 'EcalMonitor/142' )

  # HCAL Simulation monitoring histos

  HCALSubhits = gDirectory.Get( 'HcalMonitor/11' ) 
  HCALInnerSubhits = gDirectory.Get( 'HcalMonitor/112' ) 
  HCALOuterSubhits = gDirectory.Get( 'HcalMonitor/111' ) 
  HCALEnWeightSubhits = gDirectory.Get( 'HcalMonitor/12' ) 
  HCALInnerEnWeightSubhits = gDirectory.Get( 'HcalMonitor/122' ) 
  HCALOuterEnWeightSubhits = gDirectory.Get( 'HcalMonitor/121' )
  HCALAccEn = gDirectory.Get( 'HcalMonitor/13' ) 
  HCALInnerAccEn = gDirectory.Get( 'HcalMonitor/132' ) 
  HCALOuterAccEn = gDirectory.Get( 'HcalMonitor/131' )
  HCALNumSubhits = gDirectory.Get( 'HcalMonitor/14' )
  HCALNumSubhitsBCm1 = gDirectory.Get( 'HcalMonitor/140' )
  HCALNumSubhitsBC0 = gDirectory.Get( 'HcalMonitor/141' )
  HCALNumSubhitsBCp1 = gDirectory.Get( 'HcalMonitor/142' )

  # PRS Simulation monitoring histos

  PRSSubhits = gDirectory.Get( 'PrsMonitor/11' ) 
  PRSInnerSubhits = gDirectory.Get( 'PrsMonitor/113' ) 
  PRSMiddleSubhits = gDirectory.Get( 'PrsMonitor/112' ) 
  PRSOuterSubhits = gDirectory.Get( 'PrsMonitor/111' ) 
  PRSEnWeightSubhits = gDirectory.Get( 'PrsMonitor/12' ) 
  PRSInnerEnWeightSubhits = gDirectory.Get( 'PrsMonitor/123' ) 
  PRSMiddleEnWeightSubhits = gDirectory.Get( 'PrsMonitor/122' )
  PRSOuterEnWeightSubhits = gDirectory.Get( 'PrsMonitor/121' )
  PRSAccEn = gDirectory.Get( 'PrsMonitor/13' ) 
  PRSInnerAccEn = gDirectory.Get( 'PrsMonitor/133' ) 
  PRSMiddleAccEn = gDirectory.Get( 'PrsMonitor/132' )
  PRSOuterAccEn = gDirectory.Get( 'PrsMonitor/131' )
  PRSNumSubhits = gDirectory.Get( 'PrsMonitor/14' )
  PRSNumSubhitsBCm1 = gDirectory.Get( 'PrsMonitor/140' )
  PRSNumSubhitsBC0 = gDirectory.Get( 'PrsMonitor/141' )
  PRSNumSubhitsBCp1 = gDirectory.Get( 'PrsMonitor/142' )

  # SPD Simulation monitoring histos

  SPDSubhits = gDirectory.Get( 'SpdMonitor/11' ) 
  SPDInnerSubhits = gDirectory.Get( 'SpdMonitor/113' ) 
  SPDMiddleSubhits = gDirectory.Get( 'SpdMonitor/112' ) 
  SPDOuterSubhits = gDirectory.Get( 'SpdMonitor/111' ) 
  SPDEnWeightSubhits = gDirectory.Get( 'SpdMonitor/12' ) 
  SPDInnerEnWeightSubhits = gDirectory.Get( 'SpdMonitor/123' ) 
  SPDMiddleEnWeightSubhits = gDirectory.Get( 'SpdMonitor/122' )
  SPDOuterEnWeightSubhits = gDirectory.Get( 'SpdMonitor/121' )
  SPDAccEn = gDirectory.Get( 'SpdMonitor/13' ) 
  SPDInnerAccEn = gDirectory.Get( 'SpdMonitor/133' ) 
  SPDMiddleAccEn = gDirectory.Get( 'SpdMonitor/132' )
  SPDOuterAccEn = gDirectory.Get( 'SpdMonitor/131' )
  SPDNumSubhits = gDirectory.Get( 'SpdMonitor/14' )
  SPDNumSubhitsBCm1 = gDirectory.Get( 'SpdMonitor/140' )
  SPDNumSubhitsBC0 = gDirectory.Get( 'SpdMonitor/141' )
  SPDNumSubhitsBCp1 = gDirectory.Get( 'SpdMonitor/142' )


# RICH Simulation monitoring histos

  RICHSatHitsAeroR1 = gDirectory.Get( 'RICHG4HISTOSET2/129' )
  RICHSatHitsC4F10R1 = gDirectory.Get( 'RICHG4HISTOSET2/159' )
  RICHSatHitsCF4R1 = gDirectory.Get( 'RICHG4HISTOSET2/179' )
  RICHMomPartR1 = gDirectory.Get( 'RICHG4HISTOSET2/501' )
  RICHMomPartR2 = gDirectory.Get( 'RICHG4HISTOSET2/511' )

  RICHTvsMomAero = gDirectory.Get( 'RICHG4HISTOSET2/600' )
  RICHTvsMomC4F10 = gDirectory.Get( 'RICHG4HISTOSET2/610' )
  RICHTvsMomCF4 = gDirectory.Get( 'RICHG4HISTOSET2/620' )


# MUON Simulation monitoring histos

  MU1000 = gDirectory.Get( 'MuonHitChecker/1000' )
  MU1001 = gDirectory.Get( 'MuonHitChecker/1001' )
  MU1002 = gDirectory.Get( 'MuonHitChecker/1002' )
  MU1003 = gDirectory.Get( 'MuonHitChecker/1003' )
  MU1004 = gDirectory.Get( 'MuonHitChecker/1004' )
  MU1005 = gDirectory.Get( 'MuonHitChecker/1005' )
  MU1006 = gDirectory.Get( 'MuonHitChecker/1006' )
  MU1007 = gDirectory.Get( 'MuonHitChecker/1007' )
  MU1008 = gDirectory.Get( 'MuonHitChecker/1008' )
  MU1009 = gDirectory.Get( 'MuonHitChecker/1009' )
  MU1010 = gDirectory.Get( 'MuonHitChecker/1010' )
  MU1011 = gDirectory.Get( 'MuonHitChecker/1011' )
  MU1012 = gDirectory.Get( 'MuonHitChecker/1012' )
  MU1013 = gDirectory.Get( 'MuonHitChecker/1013' )
  MU1014 = gDirectory.Get( 'MuonHitChecker/1014' )
  MU1015 = gDirectory.Get( 'MuonHitChecker/1015' )
  MU1016 = gDirectory.Get( 'MuonHitChecker/1016' )
  MU1017 = gDirectory.Get( 'MuonHitChecker/1017' )
  MU1018 = gDirectory.Get( 'MuonHitChecker/1018' )
  MU1019 = gDirectory.Get( 'MuonHitChecker/1019' )

  MU2000 = gDirectory.Get( 'MuonHitChecker/2000' )
  MU2001 = gDirectory.Get( 'MuonHitChecker/2001' )
  MU2002 = gDirectory.Get( 'MuonHitChecker/2002' )
  MU2003 = gDirectory.Get( 'MuonHitChecker/2003' )
  MU2004 = gDirectory.Get( 'MuonHitChecker/2004' )
  MU2005 = gDirectory.Get( 'MuonHitChecker/2005' )
  MU2006 = gDirectory.Get( 'MuonHitChecker/2006' )
  MU2007 = gDirectory.Get( 'MuonHitChecker/2007' )
  MU2008 = gDirectory.Get( 'MuonHitChecker/2008' )
  MU2009 = gDirectory.Get( 'MuonHitChecker/2009' )
  MU2010 = gDirectory.Get( 'MuonHitChecker/2010' )
  MU2011 = gDirectory.Get( 'MuonHitChecker/2011' )
  MU2012 = gDirectory.Get( 'MuonHitChecker/2012' )
  MU2013 = gDirectory.Get( 'MuonHitChecker/2013' )
  MU2014 = gDirectory.Get( 'MuonHitChecker/2014' )
  MU2015 = gDirectory.Get( 'MuonHitChecker/2015' )
  MU2016 = gDirectory.Get( 'MuonHitChecker/2016' )
  MU2017 = gDirectory.Get( 'MuonHitChecker/2017' )
  MU2018 = gDirectory.Get( 'MuonHitChecker/2018' )
  MU2019 = gDirectory.Get( 'MuonHitChecker/2019' )

  
  c1 = TCanvas( 'c1' , 'Gauss' , 200 , 10 , 800 , 800 ) 
  
  gStyle.SetOptStat(2210)
  

  if (options.kstopt != ""):
    print "Read kolmogorov options from command line: ", options.kstopt
  kopt=options.kstopt

  ####################################################################
  if(not (ZOrigPartREF==None or ZOrigPart==None)):
    ZOrigPartRefHist = SimulationHisto( c1 , ZOrigPartREF  , ZOrigPart ,
                                        "" , "" , "Z origin of all particles" ,
                                        "ZOrigPart" , False )
    ok_add=ZOrigPartRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ZOrigPartRefHist,"MC",kopt)

  ####################################################################
  if(not (MomPartREF==None or MomPart==None)):
    MomPartRefHist = SimulationHisto( c1 , MomPartREF  , MomPart ,
                                      "" , "" , "Momentum of all particles" ,
                                      "MomPart" , False )
    ok_add=MomPartRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(MomPartRefHist,"MC",kopt)

  ####################################################################
  if(not (MomPrimPartREF==None or MomPrimPart==None)):
    MomPrimPartRefHist = SimulationHisto( c1 , MomPrimPartREF  , MomPrimPart ,
                                          "" , "" , "Momentum of primary particles" ,
                                          "MomPrimPart" , False )
    ok_add=MomPrimPartRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(MomPrimPartRefHist,"MC",kopt)

  ####################################################################
  if(not (MomProtPartREF==None or MomProtPart==None)):
    MomProtPartRefHist = SimulationHisto( c1 , MomProtPartREF  , MomProtPart ,
                                          "" , "" , "Momentum of protons particles" ,
                                          "MomProtPart" , False )
    ok_add=MomProtPartRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(MomProtPartRefHist,"MC",kopt)

  ####################################################################
  if(not (VertexTypeREF==None or VertexType==None)):
    VertexTypeRefHist = SimulationHisto( c1 , VertexTypeREF  , VertexType ,
                                         "" , "" , "Vertex type" ,
                                         "VertexType" , False )
    ok_add=VertexTypeRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(VertexTypeRefHist,"MC" ,kopt)

  ####################################################################
  if(not (ZPosVertREF==None or ZPosVert==None)):
    ZPosVertRefHist = SimulationHisto( c1 , ZPosVertREF  , ZPosVert ,
                                       "" , "" , "Z position of all vertices (mm)" ,
                                       "ZPosVert" , False )
    ok_add=ZPosVertRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ZPosVertRefHist,"MC" ,kopt)

  ####################################################################
  if(not (ZPosVertZoomREF==None or ZPosVertZoom==None)):
    ZPosVertZoomRefHist = SimulationHisto( c1 , ZPosVertZoomREF  , ZPosVertZoom ,
                                           "" , "" , "Z position of all vertices Zoomed (mm)" ,
                                           "ZPosVertZoom" , False )
    ok_add=ZPosVertZoomRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ZPosVertZoomRefHist,"MC" ,kopt)

  ####################################################################
  if(not (TimeVertREF==None or TimeVert==None)):
    TimeVertRefHist = SimulationHisto( c1 , TimeVertREF  , TimeVert ,
                                       "" , "" , "Time of all vertices (ns)" ,
                                       "TimeVert" , False )
    ok_add=TimeVertRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(TimeVertRefHist,"MC" ,kopt)

  ####################################################################
  if(not (VeloTOFREF==None or VeloTOF==None)):
    VeloTOFRefHist = SimulationHisto( c1 , VeloTOFREF  , VeloTOF ,
                                      "" , "" , "Time of Flight" ,
                                      "VeloTOF" , False )
    ok_add=VeloTOFRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(VeloTOFRefHist,"Velo" ,kopt)

  ####################################################################
  if (not PGun):
    if(not (VeloTOFPUREF==None or VeloTOFPU==None)):  
      VeloTOFPURefHist = SimulationHisto( c1 , VeloTOFPUREF  , VeloTOFPU ,
                                          "" , "" , "PileUP: Time of Flight" ,
                                          "VeloTOFPU" , False )
      ok_add=VeloTOFPURefHist.plot()
      if(ok_add==1):
        webPage.addPlot(VeloTOFPURefHist,"Velo" ,kopt)

  ####################################################################
  if(not (VeloeDepSiREF==None or VeloeDepSi==None)):  
    VeloeDepSiRefHist = SimulationHisto( c1 , VeloeDepSiREF  , VeloeDepSi ,
                                         "" , "" , "Energy Deposited in Si" ,
                                         "VeloeDepSi" , False )
    ok_add=VeloeDepSiRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(VeloeDepSiRefHist,"Velo" ,kopt)

  ####################################################################
  if (not PGun):
    if(not (VeloeDepSiPUREF==None or VeloeDepSiPU==None)):  
      VeloeDepSiPURefHist = SimulationHisto( c1 , VeloeDepSiPUREF  , VeloeDepSiPU ,
                                             "" , "" , "PileUp: Energy Deposited in Si" ,
                                             "VeloeDepSiPU" , False )
      ok_add=VeloeDepSiPURefHist.plot()
      if(ok_add==1):
        webPage.addPlot(VeloeDepSiPURefHist,"Velo" ,kopt)

  ####################################################################
  if (not PGun):
    if(not (VeloNumHitsPUREF==None or VeloNumHitsPU==None)):  
      VeloNumberOfHitsPURefHist = SimulationHisto( c1 , VeloNumHitsPUREF  , VeloNumHitsPU ,
                                                   "N(hits)" , "Events" , "Pile Up:Number of Hits in Velo per event" ,
                                                   "VeloNHitsPU" , False )
      ok_add=VeloNumberOfHitsPURefHist.plot()
      if(ok_add==1):
        webPage.addPlot(VeloNumberOfHitsPURefHist,"Velo" ,kopt)

  ####################################################################
  if (not PGun):
    if(not (VeloEntryZXREF==None or VeloEntryZX==None) ):  
      VeloEntryZXRefHist = SimulationHisto( c1 , VeloEntryZXREF  , VeloEntryZX ,
                                            " " , " " , "Particle Entry Point in Si(cm) - ZX Plane" ,
                                            "VeloEntryZX" , False )
      ok_add=VeloEntryZXRefHist.plot()
      if(ok_add==1):
        webPage.addPlot(VeloEntryZXRefHist,"Velo" ,kopt)
  

  ####################################################################
  if (not PGun):
    if(not (VeloEntryXYREF==None or VeloEntryXY==None)):  
      VeloEntryXYRefHist = SimulationHisto( c1 , VeloEntryXYREF  , VeloEntryXY ,
                                            " " , " " , "Particle Entry Point in Si(cm) - XY Plane" ,
                                            "VeloEntryXY" , False )
      ok_add=VeloEntryXYRefHist.plot()
      if(ok_add==1):
        webPage.addPlot(VeloEntryXYRefHist,"Velo" ,kopt)

  ####################################################################
  if(not (TTTOF100REF==None or TTTOF100==None)):  
    TTTOF100RefHist = SimulationHisto( c1 , TTTOF100REF  , TTTOF100 ,
                                       " " , " " , "Time of Flight100 TT" ,
                                       "TTTOF100" , False )
    ok_add=TTTOF100RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(TTTOF100RefHist,"TT" ,kopt)

  ####################################################################
  if(not (TTNumHitsREF==None or TTNumHits==None)):  
    TTNumHitsRefHist = SimulationHisto( c1 , TTNumHitsREF  , TTNumHits ,
                                        " " , " " , "Num Hits TT " ,
                                        "TTNumHits" , False )
    ok_add=TTNumHitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(TTNumHitsRefHist,"TT" ,kopt)

  ####################################################################
  if(not (TTPMagREF==None or TTPMag==None)):  
    TTPMagRefHist = SimulationHisto( c1 , TTPMagREF  , TTPMag ,
                                     " " , " " , "P Mag TT" ,
                                     "TTPMag" , False )
    ok_add=TTPMagRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(TTPMagRefHist,"TT" ,kopt)

  ####################################################################
  if(not (TTloss300REF==None or TTloss300==None)):  
    TTloss300RefHist = SimulationHisto( c1 , TTloss300REF  , TTloss300 ,
                                        " " , " " , "lossHisto300 TT" ,
                                        "TTloss300" , False )
    ok_add=TTloss300RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(TTloss300RefHist,"TT" ,kopt)

  ####################################################################
  if(not (TTpathlengthREF==None or TTpathlength==None)):  
    TTpathlengthRefHist = SimulationHisto( c1 , TTpathlengthREF  , TTpathlength ,
                                           " " , " " , "path length TT" ,
                                           "TTpathlength" , False )
    ok_add=TTpathlengthRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(TTpathlengthRefHist,"TT" ,kopt)

  ####################################################################
  if (not PGun):
    if(not (TTXvsYREF==None or TTXvsY==None)):  
      TTXvsYRefHist = SimulationHisto( c1 , TTXvsYREF  , TTXvsY ,
                                       " " , " " , "X vs Y 200 TT" ,
                                       "TTXvsY" , False )
      ok_add=TTXvsYRefHist.plot()
      if(ok_add==1):
        webPage.addPlot(TTXvsYRefHist,"TT" ,kopt)

  ####################################################################
  if(not (ITTOF100REF==None or ITTOF100==None)):  
    ITTOF100RefHist = SimulationHisto( c1 , ITTOF100REF  , ITTOF100 ,
                                       " " , " " , "Time of Flight100 IT" ,
                                       "ITTOF100" , False )
    ok_add=ITTOF100RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ITTOF100RefHist,"IT" ,kopt)

  ####################################################################
  if(not (ITNumHitsREF==None or ITNumHits==None)):  
    ITNumHitsRefHist = SimulationHisto( c1 , ITNumHitsREF  , ITNumHits ,
                                        " " , " " , "Num Hits IT" ,
                                        "ITNumHits" , False )
    ok_add=ITNumHitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ITNumHitsRefHist,"IT" ,kopt)

  ####################################################################
  if(not (ITPMagREF==None or ITPMag==None)):  
    ITPMagRefHist = SimulationHisto( c1 , ITPMagREF  , ITPMag ,
                                     " " , " " , "P Mag IT" ,
                                     "ITPMag" , False )
    ok_add=ITPMagRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ITPMagRefHist,"IT" ,kopt)

  ####################################################################
  if(not (ITloss300REF==None or ITloss300==None)):  
    ITloss300RefHist = SimulationHisto( c1 , ITloss300REF  , ITloss300 ,
                                        " " , " " , "lossHisto300 IT" ,
                                        "ITloss300" , False )
    ok_add=ITloss300RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ITloss300RefHist,"IT" ,kopt)

  ####################################################################
  if(not (ITpathlengthREF==None or ITpathlength==None)):  
    ITpathlengthRefHist = SimulationHisto( c1 , ITpathlengthREF  , ITpathlength ,
                                           " " , " " , "path length IT" ,
                                           "ITpathlength" , False )
    ok_add=ITpathlengthRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ITpathlengthRefHist,"IT" ,kopt)

  ####################################################################
  if (not PGun):
    if(not (ITXvsYREF==None or ITXvsY==None)):  
      ITXvsYRefHist = SimulationHisto( c1 , ITXvsYREF  , ITXvsY ,
                                       " " , " " , "X vs Y 200 IT" ,
                                       "ITXvsY" , False )
      ok_add=ITXvsYRefHist.plot()
      if(ok_add==1):
        webPage.addPlot(ITXvsYRefHist,"IT" ,kopt)

  ####################################################################
  if(not (OTTOF100REF==None or OTTOF100==None)):  
    OTTOF100RefHist = SimulationHisto( c1 , OTTOF100REF  , OTTOF100 ,
                                       " " , " " , "Time of Flight100 OT" ,
                                       "OTTOF100" , False )
    ok_add=OTTOF100RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(OTTOF100RefHist,"OT" ,kopt)

  ####################################################################
  if(not (OTNumHitsREF==None or OTNumHits==None)):  
    OTNumHitsRefHist = SimulationHisto( c1 , OTNumHitsREF  , OTNumHits ,
                                        " " , " " , "Num Hits OT" ,
                                        "OTNumHits" , False )
    ok_add=OTNumHitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(OTNumHitsRefHist,"OT" ,kopt)

  ####################################################################
  if(not (OTPMagREF==None or OTPMag==None)):  
    OTPMagRefHist = SimulationHisto( c1 , OTPMagREF  , OTPMag ,
                                     " " , " " , "P Mag OT" ,
                                     "OTPMag" , False )
    ok_add=OTPMagRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(OTPMagRefHist,"OT" ,kopt)

  ####################################################################
  if(not (OTloss300REF==None or OTloss300==None)):  
    OTloss300RefHist = SimulationHisto( c1 , OTloss300REF  , OTloss300 ,
                                        " " , " " , "lossHisto300 OT" ,
                                        "OTloss300" , False )
    ok_add=OTloss300RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(OTloss300RefHist,"OT" ,kopt)

  ####################################################################
  if(not (OTpathlengthREF==None or OTpathlength==None)):  
    OTpathlengthRefHist = SimulationHisto( c1 , OTpathlengthREF  , OTpathlength ,
                                           " " , " " , "path length OT" ,
                                           "OTpathlength" , False )
    ok_add=OTpathlengthRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(OTpathlengthRefHist,"OT" ,kopt)

  ####################################################################
  if (not PGun):
    if(not (OTXvsYREF==None or OTXvsY==None)):  
      OTXvsYRefHist = SimulationHisto( c1 , OTXvsYREF  , OTXvsY ,
                                       " " , " " , "X vs Y 200 OT" ,
                                       "OTXvsY" , False )
      ok_add=OTXvsYRefHist.plot()
      if(ok_add==1):
        webPage.addPlot(OTXvsYRefHist,"OT" ,kopt)

  ####################################################################
  if(not (ECALSubhitsREF==None or ECALSubhits==None)):  
    ECALSubhitsRefHist = SimulationHisto( c1 , ECALSubhitsREF  , ECALSubhits ,
                                          " " , " " , "Subhits in the ECAL" ,
                                          "ECALSubhits" , False )
    ok_add=ECALSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ECALSubhitsRefHist,"ECAL" ,kopt)

  ####################################################################
  if(not (ECALInnerSubhitsREF==None or ECALInnerSubhits==None)):  
    ECALInnerSubhitsRefHist = SimulationHisto( c1 , ECALInnerSubhitsREF  , ECALInnerSubhits ,
                                               " " , " " , "Subhits in the Inner ECAL" ,
                                               "ECALInnerSubhits" , False )
    ok_add=ECALInnerSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ECALInnerSubhitsRefHist,"ECAL" ,kopt)

  ####################################################################
  if(not (ECALMiddleSubhitsREF==None or ECALMiddleSubhits==None)):  
    ECALMiddleSubhitsRefHist = SimulationHisto( c1 , ECALMiddleSubhitsREF  , ECALMiddleSubhits ,
                                                " " , " " , "Subhits in the Middle  ECAL" ,
                                                "ECALMiddleSubhits" , False )
    ok_add=ECALMiddleSubhitsRefHist.plot()
    webPage.addPlot(ECALMiddleSubhitsRefHist,"ECAL" ,kopt)

  ####################################################################
  if(not (ECALOuterSubhitsREF==None or ECALOuterSubhits==None)):  
    ECALOuterSubhitsRefHist = SimulationHisto( c1 , ECALOuterSubhitsREF  , ECALOuterSubhits ,
                                               " " , " " , "Subhits in the Outer ECAL" ,
                                               "ECALOuterSubhits" , False )
    ok_add=ECALOuterSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ECALOuterSubhitsRefHist,"ECAL" ,kopt)

  ####################################################################
  if(not (ECALEnWeightSubhitsREF==None or ECALEnWeightSubhits==None)):  
    ECALEnWeightSubhitsRefHist = SimulationHisto( c1 , ECALEnWeightSubhitsREF  , ECALEnWeightSubhits  ,
                                                  " " , " " , "Energy Weighted Subhits in the ECAL" ,
                                                  "ECALEnWeightSubhits" , False )
    ok_add=ECALEnWeightSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ECALEnWeightSubhitsRefHist,"ECAL" ,kopt)

  ####################################################################
  if(not (ECALInnerEnWeightSubhitsREF==None or ECALInnerEnWeightSubhits==None)):  
    ECALInnerEnWeightSubhitsRefHist = SimulationHisto( c1 , ECALInnerEnWeightSubhitsREF  , ECALInnerEnWeightSubhits  ,
                                                       " " , " " , "Energy Weighted Subhits in the Inner ECAL" ,
                                                       "ECALInnerEnWeightSubhits" , False )
    ok_add=ECALInnerEnWeightSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ECALInnerEnWeightSubhitsRefHist,"ECAL" ,kopt)

  ####################################################################
  if(not (ECALMiddleEnWeightSubhitsREF==None or ECALMiddleEnWeightSubhits==None)):  
    ECALMiddleEnWeightSubhitsRefHist = SimulationHisto( c1 , ECALMiddleEnWeightSubhitsREF  , ECALMiddleEnWeightSubhits  ,
                                                        " " , " " , "Energy Weighted Subhits in the Middle ECAL" ,
                                                        "ECALMiddleEnWeightSubhits" , False )
    ok_add=ECALMiddleEnWeightSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ECALMiddleEnWeightSubhitsRefHist,"ECAL" ,kopt)

  ####################################################################
  if(not (ECALOuterEnWeightSubhitsREF==None or ECALOuterEnWeightSubhits==None)):  
    ECALOuterEnWeightSubhitsRefHist = SimulationHisto( c1 , ECALOuterEnWeightSubhitsREF  , ECALOuterEnWeightSubhits  ,
                                                       " " , " " , "Energy Weighted Subhits in the Outer ECAL" ,
                                                       "ECALOuterEnWeightSubhits" , False )
    ok_add=ECALOuterEnWeightSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ECALOuterEnWeightSubhitsRefHist,"ECAL" ,kopt)

  ####################################################################
  if(not (ECALAccEnREF==None or ECALAccEn==None)):  
    ECALAccEnRefHist = SimulationHisto( c1 , ECALAccEnREF  , ECALAccEn  ,
                                        " " , " " , "Accumulated Energy in the ECAL" ,
                                        "ECALAccEn" , False )
    ok_add=ECALAccEnRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ECALAccEnRefHist,"ECAL" ,kopt)

  ####################################################################
  if(not (ECALInnerAccEnREF==None or ECALInnerAccEn==None)):  
    ECALInnerAccEnRefHist = SimulationHisto( c1 , ECALInnerAccEnREF  , ECALInnerAccEn  ,
                                             " " , " " , "Accumulated Energy in the Inner ECAL" ,
                                             "ECALInnerAccEn" , False )
    ok_add=ECALInnerAccEnRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ECALInnerAccEnRefHist,"ECAL" ,kopt)

  ####################################################################
  if(not (ECALMiddleAccEnREF==None or ECALMiddleAccEn==None)):  
    ECALMiddleAccEnRefHist = SimulationHisto( c1 , ECALMiddleAccEnREF  , ECALMiddleAccEn  ,
                                              " " , " " , "Accumulated Energy in the Middle ECAL" ,
                                              "ECALMiddleAccEn" , False )
    ok_add=ECALMiddleAccEnRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ECALMiddleAccEnRefHist,"ECAL" ,kopt)

  ####################################################################
  if(not (ECALOuterAccEnREF==None or ECALOuterAccEn==None)):  
    ECALOuterAccEnRefHist = SimulationHisto( c1 , ECALOuterAccEnREF  , ECALOuterAccEn  ,
                                             " " , " " , "Accumulated Energy in the Outer ECAL" ,
                                             "ECALOuterAccEn" , False )
    ok_add=ECALOuterAccEnRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ECALOuterAccEnRefHist,"ECAL" ,kopt)

  ####################################################################
  if(not (ECALNumSubhitsREF==None or ECALNumSubhits==None)):  
    ECALNumSubhitsRefHist = SimulationHisto( c1 , ECALNumSubhitsREF  , ECALNumSubhits  ,
                                             " " , " " , "Number of Subhits in the ECAL" ,
                                             "ECALNumSubhits" , False )
    ok_add=ECALNumSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ECALNumSubhitsRefHist,"ECAL" ,kopt)

  ####################################################################
  if(not (ECALNumSubhitsBCm1REF==None or ECALNumSubhitsBCm1==None)):  
    ECALNumSubhitsBCm1RefHist = SimulationHisto( c1 , ECALNumSubhitsBCm1REF  , ECALNumSubhitsBCm1  ,
                                                 " " , " " , "Number of Subhits in the ECAL (BC=-1)" ,
                                                 "ECALNumSubhitsBCm1" , False )
    ok_add=ECALNumSubhitsBCm1RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ECALNumSubhitsBCm1RefHist,"ECAL" ,kopt)

  ####################################################################
  if(not (ECALNumSubhitsBC0REF==None or ECALNumSubhitsBC0==None)):  
    ECALNumSubhitsBC0RefHist = SimulationHisto( c1 , ECALNumSubhitsBC0REF  , ECALNumSubhitsBC0  ,
                                                " " , " " , "Number of Subhits in the ECAL (BC=0)" ,
                                                "ECALNumSubhitsBC0" , False )
    ok_add=ECALNumSubhitsBC0RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ECALNumSubhitsBC0RefHist,"ECAL" ,kopt)

  ####################################################################
  if(not (ECALNumSubhitsBCp1REF==None or ECALNumSubhitsBCp1==None)):  
    ECALNumSubhitsBCp1RefHist = SimulationHisto( c1 , ECALNumSubhitsBCp1REF  , ECALNumSubhitsBCp1  ,
                                                 " " , " " , "Number of Subhits in the ECAL (BC=+1)" ,
                                                 "ECALNumSubhitsBCp1" , False )
    ok_add=ECALNumSubhitsBCp1RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(ECALNumSubhitsBCp1RefHist,"ECAL" ,kopt)

  ####################################################################
  if(not (HCALSubhitsREF==None or HCALSubhits==None)):  
    HCALSubhitsRefHist = SimulationHisto( c1 , HCALSubhitsREF  , HCALSubhits ,
                                          " " , " " , "Subhits in the HCAL" ,
                                          "HCALSubhits" , False )
    ok_add=HCALSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(HCALSubhitsRefHist,"HCAL" ,kopt)

  ####################################################################
  if(not (HCALInnerSubhitsREF==None or HCALInnerSubhits==None)):  
    HCALInnerSubhitsRefHist = SimulationHisto( c1 , HCALInnerSubhitsREF  , HCALInnerSubhits ,
                                               " " , " " , "Subhits in the Inner HCAL" ,
                                               "HCALInnerSubhits" , False )
    ok_add=HCALInnerSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(HCALInnerSubhitsRefHist,"HCAL" ,kopt)

  ####################################################################
  if(not (HCALOuterSubhitsREF==None or HCALOuterSubhits==None)):  
    HCALOuterSubhitsRefHist = SimulationHisto( c1 , HCALOuterSubhitsREF  , HCALOuterSubhits ,
                                               " " , " " , "Subhits in the Outer HCAL" ,
                                               "HCALOuterSubhits" , False )
    ok_add=HCALOuterSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(HCALOuterSubhitsRefHist,"HCAL" ,kopt)

  ####################################################################
  if(not (HCALEnWeightSubhitsREF==None or HCALEnWeightSubhits==None)):  
    HCALEnWeightSubhitsRefHist = SimulationHisto( c1 , HCALEnWeightSubhitsREF  , HCALEnWeightSubhits  ,
                                                  " " , " " , "Energy Weighted Subhits in the HCAL" ,
                                                  "HCALEnWeightSubhits" , False )
    ok_add=HCALEnWeightSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(HCALEnWeightSubhitsRefHist,"HCAL" ,kopt)

  ####################################################################
  if(not (HCALInnerEnWeightSubhitsREF==None or HCALInnerEnWeightSubhits==None)):  
    HCALInnerEnWeightSubhitsRefHist = SimulationHisto( c1 , HCALInnerEnWeightSubhitsREF  , HCALInnerEnWeightSubhits  ,
                                                       " " , " " , "Energy Weighted Subhits in the Inner HCAL" ,
                                                       "HCALInnerEnWeightSubhits" , False )
    ok_add=HCALInnerEnWeightSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(HCALInnerEnWeightSubhitsRefHist,"HCAL" ,kopt)

  ####################################################################
  if(not (HCALOuterEnWeightSubhitsREF==None or HCALOuterEnWeightSubhits==None)):  
    HCALOuterEnWeightSubhitsRefHist = SimulationHisto( c1 , HCALOuterEnWeightSubhitsREF  , HCALOuterEnWeightSubhits  ,
                                                       " " , " " , "Energy Weighted Subhits in the Outer HCAL" ,
                                                       "HCALOuterEnWeightSubhits" , False )
    ok_add=HCALOuterEnWeightSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(HCALOuterEnWeightSubhitsRefHist,"HCAL" ,kopt)

  ####################################################################
  if(not (HCALAccEnREF==None or HCALAccEn==None)):  
    HCALAccEnRefHist = SimulationHisto( c1 , HCALAccEnREF  , HCALAccEn  ,
                                        " " , " " , "Accumulated Energy in the HCAL" ,
                                        "HCALAccEn" , False )
    ok_add=HCALAccEnRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(HCALAccEnRefHist,"HCAL" ,kopt)

  ####################################################################
  if(not (HCALInnerAccEnREF==None or HCALInnerAccEn==None)):  
    HCALInnerAccEnRefHist = SimulationHisto( c1 , HCALInnerAccEnREF  , HCALInnerAccEn  ,
                                             " " , " " , "Accumulated Energy in the Inner HCAL" ,
                                             "HCALInnerAccEn" , False )
    ok_add=HCALInnerAccEnRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(HCALInnerAccEnRefHist,"HCAL" ,kopt)

  ####################################################################
  if(not (HCALOuterAccEnREF==None or HCALOuterAccEn==None)):  
    HCALOuterAccEnRefHist = SimulationHisto( c1 , HCALOuterAccEnREF  , HCALOuterAccEn  ,
                                             " " , " " , "Accumulated Energy in the Outer HCAL" ,
                                             "HCALOuterAccEn" , False )
    ok_add=HCALOuterAccEnRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(HCALOuterAccEnRefHist,"HCAL" ,kopt)

  ####################################################################
  if(not (HCALNumSubhitsREF==None or HCALNumSubhits==None)):  
    HCALNumSubhitsRefHist = SimulationHisto( c1 , HCALNumSubhitsREF  , HCALNumSubhits  ,
                                             " " , " " , "Number of Subhits in the HCAL" ,
                                             "HCALNumSubhits" , False )
    ok_add=HCALNumSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(HCALNumSubhitsRefHist,"HCAL" ,kopt)

  ####################################################################
  if(not (HCALNumSubhitsBCm1REF==None or HCALNumSubhitsBCm1==None)):  
    HCALNumSubhitsBCm1RefHist = SimulationHisto( c1 , HCALNumSubhitsBCm1REF  , HCALNumSubhitsBCm1  ,
                                                 " " , " " , "Number of Subhits in the HCAL (BC=-1)" ,
                                                 "HCALNumSubhitsBCm1" , False )
    ok_add=HCALNumSubhitsBCm1RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(HCALNumSubhitsBCm1RefHist,"HCAL" ,kopt)

  ####################################################################
  if(not (HCALNumSubhitsBC0REF==None or HCALNumSubhitsBC0==None)):  
    HCALNumSubhitsBC0RefHist = SimulationHisto( c1 , HCALNumSubhitsBC0REF  , HCALNumSubhitsBC0  ,
                                                " " , " " , "Number of Subhits in the HCAL (BC=0)" ,
                                                "HCALNumSubhitsBC0" , False )
    ok_add=HCALNumSubhitsBC0RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(HCALNumSubhitsBC0RefHist,"HCAL" ,kopt)

  ####################################################################
  if(not (HCALNumSubhitsBCp1REF==None or HCALNumSubhitsBCp1==None)):  
    HCALNumSubhitsBCp1RefHist = SimulationHisto( c1 , HCALNumSubhitsBCp1REF  , HCALNumSubhitsBCp1  ,
                                                 " " , " " , "Number of Subhits in the HCAL (BC=+1)" ,
                                                 "HCALNumSubhitsBCp1" , False )
    ok_add=HCALNumSubhitsBCp1RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(HCALNumSubhitsBCp1RefHist,"HCAL" ,kopt)

  ####################################################################
  if(not (PRSSubhitsREF==None or PRSSubhits==None)):  
    PRSSubhitsRefHist = SimulationHisto( c1 , PRSSubhitsREF  , PRSSubhits ,
                                         " " , " " , "Subhits in the PRS" ,
                                         "PRSSubhits" , False )
    ok_add=PRSSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(PRSSubhitsRefHist,"PRS" ,kopt)

  ####################################################################
  if(not (PRSInnerSubhitsREF==None or PRSInnerSubhits==None)):  
    PRSInnerSubhitsRefHist = SimulationHisto( c1 , PRSInnerSubhitsREF  , PRSInnerSubhits ,
                                              " " , " " , "Subhits in the Inner PRS" ,
                                              "PRSInnerSubhits" , False )
    ok_add=PRSInnerSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(PRSInnerSubhitsRefHist,"PRS" ,kopt)
    
  ####################################################################
  if(not (PRSMiddleSubhitsREF==None or PRSMiddleSubhits==None)):  
    PRSMiddleSubhitsRefHist = SimulationHisto( c1 , PRSMiddleSubhitsREF  , PRSMiddleSubhits ,
                                               " " , " " , "Subhits in the Middle PRS" ,
                                               "PRSMiddleSubhits" , False )
    ok_add=PRSMiddleSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(PRSMiddleSubhitsRefHist,"PRS" ,kopt)

  ####################################################################
  if(not (PRSOuterSubhitsREF==None or PRSOuterSubhits==None)):  
    PRSOuterSubhitsRefHist = SimulationHisto( c1 , PRSOuterSubhitsREF  , PRSOuterSubhits ,
                                              " " , " " , "Subhits in the Outer PRS" ,
                                              "PRSOuterSubhits" , False )
    ok_add=PRSOuterSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(PRSOuterSubhitsRefHist,"PRS" ,kopt)

  ####################################################################
  if(not (PRSEnWeightSubhitsREF==None or PRSEnWeightSubhits==None)):  
    PRSEnWeightSubhitsRefHist = SimulationHisto( c1 , PRSEnWeightSubhitsREF  , PRSEnWeightSubhits  ,
                                                 " " , " " , "Energy Weighted Subhits in the PRS" ,
                                                 "PRSEnWeightSubhits" , False )
    ok_add=PRSEnWeightSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(PRSEnWeightSubhitsRefHist,"PRS" ,kopt)

  ####################################################################
  if(not (PRSInnerEnWeightSubhitsREF==None or PRSInnerEnWeightSubhits==None)):  
    PRSInnerEnWeightSubhitsRefHist = SimulationHisto( c1 , PRSInnerEnWeightSubhitsREF  , PRSInnerEnWeightSubhits  ,
                                                      " " , " " , "Energy Weighted Subhits in the Inner PRS" ,
                                                      "PRSInnerEnWeightSubhits" , False )
    ok_add=PRSInnerEnWeightSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(PRSInnerEnWeightSubhitsRefHist,"PRS" ,kopt)

  ####################################################################
  if(not (PRSMiddleEnWeightSubhitsREF==None or PRSMiddleEnWeightSubhits==None)):  
    PRSMiddleEnWeightSubhitsRefHist = SimulationHisto( c1 , PRSMiddleEnWeightSubhitsREF  , PRSMiddleEnWeightSubhits  ,
                                                       " " , " " , "Energy Weighted Subhits in the Middle  PRS" ,
                                                       "PRSMiddleEnWeightSubhits" , False )
    ok_add=PRSMiddleEnWeightSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(PRSMiddleEnWeightSubhitsRefHist,"PRS" ,kopt)

  ####################################################################
  if(not (PRSOuterEnWeightSubhitsREF==None or PRSOuterEnWeightSubhits==None)):  
    PRSOuterEnWeightSubhitsRefHist = SimulationHisto( c1 , PRSOuterEnWeightSubhitsREF  , PRSOuterEnWeightSubhits  ,
                                                      " " , " " , "Energy Weighted Subhits in the Outer PRS" ,
                                                      "PRSOuterEnWeightSubhits" , False )
    ok_add=PRSOuterEnWeightSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(PRSOuterEnWeightSubhitsRefHist,"PRS" ,kopt)

  ####################################################################
  if(not (PRSAccEnREF==None or PRSAccEn==None)):  
    PRSAccEnRefHist = SimulationHisto( c1 , PRSAccEnREF  , PRSAccEn  ,
                                       " " , " " , "Accumulated Energy in the PRS" ,
                                       "PRSAccEn" , False )
    ok_add=PRSAccEnRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(PRSAccEnRefHist,"PRS" ,kopt)

  ####################################################################
  if(not (PRSInnerAccEnREF==None or PRSInnerAccEn==None)):  
    PRSInnerAccEnRefHist = SimulationHisto( c1 , PRSInnerAccEnREF  , PRSInnerAccEn  ,
                                            " " , " " , "Accumulated Energy in the Inner PRS" ,
                                            "PRSInnerAccEn" , False )
    ok_add=PRSInnerAccEnRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(PRSInnerAccEnRefHist,"PRS" ,kopt)

  ####################################################################
  if(not (PRSMiddleAccEnREF==None or PRSMiddleAccEn==None)):  
    PRSMiddleAccEnRefHist = SimulationHisto( c1 , PRSMiddleAccEnREF  , PRSMiddleAccEn  ,
                                             " " , " " , "Accumulated Energy in the Middle PRS" ,
                                             "PRSMiddleAccEn" , False )
    ok_add=PRSMiddleAccEnRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(PRSMiddleAccEnRefHist,"PRS" ,kopt)

  ####################################################################
  if(not (PRSOuterAccEnREF==None or PRSOuterAccEn==None)):  
    PRSOuterAccEnRefHist = SimulationHisto( c1 , PRSOuterAccEnREF  , PRSOuterAccEn  ,
                                            " " , " " , "Accumulated Energy in the Outer PRS" ,
                                            "PRSOuterAccEn" , False )
    ok_add=PRSOuterAccEnRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(PRSOuterAccEnRefHist,"PRS" ,kopt)
    
  ####################################################################
  if(not (PRSNumSubhitsREF==None or PRSNumSubhits==None)):  
    PRSNumSubhitsRefHist = SimulationHisto( c1 , PRSNumSubhitsREF  , PRSNumSubhits  ,
                                            " " , " " , "Number of Subhits in the PRS" ,
                                            "PRSNumSubhits" , False )
    ok_add=PRSNumSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(PRSNumSubhitsRefHist,"PRS" ,kopt)

  ####################################################################
  if(not (PRSNumSubhitsBCm1REF==None or PRSNumSubhitsBCm1==None)):  
    PRSNumSubhitsBCm1RefHist = SimulationHisto( c1 , PRSNumSubhitsBCm1REF  , PRSNumSubhitsBCm1  ,
                                                " " , " " , "Number of Subhits in the PRS (BC=-1)" ,
                                                "PRSNumSubhitsBCm1" , False )
    ok_add=PRSNumSubhitsBCm1RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(PRSNumSubhitsBCm1RefHist,"PRS" ,kopt)

  ####################################################################
  if(not (PRSNumSubhitsBC0REF==None or PRSNumSubhitsBC0==None)):  
    PRSNumSubhitsBC0RefHist = SimulationHisto( c1 , PRSNumSubhitsBC0REF  , PRSNumSubhitsBC0  ,
                                               " " , " " , "Number of Subhits in the PRS (BC=0)" ,
                                               "PRSNumSubhitsBC0" , False )
    ok_add=PRSNumSubhitsBC0RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(PRSNumSubhitsBC0RefHist,"PRS" ,kopt)

  ####################################################################
  if(not (PRSNumSubhitsBCp1REF==None or PRSNumSubhitsBCp1==None)):  
    PRSNumSubhitsBCp1RefHist = SimulationHisto( c1 , PRSNumSubhitsBCp1REF  , PRSNumSubhitsBCp1  ,
                                                " " , " " , "Number of Subhits in the PRS (BC=+1)" ,
                                                "PRSNumSubhitsBCp1" , False )
    ok_add=PRSNumSubhitsBCp1RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(PRSNumSubhitsBCp1RefHist,"PRS" ,kopt)

  ####################################################################
  if(not (SPDSubhitsREF==None or SPDSubhits==None)):  
    SPDSubhitsRefHist = SimulationHisto( c1 , SPDSubhitsREF  , SPDSubhits ,
                                         " " , " " , "Subhits in the SPD" ,
                                         "SPDSubhits" , False )
    ok_add=SPDSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(SPDSubhitsRefHist,"SPD" ,kopt)

  ####################################################################
  if(not (SPDInnerSubhitsREF==None or SPDInnerSubhits==None)):  
    SPDInnerSubhitsRefHist = SimulationHisto( c1 , SPDInnerSubhitsREF  , SPDInnerSubhits ,
                                              " " , " " , "Subhits in the Inner SPD" ,
                                              "SPDInnerSubhits" , False )
    ok_add=SPDInnerSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(SPDInnerSubhitsRefHist,"SPD" ,kopt)

  ####################################################################
  if(not (SPDMiddleSubhitsREF==None or SPDMiddleSubhits==None)):  
    SPDMiddleSubhitsRefHist = SimulationHisto( c1 , SPDMiddleSubhitsREF  , SPDMiddleSubhits ,
                                               " " , " " , "Subhits in the Middle SPD" ,
                                               "SPDMiddleSubhits" , False )
    ok_add=SPDMiddleSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(SPDMiddleSubhitsRefHist,"SPD" ,kopt)
    
  ####################################################################
  if(not (SPDOuterSubhitsREF==None or SPDOuterSubhits==None)):  
    SPDOuterSubhitsRefHist = SimulationHisto( c1 , SPDOuterSubhitsREF  , SPDOuterSubhits ,
                                              " " , " " , "Subhits in the Outer SPD" ,
                                              "SPDOuterSubhits" , False )
    ok_add=SPDOuterSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(SPDOuterSubhitsRefHist,"SPD" ,kopt)
    
  ####################################################################
  if(not (SPDEnWeightSubhitsREF==None or SPDEnWeightSubhits==None)):  
    SPDEnWeightSubhitsRefHist = SimulationHisto( c1 , SPDEnWeightSubhitsREF  , SPDEnWeightSubhits  ,
                                                 " " , " " , "Energy Weighted Subhits in the SPD" ,
                                                 "SPDEnWeightSubhits" , False )
    ok_add=SPDEnWeightSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(SPDEnWeightSubhitsRefHist,"SPD" ,kopt)
    
  ####################################################################
  if(not (SPDInnerEnWeightSubhitsREF==None or SPDInnerEnWeightSubhits==None)):  
    SPDInnerEnWeightSubhitsRefHist = SimulationHisto( c1 , SPDInnerEnWeightSubhitsREF  , SPDInnerEnWeightSubhits  ,
                                                      " " , " " , "Energy Weighted Subhits in the Inner SPD" ,
                                                      "SPDInnerEnWeightSubhits" , False )
    ok_add=SPDInnerEnWeightSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(SPDInnerEnWeightSubhitsRefHist,"SPD" ,kopt)

  ####################################################################
  if(not (SPDMiddleEnWeightSubhitsREF==None or SPDMiddleEnWeightSubhits==None)):  
    SPDMiddleEnWeightSubhitsRefHist = SimulationHisto( c1 , SPDMiddleEnWeightSubhitsREF  , SPDMiddleEnWeightSubhits  ,
                                                       " " , " " , "Energy Weighted Subhits in the Middle SPD" ,
                                                       "SPDMiddleEnWeightSubhits" , False )
    ok_add=SPDMiddleEnWeightSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(SPDMiddleEnWeightSubhitsRefHist,"SPD" ,kopt)

  ####################################################################
  if(not (SPDOuterEnWeightSubhitsREF==None or SPDOuterEnWeightSubhits==None)):  
    SPDOuterEnWeightSubhitsRefHist = SimulationHisto( c1 , SPDOuterEnWeightSubhitsREF  , SPDOuterEnWeightSubhits  ,
                                                      " " , " " , "Energy Weighted Subhits in the Outer SPD" ,
                                                      "SPDOuterEnWeightSubhits" , False )
    ok_add=SPDOuterEnWeightSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(SPDOuterEnWeightSubhitsRefHist,"SPD" ,kopt)

  ####################################################################
  if(not (SPDAccEnREF==None or SPDAccEn==None)):  
    SPDAccEnRefHist = SimulationHisto( c1 , SPDAccEnREF  , SPDAccEn  ,
                                       " " , " " , "Accumulated Energy in the SPD" ,
                                       "SPDAccEn" , False )
    ok_add=SPDAccEnRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(SPDAccEnRefHist,"SPD" ,kopt)

  ####################################################################
  if(not (SPDInnerAccEnREF==None or SPDInnerAccEn==None)):  
    SPDInnerAccEnRefHist = SimulationHisto( c1 , SPDInnerAccEnREF  , SPDInnerAccEn  ,
                                            " " , " " , "Accumulated Energy in the Inner SPD" ,
                                            "SPDInnerAccEn" , False )
    ok_add=SPDInnerAccEnRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(SPDInnerAccEnRefHist,"SPD" ,kopt)

  ####################################################################
  if(not (SPDMiddleAccEnREF==None or SPDMiddleAccEn==None)):  
    SPDMiddleAccEnRefHist = SimulationHisto( c1 , SPDMiddleAccEnREF  , SPDMiddleAccEn  ,
                                             " " , " " , "Accumulated Energy in the Middle SPD" ,
                                             "SPDMiddleAccEn" , False )
    ok_add=SPDMiddleAccEnRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(SPDMiddleAccEnRefHist,"SPD" ,kopt)

  ####################################################################
  if(not (SPDOuterAccEnREF==None or SPDOuterAccEn==None)):  
    SPDOuterAccEnRefHist = SimulationHisto( c1 , SPDOuterAccEnREF  , SPDOuterAccEn  ,
                                            " " , " " , "Accumulated Energy in the Outer SPD" ,
                                            "SPDOuterAccEn" , False )
    ok_add=SPDOuterAccEnRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(SPDOuterAccEnRefHist,"SPD" ,kopt)

  ####################################################################
  if(not (SPDNumSubhitsREF==None or SPDNumSubhits==None)):  
    SPDNumSubhitsRefHist = SimulationHisto( c1 , SPDNumSubhitsREF  , SPDNumSubhits  ,
                                            " " , " " , "Number of Subhits in the SPD" ,
                                            "SPDNumSubhits" , False )
    ok_add=SPDNumSubhitsRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(SPDNumSubhitsRefHist,"SPD" ,kopt)

  ####################################################################
  if(not (SPDNumSubhitsBCm1REF==None or SPDNumSubhitsBCm1==None)):  
    SPDNumSubhitsBCm1RefHist = SimulationHisto( c1 , SPDNumSubhitsBCm1REF  , SPDNumSubhitsBCm1  ,
                                                " " , " " , "Number of Subhits in the SPD (BC=-1)" ,
                                                "SPDNumSubhitsBCm1" , False )
    ok_add=SPDNumSubhitsBCm1RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(SPDNumSubhitsBCm1RefHist,"SPD" ,kopt)

  ####################################################################
  if(not (SPDNumSubhitsBC0REF==None or SPDNumSubhitsBC0==None)):  
    SPDNumSubhitsBC0RefHist = SimulationHisto( c1 , SPDNumSubhitsBC0REF  , SPDNumSubhitsBC0  ,
                                               " " , " " , "Number of Subhits in the SPD (BC=0)" ,
                                               "SPDNumSubhitsBC0" , False )
    ok_add=SPDNumSubhitsBC0RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(SPDNumSubhitsBC0RefHist,"SPD" ,kopt)

  ####################################################################
  if(not (SPDNumSubhitsBCp1REF==None or SPDNumSubhitsBCp1==None)):  
    SPDNumSubhitsBCp1RefHist = SimulationHisto( c1 , SPDNumSubhitsBCp1REF  , SPDNumSubhitsBCp1  ,
                                                " " , " " , "Number of Subhits in the SPD (BC=+1)" ,
                                                "SPDNumSubhitsBCp1" , False )
    ok_add=SPDNumSubhitsBCp1RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(SPDNumSubhitsBCp1RefHist,"SPD" ,kopt)

  ####################################################################
  if(not (RICHSatHitsAeroR1REF==None or RICHSatHitsAeroR1==None)):  
    RICHSatHitsAeroR1RefHist = SimulationHisto( c1 , RICHSatHitsAeroR1REF  , RICHSatHitsAeroR1  ,
                                                " " , " " , "Number of Saturated (beta gt 0.999) Rich1 Hits in Aerogel per track" ,
                                                "RICHSatHitsAeroR1" , False )
    ok_add=RICHSatHitsAeroR1RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(RICHSatHitsAeroR1RefHist,"RICH" ,kopt)
    
  ####################################################################
  if(not (RICHSatHitsC4F10R1REF==None or RICHSatHitsC4F10R1==None)):  
    RICHSatHitsC4F10R1RefHist = SimulationHisto( c1 , RICHSatHitsC4F10R1REF  , RICHSatHitsC4F10R1  ,
                                                 " " , " " , "Number of Saturated (beta gt 0.999) Rich1 Hits in C4F10 per track" ,
                                                 "RICHSatHitsC4F10R1" , False )
    ok_add=RICHSatHitsC4F10R1RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(RICHSatHitsC4F10R1RefHist,"RICH" ,kopt)
    
  ####################################################################
  if(not (RICHSatHitsCF4R1REF==None or RICHSatHitsCF4R1==None)):  
    RICHSatHitsCF4R1RefHist = SimulationHisto( c1 , RICHSatHitsCF4R1REF  , RICHSatHitsCF4R1  ,
                                               " " , " " , "Number of Saturated (beta gt 0.999) Rich1 Hits in CF4 per track" ,
                                               "RICHSatHitsCF4R1" , False )
    ok_add=RICHSatHitsCF4R1RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(RICHSatHitsCF4R1RefHist,"RICH" ,kopt)

  ####################################################################
  if(not (RICHMomPartR1REF==None or RICHMomPartR1==None)):  
    RICHMomPartR1RefHist = SimulationHisto( c1 , RICHMomPartR1REF  , RICHMomPartR1  ,
                                            " " , " " , "Momentum of Charged particles creating hits in Rich1" ,
                                            "RICHMomPartR1" , False )
    ok_add=RICHMomPartR1RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(RICHMomPartR1RefHist,"RICH" ,kopt)

  ####################################################################
  if(not (RICHMomPartR2REF==None or RICHMomPartR2==None)):  
    RICHMomPartR2RefHist = SimulationHisto( c1 , RICHMomPartR2REF  , RICHMomPartR2  ,
                                            " " , " " , "Momentum of Charged particles creating hits in Rich2" ,
                                            "RICHMomPartR2" , False )
    ok_add=RICHMomPartR2RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(RICHMomPartR2RefHist,"RICH" ,kopt)

  ####################################################################
  if(not (RICHTvsMomAeroREF==None or RICHTvsMomAero==None)):  
    RICHTvsMomAeroRefHist = SimulationHisto( c1 , RICHTvsMomAeroREF  , RICHTvsMomAero  ,
                                             " " , " " , "Cherenkov Theta Angle at Proton Production in Aerogel vs Track Momentum" ,
                                             "RICHTvsMomAero" , False )
    ok_add=RICHTvsMomAeroRefHist.plot()
    if(ok_add==1):
      webPage.addPlot(RICHTvsMomAeroRefHist,"RICH" ,kopt)

  ####################################################################
  if(not (RICHTvsMomC4F10REF==None or RICHTvsMomC4F10==None)):  
    RICHTvsMomC4F10RefHist = SimulationHisto( c1 , RICHTvsMomC4F10REF  , RICHTvsMomC4F10  ,
                                              " " , " " , "Cherenkov Theta Angle at Proton Production in C4F10gel vs Track Momentum" ,
                                              "RICHTvsMomC4F10" , False )
    ok_add=RICHTvsMomC4F10RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(RICHTvsMomC4F10RefHist,"RICH" ,kopt)

  ####################################################################
  if(not (RICHTvsMomCF4REF==None or RICHTvsMomCF4==None)):  
    RICHTvsMomCF4RefHist = SimulationHisto( c1 , RICHTvsMomCF4REF  , RICHTvsMomCF4  ,
                                            " " , " " , "Cherenkov Theta Angle at Proton Production in CF4gel vs Track Momentum" ,
                                            "RICHTvsMomCF4" , False )
    ok_add=RICHTvsMomCF4RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(RICHTvsMomCF4RefHist,"RICH" ,kopt)

  ####################################################################
  if(not(MU1000REF==None or MU1001REF==None or MU1002REF==None or MU1003REF==None or
         MU1000==None or MU1001==None or MU1002==None or MU1003==None)):  
    MU1000REF.Add(MU1001REF, 1.)
    MU1000REF.Add(MU1002REF, 1.)
    MU1000REF.Add(MU1003REF, 1.)
    
    MU1000.Add(MU1001, 1.)
    MU1000.Add(MU1002, 1.)
    MU1000.Add(MU1003, 1.)
    
    MU1000RefHist = SimulationHisto( c1 , MU1000REF  , MU1000  ,
                                     " " , " " , "Time Multiplicity M1" ,
                                     "MU1000" , False )
    ok_add=MU1000RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(MU1000RefHist,"MUON" ,kopt)
    
  ####################################################################
  if(not(MU1004REF==None or MU1005REF==None or MU1006REF==None or MU1007REF==None or
         MU1004==None or MU1005==None or MU1006==None or MU1007==None)):  

    MU1004REF.Add(MU1005REF, 1.)
    MU1004REF.Add(MU1006REF, 1.)
    MU1004REF.Add(MU1007REF, 1.)
    
    MU1004.Add(MU1005, 1.)
    MU1004.Add(MU1006, 1.)
    MU1004.Add(MU1007, 1.)
    
    MU1004RefHist = SimulationHisto( c1 , MU1004REF  , MU1004  ,
                                     " " , " " , "Time Multiplicity M2" ,
                                     "MU1004" , False )
    ok_add=MU1004RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(MU1004RefHist,"MUON" ,kopt)
    
  ####################################################################
  if(not(MU1008REF==None or MU1009REF==None or MU1010REF==None or MU1011REF==None or
         MU1008==None or MU1009==None or MU1010==None or MU1011==None)):  

    MU1008REF.Add(MU1009REF, 1.)
    MU1008REF.Add(MU1010REF, 1.)
    MU1008REF.Add(MU1011REF, 1.)
    
    MU1008.Add(MU1009, 1.)
    MU1008.Add(MU1010, 1.)
    MU1008.Add(MU1011, 1.)
    
    MU1008RefHist = SimulationHisto( c1 , MU1008REF  , MU1008  ,
                                     " " , " " , "Time Multiplicity M3" ,
                                     "MU1008" , False )
    ok_add=MU1008RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(MU1008RefHist,"MUON" ,kopt)
    
  ####################################################################
  if(not(MU1012REF==None or MU1013REF==None or MU1014REF==None or MU1015REF==None or
         MU1012==None or MU1013==None or MU1014==None or MU1015==None)):  
    
    MU1012REF.Add(MU1013REF, 1.)
    MU1012REF.Add(MU1014REF, 1.)
    MU1012REF.Add(MU1015REF, 1.)
    
    MU1012.Add(MU1013, 1.)
    MU1012.Add(MU1014, 1.)
    MU1012.Add(MU1015, 1.)
    
    
    MU1012RefHist = SimulationHisto( c1 , MU1012REF  , MU1012  ,
                                     " " , " " , "Time Multiplicity M4" ,
                                     "MU1012" , False )
    ok_add=MU1012RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(MU1012RefHist,"MUON" ,kopt)

  ####################################################################
  if(not(MU1016REF==None or MU1017REF==None or MU1018REF==None or MU1019REF==None or
         MU1016==None or MU1017==None or MU1018==None or MU1019==None)):  
    
    MU1016REF.Add(MU1017REF, 1.)
    MU1016REF.Add(MU1018REF, 1.)
    MU1016REF.Add(MU1019REF, 1.)
    
    MU1016.Add(MU1017, 1.)
    MU1016.Add(MU1018, 1.)
    MU1016.Add(MU1019, 1.)
    
    
    MU1016RefHist = SimulationHisto( c1 , MU1016REF  , MU1016  ,
                                     " " , " " , "Time Multiplicity M5" ,
                                     "MU1016" , False )
    ok_add=MU1016RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(MU1016RefHist,"MUON" ,kopt)
    
  ####################################################################
  if(not(MU2000REF==None or MU2001REF==None or MU2002REF==None or MU2003REF==None or
         MU2000==None or MU2001==None or MU2002==None or MU2003==None)):  
    
    MU2000REF.Add(MU2001REF, 1.)
    MU2000REF.Add(MU2002REF, 1.)
    MU2000REF.Add(MU2003REF, 1.)
    
    MU2000.Add(MU2001, 1.)
    MU2000.Add(MU2002, 1.)
    MU2000.Add(MU2003, 1.)
    
    MU2000RefHist = SimulationHisto( c1 , MU2000REF  , MU2000  ,
                                     " " , " " , "Radial Multiplicity M1" ,
                                     "MU2000" , False )
    ok_add=MU2000RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(MU2000RefHist,"MUON" ,kopt)
    
  ####################################################################
  if(not(MU2004REF==None or MU2005REF==None or MU2006REF==None or MU2007REF==None or
         MU2004==None or MU2005==None or MU2006==None or MU2007==None)):  
    
    MU2004REF.Add(MU2005REF, 1.)
    MU2004REF.Add(MU2006REF, 1.)
    MU2004REF.Add(MU2007REF, 1.)
    
    MU2004.Add(MU2005, 1.)
    MU2004.Add(MU2006, 1.)
    MU2004.Add(MU2007, 1.)
    
    
    MU2004RefHist = SimulationHisto( c1 , MU2004REF  , MU2004  ,
                                     " " , " " , "Radial Multiplicity M2" ,
                                     "MU2004" , False )
    ok_add=MU2004RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(MU2004RefHist,"MUON" ,kopt)
    
  ####################################################################
  if(not(MU2008REF==None or MU2009REF==None or MU2010REF==None or MU2011REF==None or
         MU2008==None or MU2009==None or MU2010==None or MU2011==None)):  
    
    MU2008REF.Add(MU2009REF, 1.)
    MU2008REF.Add(MU2010REF, 1.)
    MU2008REF.Add(MU2011REF, 1.)
    
    MU2008.Add(MU2009, 1.)
    MU2008.Add(MU2010, 1.)
    MU2008.Add(MU2011, 1.)
    
    MU2008RefHist = SimulationHisto( c1 , MU2008REF  , MU2008  ,
                                     " " , " " , "Radial Multiplicity M3" ,
                                     "MU2008" , False )
    ok_add=MU2008RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(MU2008RefHist,"MUON" ,kopt)
    
  ####################################################################
  if(not(MU2012REF==None or MU2013REF==None or MU2014REF==None or MU2015REF==None or
         MU2012==None or MU2013==None or MU2014==None or MU2015==None)):  
    
    MU2012REF.Add(MU2013REF, 1.)
    MU2012REF.Add(MU2014REF, 1.)
    MU2012REF.Add(MU2015REF, 1.)
    
    MU2012.Add(MU2013, 1.)
    MU2012.Add(MU2014, 1.)
    MU2012.Add(MU2015, 1.)
    
    MU2012RefHist = SimulationHisto( c1 , MU2012REF  , MU2012  ,
                                     " " , " " , "Radial Multiplicity M4" ,
                                     "MU2012" , False )
    ok_add=MU2012RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(MU2012RefHist,"MUON" ,kopt)
    
  ####################################################################
  if(not(MU2016REF==None or MU2017REF==None or MU2018REF==None or MU2019REF==None or
         MU2016==None or MU2017==None or MU2018==None or MU2019==None)):  
    
    MU2016REF.Add(MU2017REF, 1.)
    MU2016REF.Add(MU2018REF, 1.)
    MU2016REF.Add(MU2019REF, 1.)
    
    MU2016.Add(MU2017, 1.)
    MU2016.Add(MU2018, 1.)
    MU2016.Add(MU2019, 1.)
    
    
    MU2016RefHist = SimulationHisto( c1 , MU2016REF  , MU2016  ,
                                     " " , " " , "Radial Multiplicity M5" ,
                                     "MU2016" , False )
    ok_add=MU2016RefHist.plot()
    if(ok_add==1):
      webPage.addPlot(MU2016RefHist,"MUON" ,kopt)
    


  refFile.Close()
  aFile.Close()
  
  ###########################################################################
  webPage.create()
  if options.install:
    webPage.install()
  if options.addToIndex:
    webPage.addToIndex()
    
if __name__ == "__main__":
  main()
  
