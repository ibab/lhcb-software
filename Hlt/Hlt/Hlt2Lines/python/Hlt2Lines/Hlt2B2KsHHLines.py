###############
#
# @author Jussara Miranda jussara@cbpf.br
# @date 10.07.2010
#
## ######################################################################
## Defines a configurable to define and configure Hlt2 lines for selecting
## B->Ks HH (B==B0,Bs) ; Ks==KsLL,KsDD, H==K,pi)
## ######################################################################
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2KsHHLinesConf(HltLinesConfigurableUser) :

    # steering variables
    #------------------------
    # Don't touch my variables!
    __slots__ = {  'B_Mlow'          :  200. 
                ,  'B_Mhigh'         :  280.   
                ,  'B_DIRA'          :  0.998   
                ,  'B_FDpvsv'        :  0.8   
                ,  'B_FDCHI2LL'      :  40.   
                ,  'B_FDCHI2DD'      :  20.  
                ,  'B_IPLL'          :  0.1   
                ,  'B_IPDD'          :  0.2  
                ,  'B_VCHI2'         :  15.   
                ,  'B_POINTPT'       :  0.2   
            ,  'trkCHI2Dof'      :  10. 
                ,  'hhMAXM'          :  4800.   
                ,  'hhDOCA'          :  0.5   
                ,  'hhVCHI2'         :  8.0   
                ,  'hhFDCHI2'        :  30.   
                ,  'hhIPsum'         :  0.1   
                ,  'hhPTmax'         :  700   
                ,  'KS_Mwindow'      :  30.   
                ,  'KS_FDCHI2opv_LL' :  40.    
                ,  'KS_FDCHI2opv_DD' :  20.   
                ,  'KS_VCHI2_LL'     :  5.    
                ,  'KS_VCHI2_DD'     :  10.   
            ,  'Prescale'                : {
                                  'Hlt2B2KsHHTFKsLL' : 1.0
                                , 'Hlt2B2KsHHTFKsDD' : 1.0
                                                
                                              }
                , 'Postscale'               : { 
                                              }
                }


  
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member, bindMembers
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from HltTracking.HltPVs import PV3D


        ########
        # hh  
        #######
        hhdaugcuts = {"K+":"(TRCHI2DOF<%(trkCHI2Dof)s)"% self.getProps() ,"pi+": "(TRCHI2DOF<%(trkCHI2Dof)s)"% self.getProps()}
        hhcombcuts = """(AM<%(hhMAXM)s *MeV)
                        &(AMAXDOCA('LoKi::TrgDistanceCalculator')< %(hhDOCA)s )
                        &((ACHILD(MIPCHI2DV(PRIMARY),1)+ACHILD(MIPCHI2DV(PRIMARY),2))>%(hhIPsum)s)
                        &(AHASCHILD(PT > %(hhPTmax)s *MeV)) &(AALLSAMEBPV) """ % self.getProps()
        hhmothercuts = """(VFASPF(VCHI2)<%(hhVCHI2)s)
                        & (BPVVDCHI2>%(hhFDCHI2)s)""" % self.getProps()

        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedPions
        hhforB2KsHH = Hlt2Member( CombineParticles
                                  , 'HHforB2KsHH'
                                  , DecayDescriptors = [ "K*(892)0 -> pi+ pi-", "K*(892)0 -> K+ K-", \
                                         "K*(892)0 -> K+ pi-", "K*(892)0 -> pi+ K-",\
                             "K*(892)0 -> pi+ pi+", "K*(892)0 -> K+ K+",\
                             "K*(892)0 -> pi- pi-", "K*(892)0 -> K- K-" ]
                                  , DaughtersCuts = hhdaugcuts
                                  , CombinationCut = hhcombcuts
                                  , MotherCut = hhmothercuts
                                  , Inputs = [BiKalmanFittedKaons,BiKalmanFittedPions ]
                                     )
        B2KsHHTFHH = bindMembers( 'B2KsHHTFhh',  [PV3D(), BiKalmanFittedKaons, BiKalmanFittedPions, hhforB2KsHH ] )


        ########
        # KsLL  
        #######
        KSLL_FilterCuts= """(ADMASS('KS0')<%(KS_Mwindow)s) 
                       & (BPVVDCHI2 > %(KS_FDCHI2opv_LL)s) & (VFASPF(VCHI2)<%(KS_VCHI2_LL)s) 
               & (CHILDCUT((TRCHI2DOF < %(trkCHI2Dof)s),1)) 
               & (CHILDCUT((TRCHI2DOF < %(trkCHI2Dof)s),2)) """% self.getProps()
        from Hlt2SharedParticles.Ks import KsLLTF
        KSLLforB2KsHH = Hlt2Member( FilterDesktop # type
                               , "KsLLforB2KsHH"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , Code =KSLL_FilterCuts
                               , Inputs = [KsLLTF]
                      )
        B2KsHHKsLL = bindMembers( "B2KsHHKsLL", [ PV3D(), KsLLTF, KSLLforB2KsHH ] )


        ########
        # KsDD  
        #######
        KSDD_FilterCuts= """(ADMASS('KS0')<%(KS_Mwindow)s) 
                       & (BPVVDCHI2 > %(KS_FDCHI2opv_DD)s) & (VFASPF(VCHI2)<%(KS_VCHI2_DD)s) """% self.getProps()

        from Hlt2SharedParticles.Ks import KsDD
        KSDDforB2KsHH = Hlt2Member( FilterDesktop # type
                               , "KsDDforB2KsHH"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , Code = KSDD_FilterCuts
                               , Inputs = [KsDD]
                      )
        B2KsHHKsDD = bindMembers( "B2KsHHKsDD", [ PV3D(), KsDD, KSDDforB2KsHH ] )

        ########
        # B2KsLLhh  
        #######
        BLL_MotherCuts = """(BPVTRGPOINTINGWPT<%(B_POINTPT)s)
                   &(VFASPF(VCHI2)<%(B_VCHI2)s)
                   &(BPVDIRA>%(B_DIRA)s)
                   &(MIPDV(PRIMARY)<%(B_IPLL)s)
                   &(VFASPF(VMINVDDV(PRIMARY))>%(B_FDpvsv)s)
                   &(BPVVDCHI2>%(B_FDCHI2LL)s)"""% self.getProps()

        B2KsLLhh = Hlt2Member( CombineParticles # type
                               , "B2KsLLhh"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , DecayDescriptor = "B0 -> K*(892)0 KS0"
                               , CombinationCut = "(AM>(5279-%(B_Mlow)s))&(AM<(5279+%(B_Mhigh)s))"% self.getProps()
                               , MotherCut = BLL_MotherCuts
                               , Inputs = [B2KsHHTFHH,B2KsHHKsLL]
                               )


        ########
        #   B2KsDDhh 
        #######
        BDD_MotherCuts="""(BPVTRGPOINTINGWPT<%(B_POINTPT)s)
               &(VFASPF(VCHI2)<%(B_VCHI2)s)
               &(BPVDIRA>%(B_DIRA)s)
               &(MIPDV(PRIMARY)<%(B_IPDD)s)
               &(VFASPF(VMINVDDV(PRIMARY))>%(B_FDpvsv)s)
               &(BPVVDCHI2>%(B_FDCHI2DD)s)"""% self.getProps()
        B2KsDDhh = Hlt2Member( CombineParticles # type
                               , "B2KsDDhh"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , DecayDescriptor = "B0 -> K*(892)0 KS0"
                               , CombinationCut = "(AM>(5279-%(B_Mlow)s) *MeV)&(AM<(5279+%(B_Mhigh)s) *MeV)"% self.getProps()
                               , MotherCut = BDD_MotherCuts
                               , Inputs = [B2KsHHTFHH,B2KsHHKsDD]
                               )

       ###########################################################################
       # Define the Hlt2 Lines
#                        , algos = [PV3D(), BiKalmanFittedKaons,BiKalmanFittedPions, HHforB2KsHH,KsLL, KSLLforB2KsHH,B2KsLLhh]
       #
        line = Hlt2Line('B2KsHHTFKsLL'
                    , prescale = self.prescale
                        , HLT =  "HLT_PASS_RE('Hlt1(SingleHadron|DiHadron)Decision')"
                        , algos = [B2KsHHTFHH,B2KsHHKsLL,B2KsLLhh]
                        , postscale = self.postscale
                        )
        line = Hlt2Line('B2KsHHTFKsDD'
                        , prescale = self.prescale
                        , HLT =  "HLT_PASS_RE('Hlt1(SingleHadron|DiHadron)Decision')"
                        , algos = [B2KsHHTFHH,B2KsHHKsDD,B2KsDDhh]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2KsHHTFKsLLDecision" : 50620 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2KsHHTFKsDDDecision" : 50621 } )



