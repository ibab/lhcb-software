from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2HHPi0LinesConf(HltLinesConfigurableUser) :    
    __slots__ = {
                     'PiMinPT'              : 300       # MeV
                    ,'PiMinP'               : 2000      # MeV
                    ,'PiMinIPChi2'          : 9         # unitless
                    ,'TrackMaxChi2Ndof'     : 10        # unitless 
                    ,'Pi0MinPT'             : 1000      # MeV
                    ,'BMinVtxProb'          : 0.0001    # unitless
                    ,'BMaxIPChi2'           : 25.0      # unitless
                    ,'BMinDIRA'             : 0.99987   # unitless
                    ,'BMinPT'               : 2000      # MeV
                    ,'BMaxADMass'           : 1000      # MeV
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
        Hlt2B2HHPi0 = Hlt2Member(
            CombineParticles , "CombineB"
            , DecayDescriptor = "B0 -> pi+ pi- pi0"
            , DaughtersCuts = {
                                 "pi-" : "(TRCHI2DOF<%(TrackMaxChi2Ndof)s) & (PT>%(PiMinPT)s *MeV) & (P>%(PiMinP)s *MeV) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2)s)" % self.getProps()
                                ,"pi+" : "(TRCHI2DOF<%(TrackMaxChi2Ndof)s) & (PT>%(PiMinPT)s *MeV) & (P>%(PiMinP)s *MeV) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2)s)" % self.getProps()   
                                ,"pi0" : "(PT>%(Pi0MinPT)s *MeV)" % self.getProps()   
                                 } 
            , CombinationCut = "(ADAMASS('B0')<%(BMaxADMass)s *MeV)" % self.getProps()
            , MotherCut      = "(VFASPF(VPCHI2)>%(BMinVtxProb)s) & (BPVIPCHI2()<%(BMaxIPChi2)s) & (BPVDIRA>%(BMinDIRA)s) & (PT>%(BMinPT)s *MeV)" % self.getProps()
            , InputLocations = [ BiKalmanFittedPions, MergedPi0s ])
        
        
        ############################################################################
        #    Merged Pi0
        ############################################################################
        line = Hlt2Line('B2HHPi0_Merged'
                        , prescale = self.prescale
                        , algos = [ BiKalmanFittedPions, MergedPi0s, PV3D(), Hlt2B2HHPi0 ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2HHPi0_MergedDecision" : self.getProp('HltANNSvcID')['Hlt2B2HHPi0_Merged'] } )
        
        ############################################################################
        #    Resolved Pi0
        ############################################################################
        line.clone('B2HHPi0_Resolved'
                   , prescale = self.prescale
                   , algos = [ BiKalmanFittedPions, ResolvedPi0s, PV3D(), Hlt2B2HHPi0 ]
                   , CombineB =
                   {
                     "DaughtersCuts" :
                     {
                        "pi-" : "(TRCHI2DOF<%(TrackMaxChi2Ndof)s) & (PT>%(PiMinPT)s *MeV) & (P>%(PiMinP)s *MeV) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2)s)" % self.getProps()
                       ,"pi+" : "(TRCHI2DOF<%(TrackMaxChi2Ndof)s) & (PT>%(PiMinPT)s *MeV) & (P>%(PiMinP)s *MeV) & (MIPCHI2DV(PRIMARY)>%(PiMinIPChi2)s)" % self.getProps()   
                       ,"pi0" : "(PT>%(Pi0MinPT)s *MeV)" % self.getProps()   
                        }
                     , "CombinationCut" : "(ADAMASS('B0')<%(BMaxADMass)s *MeV)" % self.getProps()
                     , "MotherCut"      : "(VFASPF(VPCHI2)>%(BMinVtxProb)s) & (BPVIPCHI2()<%(BMaxIPChi2)s) & (BPVDIRA>%(BMinDIRA)s) & (PT>%(BMinPT)s *MeV)" % self.getProps()
                     , "InputLocations" : [BiKalmanFittedPions, ResolvedPi0s]
                     }
                   , postscale = self.postscale
                   )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2HHPi0_ResolvedDecision" :  self.getProp('HltANNSvcID')['Hlt2B2HHPi0_Resolved'] } )
