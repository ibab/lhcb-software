from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2HHPi0LinesConf(HltLinesConfigurableUser) :    
    __slots__ = {
                    'PiMinPT'               : 270       # MeV
                    ,'PiMinP'               : 1800      # MeV
                    ,'TrackMaxChi2Ndof'     : 10        # unitless
                    ,'PiMinIPChi2'          : 16        # unitless
                    ,'Pi0MinPT_M'           : 2250      # MeV
                    ,'Pi0MinPT_R'           : 1350      # MeV
                    ,'BMinM'                : 4000      # MeV
                    ,'BMaxM'                : 6600      # MeV
                    ,'BMinPT_M'             : 2700      # MeV
                    ,'BMinPT_R'             : 2250      # MeV
                    ,'BMinVtxProb'          : 0.0001    # unitless
                    ,'BMaxIPChi2'           : 16        # unitless
                    ,'BMinDIRA'             : 0.9999    # unitless
                    ,'BMinVVDChi2'          : 36        # unitless
                    ,'Prescale'           : {   'Hlt2B2HHPi0_Merged'         : 1.0
                                              , 'Hlt2B2HHPi0_Resolved'       : 1.0
                                                }
        
                    ,'Postscale'          : {   'Hlt2B2HHPi0_Merged'         : 1.0
                                              , 'Hlt2B2HHPi0_Resolved'       : 1.0
                                                }
                     
                    ,'HltANNSvcID'        : {   'Hlt2B2HHPi0_Merged'         : 59991
                                              , 'Hlt2B2HHPi0_Resolved'       : 59992
                                                }
                     }
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import CombineParticles, PhysDesktop
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions
        from Hlt2SharedParticles.Pi0 import MergedPi0s,ResolvedPi0s
        from HltTracking.HltPVs import PV3D
        
       ###########################################################################
        Hlt2Rho4HHPi0 = Hlt2Member(
            CombineParticles , "CombineRho"
            , DecayDescriptor = "rho(770)0 -> pi+ pi-"
            , DaughtersCuts = {
            "pi-"  : "(PT>%(PiMinPT)s *MeV) & (P>%(PiMinP)s *MeV) & (TRCHI2DOF<%(TrackMaxChi2Ndof)s) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2)s)" % self.getProps()
            ,"pi+" : "(PT>%(PiMinPT)s *MeV) & (P>%(PiMinP)s *MeV) & (TRCHI2DOF<%(TrackMaxChi2Ndof)s) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2)s)" % self.getProps()   
                                 } 
            , CombinationCut = "AALL" % self.getProps()
            , MotherCut      = "(VFASPF(VPCHI2)>%(BMinVtxProb)s) & (BPVVDCHI2>%(BMinVVDChi2)s)" % self.getProps()
            , ParticleCombiners = {'' : 'TrgVertexFitter'}
            , InputLocations = [ BiKalmanFittedPions ] )

       ###########################################################################
        Hlt2B2HHPi0 = Hlt2Member(
            CombineParticles , "CombineB"
            , DecayDescriptor = "B0 -> rho(770)0 pi0"
            , DaughtersCuts = {
            "rho(770)0" : "ALL" % self.getProps()
            ,"pi0"      : "(PT>%(Pi0MinPT_M)s *MeV)" % self.getProps()   
            } 
            , CombinationCut = "(AM>%(BMinM)s *MeV) & (AM<%(BMaxM)s *MeV)" % self.getProps()
            , MotherCut      = "(PT>%(BMinPT_M)s *MeV) & (BPVIPCHI2()<%(BMaxIPChi2)s) & (BPVDIRA>%(BMinDIRA)s)" % self.getProps()
            , ParticleCombiners = {'' : 'TrgVertexFitter'}
            , InputLocations = [ Hlt2Rho4HHPi0, MergedPi0s ])
        
        
        ############################################################################
        #    Merged Pi0
        ############################################################################
        line = Hlt2Line('B2HHPi0_Merged'
                        , prescale = self.prescale
                        , algos = [ BiKalmanFittedPions, PV3D(), Hlt2Rho4HHPi0, MergedPi0s, Hlt2B2HHPi0 ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2HHPi0_MergedDecision" : self.getProp('HltANNSvcID')['Hlt2B2HHPi0_Merged'] } )
        
        ############################################################################
        #    Resolved Pi0
        ############################################################################
        line.clone('B2HHPi0_Resolved'
                   , prescale = self.prescale
                   , algos = [ BiKalmanFittedPions, PV3D(), Hlt2Rho4HHPi0, ResolvedPi0s, Hlt2B2HHPi0 ]
                   , CombineB =
                   {
            "DaughtersCuts" : {
            "rho(770)0" : "ALL" % self.getProps()
            ,"pi0"      : "(PT>%(Pi0MinPT_R)s *MeV)" % self.getProps()   
            }
            , "CombinationCut" : "(AM>%(BMinM)s *MeV) & (AM<%(BMaxM)s *MeV)" % self.getProps()
            , "MotherCut"      : "(PT>%(BMinPT_R)s *MeV) & (BPVIPCHI2()<%(BMaxIPChi2)s) & (BPVDIRA>%(BMinDIRA)s)" % self.getProps()
            , "InputLocations" : [ Hlt2Rho4HHPi0, ResolvedPi0s ]
            }
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2HHPi0_ResolvedDecision" :  self.getProp('HltANNSvcID')['Hlt2B2HHPi0_Resolved'] } )
