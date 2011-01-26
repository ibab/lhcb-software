from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2HHPi0LinesConf(HltLinesConfigurableUser) :    
    __slots__ = {
                    'PiMinPT'               : 500       # MeV
                    ,'PiMinP'               : 5000      # MeV
                    ,'TrackMaxChi2Ndof'     : 5         # unitless
                    ,'PiMinIPChi2'          : 9         # unitless

                    ,'BMinVtxProb'          : 0.0001    # unitless
                    ,'BMinVVDChi2'          : 64        # unitless

                    ,'Pi0MinPT_M'           : 1200      # MeV
                    ,'Pi0MinPT_R'           : 1200      # MeV

                    ,'BMinM_M'              : 4000      # MeV
                    ,'BMaxM_M'              : 6600      # MeV
                    ,'BMinM_R'              : 4000      # MeV
                    ,'BMaxM_R'              : 6600      # MeV
                    ,'BMinPT_M'             : 2000      # MeV
                    ,'BMinPT_R'             : 2000      # MeV
                    
                    ,'BMaxIPChi2_M'         : 25        # unitless
                    ,'BMaxIPChi2_R'         : 25        # unitless
                    ,'BMinDIRA_M'           : 0.99987   # unitless
                    ,'BMinDIRA_R'           : 0.99987   # unitless

                    ,'HLT1FILTER'           : "HLT_PASS_RE('Hlt1.*Photon.*Decision')"

                    ,'Prescale'             : {   'Hlt2B2HHPi0_Merged'       : 1.0
                                                , 'Hlt2B2HHPi0_Resolved'     : 1.0
                                                  } 
        
                    ,'Postscale'            : {   'Hlt2B2HHPi0_Merged'       : 1.0
                                                , 'Hlt2B2HHPi0_Resolved'     : 1.0
                                                  }
                     
                    ,'HltANNSvcID'          : {   'Hlt2B2HHPi0_Merged'       : 59991
                                                , 'Hlt2B2HHPi0_Resolved'     : 59992
                                                  }
                     }
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
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
            , CombinationCut = "in_range(%(BMinM_M)s *MeV, AM, %(BMaxM_M)s *MeV)" % self.getProps()
            , MotherCut      = "(PT>%(BMinPT_M)s *MeV) & (BPVIPCHI2()<%(BMaxIPChi2_M)s) & (BPVDIRA>%(BMinDIRA_M)s)" % self.getProps()
            , ParticleCombiners = {'' : 'TrgVertexFitter'}
            , InputLocations = [ Hlt2Rho4HHPi0, MergedPi0s ])
        
        
        ############################################################################
        #    Merged Pi0
        ############################################################################
        hltfilter = self.getProp("HLT1FILTER")
        if hltfilter == "" : hltfilter = None
        
        line = Hlt2Line('B2HHPi0_Merged'
                        , HLT = hltfilter
                        , prescale = self.prescale
                        , algos = [ PV3D(), BiKalmanFittedPions, Hlt2Rho4HHPi0, MergedPi0s, Hlt2B2HHPi0 ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2HHPi0_MergedDecision" : self.getProp('HltANNSvcID')['Hlt2B2HHPi0_Merged'] } )
        
        ############################################################################
        #    Resolved Pi0
        ############################################################################
        line.clone('B2HHPi0_Resolved'
                   , HLT = hltfilter
                   , prescale = self.prescale
                   , algos = [ PV3D(), BiKalmanFittedPions, Hlt2Rho4HHPi0, ResolvedPi0s, Hlt2B2HHPi0 ]
                   , CombineB =
                   {
            "DaughtersCuts" : {
            "rho(770)0" : "ALL" % self.getProps()
            ,"pi0"      : "(PT>%(Pi0MinPT_R)s *MeV)" % self.getProps()   
            }
            , "CombinationCut" : "in_range(%(BMinM_R)s *MeV, AM, %(BMaxM_R)s *MeV)" % self.getProps()
            , "MotherCut"      : "(PT>%(BMinPT_R)s *MeV) & (BPVIPCHI2()<%(BMaxIPChi2_R)s) & (BPVDIRA>%(BMinDIRA_R)s)" % self.getProps()
            , "InputLocations" : [ Hlt2Rho4HHPi0, ResolvedPi0s ]
            }
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2HHPi0_ResolvedDecision" :  self.getProp('HltANNSvcID')['Hlt2B2HHPi0_Resolved'] } )
