__author__  = [ 'Mike Sokoloff' ]
__date__    = '$Date: 28 July 2012'
__version__ = '$Revision: 0.1'

##  trigger lines to select Omega --> Lambda,K and Xi --> Lambda,Pi decays

##  derived from Hlt2CharmHadLambdaC2LambdaHLines.py by Sam Gregson


from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, mm

## Create a python class that inherits from HltLinesConfigurableUser
class Hlt2ChargedHyperon2LambdaHLinesConf(HltLinesConfigurableUser) :

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
                  , 'Lambda0LLMIPChi2'       : 25
                  , 'Lambda0LLZDiff'         : 20 * mm

                  ## Lambda0 mother cuts - Lambda0DD
                  , 'Lambda0DDVertexChi2'    : 20.0
                  , 'Lambda0DDPT'            : 500 * MeV
                  , 'Lambda0DDMIPChi2'       : 0
                  , 'Lambda0DDZDiff'         : 400 * mm  

                  ## Bachelor pion cuts - Lambda0LL
                  , 'Lambda0LLBachPionP'         : 2500.0 * MeV
                  , 'Lambda0LLBachPionPT'        : 350.0 * MeV
                  , 'Lambda0LLBachPionTrackChi2' : 3.0
                  , 'Lambda0LLBachPionMIPChi2'   : 25.0

                  ## Bachelor pion cuts - Lambda0DD
                  , 'Lambda0DDBachPionP'         : 2500.0 * MeV
                  , 'Lambda0DDBachPionPT'        : 350.0 * MeV
                  , 'Lambda0DDBachPionTrackChi2' : 3.0
                  , 'Lambda0DDBachPionMIPChi2'   : 25.0

                  ## Bachelor kaon cuts - Lambda0LL
                  , 'Lambda0LLBachKaonP'         : 2500.0 * MeV
                  , 'Lambda0LLBachKaonPT'        : 350. * MeV
                  , 'Lambda0LLBachKaonTrackChi2' : 3.0
                  , 'Lambda0LLBachKaonMIPChi2'   : 25.0

                  ## Bachelor kaon cuts - Lambda0DD
                  , 'Lambda0DDBachKaonP'         : 2500.0 * MeV
                  , 'Lambda0DDBachKaonPT'        : 350. * MeV
                  , 'Lambda0DDBachKaonTrackChi2' : 3.0
                  , 'Lambda0DDBachKaonMIPChi2'   : 25.0

                  ## Omega cuts
                  ## Combo cuts
                  , 'OmegaComboLowMass'   : 1575.0 * MeV
                  , 'OmegaComboHighMass'  : 1775.0 * MeV
                  ## Mother cuts
                  , 'OmegaMotherLowMass'    : 1625.0 * MeV
                  , 'OmegaMotherHighMass'   : 1725.0 * MeV
                  , 'OmegaMotherVertexChi2' : 15.0
                  , 'OmegaMotherMIPChi2'    : 100.0
                  , 'OmegaMotherPT'         : 1500.0 * MeV
                  , 'OmegaMotherDecayTimeCut'     : 2.0          ##  meant to be ps

                  ## Xi cuts
                  ## Combo cuts
                  , 'XiComboLowMass'   : 1275.0 * MeV
                  , 'XiComboHighMass'  : 1375.0 * MeV
                  ## Mother cuts
                  , 'XiMotherLowMass'    : 1300.0 * MeV
                  , 'XiMotherHighMass'   : 1350.0 * MeV
                  , 'XiMotherVertexChi2' : 15.0
                  , 'XiMotherMIPChi2'    : 100.0
                  , 'XiMotherPT'         : 1500.0 * MeV
                  , 'XiMotherDecayTimeCut'     : 2.0            ##  meant to be ps
                 
                  , 'TisTosParticleTaggerSpecs': { "Hlt1Track.*Decision%TOS":0 }
                  , 'name_prefix'              : 'ChargedHyperon'
                  ## prescales
                  , 'Prescale'                  : {
                         'Hlt2ChargedHyperon_Omega2Lambda0LLK'    : 1.0
                       , 'Hlt2ChargedHyperon_Omega2Lambda0DDK'     : 1.0
                       , 'Hlt2ChargedHyperon_Xi2Lambda0LLPi'     : 1.0
                       , 'Hlt2ChargedHyperon_Xi2Lambda0DDPi'     : 1.0
                        
                        }
                  , 'HltANNSvcID'  : {
                       'Hlt2ChargedHyperon_Omega2Lambda0LLKDecision'  : 51923
                      ,'Hlt2ChargedHyperon_Omega2Lambda0DDKDecision'  : 51924
                      ,'Hlt2ChargedHyperon_Omega2Lambda0DDKDDecision' : 51925
                      ,'Hlt2ChargedHyperon_Xi2Lambda0LLPiDecision'    : 51926  
                      ,'Hlt2ChargedHyperon_Xi2Lambda0DDPiDecision'    : 51927
                      ,'Hlt2ChargedHyperon_Xi2Lambda0DDPiDDecision'   : 51928
                       
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
    

    def __OmegaLLCombine(self, name, inputSeq, decayDesc) :
        """Use this section to define the Omega LL decay/combine particles
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        combcuts = "in_range(%(OmegaComboLowMass)s, AM, %(OmegaComboHighMass)s)"  % self.getProps()


        mothercuts = "in_range(%(OmegaMotherLowMass)s, MM, %(OmegaMotherHighMass)s)" \
                     "& (VFASPF(VCHI2PDOF) < %(OmegaMotherVertexChi2)s)" \
                     "& (PT > %(OmegaMotherPT)s)" \
                     "& (MIPCHI2DV(PRIMARY) < %(OmegaMotherMIPChi2)s)" \
                     "& (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(OmegaMotherDecayTimeCut)s * ps )" \
                     "& ((CHILD( VFASPF(VZ) , 'Lambda0' == ABSID ) - VFASPF(VZ)) > %(Lambda0LLZDiff)s)" % self.getProps()

        # Daughter cuts defined in bachelor pion/kaon filter      
        
        combineOmegaLL= Hlt2Member( CombineParticles
                          , "Combine_Stage"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineOmegaLL])


##
##----------------------------------------------------------------------------------------
##
    

    def __OmegaDDCombine(self, name, inputSeq, decayDesc) :
        """Use this section to define the Lambda_C DD decay/combine particles
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        combcuts = "in_range(%(OmegaComboLowMass)s, AM, %(OmegaComboHighMass)s)"  % self.getProps()


        mothercuts = "in_range(%(OmegaMotherLowMass)s, MM, %(OmegaMotherHighMass)s)" \
                     "& (VFASPF(VCHI2PDOF) < %(OmegaMotherVertexChi2)s)" \
                     "& (PT > %(OmegaMotherPT)s)" \
                     "& (MIPCHI2DV(PRIMARY) < %(OmegaMotherMIPChi2)s)" \
                     "& (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(OmegaMotherDecayTimeCut)s * ps )" \
                     "& ((CHILD( VFASPF(VZ) , 'Lambda0' == ABSID ) - VFASPF(VZ)) > %(Lambda0DDZDiff)s)" % self.getProps()

        # Daughter cuts defined in bachelor pion/kaon filter      
        
        combineOmegaDD= Hlt2Member( CombineParticles
                          , "Combine_Stage"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineOmegaDD])

##
##----------------------------------------------------------------------------------------
##

##
##----------------------------------------------------------------------------------------
##
    

    def __XiLLCombine(self, name, inputSeq, decayDesc) :
        """Use this section to define the Xi LL decay/combine particles
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        combcuts = "in_range(%(XiComboLowMass)s, AM, %(XiComboHighMass)s)"  % self.getProps()


        mothercuts = "in_range(%(XiMotherLowMass)s, MM, %(XiMotherHighMass)s)" \
                     "& (VFASPF(VCHI2PDOF) < %(XiMotherVertexChi2)s)" \
                     "& (PT > %(XiMotherPT)s)" \
                     "& (MIPCHI2DV(PRIMARY) < %(XiMotherMIPChi2)s)" \
                     "& (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(XiMotherDecayTimeCut)s * ps )" \
                     "& ((CHILD( VFASPF(VZ) , 'Lambda0' == ABSID ) - VFASPF(VZ)) > %(Lambda0LLZDiff)s)" % self.getProps()

        # Daughter cuts defined in bachelor pion/kaon filter      
        
        combineXiLL= Hlt2Member( CombineParticles
                          , "Combine_Stage"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineXiLL])


##
##----------------------------------------------------------------------------------------
##
    

    def __XiDDCombine(self, name, inputSeq, decayDesc) :
        """Use this section to define the Lambda_C DD decay/combine particles
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        combcuts = "in_range(%(XiComboLowMass)s, AM, %(XiComboHighMass)s)"  % self.getProps()


        mothercuts = "in_range(%(XiMotherLowMass)s, MM, %(XiMotherHighMass)s)" \
                     "& (VFASPF(VCHI2PDOF) < %(XiMotherVertexChi2)s)" \
                     "& (PT > %(XiMotherPT)s)" \
                     "& (MIPCHI2DV(PRIMARY) < %(XiMotherMIPChi2)s)" \
                     "& (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(XiMotherDecayTimeCut)s * ps )" \
                     "& ((CHILD( VFASPF(VZ) , 'Lambda0' == ABSID ) - VFASPF(VZ)) > %(Lambda0DDZDiff)s)" % self.getProps()

        # Daughter cuts defined in bachelor pion/kaon filter      
        
        combineXiDD= Hlt2Member( CombineParticles
                          , "Combine_Stage"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineXiDD])

##
##  --
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

##  also import downstream tracks for bachelor candidates
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownPions  
##        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownKaons  

        from Hlt2SharedParticles.Lambda import LambdaLLTrackFitted as Lambda0LLData 
        from Hlt2SharedParticles.Lambda import LambdaDDTrackFitted as Lambda0DDData 

        from HltTracking.HltPVs import PV3D

        ## Start applying the member functions we have been defining above

        #-------------------------------------------------------------------------------------------------
        # Stage 1
        # Filter the Lambda0s 
       
        # Filter the LL Lambdas
        Lambda0LLName = self.getProp('name_prefix') + '_Lambda0LL'
        Lambda0LLForChargedHyperon2Lambda0LLh = self.__Lambda0LLFilter(Lambda0LLName, [ Lambda0LLData ] ) # uses the Lambda0 LL data, filters and names

        # Filter the DD Lambdas
        Lambda0DDName = self.getProp('name_prefix') + '_Lambda0DD'
        Lambda0DDForChargedHyperon2Lambda0DDh = self.__Lambda0DDFilter(Lambda0DDName, [ Lambda0DDData ] ) # uses the Lambda0 DD data, filters and names

        #--------------------------------------------------------------------------------------------------

        # Filter the bachelor pions and kaons (Lambda0 LL) 
        Lambda0LLpionName = self.getProp('name_prefix') + 'BachelorPionsLambda0LL'
        Lambda0LLkaonName = self.getProp('name_prefix') + 'BachelorKaonsLambda0LL'

        pionsBachelorForChargedHyperon2Lambda0LLh = self.__Lambda0LLBachelorPionFilter(Lambda0LLpionName, [ BiKalmanFittedPions] )
        kaonsBachelorForChargedHyperon2Lambda0LLh = self.__Lambda0LLBachelorKaonFilter(Lambda0LLkaonName, [ BiKalmanFittedKaons] )

        # Filter the bachelor pions and kaons (Lambda0 DD) 
        Lambda0DDpionName = self.getProp('name_prefix') + 'BachelorPionsLambda0DD'
        Lambda0DDkaonName = self.getProp('name_prefix') + 'BachelorKaonsLambda0DD'

        Lambda0DDpionDName = self.getProp('name_prefix') + 'DownstreamPionsLambda0DD'
        Lambda0DDkaonDName = self.getProp('name_prefix') + 'DownstreamKaonsLambda0DD'

        pionsBachelorForChargedHyperon2Lambda0DDh = self.__Lambda0DDBachelorPionFilter(Lambda0DDpionName, [ BiKalmanFittedPions] )
        kaonsBachelorForChargedHyperon2Lambda0DDh = self.__Lambda0DDBachelorKaonFilter(Lambda0DDkaonName, [ BiKalmanFittedKaons] )
        downstreamPionsForChargedHyperon2Lambda0DDh = self.__Lambda0DDBachelorPionFilter(Lambda0DDpionDName, [ BiKalmanFittedPions] )
        downstreamKaonsForChargedHyperon2Lambda0DDh = self.__Lambda0DDBachelorKaonFilter(Lambda0DDkaonDName, [ BiKalmanFittedKaons] )
        #--------------------------------------------------------------------------------------------------
       
        # Stage 2
        
        # Build with Lambda0LL

        # Make Xi candidates
        Lambda0LLPiName = self.getProp('name_prefix') + '_Xi2Lambda0LLPi'
        hlt2Xi2Lambda0LLPiComb = self.__XiLLCombine(  name = Lambda0LLPiName 
                                                  , inputSeq = [ Lambda0LLForChargedHyperon2Lambda0LLh, pionsBachelorForChargedHyperon2Lambda0LLh]           
                                                  , decayDesc =  ['[Xi- -> Lambda0 pi-]cc'] 
                                                 )   

        Hlt2Xi2Lambda0LLPi = self.__filterHlt1TOS(Lambda0LLPiName, hlt2Xi2Lambda0LLPiComb)

        #--------------------------------------------------------------------------------------------------

        
        # Build with Lambda0DD and long pion

        # Make Xi candidates
        Lambda0DDPiName = self.getProp('name_prefix') + '_Xi2Lambda0DDPi'
        hlt2Xi2Lambda0DDPiComb = self.__XiDDCombine(  name = Lambda0DDPiName 
                                                  , inputSeq = [ Lambda0DDForChargedHyperon2Lambda0DDh, pionsBachelorForChargedHyperon2Lambda0DDh]           
                                                  , decayDesc =  ['[Xi- -> Lambda0 pi-]cc'] 
                                                 )   

        Hlt2Xi2Lambda0DDPi = self.__filterHlt1TOS(Lambda0DDPiName, hlt2Xi2Lambda0DDPiComb)

        # Build with Lambda0DD and downstream pion

        # Make Xi candidates
        Lambda0DDPiDName = self.getProp('name_prefix') + '_Xi2Lambda0DDPiD'
        hlt2Xi2Lambda0DDPiDComb = self.__XiDDCombine(  name = Lambda0DDPiDName 
                                                  , inputSeq = [ Lambda0DDForChargedHyperon2Lambda0DDh, downstreamPionsForChargedHyperon2Lambda0DDh]           
                                                  , decayDesc =  ['[Xi- -> Lambda0 pi-]cc'] 
                                                 )   

        Hlt2Xi2Lambda0DDPiD = self.__filterHlt1TOS(Lambda0DDPiDName, hlt2Xi2Lambda0DDPiDComb)

        #--------------------------------------------------------------------------------------------------

        # Stage 2b - bachelor hadron = kaon
        # Stage 2bi - Lambda0 = Lambda0LL

        # Build with Lambda0LL 

        # Make Lambda_c
        Lambda0LLKName  = self.getProp('name_prefix') + '_Omega2Lambda0LLK'
        hlt2Omega2Lambda0LLKComb = self.__OmegaLLCombine(  name = Lambda0LLKName
                                                  , inputSeq = [ Lambda0LLForChargedHyperon2Lambda0LLh, kaonsBachelorForChargedHyperon2Lambda0LLh]           
                                                  , decayDesc =  ['[Omega- -> Lambda0 K-]cc'] 
                                                 )   

        Hlt2Omega2Lambda0LLK = self.__filterHlt1TOS(Lambda0LLKName, hlt2Omega2Lambda0LLKComb)

        #---------------------------------------------------------------------------------------------------
    
        # Stage 2bii - Lambda0 = Lambda0DD

        # Build with Lambda0DD 

        # Make Lambda_c
        Lambda0DDKName  = self.getProp('name_prefix') + '_Omega2Lambda0DDK'
        hlt2Omega2Lambda0DDKComb = self.__OmegaDDCombine(  name = Lambda0DDKName
                                                  , inputSeq = [ Lambda0DDForChargedHyperon2Lambda0DDh, kaonsBachelorForChargedHyperon2Lambda0DDh]           
                                                  , decayDesc =  ['[Omega- -> Lambda0 K-]cc'] 
                                                 )   

        Hlt2Omega2Lambda0DDK = self.__filterHlt1TOS(Lambda0DDKName, hlt2Omega2Lambda0DDKComb)



        ######################################################################################################################
        # Define the Hlt2 Lines for ChargedHyperon -> Lambda0 h - seperate Xi (pion) and Omega (kaon)
        # lines, seperate Lambda0 LL//DD lines
        ######################################################################################################################

        ### Lambda0 LL Lines

        ### Xi -> Lambda0 Pi line

        line = Hlt2Line(Lambda0LLPiName, prescale = self.prescale
                        , algos = [ PV3D(), Lambda0LLForChargedHyperon2Lambda0LLh, pionsBachelorForChargedHyperon2Lambda0LLh, Hlt2Xi2Lambda0LLPi]
                        # All the necessary algorithms
                        , postscale = self.postscale 
                        )
        decName = 'Hlt2' + Lambda0LLPiName + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        #---------------------------------------------------------------------------------------------
        
        ### Omega -> Lambda0 K line

        line2 = Hlt2Line( Lambda0LLKName, prescale = self.prescale
                        , algos = [ PV3D(), Lambda0LLForChargedHyperon2Lambda0LLh, kaonsBachelorForChargedHyperon2Lambda0LLh, Hlt2Omega2Lambda0LLK]
                        # All the necessary algorithms
                        , postscale = self.postscale
                        )
        decName2 = 'Hlt2' + Lambda0LLKName + 'Decision'
        annSvcID2 = self._scale(decName2,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName2 : annSvcID2 } )

        #---------------------------------------------------------------------------------------------

        ### Lambda0 DD lines

        ### Xi -> Lambda0 long Pi line

        line3 = Hlt2Line(Lambda0DDPiName, prescale = self.prescale
                        , algos = [ PV3D(), Lambda0DDForChargedHyperon2Lambda0DDh, pionsBachelorForChargedHyperon2Lambda0DDh, Hlt2Xi2Lambda0DDPi]
                        # All the necessary algorithms
                        , postscale = self.postscale
                        )
        decName3 = 'Hlt2' + Lambda0DDPiName + 'Decision'
        annSvcID3 = self._scale(decName3,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName3 : annSvcID3 } )
        
        ### Xi -> Lambda0 long Pi line

        line3A = Hlt2Line(Lambda0DDPiDName, prescale = self.prescale
                        , algos = [ PV3D(), Lambda0DDForChargedHyperon2Lambda0DDh, downstreamPionsForChargedHyperon2Lambda0DDh, Hlt2Xi2Lambda0DDPiD]
                        # All the necessary algorithms
                        , postscale = self.postscale
                        )
        decName3A = 'Hlt2' + Lambda0DDPiDName + 'Decision'
        annSvcID3A = self._scale(decName3A,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName3A : annSvcID3A } )
        
        ### Lambda_c -> Lambda0 K line------------------------------------------------------------------------------------

        line4 = Hlt2Line(Lambda0DDKName, prescale = self.prescale
                        , algos = [ PV3D(), Lambda0DDForChargedHyperon2Lambda0DDh, kaonsBachelorForChargedHyperon2Lambda0DDh, Hlt2Omega2Lambda0DDK]
                        # All the necessary algorithms
                        , postscale = self.postscale
                        )
        decName4 = 'Hlt2' + Lambda0DDKName + 'Decision'
        annSvcID4 = self._scale(decName4,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName4 : annSvcID4 } )

