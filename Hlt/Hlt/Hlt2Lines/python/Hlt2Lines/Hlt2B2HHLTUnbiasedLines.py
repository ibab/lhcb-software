from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2HHLTUnbiasedLinesConf(HltLinesConfigurableUser) :

    __slots__ = {  'BMassWinLow'         :  5000       # MeV
                   ,'BMassWinHigh'       :  5900       # MeV
                   ,'BLifetime'          :     0.1     # ps 
                   ,'doca'               :     0.1
                   ,'KaonPTmin'          :  1000       # MeV
                   ,'KaonPTmax'          :  1500       # MeV
                   ,'KaonPmin'           : 10000       # MeV
                   ,'BPmin'              : 10000
                   ,'PIDK_max'           :     0.1
                   ,'TrackChi2'          :    3.
                   ,'VertexChi2'         :    10.0
                   ,'AbsCosTheta'        :     0.9
                   ,'Expertise'          : '2010Tuning'
                   ,'NetCutNoPID'        :     (1.0 + ( 0.3))*0.5
                   ,'NetCut'             :     (1.0 + (-1.0))*0.5
                   ,'Prescale'           : {}
                   ,'Postscale'          : {}
                     }
    


    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from HltLine.HltLine import bindMembers 
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from Configurables import TisTosParticleTagger
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedRichKaons
        from Configurables   import NBB2HHTriggerTool
        from HltLine.HltLine import Hlt1Tool as Tool
        from NeuroBayesTools.BhhNetTrigger   import NetConfig


        #######################################################################################################
        #######################################################################################################
        ##
        ## cuts
        ##
        #######################################################################################################
        #######################################################################################################
        childCutNoPID  = "ISLONG & (TRCHI2DOF<%(TrackChi2)s) & (P> %(KaonPmin)s *MeV) &(PT> %(KaonPTmin)s *MeV)"                                                           % self.getProps()
        childCutPID    = childCutNoPID +  " & (PPCUT(PP_RICHTHRES_K))"
        combCut        = "((AM> %(BMassWinLow)s *MeV) & (AM< %(BMassWinHigh)s *MeV) & (AMAXDOCA('LoKi::DistanceCalculator')< %(doca)s ))"                                  % self.getProps()
        motherCutNoPID = "(P>%(BPmin)s * MeV) & (MAXTREE(('K+'==ABSID) ,PT) > %(KaonPTmax)s*MeV)  & (VFASPF(VCHI2PDOF)< %(VertexChi2)s) & (abs(LV01) < %(AbsCosTheta)s)"   % self.getProps()
        motherCutPID   = motherCutNoPID + " & (MAXTREE(('K+'==ABSID) , PIDK) > %(PIDK_max)s)"                                                                              % self.getProps()
        motherCutPIDLT = motherCutNoPID + " & (MAXTREE(('K+'==ABSID) , PIDK) > %(PIDK_max)s) & (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(BLifetime)s*ps)"        % self.getProps() 

        #######################################################################################################
        #######################################################################################################
        ##
        ## first part: kinematics only, no RICH ID
        ##
        #######################################################################################################
        #######################################################################################################

        ##
        ## get HLT1 TIS events 
        ##

        ## filter for kaons
        filterAll = Hlt2Member(FilterDesktop,
                               'FilterAll',
                               Inputs = [ BiKalmanFittedKaons ],
                               Code    = childCutNoPID)
        nopidAlgos = [ BiKalmanFittedKaons, filterAll ]
        ## HLT1 TIS Filter
        HLT1TISFilter = Hlt2Member ( TisTosParticleTagger
                                     , 'HLT1TISFilter'
                                     , TisTosSpecs = { "Hlt1.*Decision%TIS":0 }
                                     , Inputs = [ filterAll ]
                                     )
        nopidAlgos += [ HLT1TISFilter ]
        

        ##
        ## prepare the candidates
        ##
        CombineBsKK = Hlt2Member( CombineParticles 
                                  , "CombineBsKK"     
                                  , DecayDescriptor = "B_s0 -> K+ K-"
                                  , DaughtersCuts   = { "K+" : childCutNoPID}
                                  , CombinationCut  = combCut 
                                  , MotherCut       = motherCutNoPID
                                  , Inputs = [ HLT1TISFilter ]
                                  )
        nopidAlgos += [ CombineBsKK ]
        
        ##
        ## first NeuroBayes network - no PID information (kinemematics + calo)
        ##
        NBName        = '%(Expertise)s' % self.getProps()
        expertise     = NetConfig[ NBName+'NoPID' ]
        NBCut         = self.getProps()['NetCutNoPID']
        NBBhhNoPIDTool = Tool(type        = NBB2HHTriggerTool,
                              name        = 'TrigNBB2HHNoPID',
                              Expertise   =  expertise ,
                              UsePID      = False,
                              DoPlot      = False,
                              NetworkCut  = NBCut
                              )
        cuts = "FILTER('NBB2HHTriggerTool/TrigNBB2HHNoPID')"
        NBNoPIDFilter = Hlt2Member(FilterDesktop, 'FilterNBBhhNoPID',
                                   Code    = cuts,
                                   Inputs = [ CombineBsKK ],
                                   tools   = [NBBhhNoPIDTool])
        nopidAlgos += [ NBNoPIDFilter ]


        #######################################################################################################
        #######################################################################################################
        ##
        ## second part - on the surviving events, use RICH ID
        ##
        #######################################################################################################
        #######################################################################################################

        ##
        ## get HLT1 TIS events
        ##

        ## filter for kaons
        filterRich     = Hlt2Member(FilterDesktop,
                                    'FilterRich',
                                    Inputs = [ BiKalmanFittedRichKaons ],
                                    Code   = childCutPID)

        richpidAlgos =  [ BiKalmanFittedRichKaons, filterRich]

        ## HLT1 TIS Filter
        HLT1TISRichFilter = Hlt2Member( TisTosParticleTagger
                                        , 'HLT1TISRichFilter'
                                        , TisTosSpecs = { "Hlt1.*Decision%TIS":0 }
                                        , Inputs = [ filterRich ],
                                        )
        richpidAlgos +=  [ HLT1TISRichFilter ]

        ##
        ## prepare the candidates
        ##
        CombineBsKKRich = Hlt2Member( CombineParticles
                                      , "CombineBsKKRich"
                                      , DecayDescriptor = "B_s0 -> K+ K-"
                                      , DaughtersCuts   = { "K+" : childCutPID}
                                      , CombinationCut  = combCut
                                      , MotherCut       = motherCutPID
                                      , Inputs = [ HLT1TISRichFilter ],
                                      )
                                                                
        richpidAlgos +=  [ CombineBsKKRich  ]
        

        ##
        ## call NeuroBayes
        ##
        NBName    = '%(Expertise)s' % self.getProps() 
        expertise = NetConfig[ NBName ]
        NBCut     = self.getProps()['NetCut']
        NBBhhTool = Tool(type        = NBB2HHTriggerTool,
                         name        = 'TrigNBB2HH',
                         Expertise   =  expertise ,
                         UsePID      = True,
                         DoPlot      = False,
                         NetworkCut  = NBCut
                         )
        cuts = "FILTER('NBB2HHTriggerTool/TrigNBB2HH')"
        NBFilter = Hlt2Member(FilterDesktop, 'FilterNBBhh',
                              Code    = cuts,
                              Inputs = [ CombineBsKKRich ],
                              tools   = [NBBhhTool])
        richpidAlgos +=  [ NBFilter ]
                            
        
        ###########################################################################
        # Define the Hlt2 Line
        #
        line = Hlt2Line('B2HHLTUnbiased'
                        , prescale = self.prescale
                        #, HLT = "HLT_PASS_RE('Hlt1DiHadronLTUnbiasedDecision')" 
                        , algos = nopidAlgos + richpidAlgos 
                        , postscale = self.postscale
                        )
        from HltTracking.HltPVs import PV3D
        line.clone('B2HHLTUnbiasedDetached'
                   , prescale        = self.prescale
                   , postscale       = self.postscale
                   , algos           = [PV3D()] +nopidAlgos + richpidAlgos
                   , CombineBsKKRich = { "MotherCut"  :  motherCutPIDLT}
                   )


        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2HHLTUnbiasedDecision"         : 50081 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2HHLTUnbiasedDetachedDecision" : 50082 } )
