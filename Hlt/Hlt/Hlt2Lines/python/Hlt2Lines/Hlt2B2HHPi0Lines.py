from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2HHPi0LinesConf(HltLinesConfigurableUser) :    
    __slots__ = {
                    'PiMinPT_M'             : 500       # MeV
                    ,'PiMinP_M'             : 5000      # MeV
                    ,'PiMinPT_R'            : 500       # MeV
                    ,'PiMinP_R'             : 5000      # MeV
                    ,'TrackMaxChi2Ndof_M'   : 5         # unitless
                    ,'TrackMaxChi2Ndof_R'   : 5         # unitless
                    ,'PiMinIPChi2_M'        : 9         # unitless
                    ,'PiMinIPChi2_R'        : 16        # unitless
                    ,'HHMaxDOCA_M'          : 1         # mm
                    ,'HHMaxDOCA_R'          : 0.2       # mm
                    ,'Pi0MinPT_M'           : 1500      # MeV
                    ,'Pi0MinPT_R'           : 1500      # MeV
                    ,'BMinM_M'              : 4000      # MeV
                    ,'BMaxM_M'              : 6600      # MeV
                    ,'BMinM_R'              : 4500      # MeV
                    ,'BMaxM_R'              : 6100      # MeV
                    ,'BMinPT_M'             : 2500      # MeV
                    ,'BMinPT_R'             : 2500      # MeV
                    ,'BMinVtxProb_M'        : 0.0001    # unitless
                    ,'BMinVtxProb_R'        : 0.0001    # unitless
                    ,'BMaxIPChi2_M'         : 16        # unitless
                    ,'BMaxIPChi2_R'         : 16        # unitless
                    ,'BMinDIRA_M'           : 0.9999    # unitless
                    ,'BMinDIRA_R'           : 0.9999    # unitless
                    ,'BMinVVDChi2_M'        : 64        # unitless
                    ,'BMinVVDChi2_R'        : 100       # unitless
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
        Hlt2Rho4HHPi0_M = Hlt2Member(
            CombineParticles , "CombineRho"
            , DecayDescriptor = "rho(770)0 -> pi+ pi-"
            , DaughtersCuts = {
            "pi-"  : "(PT>%(PiMinPT_M)s *MeV) & (P>%(PiMinP_M)s *MeV) & (TRCHI2DOF<%(TrackMaxChi2Ndof_M)s) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2_M)s)" % self.getProps()
            ,"pi+" : "(PT>%(PiMinPT_M)s *MeV) & (P>%(PiMinP_M)s *MeV) & (TRCHI2DOF<%(TrackMaxChi2Ndof_M)s) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2_M)s)" % self.getProps()   
                                 } 
            , CombinationCut = "(AMAXDOCA('LoKi::DistanceCalculator')<%(HHMaxDOCA_M)s *mm)" % self.getProps()
            , MotherCut      = "(VFASPF(VPCHI2)>%(BMinVtxProb_M)s) & (BPVVDCHI2>%(BMinVVDChi2_M)s)" % self.getProps()
            , ParticleCombiners = {'' : 'TrgVertexFitter'}
            , InputLocations = [ BiKalmanFittedPions ] )

        Hlt2Rho4HHPi0_R = Hlt2Member(
            CombineParticles , "CombineRho"
            , DecayDescriptor = "rho(770)0 -> pi+ pi-"
            , DaughtersCuts = {
            "pi-"  : "(PT>%(PiMinPT_R)s *MeV) & (P>%(PiMinP_R)s *MeV) & (TRCHI2DOF<%(TrackMaxChi2Ndof_R)s) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2_R)s)" % self.getProps()
            ,"pi+" : "(PT>%(PiMinPT_R)s *MeV) & (P>%(PiMinP_R)s *MeV) & (TRCHI2DOF<%(TrackMaxChi2Ndof_R)s) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2_R)s)" % self.getProps()   
                                 } 
            , CombinationCut = "(AMAXDOCA('LoKi::DistanceCalculator')<%(HHMaxDOCA_R)s *mm)" % self.getProps()
            , MotherCut      = "(VFASPF(VPCHI2)>%(BMinVtxProb_R)s) & (BPVVDCHI2>%(BMinVVDChi2_R)s)" % self.getProps()
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
            , CombinationCut = "(AM>%(BMinM_M)s *MeV) & (AM<%(BMaxM_M)s *MeV)" % self.getProps()
            , MotherCut      = "(PT>%(BMinPT_M)s *MeV) & (BPVIPCHI2()<%(BMaxIPChi2_M)s) & (BPVDIRA>%(BMinDIRA_M)s)" % self.getProps()
            , ParticleCombiners = {'' : 'TrgVertexFitter'}
            , InputLocations = [ Hlt2Rho4HHPi0_M, MergedPi0s ])
        
        
        ############################################################################
        #    Merged Pi0
        ############################################################################
        line = Hlt2Line('B2HHPi0_Merged'
                        , prescale = self.prescale
                        , algos = [ BiKalmanFittedPions, PV3D(), Hlt2Rho4HHPi0_M, MergedPi0s, Hlt2B2HHPi0 ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2HHPi0_MergedDecision" : self.getProp('HltANNSvcID')['Hlt2B2HHPi0_Merged'] } )
        
        ############################################################################
        #    Resolved Pi0
        ############################################################################
        line.clone('B2HHPi0_Resolved'
                   , prescale = self.prescale
                   , algos = [ BiKalmanFittedPions, PV3D(), Hlt2Rho4HHPi0_R, ResolvedPi0s, Hlt2B2HHPi0 ]
                   , CombineB =
                   {
            "DaughtersCuts" : {
            "rho(770)0" : "ALL" % self.getProps()
            ,"pi0"      : "(PT>%(Pi0MinPT_R)s *MeV)" % self.getProps()   
            }
            , "CombinationCut" : "(AM>%(BMinM_R)s *MeV) & (AM<%(BMaxM_R)s *MeV)" % self.getProps()
            , "MotherCut"      : "(PT>%(BMinPT_R)s *MeV) & (BPVIPCHI2()<%(BMaxIPChi2_R)s) & (BPVDIRA>%(BMinDIRA_R)s)" % self.getProps()
            , "InputLocations" : [ Hlt2Rho4HHPi0_R, ResolvedPi0s ]
            }
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2HHPi0_ResolvedDecision" :  self.getProp('HltANNSvcID')['Hlt2B2HHPi0_Resolved'] } )

