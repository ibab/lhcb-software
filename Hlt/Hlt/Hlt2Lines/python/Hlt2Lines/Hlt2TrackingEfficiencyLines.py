# Hlt2 tracking efficiency lines 27/01/2015
# Combining tag and partially reconstructed probe track and matching with long 
# Michael Kolpin (michael.kolpin@cern.ch)
# 
#


from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt2TrackingEfficiencyLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = { 'Prescale'   : { 'Hlt2TrackEffMuonTT1'      	    : 1.0
				   ,'Hlt2TrackEffMuonTT2'      	    : 1.0
				   ,'Hlt2TrackEffMuonTTLL1'    	    : 1.0
				   ,'Hlt2TrackEffMuonTTLL2'    	    : 1.0
				   ,'Hlt2TrackEffVeloMuon1'   	    : 1.0
				   ,'Hlt2TrackEffVeloMuon2'   	    : 1.0
				   ,'Hlt2TrackEffVeloMuonLL1'  	    : 1.0
				   ,'Hlt2TrackEffVeloMuonLL2'  	    : 1.0
				   ,'Hlt2TrackEffDownstream1'	    : 1.0
				   ,'Hlt2TrackEffDownstream2'	    : 1.0
				   ,'Hlt2TrackEffDownstreamLL1'	    : 1.0
				   ,'Hlt2TrackEffDownstreamLL2'	    : 1.0
                                   }
                  
		  # Muon TT cuts
                  ,'MuonTTProbeP'              :    0 #MeV
                  ,'MuonTTProbePt'             :    0 #MeV
                  ,'MuonTTLongMuonPID'         :    2 #dimensionless
                  ,'MuonTTLongP'               :10000 #MeV
                  ,'MuonTTLongPt'              : 1300 #MeV
                  ,'MuonTTLongTrChi2'          :    5 #dimensionless
                  ,'MuonTTJPsiP'               : 1000 #MeV
                  ,'MuonTTJPsiPt'              : 1000 #MeV
                  ,'MuonTTJPsiMaxIP'           :    1 #mm
                  ,'MuonTTJPsiVtxChi2'         :    5 #dimensionless
                  ,'MuonTTMassWindow'          :  500 #MeV
		  # VeloMuon cuts
                  ,'VeloProbePt'                  :   0 #MeV
		  ,'VeloProbeTrChi2'		  :   5 #dimensionless
                  ,'VeloLongMuonPID'              :   1 #dimensionless
                  ,'VeloLongPt'                   :   0 #MeV
                  ,'VeloLongP'                    :7000 #MeV
		  ,'VeloLongTrchi2'		  :   3 #dimensionless
                  ,'VeloMassWindow'               : 500 #MeV
                  ,'VeloJPsiPt'                   : 500 #MeV
		  ,'VeloVertchi2'		  :   2 #dimensionless
		  # DownstreamMuon cuts
                  ,'DownstreamProbePt'                  : 200 #MeV
                  ,'DownstreamProbeP'                   :3000 #MeV
		  ,'DownstreamProbeTrchi2'	        :  10 #dimensionless
                  ,'DownstreamLongPt'                   : 700 #MeV
                  ,'DownstreamLongP'                    :5000 #MeV
		  ,'DownstreamLongTrchi2'		:  10 #dimensionless
                  ,'DownstreamLongMuonPID'	        :   2 #dimensionless
                  ,'DownstreamLongMinIP'	        : 0.5 #mm
                  ,'DownstreamJPsiDOCA'                 :   5 #mm
                  ,'DownstreamMassWindow'               : 200 #MeV
                  ,'DownstreamJPsiPt'                   :   0 #MeV
		  ,'DownstreamVertchi2'			:   5 #dimensionless
                  }
    
    
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from HltLine.HltLine import bindMembers
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import ( BiKalmanFittedMuons,
                                                                    BiKalmanFittedPions )
        from Hlt2SharedParticles.TagAndProbeParticles import ProbeMuonTTMuons, ProbeVeloMuons, ProbeDownstreamMuons

        from Configurables import TisTosParticleTagger
        from HltLine.Hlt2Monitoring import Hlt2Monitor,Hlt2MonitorMinMax



        #--------------Muon+TT track combination lines------------------------------------------------

	    # Create two triggerlines to distunguish positive-charge long and negative tag-track and vice versa

	    # Positive long track / negative tag-track


        filterPlus1 = Hlt2Member( FilterDesktop
                                , "filterPlus"
                                , Code = "(Q > 0) & (PT>%(MuonTTLongPt)s*MeV) & (P>%(MuonTTLongP)s*MeV) & (TRCHI2DOF<%(MuonTTLongTrChi2)s) & (PIDmu >-%(MuonTTLongMuonPID)s)"%self.getProps()
                                , Inputs  = [ BiKalmanFittedMuons ]
                                )

        TOSTagMuonsFilter1 = Hlt2Member( TisTosParticleTagger
                                         ,"TOSTagMuonsFilter"
                                         ,TisTosSpecs = { "Hlt1TrackMuonDecision%TOS":0 }
                                         ,Inputs = [ filterPlus1 ]
                                         ,NoRegex=True
                                         ,ProjectTracksToCalo = False
                                         ,CaloClustForCharged = False
                                         ,CaloClustForNeutral = False
                                         ,TOSFrac = { 4:0.0, 5:0.0 }
                                         )

        filterMinus1 = Hlt2Member( FilterDesktop
                                 , "filterMinus"
                                 , Code = "(Q < 0) & (PT>%(MuonTTProbePt)s*MeV) & (P>%(MuonTTProbeP)s*MeV)"%self.getProps()
                                 , Inputs  = [ ProbeMuonTTMuons ]
                                 , PreMonitor = Hlt2MonitorMinMax ("TRCHI2DOF","M(#mu#mu)",0,10)
                                 )
                             

        JPsiCombine1 = Hlt2Member( CombineParticles
                                , 'JPsiCombine'
                                , Inputs = [ TOSTagMuonsFilter1, filterMinus1 ]
                                , DecayDescriptor = 'J/psi(1S) -> mu+ mu-'
                                , MotherCut =  "(PT>%(MuonTTJPsiPt)s*MeV) & (P>%(MuonTTJPsiP)s*MeV) & (MIPDV(PRIMARY)<%(MuonTTJPsiMaxIP)s) & (VFASPF(VCHI2/VDOF)<%(MuonTTJPsiVtxChi2)s) & (ADMASS('J/psi(1S)')<%(MuonTTMassWindow)s*MeV)"%self.getProps()
                                , MotherMonitor = Hlt2MonitorMinMax ("M","M(#mu#mu)",2600,3600)
                                )


        line = Hlt2Line('TrackEffMuonTT1'
                      , prescale = self.prescale
                      , L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('DiMuon')"
                      , algos = [ BiKalmanFittedMuons, filterPlus1, TOSTagMuonsFilter1, ProbeMuonTTMuons, filterMinus1, JPsiCombine1 ]
                      , postscale = self.postscale
                      )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TrackEffMuonTT1Decision" : 50601 } )

	##################### respective line with two long tracks, needed for association #########################

        filterMinusLL1 = Hlt2Member( FilterDesktop
                                 , "filterMinus"
                                 , Code = "(Q < 0) & (PT>%(MuonTTProbePt)s*MeV) & (P>%(MuonTTProbeP)s*MeV)"%self.getProps()
                                 , Inputs  = [ BiKalmanFittedMuons ]
                                 , PreMonitor = Hlt2MonitorMinMax ("TRCHI2DOF","M(#mu#mu)",0,10)
                                 )
                             

        JPsiCombineLL1 = Hlt2Member( CombineParticles
                                , 'JPsiCombine'
                                , Inputs = [ TOSTagMuonsFilter1, filterMinusLL1 ]
                                , DecayDescriptor = 'J/psi(1S) -> mu+ mu-'
                                , MotherCut =  "(PT>%(MuonTTJPsiPt)s*MeV) & (P>%(MuonTTJPsiP)s*MeV) & (MIPDV(PRIMARY)<%(MuonTTJPsiMaxIP)s) & (VFASPF(VCHI2/VDOF)<%(MuonTTJPsiVtxChi2)s) & (ADMASS('J/psi(1S)')<%(MuonTTMassWindow)s*MeV)"%self.getProps()
                                , MotherMonitor = Hlt2MonitorMinMax ("M","M(#mu#mu)",2600,3600)
                                )


        line = Hlt2Line('TrackEffMuonTTLL1'
                      , prescale = self.prescale
                      , L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('DiMuon')"
                      , algos = [ BiKalmanFittedMuons, filterPlus1, TOSTagMuonsFilter1, filterMinusLL1, JPsiCombineLL1 ]
                      , postscale = self.postscale
                      )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TrackEffMuonTTLL1Decision" : 50611 } )

	###################### positive tag track, negative long track ###########################

        filterMinus2 = Hlt2Member( FilterDesktop
                                , "filterMinus"
                                , Code = "(Q < 0) & (PT>%(MuonTTLongPt)s*MeV) & (P>%(MuonTTLongP)s*MeV) & (TRCHI2DOF<%(MuonTTLongTrChi2)s) & (PIDmu >-%(MuonTTLongMuonPID)s)"%self.getProps()
                                , Inputs  = [ BiKalmanFittedMuons ]
                                )

        TOSTagMuonsFilter2 = Hlt2Member( TisTosParticleTagger
                                         ,"TOSTagMuonsFilter"
                                         ,TisTosSpecs = { "Hlt1TrackMuonDecision%TOS":0 }
                                         ,Inputs = [ filterMinus2 ]
                                         ,NoRegex=True
                                         ,ProjectTracksToCalo = False
                                         ,CaloClustForCharged = False
                                         ,CaloClustForNeutral = False
                                         ,TOSFrac = { 4:0.0, 5:0.0 }
                                         )

        filterPlus2 = Hlt2Member( FilterDesktop
                                 , "filterPlus"
                                 , Code = "(Q > 0) & (PT>%(MuonTTProbePt)s*MeV) & (P>%(MuonTTProbeP)s*MeV)"%self.getProps()
                                 , Inputs  = [ ProbeMuonTTMuons ]
                                 , PreMonitor = Hlt2MonitorMinMax ("TRCHI2DOF","M(#mu#mu)",0,10)
                                 )
                             
        JPsiCombine2 = Hlt2Member( CombineParticles
                      , 'JPsiCombine'
                      , Inputs = [ TOSTagMuonsFilter2, filterPlus2 ]
                      , DecayDescriptor = 'J/psi(1S) -> mu+ mu-'
                      , MotherCut =  "(PT>%(MuonTTJPsiPt)s*MeV) & (P>%(MuonTTJPsiP)s*MeV) & (MIPDV(PRIMARY)<%(MuonTTJPsiMaxIP)s) & (VFASPF(VCHI2/VDOF)<%(MuonTTJPsiVtxChi2)s) & (ADMASS('J/psi(1S)')<%(MuonTTMassWindow)s*MeV)"%self.getProps()
                      , MotherMonitor = Hlt2MonitorMinMax ("M","M(#mu#mu)",2600,3600)
                      )


        line = Hlt2Line('TrackEffMuonTT2'
                      , prescale = self.prescale
                      , L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('DiMuon')"
                      , algos = [ BiKalmanFittedMuons, filterMinus2, TOSTagMuonsFilter2, ProbeMuonTTMuons, filterPlus2, JPsiCombine2 ]
                      , postscale = self.postscale
                      )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TrackEffMuonTT2Decision" : 50602 } )

	##################### respective line with two long tracks, needed for association #########################

        filterPlusLL2 = Hlt2Member( FilterDesktop
                                 , "filterPlus"
                                 , Code = "(Q > 0) & (PT>%(MuonTTProbePt)s*MeV) & (P>%(MuonTTProbeP)s*MeV)"%self.getProps()
                                 , Inputs  = [ BiKalmanFittedMuons ]
                                 , PreMonitor = Hlt2MonitorMinMax ("TRCHI2DOF","M(#mu#mu)",0,10)
                                 )
                             
        JPsiCombineLL2 = Hlt2Member( CombineParticles
                      , 'JPsiCombine'
                      , Inputs = [ TOSTagMuonsFilter2, filterPlusLL2 ]
                      , DecayDescriptor = 'J/psi(1S) -> mu+ mu-'
                      , MotherCut =  "(PT>%(MuonTTJPsiPt)s*MeV) & (P>%(MuonTTJPsiP)s*MeV) & (MIPDV(PRIMARY)<%(MuonTTJPsiMaxIP)s) & (VFASPF(VCHI2/VDOF)<%(MuonTTJPsiVtxChi2)s) & (ADMASS('J/psi(1S)')<%(MuonTTMassWindow)s*MeV)"%self.getProps()
                      , MotherMonitor = Hlt2MonitorMinMax ("M","M(#mu#mu)",2600,3600)
                      )


        line = Hlt2Line('TrackEffMuonTTLL2'
                      , prescale = self.prescale
                      , L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('DiMuon')"
                      , algos = [ BiKalmanFittedMuons, filterMinus2, TOSTagMuonsFilter2, filterPlusLL2, JPsiCombineLL2 ]
                      , postscale = self.postscale
                      )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TrackEffMuonTTLL2Decision" : 50612 } )

	########################### VeloMuon method #######################

	# Create two triggerlines to distunguish positive-charge long and negative tag-track and vice versa

	############### positive long track, negative tag track #########################


        filterVeloPlus1 = Hlt2Member( FilterDesktop
                                , "filterPlus"
                                , Code = "(Q > 0) & (TRCHI2DOF <%(VeloLongTrchi2)s) & (P>%(VeloLongP)s*MeV) & (PT>%(VeloLongPt)s*MeV) & (PIDmu >-%(VeloLongMuonPID)s)"%self.getProps()
                                , Inputs  = [ BiKalmanFittedMuons ]
                                )

        TOSTagVeloMuonsFilter1 = Hlt2Member( TisTosParticleTagger
                                         ,"TOSTagMuonsFilter"
					 ,TisTosSpecs = { "Hlt1TrackMuonDecision%TOS":0 }
                                         ,Inputs = [ filterVeloPlus1 ]
                                         ,NoRegex=True
                                         ,ProjectTracksToCalo = False
                                         ,CaloClustForCharged = False
                                         ,CaloClustForNeutral = False
                                         ,TOSFrac = { 4:0.0, 5:0.0 }
                                         )

        filterVeloMinus1 = Hlt2Member( FilterDesktop
                                 , "filterMinus"
                                 , Code = "(Q < 0) & (TRCHI2DOF <%(VeloProbeTrChi2)s) & (PT>%(VeloProbePt)s*MeV)"%self.getProps()
                                 , Inputs  = [ ProbeVeloMuons ]
                                 )


        JPsiVeloCombine1 = Hlt2Member( CombineParticles
                                , 'JPsiCombine'
                                , Inputs = [ TOSTagVeloMuonsFilter1, filterVeloMinus1 ]
                                , DecayDescriptor = 'J/psi(1S) -> mu+ mu-'
                                , MotherCut =  "(VFASPF(VCHI2/VDOF)<%(VeloVertchi2)s) & (PT>%(VeloJPsiPt)s*MeV) & (ADMASS('J/psi(1S)')<%(VeloMassWindow)s*MeV)"%self.getProps()
                                , MotherMonitor = Hlt2MonitorMinMax ("M","M(#mu#mu)",2600,3600)
                                )


        line = Hlt2Line('TrackEffVeloMuon1'
                      , prescale = self.prescale
                      , L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('DiMuon')"
                      , algos = [ BiKalmanFittedMuons, filterVeloPlus1, TOSTagVeloMuonsFilter1, ProbeVeloMuons, filterVeloMinus1, JPsiVeloCombine1 ]
                      , postscale = self.postscale
                      )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TrackEffVeloMuon1Decision" : 50603 } )

	##################### respective line with two long tracks, needed for association #########################

        filterVeloMinusLL1 = Hlt2Member( FilterDesktop
                                 , "filterMinus"
                                 , Code = "(Q < 0) & (TRCHI2DOF <%(VeloProbeTrChi2)s) & (PT>%(VeloProbePt)s*MeV)"%self.getProps()
                                 , Inputs  = [ BiKalmanFittedMuons ]
                                 )


        JPsiVeloCombineLL1 = Hlt2Member( CombineParticles
                                , 'JPsiCombine'
                                , Inputs = [ TOSTagVeloMuonsFilter1, filterVeloMinusLL1 ]
                                , DecayDescriptor = 'J/psi(1S) -> mu+ mu-'
                                , MotherCut =  "(VFASPF(VCHI2/VDOF)<%(VeloVertchi2)s) & (PT>%(VeloJPsiPt)s*MeV) & (ADMASS('J/psi(1S)')<%(VeloMassWindow)s*MeV)"%self.getProps()
                                , MotherMonitor = Hlt2MonitorMinMax ("M","M(#mu#mu)",2600,3600)
                                )


        line = Hlt2Line('TrackEffVeloMuonLL1'
                      , prescale = self.prescale
                      , L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('DiMuon')"
                      , algos = [ BiKalmanFittedMuons, filterVeloPlus1, TOSTagVeloMuonsFilter1, filterVeloMinusLL1, JPsiVeloCombineLL1 ]
                      , postscale = self.postscale
                      )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TrackEffVeloMuonLL1Decision" : 50613 } )

	################ positive tag track, negative long track ######################

        filterVeloMinus2 = Hlt2Member( FilterDesktop
                                , "filterMinus"
                                , Code = "(Q < 0) & (TRCHI2DOF <%(VeloLongTrchi2)s) & (P>%(VeloLongP)s*MeV) & (PT>%(VeloLongPt)s*MeV) & (PIDmu >-%(VeloLongMuonPID)s)"%self.getProps()
                                , Inputs  = [ BiKalmanFittedMuons ]
                                )

        TOSTagVeloMuonsFilter2 = Hlt2Member( TisTosParticleTagger
                                         ,"TOSTagMuonsFilter"
					 ,TisTosSpecs = { "Hlt1TrackMuonDecision%TOS":0 }
                                         ,Inputs = [ filterVeloMinus2 ]
                                         ,NoRegex=True
                                         ,ProjectTracksToCalo = False
                                         ,CaloClustForCharged = False
                                         ,CaloClustForNeutral = False
                                         ,TOSFrac = { 4:0.0, 5:0.0 }
                                         )

        filterVeloPlus2 = Hlt2Member( FilterDesktop
                                 , "filterPlus"
                                 , Code = "(Q > 0) & (TRCHI2DOF <%(VeloProbeTrChi2)s) & (PT>%(VeloProbePt)s*MeV)"%self.getProps()
                                 , Inputs  = [ ProbeVeloMuons ]
                                 )


        JPsiVeloCombine2 = Hlt2Member( CombineParticles
                                , 'JPsiCombine'
                                , Inputs = [ TOSTagVeloMuonsFilter2, filterVeloPlus2 ]
                                , DecayDescriptor = 'J/psi(1S) -> mu+ mu-'
                                , MotherCut =  "(VFASPF(VCHI2/VDOF)<%(VeloVertchi2)s) & (PT>%(VeloJPsiPt)s*MeV) & (ADMASS('J/psi(1S)')<%(VeloMassWindow)s*MeV)"%self.getProps()
                                , MotherMonitor = Hlt2MonitorMinMax ("M","M(#mu#mu)",2600,3600)
                                )


        line = Hlt2Line('TrackEffVeloMuon2'
                      , prescale = self.prescale
                      , L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('DiMuon')"
                      , algos = [ BiKalmanFittedMuons, filterVeloMinus2, TOSTagVeloMuonsFilter2, ProbeVeloMuons, filterVeloPlus2, JPsiVeloCombine2 ]
                      , postscale = self.postscale
                      )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TrackEffVeloMuon2Decision" : 50604 } )

	##################### respective line with two long tracks, needed for association #########################

        filterVeloPlusLL2 = Hlt2Member( FilterDesktop
                                 , "filterPlus"
                                 , Code = "(Q > 0) & (TRCHI2DOF <%(VeloProbeTrChi2)s) & (PT>%(VeloProbePt)s*MeV)"%self.getProps()
                                 , Inputs  = [ BiKalmanFittedMuons ]
                                 )


        JPsiVeloCombineLL2 = Hlt2Member( CombineParticles
                                , 'JPsiCombine'
                                , Inputs = [ TOSTagVeloMuonsFilter2, filterVeloPlusLL2 ]
                                , DecayDescriptor = 'J/psi(1S) -> mu+ mu-'
                                , MotherCut =  "(VFASPF(VCHI2/VDOF)<%(VeloVertchi2)s) & (PT>%(VeloJPsiPt)s*MeV) & (ADMASS('J/psi(1S)')<%(VeloMassWindow)s*MeV)"%self.getProps()
                                , MotherMonitor = Hlt2MonitorMinMax ("M","M(#mu#mu)",2600,3600)
                                )


        line = Hlt2Line('TrackEffVeloMuonLL2'
                      , prescale = self.prescale
                      , L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('DiMuon')"
                      , algos = [ BiKalmanFittedMuons, filterVeloMinus2, TOSTagVeloMuonsFilter2, filterVeloPlusLL2, JPsiVeloCombineLL2 ]
                      , postscale = self.postscale
                      )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TrackEffVeloMuonLL2Decision" : 50614 } )

	############## Downstream method #######################

	# Create two triggerlines to distunguish positive-charge long and negative tag-track and vice versa

	############### positive long track, negative tag track #########################

        filterDownstreamPlus1 = Hlt2Member( FilterDesktop
                                , "filterPlus"
                                , Code = "(Q > 0) & (TRCHI2DOF <%(DownstreamLongTrchi2)s) & (P>%(DownstreamLongP)s*MeV) & (PT>%(DownstreamLongPt)s*MeV) & (MIPDV(PRIMARY)>%(DownstreamLongMinIP)s)"%self.getProps()
                                , Inputs  = [ BiKalmanFittedMuons ]
                                )

        TOSTagDownstreamMuonsFilter1 = Hlt2Member( TisTosParticleTagger
                                         ,"TOSTagMuonsFilter"
                                         ,TisTosSpecs = { "Hlt1TrackMuonDecision%TOS":0 }
                                         ,Inputs = [ filterDownstreamPlus1 ]
                                         ,NoRegex=True
                                         ,ProjectTracksToCalo = False
                                         ,CaloClustForCharged = False
                                         ,CaloClustForNeutral = False
                                         ,TOSFrac = { 4:0.0, 5:0.0 }
                                         )

        filterDownstreamMinus1 = Hlt2Member( FilterDesktop
                                 , "filterMinus"
                                 , Code = "(Q < 0) & (TRCHI2DOF <%(DownstreamProbeTrchi2)s) & (PT>%(DownstreamProbePt)s*MeV) & (P>%(DownstreamProbeP)s*MeV) & (PIDmu >-%(DownstreamLongMuonPID)s)"%self.getProps()
                                 , Inputs  = [ ProbeDownstreamMuons ]
                                 )


        JPsiDownstreamCombine1 = Hlt2Member( CombineParticles
                                , 'JPsiCombine'
                                , Inputs = [ TOSTagDownstreamMuonsFilter1, filterDownstreamMinus1 ]
                                , DecayDescriptor = 'J/psi(1S) -> mu+ mu-'
				, CombinationCut = "(AMAXDOCA('') < %(DownstreamJPsiDOCA)s*mm)"%self.getProps()
                                , MotherCut =  "(VFASPF(VCHI2/VDOF)<%(DownstreamVertchi2)s) & (PT>%(DownstreamJPsiPt)s*MeV) & (ADMASS('J/psi(1S)')<%(DownstreamMassWindow)s*MeV)"%self.getProps()
                                , MotherMonitor = Hlt2MonitorMinMax ("M","M(#mu#mu)",2600,3600)
                                )


        line = Hlt2Line('TrackEffDownstream1'
                      , prescale = self.prescale
                      , L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('DiMuon')"
                      , algos = [ BiKalmanFittedMuons, filterDownstreamPlus1, TOSTagDownstreamMuonsFilter1, ProbeDownstreamMuons, filterDownstreamMinus1, JPsiDownstreamCombine1 ]
                      , postscale = self.postscale
                      )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TrackEffDownstream1Decision" : 50605 } )

	##################### respective line with two long tracks, needed for association #########################

        filterDownstreamMinusLL1 = Hlt2Member( FilterDesktop
                                 , "filterMinus"
                                 , Code = "(Q < 0) & (TRCHI2DOF <%(DownstreamProbeTrchi2)s) & (PT>%(DownstreamProbePt)s*MeV) & (P>%(DownstreamProbeP)s*MeV) & (PIDmu >-%(DownstreamLongMuonPID)s)"%self.getProps()
                                 , Inputs  = [ BiKalmanFittedMuons ]
                                 )


        JPsiDownstreamCombineLL1 = Hlt2Member( CombineParticles
                                , 'JPsiCombine'
                                , Inputs = [ TOSTagDownstreamMuonsFilter1, filterDownstreamMinusLL1 ]
                                , DecayDescriptor = 'J/psi(1S) -> mu+ mu-'
				, CombinationCut = "(AMAXDOCA('') < %(DownstreamJPsiDOCA)s*mm)"%self.getProps()
                                , MotherCut =  "(VFASPF(VCHI2/VDOF)<%(DownstreamVertchi2)s) & (PT>%(DownstreamJPsiPt)s*MeV) & (ADMASS('J/psi(1S)')<%(DownstreamMassWindow)s*MeV)"%self.getProps()
                                , MotherMonitor = Hlt2MonitorMinMax ("M","M(#mu#mu)",2600,3600)
                                )


        line = Hlt2Line('TrackEffDownstreamLL1'
                      , prescale = self.prescale
                      , L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('DiMuon')"
                      , algos = [ BiKalmanFittedMuons, filterDownstreamPlus1, TOSTagDownstreamMuonsFilter1, filterDownstreamMinusLL1, JPsiDownstreamCombineLL1 ]
                      , postscale = self.postscale
                      )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TrackEffDownstreamLL1Decision" : 50615 } )

	################# positive tag track, negative long track ####################

        filterDownstreamMinus2 = Hlt2Member( FilterDesktop
                                , "filterMinus"
                                , Code = "(Q < 0) & (TRCHI2DOF <%(DownstreamLongTrchi2)s) & (P>%(DownstreamLongP)s*MeV) & (PT>%(DownstreamLongPt)s*MeV) & (PIDmu >-%(DownstreamLongMuonPID)s) & (MIPDV(PRIMARY)>%(DownstreamLongMinIP)s)"%self.getProps()
                                , Inputs  = [ BiKalmanFittedMuons ]
                                )

        TOSTagDownstreamMuonsFilter2 = Hlt2Member( TisTosParticleTagger
                                         ,"TOSTagMuonsFilter"
                                         ,TisTosSpecs = { "Hlt1TrackMuonDecision%TOS":0 }
                                         ,Inputs = [ filterDownstreamMinus2 ]
                                         ,NoRegex=True
                                         ,ProjectTracksToCalo = False
                                         ,CaloClustForCharged = False
                                         ,CaloClustForNeutral = False
                                         ,TOSFrac = { 4:0.0, 5:0.0 }
                                         )

        filterDownstreamPlus2 = Hlt2Member( FilterDesktop
                                 , "filterPlus"
                                 , Code = "(Q > 0) & (TRCHI2DOF <%(DownstreamProbeTrchi2)s) & (PT>%(DownstreamProbePt)s*MeV) & (P>%(DownstreamProbeP)s*MeV)"%self.getProps()
                                 , Inputs  = [ ProbeDownstreamMuons ]
                                 )


        JPsiDownstreamCombine2 = Hlt2Member( CombineParticles
                                , 'JPsiCombine'
                                , Inputs = [ TOSTagDownstreamMuonsFilter2, filterDownstreamPlus2 ]
                                , DecayDescriptor = 'J/psi(1S) -> mu+ mu-'
				, CombinationCut = "(AMAXDOCA('') < %(DownstreamJPsiDOCA)s*mm)"%self.getProps()
                                , MotherCut =  "(VFASPF(VCHI2/VDOF)<%(DownstreamVertchi2)s) & (PT>%(DownstreamJPsiPt)s*MeV) & (ADMASS('J/psi(1S)')<%(DownstreamMassWindow)s*MeV)"%self.getProps()
                                , MotherMonitor = Hlt2MonitorMinMax ("M","M(#mu#mu)",2600,3600)
                                )


        line = Hlt2Line('TrackEffDownstream2'
                      , prescale = self.prescale
                      , L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('DiMuon')"
                      , algos = [ BiKalmanFittedMuons, filterDownstreamMinus2, TOSTagDownstreamMuonsFilter2, ProbeDownstreamMuons, filterDownstreamPlus2, JPsiDownstreamCombine2 ]
                      , postscale = self.postscale
                      )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TrackEffDownstream2Decision" : 50606 } )

	##################### respective line with two long tracks, needed for association #########################

        filterDownstreamPlusLL2 = Hlt2Member( FilterDesktop
                                 , "filterPlus"
                                 , Code = "(Q > 0) & (TRCHI2DOF <%(DownstreamProbeTrchi2)s) & (PT>%(DownstreamProbePt)s*MeV) & (P>%(DownstreamProbeP)s*MeV)"%self.getProps()
                                 , Inputs  = [ BiKalmanFittedMuons ]
                                 )


        JPsiDownstreamCombineLL2 = Hlt2Member( CombineParticles
                                , 'JPsiCombine'
                                , Inputs = [ TOSTagDownstreamMuonsFilter2, filterDownstreamPlusLL2 ]
                                , DecayDescriptor = 'J/psi(1S) -> mu+ mu-'
				, CombinationCut = "(AMAXDOCA('') < %(DownstreamJPsiDOCA)s*mm)"%self.getProps()
                                , MotherCut =  "(VFASPF(VCHI2/VDOF)<%(DownstreamVertchi2)s) & (PT>%(DownstreamJPsiPt)s*MeV) & (ADMASS('J/psi(1S)')<%(DownstreamMassWindow)s*MeV)"%self.getProps()
                                , MotherMonitor = Hlt2MonitorMinMax ("M","M(#mu#mu)",2600,3600)
                                )


        line = Hlt2Line('TrackEffDownstreamLL2'
                      , prescale = self.prescale
                      , L0DU = "L0_CHANNEL('Muon') | L0_CHANNEL('DiMuon')"
                      , algos = [ BiKalmanFittedMuons, filterDownstreamMinus2, TOSTagDownstreamMuonsFilter2, filterDownstreamPlusLL2, JPsiDownstreamCombineLL2 ]
                      , postscale = self.postscale
                      )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TrackEffDownstreamLL2Decision" : 50616 } )
