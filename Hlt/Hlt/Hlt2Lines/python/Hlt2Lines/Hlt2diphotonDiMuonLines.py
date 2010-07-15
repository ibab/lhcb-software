## @file
#
#  Hlt2 diphoton dimuon selection
#
#  @author D.Moran
#  @date 2010-02-03
#
##
from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2diphotonDiMuonLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = {  'Prescale'                  : {  'Hlt2diphotonDiMuon'      :  1.0 }
                }
    

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedMuons
        # Get the muon tracks straight from the HLT reconstruction
        # TODO: check that this does what it is supposed to do
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        from Configurables import CombineParticles, FilterDesktop, NumberOfTracksFilter

        #-------------------------------------------
        '''
        diphoton dimuon
        '''
        
        #--------------------------------------------
        
        FilterNumMuons = NumberOfTracksFilter("FilterNumMuons")
        FilterNumMuons.MinTracks = 2  
        FilterNumMuons.MaxTracks = 2  
        FilterNumMuons.TrackLocations  = [ Hlt2BiKalmanFittedForwardTracking()._trackifiedMuonIDLocation() ]

       #------------------------------------------------
        
        Filter = Hlt2Member(CombineParticles
                            , "Filter"
                            , DecayDescriptors = ["J/psi(1S) -> mu+ mu-"] 
                            , DaughtersCuts = { "mu+" : "(PT>400*MeV) ",     
                                                "mu-" : "(PT>400*MeV) "}
                            , CombinationCut = "(AM>1000*MeV)&(APT<900*MeV)&(AMAXDOCA('')<0.15)"    
                            , MotherCut = "ALL"  
                            , InputPrimaryVertices = "None"
                            , UseP2PVRelations = False
                            , InputLocations  = [ BiKalmanFittedMuons ]
                            )
        

        
        line = Hlt2Line( 'diphotonDiMuon'
                       , prescale = self.prescale
### TODO: where is the input to FilterNumMuons???
                       , algos = [ FilterNumMuons, BiKalmanFittedMuons, Filter ]
                       , postscale = self.postscale
                       )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2diphotonDiMuonDecision" : 50301 } )

        
       
#------------------------------------------------
