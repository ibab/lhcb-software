
## @file
#
#  Hlt2 Exclusive DiPhoton DiMuon Line for Lumi Measurement
#  + Exclusive Low Mult Lines
#
#  @author D.Moran
#  @date 2010-02-03
#
##

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2diphotonDiMuonLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = {  'Prescale'                  : {  'Hlt2diPhotonDiMuon'    :  1.0 
                                                    ,'Hlt2LowMultMuon'       :  0.1
                                                    ,'Hlt2LowMultHadron'     :  0.1
                                                    ,'Hlt2LowMultPhoton'     :  1.0
                                                    ,'Hlt2LowMultElectron'   :  1.0
                                                    ,'Hlt2LowMultHadron_nofilter'     :  0.01
                                                    ,'Hlt2LowMultElectron_nofilter'   :  0.05
                                                 }
                }
    

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedMuons
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedElectrons, BiKalmanFittedKaons, BiKalmanFittedPions
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPhotons    
        from Hlt2SharedParticles.Pi0 import AllPi0s,MergedPi0s,ResolvedPi0s
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        from Configurables import CombineParticles, FilterDesktop
        from Configurables import LoKi__VoidFilter as VoidFilter
        #-------------------------------------------

        velotracks = Hlt2BiKalmanFittedForwardTracking().hlt2VeloTracking()
        FilterNumVeloTracks = VoidFilter('Hlt2LowMultMuonFilterNumVeloTracks', Code="CONTAINS('" + velotracks.outputSelection() + "')<4")
        FilterNumVeloTracksEl = VoidFilter('Hlt2LowMultElectronFilterNumVeloTracks', Code="CONTAINS('" + velotracks.outputSelection() + "')<8")       
        FilterNumVeloTracksHad = VoidFilter('Hlt2LowMultHadronFilterNumVeloTracks', Code="CONTAINS('" + velotracks.outputSelection() + "')<8")

        
        MuBackTrackFilter = VoidFilter('LowMultMuBackTrackKill', Code = "TrNUM('%s', TrBACKWARD)<1" %
                                     velotracks.outputSelection() )
        ElBackTrackFilter = VoidFilter('LowMultElBackTrackKill', Code = "TrNUM('%s', TrBACKWARD)<1" %
                                       velotracks.outputSelection() )
        HadBackTrackFilter = VoidFilter('LowMultHadBackTrackKill', Code = "TrNUM('%s', TrBACKWARD)<1" %
                                        velotracks.outputSelection() )

                
         
        FilterMu = Hlt2Member(FilterDesktop
                             , "FilterMu"
                             , Code = "(PT>400*MeV)"
                             , InputPrimaryVertices = "None"
                             , UseP2PVRelations = False
                             , Inputs  = [BiKalmanFittedMuons]
                                                )
        
        FilterDiMu = Hlt2Member(CombineParticles
                            , "FilterDiMu"
                            , DecayDescriptors = ["J/psi(1S) -> mu+ mu-","J/psi(1S) -> mu+ mu+","J/psi(1S) -> mu- mu-"] 
                            , DaughtersCuts = { "mu+" : "(PT>400*MeV) ",     
                                                "mu-" : "(PT>400*MeV) "}   
                            , MotherCut = "ALL"  
                            , InputPrimaryVertices = "None"
                            , UseP2PVRelations = False
                            , Inputs = [ BiKalmanFittedMuons ]
                            )

        
        FilterEl = Hlt2Member(CombineParticles
                            , "FilterEl"
                            , DecayDescriptors = ["J/psi(1S) -> e+ e-","J/psi(1S) -> e+ e+","J/psi(1S) -> e- e-"] 
                            , DaughtersCuts = { "e+" : "(PT>250*MeV) ",     
                                                "e-" : "(PT>250*MeV) "}   
                            , MotherCut = "ALL"  
                            , InputPrimaryVertices = "None"
                            , UseP2PVRelations = False
                            , Inputs = [ BiKalmanFittedElectrons ]
                            )


        FilterPh = Hlt2Member(CombineParticles
                            , "FilterPh"
                            , DecayDescriptors = ["pi0 -> gamma gamma"] 
                            , DaughtersCuts = { "gamma" : "(PT>250*MeV) "}                                                                  
                            , MotherCut = "ALL"  
                            , InputPrimaryVertices = "None"
                            , UseP2PVRelations = False
                            , Inputs  = [ BiKalmanFittedPhotons ]
                            )

        FilterPhFilter = Hlt2Member(FilterDesktop, "FilterPhFilter", Code = "PT>250*MeV", 
                                    Inputs  = [MergedPi0s,ResolvedPi0s])

        FilterH = Hlt2Member(CombineParticles
                            , "FilterH"
                            , DecayDescriptors = ["J/psi(1S) -> K+ K-","J/psi(1S) -> K+ K+","J/psi(1S) -> K- K-"] 
                            , DaughtersCuts = { "K+" : "(PT>1000*MeV) ",     
                                                "K-" : "(PT>1000*MeV) "}   
                            , MotherCut = "ALL"  
                            , InputPrimaryVertices = "None"
                            , UseP2PVRelations = False
                            , Inputs  = [ BiKalmanFittedKaons ]
                            )
      
        
        line = Hlt2Line( 'diPhotonDiMuon'
                       , prescale = self.prescale
                       , HLT =  "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')"  
                       , L0DU = "L0_CHANNEL('Muon,lowMult')|L0_CHANNEL('DiMuon,lowMult')"
                       , algos = [  BiKalmanFittedMuons,FilterDiMu ]
                       , postscale = self.postscale
                       )

        line = Hlt2Line( 'LowMultMuon'
                       , prescale = self.prescale
                       , HLT =  "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')"    
                       , L0DU = "L0_CHANNEL('Muon,lowMult')|L0_CHANNEL('DiMuon,lowMult')"
                       , algos = [ velotracks,  MuBackTrackFilter,  FilterNumVeloTracks, BiKalmanFittedMuons,FilterMu ]
                       , postscale = self.postscale
                       )
        
        
        line = Hlt2Line( 'LowMultHadron'
                       , prescale = self.prescale
                       , HLT =  "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')"    
                       , L0DU = "L0_CHANNEL('DiHadron,lowMult')"
                       , algos = [ velotracks, HadBackTrackFilter, FilterNumVeloTracksHad, BiKalmanFittedKaons, FilterH]
                       , postscale = self.postscale
                       )

        line = Hlt2Line( 'LowMultHadron_nofilter'
                       , prescale = self.prescale
                       , HLT =  "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')"    
                       , L0DU = "L0_CHANNEL('DiHadron,lowMult')"
                       , algos = [ BiKalmanFittedKaons, FilterH]
                       , postscale = self.postscale
                       )
        
        line = Hlt2Line( 'LowMultPhoton'
                       , prescale = self.prescale
                       , HLT =  "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')"    
                       , L0DU = "L0_CHANNEL('Photon,lowMult')|L0_CHANNEL('DiEM,lowMult')"
                       , algos = [ BiKalmanFittedPhotons,AllPi0s,FilterPhFilter ]  
                       , postscale = self.postscale
                       )
        
        line = Hlt2Line( 'LowMultElectron'
                       , prescale = self.prescale
                       , HLT =  "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')"    
                       , L0DU = "L0_CHANNEL('Electron,lowMult')|L0_CHANNEL('DiEM,lowMult')"
                       , algos = [ velotracks,  ElBackTrackFilter, FilterNumVeloTracksEl, BiKalmanFittedElectrons,FilterEl]   
                       , postscale = self.postscale
                       )
        
        line = Hlt2Line( 'LowMultElectron_nofilter'
                       , prescale = self.prescale
                       , HLT =  "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')"    
                       , L0DU = "L0_CHANNEL('Electron,lowMult')|L0_CHANNEL('DiEM,lowMult')"
                       , algos = [ BiKalmanFittedElectrons,FilterEl]   
                       , postscale = self.postscale
                       )
        
    

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2diPhotonDiMuonDecision" : 50301 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultMuonDecision" : 50302 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultHadronDecision" : 50303 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultPhotonDecision" : 50304 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultElectronDecision" : 50305 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultHadron_nofilterDecision" : 50306 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2LowMultElectron_nofilterDecision" : 50307 } )
      
