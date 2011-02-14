# Import some useful modules
from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

## Create a python class that inherits from HltLinesConfigurableUser
class Hlt2CharmHadD2KS0HLinesConf(HltLinesConfigurableUser) :

# define some values so that they are not hard-coded
    __slots__ = {

   # KS0 daughter pion cuts
      'KS0DaugP'         : 1800
,     'KS0DaugPT'        : 100
,     'KS0DaugTrackChi2' : 5  
,     'KS0DaugMIPChi2'   : 50
#,     'KS0DaugPIDK'      : 20

   # KS0 mother cuts
,     'KS0MassWindow'    : 50
,     'KS0MIPChi2'       : 0
,     'KS0VertexChi2'    : 20
,     'KS0PT'            : 700

      # Bachelor pion cuts
,     'BachPionP'         : 4000       
,     'BachPionPT'        : 400 
,     'BachPionTrackChi2' : 5
,     'BachPionMIPChi2'   : 30
#,     'BachPionPIDK'      : 10

      # Bachelor kaon cuts
,     'BachKaonP'         : 4000       
,     'BachKaonPT'        : 400
,     'BachKaonTrackChi2' : 5
,     'BachKaonMIPChi2'   : 30
#,     'BachKaonPIDK'      : -30

      # D meson cuts
      # Combo cuts
,     'DMesonComboLowMass'   : 1790       
,     'DMesonComboHighMass'  : 2040 
,     'DMesonComboDOCA'      : 0.6
      # Mother cuts
,     'DMesonMotherLowMass'    : 1800         
,     'DMesonMotherHighMass'   : 2030
,     'DMesonMotherVertexChi2' : 20
,     'DMesonMotherMIPChi2'    : 25
,     'DMesonMotherPT'         : 1800

# Extra stuff
        
# prescales
                  , 'Prescale'                  : {
                        'Hlt2CharmHadD2KS0Pi'    : 1.0
                       , 'Hlt2CharmHadD2KS0K'    : 1.0 
                        }
                  , 'HltANNSvcID'  : {
                       'Hlt2CharmHadD2KS0PiDecision' : 50913
                       ,'Hlt2CharmHadD2KS0KDecision' : 50914
                        }
                }

# Use this section to create a KS0 LL filter - DD not possible due to seeding issues 

    def __KS0LLFilter(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers           
        from Configurables import FilterDesktop, CombineParticles      
        from HltTracking.HltPVs import PV3D
        from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

        incuts = "CHILDCUT((TRCHI2DOF < %(KS0DaugTrackChi2)s),1) & CHILDCUT((TRCHI2DOF < %(KS0DaugTrackChi2)s),2) & CHILDCUT((P > %(KS0DaugP)s*MeV),1) & CHILDCUT((P > %(KS0DaugP)s*MeV),2) & CHILDCUT((PT > %(KS0DaugPT)s*MeV),1) & CHILDCUT((PT > %(KS0DaugPT)s*MeV),2) & CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s),1) & CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s),2) & (ADMASS('KS0') < %(KS0MassWindow)s*MeV) & (PT > %(KS0PT)s*MeV) & (VFASPF(VCHI2/VDOF) < %(KS0VertexChi2)s)"   % self.getProps()

  # CHILDCUT((PIDK < %(KS0DaugPIDK)s),1) & CHILDCUT((PIDK < %(KS0DaugPIDK)s),2)
  # & (MIPCHI2DV(PRIMARY) > %(KS0MIPChi2)s)

# Define the HLT2 member with it's necessary inputs
        filter = Hlt2Member( FilterDesktop	
                            , 'Filter'
                            , InputLocations = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP 
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )      

        return filterSeq

# Use this section to create a bachelor filter

    def __BachelorPionFilter(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers           
        from Configurables import FilterDesktop, CombineParticles      
        from HltTracking.HltPVs import PV3D                           

        Bachelor_Cuts_Pion = "(P > %(BachPionP)s*MeV) & (PT > %(BachPionPT)s*MeV) & (TRCHI2DOF < %(BachPionTrackChi2)s) & (MIPCHI2DV(PRIMARY) > %(BachPionMIPChi2)s)"  % self.getProps()

#(PIDK < %(BachPionPIDK)s) & 

# Define the HLT2 member with it's necessary inputs
        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = inputContainers
                            , Code = Bachelor_Cuts_Pion
                           )

        ## Require the PV3D reconstruction before our cut on IP - what does this actually mean???
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )      

        return filterSeq

    def __BachelorKaonFilter(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers           
        from Configurables import FilterDesktop, CombineParticles      
        from HltTracking.HltPVs import PV3D                           

        Bachelor_Cuts_Kaon =  "(P > %(BachKaonP)s*MeV) & (PT > %(BachKaonPT)s*MeV) & (TRCHI2DOF < %(BachKaonTrackChi2)s) & (MIPCHI2DV(PRIMARY) > %(BachKaonMIPChi2)s)"  % self.getProps()

#(PIDK > %(BachKaonPIDK)s) & 

# Define the HLT2 member with it's necessary inputs
        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = inputContainers
                            , Code = Bachelor_Cuts_Kaon
                           )

        ## Require the PV3D reconstruction before our cut on IP - what does this actually mean???
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )      

        return filterSeq
    
    
# Use this section to define the D->KS0h decays/combine particles

    def __D2KS0hCombine(self, name, inputSeq, decayDesc) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        combcuts = "(AM > %(DMesonComboLowMass)s*MeV) & (AM < %(DMesonComboHighMass)s*MeV)"  % self.getProps()

               # &(AMAXDOCA('LoKi::DistanceCalculator') < %(DMesonComboDOCA)s * mm)

        mothercuts = "(PT > %(DMesonMotherPT)s*MeV) & (VFASPF(VCHI2/VDOF) < %(DMesonMotherVertexChi2)s) & (MM > %(DMesonMotherLowMass)s*MeV) & (MM < %(DMesonMotherHighMass)s*MeV) & (MIPCHI2DV(PRIMARY) < %(DMesonMotherMIPChi2)s)"  % self.getProps()

        # Daughter cuts defined in bachelor pion/kaon filter      
        
        combineCharmD2KS0h= Hlt2Member( CombineParticles
                          , "Combine_Stage"
                          , DecayDescriptors = decayDesc
                          , InputLocations = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineCharmD2KS0h])

# Use this section to put everything together
 
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions, BiKalmanFittedKaons
        from Hlt2SharedParticles.Ks import KsLL as KsData 
        from HltTracking.HltPVs import PV3D

        ## Start applying the member functions we have been defining above

        # Stage 1
        # Filter the KS0s and the bachelor pions and kaons
       
        # Filter the StdLooseLL KS0s
        KS0LLForD2KS0h = self.__KS0LLFilter('CharmInputKS0sForD2KS0h', [ KsData ] )

        # Filter the bachelor pions and kaons 

        pionsBachelorForD2KS0h = self.__BachelorPionFilter('CharmInputBachelorPionsD2KS0h', [ BiKalmanFittedPions] )
        kaonsBachelorForD2KS0h = self.__BachelorKaonFilter('CharmInputBachelorKaonsD2KS0h', [ BiKalmanFittedKaons] )
       
        # Stage 2
        # Stage 2a - bachelor hadron = pion

        # Build the D(s)

        # Make D2KS0Pi
        CharmD2KS0hCombinePion = self.__D2KS0hCombine(  name = 'CharmHadD2KS0Pi'  
                                                  , inputSeq = [ KS0LLForD2KS0h, pionsBachelorForD2KS0h]           
                                                  , decayDesc =  ['[D+ -> KS0 pi+]cc'] 
                                                 )   
        # D2KS0Pi line
        
	Hlt2CharmD2KS0Pi = bindMembers('CharmHadD2KS0Pi', [CharmD2KS0hCombinePion] )

        # Stage 2b - bachelor hadron = kaon

        # Build the D(s)

        # Make D2KS0K
        CharmD2KS0hCombineKaon = self.__D2KS0hCombine(  name = 'CharmHadD2KS0K'  
                                                  , inputSeq = [ KS0LLForD2KS0h, kaonsBachelorForD2KS0h]           
                                                  , decayDesc =  ['[D+ -> KS0 K+]cc'] 
                                                 )   
        # D2KS0K line
        
	Hlt2CharmD2KS0K = bindMembers('CharmHadD2KS0K', [CharmD2KS0hCombineKaon] )


        ###########################################################################
        # Define the Hlt2 Lines for D(s)->KS0h - seperate pion and kaon lines
        ##########################################################################

        ### D(s)->KS0Pi line

        line = Hlt2Line('CharmHadD2KS0Pi', prescale = self.prescale
                        , algos = [ PV3D(), KS0LLForD2KS0h, pionsBachelorForD2KS0h, Hlt2CharmD2KS0Pi]
                        # All the necessary algorithms
			, postscale = self.postscale
                        )
        decName = "Hlt2CharmHadD2KS0PiDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
	
        ### D(s)->KS0K line

        line2 = Hlt2Line('CharmHadD2KS0K', prescale = self.prescale
                        , algos = [ PV3D(), KS0LLForD2KS0h, kaonsBachelorForD2KS0h, Hlt2CharmD2KS0K]
                        # All the necessary algorithms
			, postscale = self.postscale
                        )
        decName2 = "Hlt2CharmHadD2KS0KDecision"
        annSvcID2 = self._scale(decName2,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName2 : annSvcID2 } )
