__author__  = [ 'Sam Gregson' ]


from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, mm

## Create a python class that inherits from HltLinesConfigurableUser
class Hlt2CharmHadD2KS0HLinesConf(HltLinesConfigurableUser) :

   # define some values so that they are not hard-coded
    __slots__ = {
                  ## KS0 daughter pion cuts - KS0LL
                    'KS0DaugTrackChi2' : 4.0
                  , 'KS0DaugMIPChi2'   : 36.0

                  ## KS0 daughter pion cuts - KS0DD
                  , 'KS0DDDaugTrackChi2' : 4.0
                  , 'KS0DDDaugMIPChi2'   : 36.0
                                        
                  ## KS0 mother cuts - KS0LL
                  , 'KS0VertexChi2'    : 10.0
                  , 'KS0PT'            : 0.0 * MeV
                  , 'KS0MIPChi2'       : 0
                  , 'ZDiff'       : 0 * mm
                  , 'KS0BPVVDCHI2'       : 300.0  # new added

                  ## KS0 mother cuts - KS0DD
                  , 'KS0DDVertexChi2'    : 10.0
                  , 'KS0DDPT'            : 1000 * MeV
                  , 'KS0DDMIPChi2'       : 0
                  , 'DDZDiff'       : 0 * mm  
                  , 'KS0DDBPVVDCHI2'       : 200.0  # new added

                  ## Bachelor pion cuts - KS0LL
                  , 'BachPionP'         : 2000.0 * MeV
                  , 'BachPionPT'        : 200.0 * MeV
                  , 'BachPionTrackChi2' : 4.0
                  , 'BachPionMIPChi2'   : 15.0

                  ## Bachelor pion cuts - KS0DD
                  , 'BachPionPDD'         : 2000.0 * MeV
                  , 'BachPionPTDD'        : 200.0 * MeV
                  , 'BachPionTrackChi2DD' : 4.0
                  , 'BachPionMIPChi2DD'   : 15.0

                  ## Bachelor kaon cuts - KS0LL
                  , 'BachKaonP'         : 2000.0 * MeV
                  , 'BachKaonPT'        : 200. * MeV
                  , 'BachKaonTrackChi2' : 4.0
                  , 'BachKaonMIPChi2'   : 15.0

                  ## Bachelor kaon cuts - KS0DD
                  , 'BachKaonPDD'         : 2000.0 * MeV
                  , 'BachKaonPTDD'        : 200. * MeV
                  , 'BachKaonTrackChi2DD' : 4.0
                  , 'BachKaonMIPChi2DD'   : 15.0

                  ## D meson cuts
                  ## Combo cuts
                  , 'DMesonComboLowMass'   : 1760.0 * MeV
                  , 'DMesonComboHighMass'  : 2080.0 * MeV
                  ## Mother cuts
                  , 'DMesonMotherLowMass'    : 1770.0 * MeV
                  , 'DMesonMotherHighMass'   : 2070.0 * MeV
                  , 'DMesonMotherVertexChi2' : 10.0
                  , 'DMesonMotherMIPChi2'    : 15.0
                  , 'DMesonMotherPT'         : 0.0 * MeV
                 
                  , 'TisTosParticleTaggerSpecs': { "Hlt1Track.*Decision%TOS":0 }
                  , 'name_prefix'              : 'CharmHadD2KS0H'
                  ## prescales
                  , 'Prescale'                  : {
                         'Hlt2CharmHadD2KS0H_D2KS0Pi'    : 1.0
                       , 'Hlt2CharmHadD2KS0H_D2KS0K'     : 1.0
                       , 'Hlt2CharmHadD2KS0DDH_D2KS0Pi'    : 1.0  # New KS0DD prescales
                       , 'Hlt2CharmHadD2KS0DDH_D2KS0K'     : 1.0
                        
                        }
                  , 'HltANNSvcID'  : {
                       'Hlt2CharmHadD2KS0H_D2KS0PiDecision'  : 50913
                       ,'Hlt2CharmHadD2KS0H_D2KS0KDecision'  : 50914
                       ,'Hlt2CharmHadD2KS0H_D2KS0DDPiDecision' : 50915  
                       ,'Hlt2CharmHadD2KS0H_D2KS0DDKDecision'  : 50916
                       
                        }
                }


    def __KS0LLFilter(self, name, inputContainers) : 
        """Use this section to create a KS0 LL filter
           - DD later 
        """
        from HltLine.HltLine import Hlt2Member, bindMembers           
        from Configurables import FilterDesktop, CombineParticles      
        from HltTracking.HltPVs import PV3D

        incuts = "CHILDCUT((TRCHI2DOF < %(KS0DaugTrackChi2)s),1)" \
                 "& CHILDCUT((TRCHI2DOF < %(KS0DaugTrackChi2)s),2)" \
                 "& (VFASPF(VCHI2PDOF) < %(KS0VertexChi2)s)" \
                 "& (BPVVDCHI2 > %(KS0BPVVDCHI2)s)" \
                 "& CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s),1)" \
                 "& CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DaugMIPChi2)s),2)" % self.getProps()
               
#"& (PT > %(KS0PT)s)" \
#"& (MIPCHI2DV(PRIMARY) > %(KS0MIPChi2)s)" % self.getProps()


        # Define the HLT2 member with it's necessary inputs
        filter = Hlt2Member( FilterDesktop        
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP 
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )      
        return filterSeq

    ##
    ##--KS0DD filter---------------------------------------------------------------------- 
    ##
    ## Similar to KS0LL filter but cuts can be specifically tailored
    
    def __KS0DDFilter(self, name, inputContainers) : 
        """Use this section to create a KS0 DD filter - seeding now possible :)
        """
        from HltLine.HltLine import Hlt2Member, bindMembers           
        from Configurables import FilterDesktop, CombineParticles      
        from HltTracking.HltPVs import PV3D

  
        incuts = "CHILDCUT((TRCHI2DOF < %(KS0DDDaugTrackChi2)s),1)" \
                 "& CHILDCUT((TRCHI2DOF < %(KS0DDDaugTrackChi2)s),2)" \
                 "& (VFASPF(VCHI2PDOF) < %(KS0DDVertexChi2)s)" \
                 "& (BPVVDCHI2 > %(KS0DDBPVVDCHI2)s)" \
                 "& (PT > %(KS0DDPT)s)" \
                 "& CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DDDaugMIPChi2)s),1)" \
                 "& CHILDCUT((MIPCHI2DV(PRIMARY) > %(KS0DDDaugMIPChi2)s),2)" % self.getProps()

                 #"& (MIPCHI2DV(PRIMARY) > %(KS0DDMIPChi2)s)" % self.getProps()


        # Define the HLT2 member with it's necessary inputs
        filter = Hlt2Member( FilterDesktop        
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP 
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )      
        return filterSeq

##
##----------------------------------------------------------------------------------------
##    

    def __BachelorPionFilter(self, name, inputContainers) : 
        """Use this section to create a bachelor filter
        """
        from HltLine.HltLine import Hlt2Member, bindMembers           
        from Configurables import FilterDesktop, CombineParticles      
        from HltTracking.HltPVs import PV3D                           

        Bachelor_Cuts_Pion = "(TRCHI2DOF < %(BachPionTrackChi2)s)" \
                             "& (PT > %(BachPionPT)s)" \
                             "& (P > %(BachPionP)s)" \
                             "& (MIPCHI2DV(PRIMARY) > %(BachPionMIPChi2)s)"  % self.getProps()


        # Define the HLT2 member with it's necessary inputs
        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = Bachelor_Cuts_Pion
                           )

        ## Require the PV3D reconstruction before our cut on IP
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )      
        return filterSeq

##
##----------------------------------------------------------------------------------------
##    

    def __BachelorPionFilterDD(self, name, inputContainers) : 
        """Use this section to create a bachelor filter (KS0 DD lines)
        """
        from HltLine.HltLine import Hlt2Member, bindMembers           
        from Configurables import FilterDesktop, CombineParticles      
        from HltTracking.HltPVs import PV3D

        Bachelor_Cuts_Pion = "(TRCHI2DOF < %(BachPionTrackChi2DD)s)" \
                             "& (PT > %(BachPionPTDD)s)" \
                             "& (P > %(BachPionPDD)s)" \
                             "& (MIPCHI2DV(PRIMARY) > %(BachPionMIPChi2DD)s)"  % self.getProps()

        # Define the HLT2 member with it's necessary inputs
        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = Bachelor_Cuts_Pion
                           )

        ## Require the PV3D reconstruction before our cut on IP
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )      
        return filterSeq

##
##----------------------------------------------------------------------------------------
##


    def __BachelorKaonFilter(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers           
        from Configurables import FilterDesktop, CombineParticles      
        from HltTracking.HltPVs import PV3D                           

        Bachelor_Cuts_Kaon = "(TRCHI2DOF < %(BachKaonTrackChi2)s)" \
                             "& (PT > %(BachKaonPT)s)" \
                             "& (P > %(BachKaonP)s)" \
                             "& (MIPCHI2DV(PRIMARY) > %(BachKaonMIPChi2)s)"  % self.getProps()


        # Define the HLT2 member with it's necessary inputs
        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = Bachelor_Cuts_Kaon
                           )

        ## Require the PV3D reconstruction before our cut on IP
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )      
        return filterSeq
    
##
##----------------------------------------------------------------------------------------
##


    def __BachelorKaonFilterDD(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers           
        from Configurables import FilterDesktop, CombineParticles      
        from HltTracking.HltPVs import PV3D

        Bachelor_Cuts_Kaon = "(TRCHI2DOF < %(BachKaonTrackChi2DD)s)" \
                             "& (PT > %(BachKaonPTDD)s)" \
                             "& (P > %(BachKaonPDD)s)" \
                             "& (MIPCHI2DV(PRIMARY) > %(BachKaonMIPChi2DD)s)"  % self.getProps()

        # Define the HLT2 member with it's necessary inputs
        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = Bachelor_Cuts_Kaon
                           )

        ## Require the PV3D reconstruction before our cut on IP
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )      
        return filterSeq

##
##----------------------------------------------------------------------------------------
##
    

    def __D2KS0hCombine(self, name, inputSeq, decayDesc) :
        """Use this section to define the D->KS0h decays/combine particles
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        combcuts = "in_range(%(DMesonComboLowMass)s, AM, %(DMesonComboHighMass)s)"  % self.getProps()


        mothercuts = "in_range(%(DMesonMotherLowMass)s, MM, %(DMesonMotherHighMass)s)" \
                     "& (VFASPF(VCHI2PDOF) < %(DMesonMotherVertexChi2)s)" \
                     "& (MIPCHI2DV(PRIMARY) < %(DMesonMotherMIPChi2)s)" % self.getProps()
                     
 #"& (PT > %(DMesonMotherPT)s)" \
 #"& ((CHILD( VFASPF(VZ) , 'KS0' == ID ) - VFASPF(VZ)) > %(ZDiff)s)" % self.getProps(

        # Daughter cuts defined in bachelor pion/kaon filter      
        
        combineCharmD2KS0h= Hlt2Member( CombineParticles
                          , "Combine_Stage"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineCharmD2KS0h])


##
##----------------------------------------------------------------------------------------
##
    

    def __D2KS0DDhCombine(self, name, inputSeq, decayDesc) :
        """Use this section to define the D->KS0h decays/combine particles (KS0 DD) - same cuts as in LL at the moment
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        combcuts = "in_range(%(DMesonComboLowMass)s, AM, %(DMesonComboHighMass)s)"  % self.getProps()


        mothercuts = "in_range(%(DMesonMotherLowMass)s, MM, %(DMesonMotherHighMass)s)" \
                     "& (VFASPF(VCHI2PDOF) < %(DMesonMotherVertexChi2)s)" \
                     "& (MIPCHI2DV(PRIMARY) < %(DMesonMotherMIPChi2)s)" % self.getProps()

#"& (PT > %(DMesonMotherPT)s)" \
#"& ((CHILD( VFASPF(VZ) , 'KS0' == ID ) - VFASPF(VZ)) > %(ZDiff)s)" % self.getProps()

        # Daughter cuts defined in bachelor pion/kaon filter      
        
        combineCharmD2KS0h= Hlt2Member( CombineParticles
                          , "Combine_Stage"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineCharmD2KS0h])

##
##----------------------------------------------------------------------------------------
##



    def __filterHlt1TOS(self, name, input) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import TisTosParticleTagger

        filterTOS = Hlt2Member( TisTosParticleTagger
                                , 'Hlt1TOSFilter'
                                , Inputs = [ input.outputSelection() ]
                                , TisTosSpecs = self.getProp('TisTosParticleTaggerSpecs')
                              )

        return bindMembers(name, [ input, filterTOS ])
    # }

##
##----------------------------------------------------------------------------------------
##


 
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions, BiKalmanFittedKaons
        from Hlt2SharedParticles.Ks import KsLLTF as KsData 
        from Hlt2SharedParticles.Ks import KsDD as KsDDData 
        from HltTracking.HltPVs import PV3D

        ## Start applying the member functions we have been defining above

        # Stage 1
        # Filter the KS0s and the bachelor pions and kaons
       
        # Filter the StdLooseLL KS0s
        ksName = self.getProp('name_prefix') + '_KS0LL'
        KS0LLForD2KS0h = self.__KS0LLFilter(ksName, [ KsData ] ) # uses the KS0 LL data, filters and names

        # Filter the StdLooseDD KS0s
        ksDDName = self.getProp('name_prefix') + '_KS0DD'
        KS0DDForD2KS0h = self.__KS0DDFilter(ksDDName, [ KsDDData ] ) # uses the KS0 DD data, filters and names

        # Filter the bachelor pions and kaons (KS0 LL) 
        pionName = self.getProp('name_prefix') + 'BachelorPions'
        kaonName = self.getProp('name_prefix') + 'BachelorKaons'

        pionsBachelorForD2KS0h = self.__BachelorPionFilter(pionName, [ BiKalmanFittedPions] )
        kaonsBachelorForD2KS0h = self.__BachelorKaonFilter(kaonName, [ BiKalmanFittedKaons] )

        # Filter the bachelor pions and kaons (KS0 DD) 
        pionNameDD = self.getProp('name_prefix') + 'BachelorPionsDD'
        kaonNameDD = self.getProp('name_prefix') + 'BachelorKaonsDD'

        pionsBachelorDDForD2KS0h = self.__BachelorPionFilterDD(pionNameDD, [ BiKalmanFittedPions] )
        kaonsBachelorDDForD2KS0h = self.__BachelorKaonFilterDD(kaonNameDD, [ BiKalmanFittedKaons] )
       
        # Stage 2
        # Stage 2a - bachelor hadron = pion
        # Stage 2ai - KS0 = KS0LL
        
        # Build the D(s)

        # Make D2KS0Pi
        kSPiName = self.getProp('name_prefix') + '_D2KS0Pi'
        hlt2CharmD2KS0PiComb = self.__D2KS0hCombine(  name = kSPiName 
                                                  , inputSeq = [ KS0LLForD2KS0h, pionsBachelorForD2KS0h]           
                                                  , decayDesc =  ['[D+ -> KS0 pi+]cc'] 
                                                 )   

        Hlt2CharmD2KS0Pi = self.__filterHlt1TOS(kSPiName, hlt2CharmD2KS0PiComb)

        # Stage 2aii - KS0 = KS0DD
        
        # Build the D(s)

        # Make D2KS0Pi
        kSDDPiName = self.getProp('name_prefix') + '_D2KS0DDPi'
        hlt2CharmD2KS0DDPiComb = self.__D2KS0DDhCombine(  name = kSDDPiName 
                                                  , inputSeq = [ KS0DDForD2KS0h, pionsBachelorDDForD2KS0h]           
                                                  , decayDesc =  ['[D+ -> KS0 pi+]cc'] 
                                                 )   

        Hlt2CharmD2KS0DDPi = self.__filterHlt1TOS(kSDDPiName, hlt2CharmD2KS0DDPiComb)

        # Stage 2b - bachelor hadron = kaon
        # Stage 2bi - KS0 = KS0LL

        # Build the D(s)

        # Make D2KS0K
        kSKName  = self.getProp('name_prefix') + '_D2KS0K'
        hlt2CharmD2KS0KComb = self.__D2KS0hCombine(  name = kSKName
                                                  , inputSeq = [ KS0LLForD2KS0h, kaonsBachelorForD2KS0h]           
                                                  , decayDesc =  ['[D+ -> KS0 K+]cc'] 
                                                 )   

        Hlt2CharmD2KS0K = self.__filterHlt1TOS(kSKName, hlt2CharmD2KS0KComb)

        # Stage 2bii - KS0 = KS0DD

        # Build the D(s)

        # Make D2KS0K
        kSDDKName  = self.getProp('name_prefix') + '_D2KS0DDK'
        hlt2CharmD2KS0DDKComb = self.__D2KS0DDhCombine(  name = kSDDKName
                                                  , inputSeq = [ KS0DDForD2KS0h, kaonsBachelorDDForD2KS0h]           
                                                  , decayDesc =  ['[D+ -> KS0 K+]cc'] 
                                                 )   

        Hlt2CharmD2KS0DDK = self.__filterHlt1TOS(kSDDKName, hlt2CharmD2KS0DDKComb)



        ##################################################################################################
        # Define the Hlt2 Lines for D(s)->KS0h - seperate pion and kaon lines, seperate KS0 LL//DD lines
        ##################################################################################################

        ### KS0 LL Lines

        ### D(s)->KS0Pi line

        line = Hlt2Line(kSPiName, prescale = self.prescale
                        , algos = [ PV3D(), KS0LLForD2KS0h, pionsBachelorForD2KS0h, Hlt2CharmD2KS0Pi]
                        # All the necessary algorithms
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + kSPiName + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
        
        ### D(s)->KS0K line

        line2 = Hlt2Line(kSKName, prescale = self.prescale
                        , algos = [ PV3D(), KS0LLForD2KS0h, kaonsBachelorForD2KS0h, Hlt2CharmD2KS0K]
                        # All the necessary algorithms
                        , postscale = self.postscale
                        )
        decName2 = 'Hlt2' + kSKName + 'Decision'
        annSvcID2 = self._scale(decName2,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName2 : annSvcID2 } )

        ### KS0 DD lines

        ### D(s)->KS0Pi line

        line3 = Hlt2Line(kSDDPiName, prescale = self.prescale
                        , algos = [ PV3D(), KS0DDForD2KS0h, pionsBachelorDDForD2KS0h, Hlt2CharmD2KS0DDPi]
                        # All the necessary algorithms
                        , postscale = self.postscale
                        )
        decName3 = 'Hlt2' + kSDDPiName + 'Decision'
        annSvcID3 = self._scale(decName3,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName3 : annSvcID3 } )
        
        ### D(s)->KS0K line

        line4 = Hlt2Line(kSDDKName, prescale = self.prescale
                        , algos = [ PV3D(), KS0DDForD2KS0h, kaonsBachelorDDForD2KS0h, Hlt2CharmD2KS0DDK]
                        # All the necessary algorithms
                        , postscale = self.postscale
                        )
        decName4 = 'Hlt2' + kSDDKName + 'Decision'
        annSvcID4 = self._scale(decName4,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName4 : annSvcID4 } )

