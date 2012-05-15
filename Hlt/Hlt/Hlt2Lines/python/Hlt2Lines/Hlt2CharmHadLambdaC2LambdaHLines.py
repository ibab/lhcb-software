__author__  = [ 'Sam Gregson' ]


from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, mm

## Create a python class that inherits from HltLinesConfigurableUser
class Hlt2CharmHadLambdaCLambdaHLinesConf(HltLinesConfigurableUser) :

   # define some values so that they are not hard-coded
    __slots__ = {
                  ## Lambda0 daughter pion cuts - Lambda0LL
                    'Lambda0LLDaugPionTrackChi2' : 3.0
                  , 'Lambda0LLDaugPionMIPChi2'   : 36.0

                  ## Lambda0 daughter pion cuts - Lambda0DD
                  , 'Lambda0DDDaugPionTrackChi2' : 4.0
                  , 'Lambda0DDDaugPionMIPChi2'   : 10.0

                  ## Lambda0 daughter proton cuts - Lambda0LL
                  , 'Lambda0LLDaugProtonTrackChi2' : 3.0
                  , 'Lambda0LLDaugProtonMIPChi2'   : 36.0

                  ## Lambda0 daughter proton cuts - Lambda0DD
                  , 'Lambda0DDDaugProtonTrackChi2' : 4.0
                  , 'Lambda0DDDaugProtonMIPChi2'   : 10.0
                                        
                  ## Lambda0 mother cuts - Lambda0LL
                  , 'Lambda0LLVertexChi2'    : 20.0
                  , 'Lambda0LLPT'            : 500 * MeV
                  , 'Lambda0LLMIPChi2'       : 4
                  , 'Lambda0LLZDiff'       : 10 * mm

                  ## Lambda0 mother cuts - Lambda0DD
                  , 'Lambda0DDVertexChi2'    : 20.0
                  , 'Lambda0DDPT'            : 500 * MeV
                  , 'Lambda0DDMIPChi2'       : 0
                  , 'Lambda0DDZDiff'       : 10 * mm  

                  ## Bachelor pion cuts - Lambda0LL
                  , 'Lambda0LLBachPionP'         : 2500.0 * MeV
                  , 'Lambda0LLBachPionPT'        : 350.0 * MeV
                  , 'Lambda0LLBachPionTrackChi2' : 3.0
                  , 'Lambda0LLBachPionMIPChi2'   : 9.0

                  ## Bachelor pion cuts - Lambda0DD
                  , 'Lambda0DDBachPionP'         : 2500.0 * MeV
                  , 'Lambda0DDBachPionPT'        : 350.0 * MeV
                  , 'Lambda0DDBachPionTrackChi2' : 3.0
                  , 'Lambda0DDBachPionMIPChi2'   : 9.0

                  ## Bachelor kaon cuts - Lambda0LL
                  , 'Lambda0LLBachKaonP'         : 2500.0 * MeV
                  , 'Lambda0LLBachKaonPT'        : 350. * MeV
                  , 'Lambda0LLBachKaonTrackChi2' : 3.0
                  , 'Lambda0LLBachKaonMIPChi2'   : 9.0

                  ## Bachelor kaon cuts - Lambda0DD
                  , 'Lambda0DDBachKaonP'         : 2500.0 * MeV
                  , 'Lambda0DDBachKaonPT'        : 350. * MeV
                  , 'Lambda0DDBachKaonTrackChi2' : 3.0
                  , 'Lambda0DDBachKaonMIPChi2'   : 9.0

                  ## Lambda_c cuts
                  ## Combo cuts
                  , 'LambdaCComboLowMass'   : 2175.0 * MeV
                  , 'LambdaCComboHighMass'  : 2395.0 * MeV
                  ## Mother cuts
                  , 'LambdaCMotherLowMass'    : 2185.0 * MeV
                  , 'LambdaCMotherHighMass'   : 2385.0 * MeV
                  , 'LambdaCMotherVertexChi2' : 15.0
                  , 'LambdaCMotherMIPChi2'    : 15.0
                  , 'LambdaCMotherPT'         : 1500.0 * MeV
                 
                  , 'TisTosParticleTaggerSpecs': { "Hlt1Track.*Decision%TOS":0 }
                  , 'name_prefix'              : 'LambdaC'
                  ## prescales
                  , 'Prescale'                  : {
                         'Hlt2LambdaC_LambdaC2Lambda0LLPi'    : 1.0
                       , 'Hlt2LambdaC_LambdaC2Lambda0LLK'     : 1.0
                       , 'Hlt2LambdaC_LambdaC2Lambda0DDPi'    : 1.0  
                       , 'Hlt2LambdaC_LambdaC2Lambda0DDK'     : 1.0
                        
                        }
                  , 'HltANNSvcID'  : {
                       'Hlt2LambdaC_LambdaC2Lambda0LLPiDecision'  : 51913
                      ,'Hlt2LambdaC_LambdaC2Lambda0LLKDecision'   : 51914
                      ,'Hlt2LambdaC_LambdaC2Lambda0DDPiDecision'  : 51915  
                      ,'Hlt2LambdaC_LambdaC2Lambda0DDKDecision'   : 51916
                       
                        }
                }


    def __Lambda0LLFilter(self, name, inputContainers) : 
        """Use this section to create a Lambda0 LL filter
        """
        from HltLine.HltLine import Hlt2Member, bindMembers           
        from Configurables import FilterDesktop, CombineParticles      
        from HltTracking.HltPVs import PV3D

        incuts = "INTREE(('pi+'==ABSID) & (TRCHI2DOF < %(Lambda0LLDaugPionTrackChi2)s))" \
                 "& INTREE(('p+'==ABSID) & (TRCHI2DOF < %(Lambda0LLDaugProtonTrackChi2)s))" \
                 "& (VFASPF(VCHI2PDOF) < %(Lambda0LLVertexChi2)s)" \
                 "& (PT > %(Lambda0LLPT)s)" \
                 "& INTREE(('pi+'==ABSID) & (MIPCHI2DV(PRIMARY) > %(Lambda0LLDaugPionMIPChi2)s))" \
                 "& INTREE(('p+'==ABSID) & (MIPCHI2DV(PRIMARY) > %(Lambda0LLDaugProtonMIPChi2)s))" \
                 "& (MIPCHI2DV(PRIMARY) > %(Lambda0LLMIPChi2)s)" % self.getProps()


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
    ##--Lambda0DD filter---------------------------------------------------------------------- 
    ##
    ## Similar to Lambda0 LL filter but cuts can be specifically tailored
    
    def __Lambda0DDFilter(self, name, inputContainers) : 
        """Use this section to create a Lambda0 DD filter - seeding now possible :)
        """
        from HltLine.HltLine import Hlt2Member, bindMembers           
        from Configurables import FilterDesktop, CombineParticles      
        from HltTracking.HltPVs import PV3D

  
        incuts = "INTREE(('pi+'==ABSID) & (TRCHI2DOF < %(Lambda0DDDaugPionTrackChi2)s))" \
                 "& INTREE(('p+'==ABSID) &  (TRCHI2DOF < %(Lambda0DDDaugProtonTrackChi2)s))" \
                 "& (VFASPF(VCHI2PDOF) < %(Lambda0DDVertexChi2)s)" \
                 "& (PT > %(Lambda0DDPT)s)" \
                 "& INTREE(('pi+'==ABSID) & (MIPCHI2DV(PRIMARY) > %(Lambda0DDDaugPionMIPChi2)s))" \
                 "& INTREE(('p+'==ABSID) &  (MIPCHI2DV(PRIMARY) > %(Lambda0DDDaugProtonMIPChi2)s))" \
                 "& (MIPCHI2DV(PRIMARY) > %(Lambda0DDMIPChi2)s)" % self.getProps()


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

    def __Lambda0LLBachelorPionFilter(self, name, inputContainers) : 
        """Use this section to create a bachelor filter
        """
        from HltLine.HltLine import Hlt2Member, bindMembers           
        from Configurables import FilterDesktop, CombineParticles      
        from HltTracking.HltPVs import PV3D


        Bachelor_Cuts_Pion = "(TRCHI2DOF < %(Lambda0LLBachPionTrackChi2)s)" \
                             "& (PT > %(Lambda0LLBachPionPT)s)" \
                             "& (P > %(Lambda0LLBachPionP)s)" \
                             "& (MIPCHI2DV(PRIMARY) > %(Lambda0LLBachPionMIPChi2)s)"  % self.getProps()


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

    def __Lambda0DDBachelorPionFilter(self, name, inputContainers) : 
        """Use this section to create a bachelor filter (Lambda0 DD lines)
        """
        from HltLine.HltLine import Hlt2Member, bindMembers           
        from Configurables import FilterDesktop, CombineParticles      
        from HltTracking.HltPVs import PV3D

        Bachelor_Cuts_Pion = "(TRCHI2DOF < %(Lambda0DDBachPionTrackChi2)s)" \
                             "& (PT > %(Lambda0DDBachPionPT)s)" \
                             "& (P > %(Lambda0DDBachPionP)s)" \
                             "& (MIPCHI2DV(PRIMARY) > %(Lambda0DDBachPionMIPChi2)s)"  % self.getProps()

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


    def __Lambda0LLBachelorKaonFilter(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers           
        from Configurables import FilterDesktop, CombineParticles      
        from HltTracking.HltPVs import PV3D                           

        Bachelor_Cuts_Kaon = "(TRCHI2DOF < %(Lambda0LLBachKaonTrackChi2)s)" \
                             "& (PT > %(Lambda0LLBachKaonPT)s)" \
                             "& (P > %(Lambda0LLBachKaonP)s)" \
                             "& (MIPCHI2DV(PRIMARY) > %(Lambda0LLBachKaonMIPChi2)s)"  % self.getProps()


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

    def __Lambda0DDBachelorKaonFilter(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers           
        from Configurables import FilterDesktop, CombineParticles      
        from HltTracking.HltPVs import PV3D

        Bachelor_Cuts_Kaon = "(TRCHI2DOF < %(Lambda0DDBachKaonTrackChi2)s)" \
                             "& (PT > %(Lambda0DDBachKaonPT)s)" \
                             "& (P > %(Lambda0DDBachKaonP)s)" \
                             "& (MIPCHI2DV(PRIMARY) > %(Lambda0DDBachKaonMIPChi2)s)"  % self.getProps()

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
    

    def __LambdaCLLCombine(self, name, inputSeq, decayDesc) :
        """Use this section to define the Lambda_C LL decay/combine particles
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        combcuts = "in_range(%(LambdaCComboLowMass)s, AM, %(LambdaCComboHighMass)s)"  % self.getProps()


        mothercuts = "in_range(%(LambdaCMotherLowMass)s, MM, %(LambdaCMotherHighMass)s)" \
                     "& (VFASPF(VCHI2PDOF) < %(LambdaCMotherVertexChi2)s)" \
                     "& (PT > %(LambdaCMotherPT)s)" \
                     "& (MIPCHI2DV(PRIMARY) < %(LambdaCMotherMIPChi2)s)" \
                     "& ((CHILD( VFASPF(VZ) , 'Lambda0' == ABSID ) - VFASPF(VZ)) > %(Lambda0LLZDiff)s)" % self.getProps()

        # Daughter cuts defined in bachelor pion/kaon filter      
        
        combineLambdaCLL= Hlt2Member( CombineParticles
                          , "Combine_Stage"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineLambdaCLL])


##
##----------------------------------------------------------------------------------------
##
    

    def __LambdaCDDCombine(self, name, inputSeq, decayDesc) :
        """Use this section to define the Lambda_C DD decay/combine particles
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        combcuts = "in_range(%(LambdaCComboLowMass)s, AM, %(LambdaCComboHighMass)s)"  % self.getProps()


        mothercuts = "in_range(%(LambdaCMotherLowMass)s, MM, %(LambdaCMotherHighMass)s)" \
                     "& (VFASPF(VCHI2PDOF) < %(LambdaCMotherVertexChi2)s)" \
                     "& (PT > %(LambdaCMotherPT)s)" \
                     "& (MIPCHI2DV(PRIMARY) < %(LambdaCMotherMIPChi2)s)" \
                     "& ((CHILD( VFASPF(VZ) , 'Lambda0' == ABSID ) - VFASPF(VZ)) > %(Lambda0DDZDiff)s)" % self.getProps()

        # Daughter cuts defined in bachelor pion/kaon filter      
        
        combineLambdaCDD= Hlt2Member( CombineParticles
                          , "Combine_Stage"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineLambdaCDD])

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
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions, BiKalmanFittedKaons # For the bachelor particles
        from Hlt2SharedParticles.Lambda import LambdaLLTrackFitted as Lambda0LLData 
        from Hlt2SharedParticles.Lambda import LambdaDDTrackFitted as Lambda0DDData 

        from HltTracking.HltPVs import PV3D

        ## Start applying the member functions we have been defining above

        #-------------------------------------------------------------------------------------------------
        # Stage 1
        # Filter the Lambda0s 
       
        # Filter the LL Lambdas
        Lambda0LLName = self.getProp('name_prefix') + '_Lambda0LL'
        Lambda0LLForLambdaC2Lambda0LLh = self.__Lambda0LLFilter(Lambda0LLName, [ Lambda0LLData ] ) # uses the Lambda0 LL data, filters and names

        # Filter the DD Lambdas
        Lambda0DDName = self.getProp('name_prefix') + '_Lambda0DD'
        Lambda0DDForLambdaC2Lambda0DDh = self.__Lambda0DDFilter(Lambda0DDName, [ Lambda0DDData ] ) # uses the Lambda0 DD data, filters and names

        #--------------------------------------------------------------------------------------------------

        # Filter the bachelor pions and kaons (Lambda0 LL) 
        Lambda0LLpionName = self.getProp('name_prefix') + 'BachelorPionsLambda0LL'
        Lambda0LLkaonName = self.getProp('name_prefix') + 'BachelorKaonsLambda0LL'

        pionsBachelorForLambdaC2Lambda0LLh = self.__Lambda0LLBachelorPionFilter(Lambda0LLpionName, [ BiKalmanFittedPions] )
        kaonsBachelorForLambdaC2Lambda0LLh = self.__Lambda0LLBachelorKaonFilter(Lambda0LLkaonName, [ BiKalmanFittedKaons] )

        # Filter the bachelor pions and kaons (Lambda0 DD) 
        Lambda0DDpionName = self.getProp('name_prefix') + 'BachelorPionsLambda0DD'
        Lambda0DDkaonName = self.getProp('name_prefix') + 'BachelorKaonsLambda0DD'

        pionsBachelorForLambdaC2Lambda0DDh = self.__Lambda0DDBachelorPionFilter(Lambda0DDpionName, [ BiKalmanFittedPions] )
        kaonsBachelorForLambdaC2Lambda0DDh = self.__Lambda0DDBachelorKaonFilter(Lambda0DDkaonName, [ BiKalmanFittedKaons] )

        #--------------------------------------------------------------------------------------------------
       
        # Stage 2
        # Stage 2a - bachelor hadron = pion
        # Stage 2ai - Lambda0 = Lambda0LL
        
        # Build the Lambda_c

        # Make Lambda_c
        Lambda0LLPiName = self.getProp('name_prefix') + '_LambdaC2Lambda0LLPi'
        hlt2CharmLambdaC2Lambda0LLPiComb = self.__LambdaCLLCombine(  name = Lambda0LLPiName 
                                                  , inputSeq = [ Lambda0LLForLambdaC2Lambda0LLh, pionsBachelorForLambdaC2Lambda0LLh]           
                                                  , decayDesc =  ['[Lambda_c+ -> Lambda0 pi+]cc'] 
                                                 )   

        Hlt2CharmLambdaC2Lambda0LLPi = self.__filterHlt1TOS(Lambda0LLPiName, hlt2CharmLambdaC2Lambda0LLPiComb)

        #--------------------------------------------------------------------------------------------------

        # Stage 2aii - Lambda0 = Lambda0DD
        
        # Build the Lambda_c

        # Make Lambda_c
        Lambda0DDPiName = self.getProp('name_prefix') + '_LambdaC2Lambda0DDPi'
        hlt2CharmLambdaC2Lambda0DDPiComb = self.__LambdaCDDCombine(  name = Lambda0DDPiName 
                                                  , inputSeq = [ Lambda0DDForLambdaC2Lambda0DDh, pionsBachelorForLambdaC2Lambda0DDh]           
                                                  , decayDesc =  ['[Lambda_c+ -> Lambda0 pi+]cc'] 
                                                 )   

        Hlt2CharmLambdaC2Lambda0DDPi = self.__filterHlt1TOS(Lambda0DDPiName, hlt2CharmLambdaC2Lambda0DDPiComb)

        #--------------------------------------------------------------------------------------------------

        # Stage 2b - bachelor hadron = kaon
        # Stage 2bi - Lambda0 = Lambda0LL

        # Build the Lambda_c 

        # Make Lambda_c
        Lambda0LLKName  = self.getProp('name_prefix') + '_LambdaC2Lambda0LLK'
        hlt2CharmLambdaC2Lambda0LLKComb = self.__LambdaCLLCombine(  name = Lambda0LLKName
                                                  , inputSeq = [ Lambda0LLForLambdaC2Lambda0LLh, kaonsBachelorForLambdaC2Lambda0LLh]           
                                                  , decayDesc =  ['[Lambda_c+ -> Lambda0 K+]cc'] 
                                                 )   

        Hlt2CharmLambdaC2Lambda0LLK = self.__filterHlt1TOS(Lambda0LLKName, hlt2CharmLambdaC2Lambda0LLKComb)

        #---------------------------------------------------------------------------------------------------
    
        # Stage 2bii - Lambda0 = Lambda0DD

        # Build the Lambda_c 

        # Make Lambda_c
        Lambda0DDKName  = self.getProp('name_prefix') + '_LambdaC2Lambda0DDK'
        hlt2CharmLambdaC2Lambda0DDKComb = self.__LambdaCDDCombine(  name = Lambda0DDKName
                                                  , inputSeq = [ Lambda0DDForLambdaC2Lambda0DDh, kaonsBachelorForLambdaC2Lambda0DDh]           
                                                  , decayDesc =  ['[Lambda_c+ -> Lambda0 K+]cc'] 
                                                 )   

        Hlt2CharmLambdaC2Lambda0DDK = self.__filterHlt1TOS(Lambda0DDKName, hlt2CharmLambdaC2Lambda0DDKComb)



        ######################################################################################################################
        # Define the Hlt2 Lines for Lambda_c -> Lambda0 h - seperate pion and kaon lines, seperate Lambda0 LL//DD lines
        ######################################################################################################################

        ### Lambda0 LL Lines

        ### Lambda_c -> Lambda0 Pi line

        line = Hlt2Line(Lambda0LLPiName, prescale = self.prescale
                        , algos = [ PV3D(), Lambda0LLForLambdaC2Lambda0LLh, pionsBachelorForLambdaC2Lambda0LLh, Hlt2CharmLambdaC2Lambda0LLPi]
                        # All the necessary algorithms
                        , postscale = self.postscale 
                        )
        decName = 'Hlt2' + Lambda0LLPiName + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        #---------------------------------------------------------------------------------------------
        
        ### Lambda_c -> Lambda0 K line

        line2 = Hlt2Line( Lambda0LLKName, prescale = self.prescale
                        , algos = [ PV3D(), Lambda0LLForLambdaC2Lambda0LLh, kaonsBachelorForLambdaC2Lambda0LLh, Hlt2CharmLambdaC2Lambda0LLK]
                        # All the necessary algorithms
                        , postscale = self.postscale
                        )
        decName2 = 'Hlt2' + Lambda0LLKName + 'Decision'
        annSvcID2 = self._scale(decName2,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName2 : annSvcID2 } )

        #---------------------------------------------------------------------------------------------

        ### Lambda0 DD lines

        ### Lambda_c -> Lambda0 Pi line

        line3 = Hlt2Line(Lambda0DDPiName, prescale = self.prescale
                        , algos = [ PV3D(), Lambda0DDForLambdaC2Lambda0DDh, pionsBachelorForLambdaC2Lambda0DDh, Hlt2CharmLambdaC2Lambda0DDPi]
                        # All the necessary algorithms
                        , postscale = self.postscale
                        )
        decName3 = 'Hlt2' + Lambda0DDPiName + 'Decision'
        annSvcID3 = self._scale(decName3,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName3 : annSvcID3 } )
        
        ### Lambda_c -> Lambda0 K line------------------------------------------------------------------------------------

        line4 = Hlt2Line(Lambda0DDKName, prescale = self.prescale
                        , algos = [ PV3D(), Lambda0DDForLambdaC2Lambda0DDh, kaonsBachelorForLambdaC2Lambda0DDh, Hlt2CharmLambdaC2Lambda0DDK]
                        # All the necessary algorithms
                        , postscale = self.postscale
                        )
        decName4 = 'Hlt2' + Lambda0DDKName + 'Decision'
        annSvcID4 = self._scale(decName4,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName4 : annSvcID4 } )

