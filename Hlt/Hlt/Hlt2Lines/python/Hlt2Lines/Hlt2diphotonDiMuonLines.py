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
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        from Configurables import CombineParticles, FilterDesktop
        from Configurables import LoKi__VoidFilter as VoidFilter

        #-------------------------------------------
        '''
        diphoton dimuon
        '''
        
        #--------------------------------------------
        
        # carefull: we cheat because we know that BiKalmanFittedMuons uses the location below internally...
        muonLoc =  Hlt2BiKalmanFittedForwardTracking()._trackifiedMuonIDLocation() 
        FilterNumMuons = VoidFilter("Hlt2diphotonDiMuonFilterNumMuons", Code = "CONTAINS('%s') == 2" % muonLoc )

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
                            , Inputs  = [ BiKalmanFittedMuons ]
                            )
        

        
        line = Hlt2Line( 'diphotonDiMuon'
                       , prescale = self.prescale
                       , algos = [ BiKalmanFittedMuons, FilterNumMuons, Filter ]
                       , postscale = self.postscale
                       )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2diphotonDiMuonDecision" : 50301 } )

        
       
#------------------------------------------------
