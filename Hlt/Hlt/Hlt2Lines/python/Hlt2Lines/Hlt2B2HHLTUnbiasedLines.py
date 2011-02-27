from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2HHLTUnbiasedLinesConf(HltLinesConfigurableUser) :

    __slots__ = {  'BMassWinLow'         :  5000      # MeV
                   ,'BMassWinHigh'       :  5900      # MeV
                   ,'doca'               :     0.1
                   ,'KaonPTmin'          :  1000       # MeV
                   ,'KaonPTmax'          :  1500       # MeV
                   ,'KaonPmin'           : 10000       # MeV
                   ,'BPmin'              : 10000
                   ,'PIDK_max'           :     0.1
                   ,'TrackChi2'          :    5.
                   ,'VertexChi2'         :    10.0
                   ,'AbsCosTheta'        :     0.9
                   ,'Expertise'          : '2010Tuning'
                   ,'NetCutNoPID'        :     (1.0 + ( 0.0))*0.5
                   ,'NetCut'             :     (1.0 + (-1.0))*0.5
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
                               Inputs  = [BiKalmanFittedKaons ],
                               Code    = childCutNoPID)
        
        bindPrelim = bindMembers('bindPrelim', [ BiKalmanFittedKaons, filterAll])
        
        ## HLT1 TIS Filter
        
        HLT1TISFilter = TisTosParticleTagger("HLT1TISFilter")
        HLT1TISFilter.TisTosSpecs = { "Hlt1.*Decision%TIS":0 }
        HLT1TISFilter.Inputs = [ bindPrelim.outputSelection() ]
        HLT1TISFilter.Output = 'Hlt2/HLT1TISFilter/Particles'
        
        
        tisFilteredKaons = bindMembers("tisFilteredKaons", [ bindPrelim, HLT1TISFilter ])

        
        ##
        ## prepare the candidates
        ##
        Hlt2B2HHLTUnbiased = Hlt2Member( CombineParticles 
                                         , "Combine"     
                                         , DecayDescriptor = "B_s0 -> K+ K-"
                                         , DaughtersCuts   = { "K+" : childCutNoPID}
                                         , CombinationCut  = combCut 
                                         , MotherCut       = motherCutNoPID
                                         , Inputs          = [ tisFilteredKaons] 
                                         )

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
                                   Inputs = [Hlt2B2HHLTUnbiased ],
                                   Code    = cuts,
                                   tools   = [NBBhhNoPIDTool])
        nbNoPIDFilteredBhh = bindMembers('NBBhhNoPIDFilter', [Hlt2B2HHLTUnbiased, NBNoPIDFilter])
        


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
                                    Inputs = [BiKalmanFittedRichKaons ],
                                    Code   = childCutPID)

        bindPrelimRich = bindMembers('bindPrelimRich', [ BiKalmanFittedRichKaons, filterRich])

        ## HLT1 TIS Filter
        HLT1TISRichFilter = TisTosParticleTagger("HLT1TISRichFilter")
        HLT1TISRichFilter.TisTosSpecs = { "Hlt1.*Decision%TIS":0 }
        HLT1TISRichFilter.Inputs      = [ bindPrelimRich.outputSelection() ]

        tisFilteredRichKaons = bindMembers("tisFilteredRichKaons", [ bindPrelimRich, HLT1TISRichFilter ])


        ##
        ## prepare the candidates
        ##
        Hlt2B2HHLTUnbiasedRich = Hlt2Member( CombineParticles
                                             , "CombineRich"
                                             , DecayDescriptor = "B_s0 -> K+ K-"
                                             , DaughtersCuts   = { "K+" : childCutPID}
                                             , CombinationCut  = combCut
                                             , MotherCut       = motherCutPID
                                             , Inputs          = [ tisFilteredRichKaons]
                                             )
                                                                
        

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
                              Inputs = [Hlt2B2HHLTUnbiasedRich ],
                              Code    = cuts,
                              tools   = [NBBhhTool])
        nbFilteredBhh = bindMembers('NBBhhFilter', [Hlt2B2HHLTUnbiasedRich, NBFilter])
                            
        
        ###########################################################################
        # Define the Hlt2 Line
        #
        line = Hlt2Line('B2HHLTUnbiased'
                        , prescale = self.prescale
                        #, HLT = "HLT_PASS_RE('Hlt1DiHadronLTUnbiasedDecision')" 
                        , algos = [ BiKalmanFittedKaons    ,tisFilteredKaons,  Hlt2B2HHLTUnbiased, nbNoPIDFilteredBhh, 
                                    BiKalmanFittedRichKaons, tisFilteredRichKaons, Hlt2B2HHLTUnbiasedRich, nbFilteredBhh ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2HHLTUnbiasedDecision" : 50081 } )
